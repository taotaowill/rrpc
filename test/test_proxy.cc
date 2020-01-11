#include <unistd.h>

#include "boost/bind.hpp"
#include "boost/smart_ptr.hpp"
#include "gtest/gtest.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpConnection.h"

#include "rrpc.h"
#include "timer.h"

#include "test/echo.pb.h"

using namespace muduo;
using namespace muduo::net;
using namespace rrpc;

void onConnection(const TcpConnectionPtr &conn) {
    if (!conn->connected()) {
//        LOG(WARNING) << "connection closed by peer";
        exit(-1);
        return;
    }

    RpcConnectionMeta meta;
    meta.conn_id = -1;
    meta.crc = 1;
    size_t size = sizeof(meta);
    char* data = new char[size];
    memset(data, 0, size);
    memcpy(data, &meta, size);
    conn->send(reinterpret_cast<void*>(data), size);
//    LOG(INFO) << "EchoClient conn meta sended to proxy";
}

// receive proxy echo back(validation)
void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time) {
//    int32_t now_time = baidu::common::timer::now_time();
//    LOG(INFO) << "now_time: " << now_time;
    muduo::string msg(buf->retrieveAllAsString());
//    LOG(INFO) << conn->name() << " echo " << msg.size() << " bytes, "
//              << "data received at " << time.toString();
    if (msg.size() != RPC_CONNECTION_META_SIZE) {
        return;
    }

    RpcMessage message;
    RpcMeta* meta = new RpcMeta();
    meta->set_sequence_id(123);
    meta->set_method("rrpc.test.EchoService.Echo");
    message.meta.CopyFrom(*meta);

    rrpc::test::EchoRequest* echo_request = new rrpc::test::EchoRequest;
    echo_request->set_message("this is request from fake client");
    std::string buff;
    echo_request->SerializeToString(&buff);

    message.data = buff.c_str();
    message.src_id = -1;
    message.dst_id = 100045;
    uint32_t size;
    void* send_buff = message.Packaging(size);
//    LOG(INFO) << "message size: " << size;
    conn->send(send_buff, size);
//    LOG(INFO) << "hhhhhh";
    free(send_buff);
}

TEST(Proxy, main) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8988);
    TcpClient client(&loop, addr, "test_proxy");
    client.setConnectionCallback(&onConnection);
    client.setMessageCallback(&onMessage);
    client.connect();
    loop.loop();
}

//int main(int argc, char* argv[]) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
