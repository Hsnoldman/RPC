### 主要技术

RPC(Remote Procedure Call Protocol)远程过程调用协议

Protobuf数据序列化和反序列化协议

ZooKeeper分布式一致性协调服务

Muduo 基于EPOLL+线程池的高性能网络库

CMake构建项目集成编译环境

### 项目代码工程目录

bin：可执行文件

build：项目编译文件 

lib：项目库文件 

src：源文件 

test：测试代码 

example：框架代码使用范例

CMakeLists.txt：顶层的cmake文件 

README.md：项目自述文件 

autobuild.sh：一键编译脚本

### 项目编译

根目录下运行一键编译脚本

```
sudo ./autobuild.sh
```

### Protobuf

#### 简介

Google Protocol Buffer( 简称 Protobuf) 是 Google 公司内部的混合语言数据标准，目前已经正在使用的有超过 48,162 种报文格式定义和超过 12,183 个 .proto 文件。他们用于 RPC 系统和持续数据存储系统。

Protocol Buffers 是一种**轻便高效的结构化数据存储格式**，可以用于结构化数据串行化、或者说序列化。它很适合做数据存储或RPC数据交换格式。可以用于即时通讯、数据存储等领域的语言无关、平台无关、可扩展的序列化结构数据格式。相比于xml、json，它的**体量更小**，**解析速度更快**，所以在网络通信应用中，非常适合将 Protobuf 作为数据传输格式。

**protobuf的核心内容包括：**
**定义消息**：消息的结构体，以message标识。
**定义接口**：接口路径和参数，以service标识。

通过protobuf提供的机制，服务端与服务端之间只需要关注接口方法名（service）和参数（message）即可通信，而不需关注繁琐的链路协议和字段解析，极大降低了服务端的设计开发成本。

#### 安装配置

