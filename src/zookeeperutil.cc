#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>
#include "logger.h"

// 全局的watcher观察器   zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type,
					int state, const char *path, void *watcherCtx)
{
	if (type == ZOO_SESSION_EVENT) // 回调的消息类型是和会话相关的消息类型
	{
		if (state == ZOO_CONNECTED_STATE) // zkclient和zkserver连接成功
		{
			sem_t *sem = (sem_t *)zoo_get_context(zh); // 获取m_zhandle句柄的信号量
			sem_post(sem);							   // 信号量资源+1
		}
	}
}

// 构造函数，初始化zk句柄
ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
	if (m_zhandle != nullptr)
	{
		zookeeper_close(m_zhandle); // 关闭句柄，释放资源
	}
}

// 连接zkserver
void ZkClient::Start()
{
	std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
	std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
	std::string connstr = host + ":" + port; // 组合连接地址，ip:port

	/*
	zookeeper_mt：多线程版本
	zookeeper的API客户端程序提供了三个线程
		1.API调用线程 worker线程
		2.网络I/O线程  pthread_create  poll
		3.watcher回调线程 pthread_create
	*/
	// 超时时间30s
	m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
	if (nullptr == m_zhandle)
	{
		// std::cout << "zookeeper_init error!" << std::endl;
		LOG_ERR("zookeeper_init error!");

		exit(EXIT_FAILURE);
	}
	//zookeeper_init()是异步函数，这里需要使用信号量进行线程同步
	sem_t sem;
	sem_init(&sem, 0, 0);
	zoo_set_context(m_zhandle, &sem); // 给m_zhandle句柄绑定信号量

	sem_wait(&sem); // 阻塞等待global_watcher函数中的连接成功，成功后信号量+1，此数继续向下执行
	// std::cout << "zookeeper_init success!" << std::endl; // 到这里，zkclient才正式连接zkserver成功
	LOG_INFO("zookeeper_init success!");
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
	char path_buffer[128];
	int bufferlen = sizeof(path_buffer);
	int flag;
	// 先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
	flag = zoo_exists(m_zhandle, path, 0, nullptr);
	if (ZNONODE == flag) // 表示path的znode节点不存在
	{
		// 创建指定path的znode节点了
		flag = zoo_create(m_zhandle, path, data, datalen,
						  &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
		if (flag == ZOK)
		{
			// std::cout << "znode create success... path:" << path << std::endl;
			LOG_INFO("znode create success... path: %s", path);
		}
		else
		{
			std::cout << "flag:" << flag << std::endl;
			// std::cout << "znode create error... path:" << path << std::endl;
			LOG_ERR("znode create error... path: %s", path);

			exit(EXIT_FAILURE);
		}
	}
}

// 根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char *path)
{
	char buffer[64];
	int bufferlen = sizeof(buffer);
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
	if (flag != ZOK)
	{
		// std::cout << "get znode error... path:" << path << std::endl;
		LOG_ERR("get znode error... path: %s", path);
		return "";
	}
	else
	{
		return buffer;
	}
}