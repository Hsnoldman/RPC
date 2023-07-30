// #include "test.pb.h"
#include "test1.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

void func1()
{
    // 封装login请求对象的数据
    LoginRequest req;
    req.set_name("Jack");
    req.set_pwd("123456");

    std::string send_str;

    // 数据序列化 -> char*
    std::cout << "序列化：";
    if (req.SerializeToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }

    // 从send_str反序列化一个login请求对象
    std::cout << "反序列化：" << std::endl;
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
}

void func2()
{
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败了");


    /*
        对象成员的访问
        当一个对象中包含了另一个对象，修改值时不能直接访问该对象，需要定义该对象的指针
        GetFriendListsResponse对象中包含了ResultCode对象，如果需要对ResultCode中的error_code赋值
        需要定义一个ResultCode对象的指针，接收rsp.mutable_result()函数返回的指针
        通过该指针访问ResultCode对象的error_code成员
        语法：对象名.mutable_[字段名]() 

        列表成员的访问
        当对象中包含了一个列表，如User，同样需要定义指针访问
        语法：对象名.add_[字段名]()  即：rsp.add_friend_list()
    */
    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;
    std::cout << rsp.friend_list(1).name() << std::endl; //访问数据
    std::cout << rsp.result().errcode() << std::endl; //访问errorcode

    //序列化为string
    std::string send_msg;
    if(!rsp.SerializeToString(&send_msg))
    {
        std::cout << "filed."<<std::endl;
    }

}

int main()
{
    // func1();
    func2();

    return 0;
}