源码下载：[GitHub - protocolbuffers/protobuf: Protocol Buffers - Google's data interchange format](https://github.com/protocolbuffers/protobuf)

安装：

1、解压压缩包：`unzip protobuf-master.zip` 

2、进入解压后的文件夹：`cd protobuf-master` 

3、安装所需工具：`sudo apt-get install autoconf automake libtool curl make g++ unzip` 

4、自动生成configure配置文件：`./autogen.sh` 

5、配置环境：`./configure` 

6、编译源代码：`make` 

7、安装：`sudo make install` 

8、刷新动态库：`sudo ldconfig`

#### .proto文件编写及使用

##### 标识符

- syntax：标识使用的protobuf是哪个版本。上面表示使用的是3.x版本。
- package：标识生成目标文件的包名。在C++中表示的是命名空间。
- enum：表示一个枚举类型。会在目标.h文件中自动生成一个枚举类型。
- message：标识一条消息。会在目标文件中自动生成一个类。

##### 定义数据结构

```protobuf
//test.proto文件
syntax = "proto3"; // 声明了protobuf的版本
package fixbug; // 声明了代码所在的包（namespace）

message ResultCode
{
    int32 errcode = 1;
    bytes errmsg = 2;
}

// 定义登录请求消息类型  name   pwd
message LoginRequest
{
    bytes name = 1;
    bytes pwd = 2;
}

// 定义登录响应消息类型
message LoginResponse
{
    ResultCode result = 1;
    bool success = 2;
}

message GetFriendListsRequest
{
    uint32 userid = 1;
}

message User
{
    bytes name = 1;
    uint32 age = 2;
    //枚举消息编号从0开始
    enum Sex
    {
        MAN = 0;
        WOMAN = 1;
    }
    Sex sex = 3;
}

message GetFriendListsResponse
{
    ResultCode result = 1;
    //repeated存储列表
    repeated User friend_list = 2;  // 定义了一个列表类型
}
```

##### 字段编号

消息定义中的每个字段都有一个唯一的编号。这些字段编号用于以二进制格式标识字段，一旦消息类型被使用，就不应该被更改。

##### 数据类型

格式：`role type name = tag [default value]`

role 有三种取值：

- **required**：该字段必须给值，不能为空。否则message被认为是未初始化的。如果试图建立一个未初始化的message将会抛出RuntimeException异常，解析未初始化的message会抛出IOException异常。
- **optional**：表示该字段是可选值，可以为空。如果不设置，会设置一个默认值。也可以自定义默认值。如果没有自定义默认值，会是用系统默认值。
- **repeated**：表示该字段可以重复，可等同于动态数组。



对于string类型的数据，定义为**bytes**类型可以减少字符间编码的转换，提高编码效率。



映射表类型Map

如果要创建一个关联映射，Protobuf提供了一种快捷的语法：

​    key_type可以是任意Integer或者string类型(除了floating和bytes的任意标量类型都可以)，value_type可以是任意类型，但不能是map类型。

```protobuf
map<key_type, value_type> map_field = N;
```

注意：

Map的字段可以是repeated。
序列化后的顺序和map迭代器的顺序是不确定的，所以不要期望以固定顺序处理Map。
当为.proto文件产生生成文本格式的时候，map会按照key 的顺序排序，数值化的key会按照数值排序。
map语法序列化后等同于如下内容，所以Map不经常使用。

```protobuf
message MapFieldEntry {
    key_type key = 1;
    value_type value = 2;
}
repeated MapFieldEntry map_field = N;
```

##### 默认值

当解析 message 时，如果被编码的 message 里没有包含某些变量，那么根据类型不同，会有不同的默认值：

string：默认是空的字符串
byte：默认是空的bytes
bool：默认为false
numeric：默认为0
enums：定义在第一位的枚举值，也就是0
messages：根据生成的不同语言有不同的表现

##### 编译.proto文件

在.proto文件中定义了数据结构，这些数据结构是面向开发者和业务程序的，并不面向存储和传输。当需要把这些数据进行存储或传输时，就需要将这些结构数据进行序列化、反序列化以及读写。ProtoBuf 提供相应的接口代码，可以通过 protoc 这个编译器来生成相应的接口代码。

```shell
protoc test.proto --cpp_out=./
```

生成test.pb.cc和test.pb.h文件

##### 代码中使用

包含相应头文件

```c++
#include "test.pb.h"
using namespace fixbug;
```

定义对象

```c++
LoginRequest req;
req.set_name("Jack");
req.set_pwd("123456");
```

序列化为string

```c++
std::string send_str;
bool result = req.SerializeToString(&send_str);
```

反序列化

```c++
std::string send_str;
LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
```

##### 定义描述rpc方法的类型

.proto文件中需要添加描述，定义下面的选项，表示生成service服务类和rpc方法描述，默认不生成

```protobuf
option cc_generic_services = true;
```

Protobuf本身不支持RPC相关功能，只是对RPC方法的描述，通过描述，protobuf可以实现对RPC请求参数和返回值的序列化及反序列化。

```protobuf
service UserServiceRpc
{
    rpc Login(LoginRequest) returns(LoginResponse);
    rpc GetFriendLists(GetFriendListsRequest) returns(GetFriendListsResponse);
}
```

Login函数的参数为LoginRequest对象中的name和pwd，返回值为LoginResponse中的值

#### 补充

1.当在.proto文件中定义一个message对象时，如：

```
message LoginRequest
{
    bytes name = 1;
    bytes pwd = 2;
}
```

protobuf会生成一个LoginRequest类，继承自google::protobuf::**Message**类

​	提供**访问对象**的方法：name(), pwd()

​	提供**设置对象**的方法：set_name(), set_pwd()

​	提供**序列化和反序列化**的方法(以string为例)：SerializeToString(), ParseFromString()

2.当定义描述rpc方法的类型时，如：

```protobuf
service UserServiceRpc
{
    rpc Login(LoginRequest) returns(LoginResponse);
}
```

protobuf会生成一个UserServiceRpc类和一个UserServiceRpc_Stub类，继承自google::protobuf::**Service**类

- **Callee** ServiceProvider(RPC服务提供者)

```c++
class UserServiceRpc : public ::PROTOBUF_NAMESPACE_ID::Service {
    /*...*/
	virtual void Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done);

	const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* GetDescriptor();
     /*...*/
}
```

- **Caller** ServiceConsumer(RPC服务消费者)

```c++
class UserServiceRpc_Stub : public UserServiceRpc {
 public:
  UserServiceRpc_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel);
  UserServiceRpc_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
                   ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership);
  ~UserServiceRpc_Stub();

  inline ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel() { return channel_; }

  // implements UserServiceRpc ------------------------------------------

  void Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(UserServiceRpc_Stub);
};
```

Login方法实现

```c++
void UserServiceRpc_Stub::Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              	const ::fixbug::LoginRequest* request,
                              	::fixbug::LoginResponse* response,
                              	::google::protobuf::Closure* done) {
	channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
```

底层通过RpcChannel类的channel对象调用CallMethod方法，该方法是一个抽象方法。

```c++
//RpcChannel类
class PROTOBUF_EXPORT RpcChannel {
 public:
  inline RpcChannel() {}
  virtual ~RpcChannel();

  virtual void CallMethod(const MethodDescriptor* method,
                          RpcController* controller, const Message* request,
                          Message* response, Closure* done) = 0;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(RpcChannel);
};
```

通过自定义一个类继承RpcChannel类，重写CallMethod()这个纯虚函数

```c++
class MyRpcChannel : public RpcChannel { /*实现CallMethod()方法*/}
```

### RPC

#### 简介

RPC（Remote Procedure Call Protocol）**远程过程调用协议**。
RPC的主要功能是让构建分布式计算更加容易，再提供强大的远程调用能力的同时而不损失本地调用的语义简洁性。为实现该目标，RPC框架需要提供一套透明的调用机制，使得使用者不用显示的区分本地调用还是分布式调用。

#### RPC服务调用过程

![img](https://img-blog.csdnimg.cn/20200511111118844.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3UwMTQ2MzA2MjM=,size_16,color_FFFFFF,t_70)

**具体调用过程：**

1. 服务消费者(client客户端)通过本地调用的方式调用服务。
2. 客户端存根(client stub)接收到请求后负责将方法、入参等信息组装(序列化)成能够进行网络传输的消息体。
3. 客户端存根(client stub)找到远程的服务地址，并且将消息通过网络发送给服务端。
4. 服务端存根(server stub)收到消息后进行解码(反序列化)。
5. 服务端存根(server stub)根据解码结果调用本地的服务进行相关处理。
6. 本地服务执行具体的业务逻辑并将处理结果返回给服务端存根（server stub）。
7. 服务端存根(server stub)将返回的结果重新打包成消息(序列化)，并通过网络发送到消费方。
8. 客户端存根(client stub)接收到消息，并进行解码(反序列化)。
9. 服务消费方得到最终的结果。

#### 将本地方法发布为RPC方法基本流程

##### **1.定义.proto文件**

​	定义请求和响应参数类型

​	定义描述RPC方法的类型，注意添加`option cc_generic_services = true;`选项

##### **2.编译.proto文件**

​	protoc命令编译

​	生成`.pb.h`和`.pb.cc`文件

​	以UserServiceRpc为例，会生成UserServiceRpc和UserServiceRpc_stub两个类

​	UserServiceRpc类给服务的生产者(发布者)提供，UserServiceRpc_stub类给服务的消费者提供

##### **3.本地实现UserService方法**

​	该方法继承自UserServiceRpc方法，实现UserServiceRpc方法中的虚函数，如Login

​		示例为./example/callee/userservice.cc文件

​		在Login方法中

​		（1）获取参数

​		（2）执行本地Login业务代码

​		（3）将业务处理结果写入响应

​		（4）执行回调(Run()方法）将响应对象数据序列化并通过网络发送

##### 4.发布该rpc服务

​	（1）调用框架初始化操作Init()，读取相应参数：rpcserver的ip地址和端口号，zookeeper的ip地址和端口号

​	（2）把UserService类的对象发布到rpc节点上

​	（3）启动这个rpc节点，启动之后进程进入阻塞状态，等待远程的rpc调用请求



### 框架主要模块

#### Config类

负责解析配置文件和配置项

```c++
class MprpcConfig
{
public:
    // 负责解析加载配置文件
    void LoadConfigFile(const char *config_file);
    // 查询配置项信息，传入key，返回value
    std::string Load(const std::string &key);
private:
    //存储参数：rpcserverip   rpcserverport    zookeeperip   zookeeperport
    std::unordered_map<std::string, std::string> m_configMap;
    // 去掉字符串前后的空格
    void Trim(std::string &src_buf);
};
```

将配置文件中的数据存入hash表

- rpc节点的ip地址：key: rpcserverip value: 127.0.0.1   
- rpc节点的port端口号：key: rpcserverport value: 8000
- zk的ip地址：key: zookeeperip value: 127.0.0.1
- zk的port端口号：key: zookeeperport value: 2181

#### Header类

使用protobuf定义一条RPC请求的参数格式：

```protobuf
//header.proto
syntax = "proto3";
package mprpc;
message RpcHeader
{
    bytes service_name = 1; //服务对象名
    bytes method_name = 2; //方法名
    uint32 args_size = 3; //参数数据长度
}
```

在框架内部，RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型

一条RPC请求的参数包括：**service_name**（对象名），**method_name**（方法名），**args_size**（参数数据长度）

约定一条请求的前4个字节为header_size，即service_name的长度+method_name的长度
将header_size直接存为二进制int类型，4个字节

例：一条请求message：**16UserServiceLoginJack123456**
		其中，16表示**UserServiceLogin**(service_name+method_name)的长度
		参数为：**Jack123456**

#### Application类

mprpc框架的基础类，负责框架的一些初始化操作，单例模式

```c++
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance(); //获取单例对象的接口
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config; 

    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};
```

##### **MprpcApplication::Init()函数**

判断传入参数是否合法，调用Config类解析参数

##### MprpcApplication::GetConfig()函数

返回存储参数的对象m_config

#### Controller类

负责管理RPC方法调用过程中的一些状态信息

记录RPC方法调用过程中的状态信息

```c++
//继承自google::protobuf::RpcController类
class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset(); //重置
    bool Failed() const; //判断当前调用是否成功
    std::string ErrorText() const; //返回错误信息
    void SetFailed(const std::string& reason); //发生错误时调用，设置错误信息内容
private:
    bool m_failed; // RPC方法执行过程中的状态
    std::string m_errText; // RPC方法执行过程中的错误信息
};
```

#### Provider类

专门发布rpc服务的网络对象类，由服务的发布方实例化对象，提供发布RPC方法的接口

```c++
class RpcProvider
{
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);
    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
private:
    // 组合EventLoop
    muduo::net::EventLoop m_eventLoop;
    // service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service; // 保存服务对象
        unordered_map<string, const google::protobuf::MethodDescriptor*> m_methodMap; // 保存服务方法
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
    // 新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    // 已建立连接用户的读写事件回调
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    // Closure的回调操作，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};
```

##### 私有成员m_serviceMap

```c++
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
```

Hash表结构，存储Service对象和其注册的所有方法的信息

unordered_map：{ 对象名(UserService)，ServiceInfo }

##### 结构体成员ServiceInfo

```c++
	// service服务类型信息
    struct ServiceInfo
    {
        // 保存服务对象UserService
        google::protobuf::Service *m_service; 
        // 保存服务方法
        unordered_map<string, const google::protobuf::MethodDescriptor*> m_methodMap; 
    };
```

例：UserService对象发布了Login方法

- ServiceInfo中存储的数据为：m_service：UserService对象
- unordered_map：{ 方法名(Login)，方法描述 }

##### NotifyService()函数

主要作用：

通过传入的google::protobuf::Service对象，获取其所携带的信息，包括Service的名称，所提供的方法，方法名，方法描述等。

将这些信息封装成ServiceInfo，最终存储在m_serviceMap(Hash表)对象中。

##### Run()函数

主要作用：启动rpc服务节点，开始提供rpc远程网络调用服务。

muduo网络库的启动封装在Run()方法里

Run()方法主要内容：

1.读取配置文件rpcserver的信息，获取ip和port，初始化InetAddress

```c++
muduo::net::InetAddress address(ip, port);
```

2.创建TcpServer对象

```c++
muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
```

3.绑定连接回调和消息读写回调方法

```c++
server.setConnectionCallback(/*...参数...*/);
server.setMessageCallback(/*...参数...*/);
```

4.设置muduo库的线程数量

```c++
server.setThreadNum(4); // 1个IO线程，3个工作线程
```

5.把当前rpc节点上要发布的服务全部注册到zookeeper上面，让RPC_Client可以从zookeeper上发现服务

6.启动网络服务

```c++
server.start();
m_eventLoop.loop();
```

##### OnMessage()函数

当发生一个rpc服务调用请求时，OnMessage方法就会响应

OnMessage()的主要内容：

1.反序列化这条请求消息，解析这条请求message内容，获取service_name，method_name，args_size

2.从m_serviceMap中获取该请求指定的对象和方法

3.生成rpc方法调用的请求request参数和响应response参数

```c++
google::protobuf::Message *request = service->GetRequestPrototype(method).New();
google::protobuf::Message *response = service->GetResponsePrototype(method).New();
```

4.给请求调用绑定一个回调函数

```c++
Closure *done = NewCallback<RpcProvider,const TcpConnectionPtr&, Message *>
(this, &SendRpcResponse, conn, response);
```

5.在框架上根据远端rpc请求，调用当前rpc节点上发布的方法

```c++
service->CallMethod(method, nullptr, request, response, done); // 执行callee注册的方法
```

##### SendRpcResponse()函数

OnMessage()函数的第4步绑定的回调函数，当方法执行结束后，通过该回调函数将结果发送给调用方

主要作用：序列化rpc的响应消息并通过网络发送

SendRpcResponse()的主要内容：

1.序列化响应消息

```c++
response->SerializeToString(&response_str);
```

2.发送响应消息

```c++
conn->send(response_str);
```

3.关闭连接

```c++
conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
```

#### Channel类

继承自google::protobuf::RpcChannel，重写CallMethod方法

所有通过stub代理对象调用的rpc方法，最终会通过MprpcChannel类的CallMethod方法进行调用

由服务的调用方实例化对象

CallMethod()的主要内容：

1.获取调用的Service对象名，方法名，参数

2.序列化请求参数

```c++
request->SerializeToString(&args_str);
```

3.定义rpc的请求header

```c++
mprpc::RpcHeader rpcHeader;
	rpcHeader.set_service_name(service_name);
	rpcHeader.set_method_name(method_name);
	rpcHeader.set_args_size(args_size);
```

4.序列化请求header

```c++
rpcHeader.SerializeToString(&rpc_header_str);
```

5.查询zookeeper上该服务所在的host信息，获取ip和port，通过socket发送调用请求，阻塞等待响应结果

6.接收rpc服务的响应结果，并将结果反序列化



### 日志模块

在项目中，日志模块必不可少，方便后期维护快速定位错误信息。

写日志属于**磁盘I/O，开销较大**，不能在业务处理中同时写日志信息，这会使得**业务开销增大**，应该采用**异步**日志写入。

在实际执行过程中，通过一个**消息队列**，将业务处理过程中需要写入的日志信息先加入到消息队列中，然后通过一个**写日志线程**单独处理磁盘I/O。

RPC框架中的Provider采用Muduo网络库作为网络模块，Muduo采用epoll+多线程的机制，因此在高并发的场景下，会同时存在多个线程处理业务，每个线程都会涉及写日志操作，因此，需要保证线程安全。

给**消息队列**加入线程安全操作，在业务进行过程当中，**工作线程**和**写日志线程**都会操作这个消息队列。
		1.工作线程：将业务处理过程中需要写入日志的信息写入内存中的消息队列。
		2.写日志线程：将消息队列中的日志信息写入磁盘。

当消息队列**不为空**时，工作线程和写日志线程通过争抢互斥锁拿到消息队列的控制权。
当消息队列**为空**时，写日志线程阻塞，只有工作线程通过争抢消息队列的互斥锁。

#### 异步日志消息队列

通过C++STL中的queue加上锁机制和条件变量实现

**queue** + **mutex** + **condition_variable**

```c++
// 异步写日志的日志消息队列
template<typename T>
class LockQueue
{
public:
    // 多个worker线程都会写日志queue 
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one();
    }

    // 一个写日志线程读日志queue，写日志文件
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            // 日志队列为空，线程进入wait状态
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};
```

#### Logger类

单例模式

```c++
// Mprpc框架提供的日志系统
class Logger
{
public:
    // 获取日志的单例
    static Logger& GetInstance();
    // 设置日志级别 
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loglevel; // 记录日志级别
    LockQueue<std::string>  m_lckQue; // 日志消息队列

    Logger(); //构造函数私有化
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};
```

使用宏来进行具体写入日志的方法调用

```c++
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
    	//获取单例对象
        Logger &logger = Logger::GetInstance(); \
        //设置日志级别
        logger.SetLogLevel(INFO); \
        //组织日志内容格式
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        //调用Log方法写入日志
        logger.Log(c); \
    } while(0) \
```

使用宏可以使得调用日志模块更加方便，不需要手动创建对象进行函数调用，使用更加方便。

```c++
LOG_INFO("service_name:%s", service_name.c_str());
```

### Zookeeper

#### 1.简介

zookeeper是一个为分布式应用提供一致性协调服务的中间件，提供**分布式协调服务**。

Zookeeper可以解决分布式系统中的**一致性**问题，典型的应用场景有**分布式配置中心**、**分布式注册中心**、**分布式锁**、**分布式队列**、**集群选举**、**分布式屏障**、**发布/订阅**等场景。它提供了一个高可用、高性能的服务，通过在集群中多个节点之间共享数据来保持一致性。

- **master节点选举**，主节点down掉后, 从节点就会接手工作, 并且保证这个节点是唯一的,这也就是所谓首脑模式,从而保证我们集群是高可用的。
- **统一配置文件管理**， 即只需要部署一台服务器, 则可以把相同的配置文件同步更新到其他所有服务器, 此操作在云计算中用的特别多(例如修改了redis统一配置)。
- **数据发布/订阅**， 类似消息队列MQ。
- **分布式锁**，分布式环境中不同进程之间争夺资源,类似于多进程中的锁。
- **集群管理**，保证集群中数据的强一致性。

**zookeeper的特性：**

- **一致性**：数据一致性, 数据按照顺序分批入库。
- **原子性**：事务要么成功要么失败。
- **单一视图**：客户端连接集群中的任意zk节点, 数据都是一致的。
- **可靠性**：每次对zk的操作状态都会保存在服务端。
- **实时性**：客户端可以读取到zk服务端的最新数据。

zookeeper = 文件系统 + 监听通知机制

#### 2.文件系统

zookeeper维护一个**类似文件系统**的数据结构，每个子目录项都被称作为 znode(目录节点)，和文件系统一样，能够自由的增加、删除znode，在一个znode下增加、删除子znode。唯一的不同在于znode是可以存储数据的。开发人员可以使用Zookeeper提供的API来创建、读取、更新和删除znode，以及监视znode的变化。

Zookeeper为了保证高吞吐和低延迟，在内存中维护了这个树状的目录结构，这种特性使得Zookeeper**不能用于存放大量的数据**，每个节点的存放数据上限为**1M**。

有四种类型的znode：

- **PERSISTENT-持久化目录节点**

  客户端与zookeeper断开连接后，该节点依旧存在。

- **PERSISTENT_SEQUENTIAL-持久化顺序编号目录节点**

  客户端与zookeeper断开连接后，该节点依旧存在，只是Zookeeper给该节点名称进行顺序编号。

- **EPHEMERAL-临时目录节点**

  客户端与zookeeper断开连接后，该节点被删除。当超时未发送心跳消息，zookeeper会自动删除该节点。

- **EPHEMERAL_SEQUENTIAL-临时顺序编号目录节点**

  客户端与zookeeper断开连接后，该节点被删除，只是Zookeeper给该节点名称进行顺序编号。

节点信息：

- **cZxid**：创建znode的事务id（Zxid的值）。
- **ctime**：znode创建时间。
- **mZxid**：最后修改znode的事务id。
- **mtime**：znode最近修改时间。
- **pZxid**：最后添加或删除子节点的事务id（子节点列表发生变化才会发生改变）。
- **cversion**：znode的子节点结果集版本（一个节点的子节点增加、删除都会影响这个版本，每变化一次自增1）。
- **dataVersion**：znode的当前数据版本。
- **aclVersion**：表示此znode的acl版本。
- **ephemeralOwner**：znode是临时znode时，表示znode所有者的sessionid，如果znode不是临时节点，则该字段设置为0。
- **dataLength**：znode数据字段的长度。
- **numChildren**：子节点数量。

#### 3.监听通知机制

客户端注册监听它关心的目录节点，当目录节点发生变化（数据改变、被删除、子目录节点增加删除）时，zookeeper会通知客户端。

client端会对某个znode建立一个**watcher事件**，当该znode发生变化时，这些client会收到zk的通知，然后client可以根据znode变化来做出业务上的改变等。

可以监听到**七种**类型变化：

1. None：**连接建立**
2. NodeCreated：**节点创建**
3. NodeDeleted：**节点删除**
4. NodeDataChanged：**节点数据变化**
5. NodeChildrenChanged：**子节点列表变化**
6. DataWatchRemoved：**节点监听被移除**
7. ChildWatchRemoved：**子节点监听被移除**

#### **4.安装及配置**

默认工作端口：**2181**

zookeeper启动需要依赖JDK环境，启动前需要先安装jdk

```shell
sudo apt install openjdk-11-jdk
```

下载zookeeper安装包`zookeeper-3.4.10.tar.gz`

解压缩后进入bin目录，启动zookeeper

```shell
./zkServer.sh start
```

zookeeper的原生开发API：

进入src/c目录，ookeeper已经提供了原生的C/C++和Java API开发接口，需要通过源码 编译生成，过程如下：

```
~/package/zookeeper-3.4.10/src/c$ sudo ./configure
~/package/zookeeper-3.4.10/src/c$ sudo make
~/package/zookeeper-3.4.10/src/c$ sudo make install
```

**原生ZkClient API存在的问题 ：**

Zookeeper原生提供了C和Java的客户端编程接口，但是使用起来相对复杂，几个弱点： 

1. 设置监听watcher只能是一次性的，每次触发后需要重复设置。
2. znode节点只存储简单的byte字节数组，如果存储对象，需要自己转换对象生成字节数组。

**zookeeper常用命令：**

1.**creat**：创建Znode
格式：
        create [-s] [-e] path data acl
        [-e]表示节点是presist，默认为persist
        [-s]表示节点为sequential
        acl是权限控制参数，缺省时不做任何权限控制

2.**ls**：列出path下所有子节点
格式：
		ls path [watch]
        [watch]参数

3.**get**：获取指定节点的数据内容和属性信息

4.**set**：更新节点中的数据

 格式：set path data [version]

5.**delete**：删除指定路径上的节点（节点中不能含有子节点）

6.**rmr**：删除节点包括节点下面的子节点

7.**stat**：查看节点状态

#### 5.Zookeeperutil类

```c++
// 封装的zookeeper客户端类
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // zkClient启动连接zkServer
    void Start();
    // 在zkServer上根据指定的path创建znode节点
    void Create(const char *path, const char *data, int datalen, int state=0);
    // 根据参数指定的znode节点路径，获取znode节点的值
    std::string GetData(const char *path);
private:
    // zk的客户端句柄
    zhandle_t *m_zhandle;
};
```

在项目中每个Znode节点的结构为：

​	<img src="C:\Users\Hsnoldman\AppData\Roaming\Typora\typora-user-images\image-20230731172536797.png" alt="image-20230731172536797" style="zoom:50%;" />

每个method节点中的值为该方法所在的地址，格式为：ip:port  **127.0.0.1:8000**

zookeeper的API客户端程序提供了三个线程

1. API调用线程 worker线程
2. 网络I/O线程  pthread_create  poll
3. watcher回调线程 pthread_create

##### Start()函数

连接zookeeper服务器

主要内容：

1.获取zookeeper服务器的ip和port

​	在项目中通过MprpcApplication类提供的GetConfig()方法读取test.conf中的配置信息

2.调用**zookeeper_init()**函数初始化zookeeper连接

```c++
m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
```

超时时间30s，**global_watcher**为zookeeper的全局watcher观察器，当连接成功后，通过回调给zk客户端通知

由于zookeeper_init()是**异步函数**，需要使用信号量进行线程同步，当受到global_watcher的回调后，才继续执行

##### Create()函数

在zookeeper服务器上根据指定的path创建znode节点

- 通过**zoo_exists()**函数判断节点是否已经存在
- 通过**zoo_create()**函数创建节点

##### GetData()函数

根据指定的path，获取znode节点的值

- 通过**zoo_get()**函数获取节点的值



### 发布RPC方法流程

#### 通过Protobuf定义服务参数结构

```protobuf
//friend.proto
syntax = "proto3";
package fixbug;
option cc_generic_services = true;
//错误码
message ResultCode
{
    int32 errcode = 1; 
    bytes errmsg = 2;
}
//请求结构
message GetFriendsListRequest
{
    uint32 userid = 1;
}
//返回值结构
message GetFriendsListResponse
{
    ResultCode result = 1;
    repeated bytes friends = 2;
}
// 定义rpc方法调用接口
service FiendServiceRpc
{
    rpc GetFriendsList(GetFriendsListRequest) returns(GetFriendsListResponse);
}
```

编译.proto文件

```shell
protoc friend.proto --cpp_out=./
```

生成friend.pb.cc和friend.pb.h文件

服务端需要有两个protobuf，一个是提供具体的服务，就是上面的friend.proto，一个是rpc框架的porotobuf，要有请求的类名、请求的函数名、请求参数的个数,还有请求的参数的值，相当于要接收两个protobuf文件

```protobuf
//rpcheader.proto
syntax = "proto3";
package mprpc;
message RpcHeader
{
    bytes service_name = 1; //服务对象名
    bytes method_name = 2; //方法名
    uint32 args_size = 3; //参数数据长度
}
```

#### 通过调用RPC框架发布方法

protobuf中的FriendServiceRpc类型是service，他会生成两个类，一个类是**FriendServiceRpc**，另一个类是**FriendServiceRpc_Stub**。Service是基类，FriendServiceRpc继承了Service，FriendService_Stub继承了FriendServiceRpc。

其中：

- FriendServiceRpc由服务发布方继承并实现具体方法
- FriendService_Stub由服务调用方使用来调用方法

FiendServiceRpc是由protobuf提供的抽象类，提供一个定义好的GetFriendsList虚函数，服务发布方需要继承这个类然后重写GetFriendsList方法。

```c++
class FriendService : public fixbug::FiendServiceRpc{}
```

##### 将本地方法重构为RPC方法

本地方法：

```c++
    std::vector<std::string> LocalGetFriendsList(uint32_t userid)
    {
        std::cout << "do GetFriendsList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("Mark");
        vec.push_back("John");
        vec.push_back("Kitto");
        return vec;
    }
```

重写protobuf提供的抽象方法GetFriendsList()

```c++
    // 重写基类方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        //调用本地方法
        std::vector<std::string> friendsList = LocalGetFriendsList(userid);
        //写返回数据
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendsList)
        {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run(); //执行回调，即RpcProvider类中的SendRpcResponse()方法，序列化响应，网络发送
    }
```

##### 通过MprpcApplication类发布方法

```c++
int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象。把FriendService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}
```

### 调用RPC方法流程

客户端需要两个类**RpcChannel**和**FriendServiceRpc_Stub**，RpcChannle是由protobuf提供的，但是它是一个抽象类，需要继承重写RpcChannle中的CallMethod函数。

客户端调用方法：

1. main.cpp创建一个请求对象，设置好要请求的值。
2. 通过FriendServiceRpc_Stub来调用，调用GetFriendsList函数，protobuf内部通过Descriptor就能感知客户端调用的是那一个函数。
3. stub调用GetFriendsList函数之后，就会调用RpcChannel类中的Callmethod()函数

```c++
int main(int argc, char **argv)
{
    // 整个程序启动以后，需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);
	// 给FiendServiceRpc_Stub传入RpcChannel对象
    fixbug::FiendServiceRpc_Stub stub(new MprpcChannel());
    // 设置请求参数
    fixbug::GetFriendsListRequest request;
    request.set_userid(1000);
    // 定义rpc方法的响应
    fixbug::GetFriendsListResponse response;
    
    // 定义controller对象
    MprpcController controller;
    
    // 发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::CallMethod
    stub.GetFriendsList(&controller, &request, &response, nullptr); 
    // RpcChannel->RpcChannel::callMethod() 集中来做所有rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读调用的结果
    if (controller.Failed())//说明这次rpc调用过程中出现了错误
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    //调用成功
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc GetFriendsList response success!" << std::endl;
            int size = response.friends_size();
            for (int i=0; i < size; ++i)
            {
                std::cout << "index:" << (i+1) << " name:" << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc GetFriendsList response error : " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}
```

