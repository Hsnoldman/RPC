#include "mprpcconfig.h"

#include <iostream>
#include <string>

// 负责解析加载配置文件
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");//读，打开文件
    if (nullptr == pf)
    {
        //文件不存在
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    //处理三种情况： 1.注释   2.正确的配置项 =    3.去掉开头结尾的多余的空格和回车 
    while(!feof(pf))
    {
        char buf[512] = {0}; //存储配置项
        fgets(buf, 512, pf);//读取一行

        // 去掉字符串前面多余的空格
        std::string read_buf(buf); //char*数组转为string类型
        Trim(read_buf);

        // 判断#的注释
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            // 配置项不合法
            continue;
        }

        //解析
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        // rpcserverip=127.0.0.1\n
        int endidx = read_buf.find('\n', idx);//处理换行符
        value = read_buf.substr(idx+1, endidx-idx-1);
        Trim(value);
        //将配置参数写入map表
        m_configMap.insert({key, value});
    }

    fclose(pf);
}

// 查询配置项信息
std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    //不存在，返回空字符串
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}

// 去掉字符串前后的空格
void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size()-idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx+1);
    }
}