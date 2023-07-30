#include "mprpccontroller.h"

//构造函数初始化
MprpcController::MprpcController()
{
    m_failed = false;
    m_errText = "";
}

//重置
void MprpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

//判断当前调用是否成功
bool MprpcController::Failed() const
{
    return m_failed;
}

//错误信息
std::string MprpcController::ErrorText() const
{
    return m_errText;
}

//发生错误时调用
void MprpcController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errText = reason;
}

// 目前未实现具体的功能
void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const {return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}