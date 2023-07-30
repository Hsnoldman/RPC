#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

//继承自google::protobuf::RpcChannel，重写CallMethod方法
class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    // 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据数据序列化和网络发送 
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf:: Closure* done);
};

/*
所有通过stub代理对象调用的rpc方法，最终会通过MprpcChannel类的CallMethod方法
CallMethod方法进行数据的序列化，并通过网络发送给对端
*/