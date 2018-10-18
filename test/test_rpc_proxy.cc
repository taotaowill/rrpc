#include <unistd.h>

#include "boost/bind.hpp"
#include "boost/smart_ptr.hpp"
#include "common/thread.h"
#include "gtest/gtest.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpConnection.h"

#include "log_setting.h"
#include "tcp_connection_meta.h"

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr &conn) {
    rrpc::TcpConnectionMeta meta;
    LOG(INFO) << sizeof(meta);
    meta.conn_type = 1;
    meta.crc = 1;
    conn->send((void*)&meta, sizeof(meta));
    LOG(INFO) << "EchoClient connected";
}

void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG(INFO) << conn->name() << " echo " << msg.size() << " bytes, "
              << "data received at " << time.toString();
}

TEST(Proxy, main) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8988);
    TcpClient client(&loop, addr, "0_1_2_3");
    client.setConnectionCallback(&onConnection);
    client.setMessageCallback(&onMessage);
    client.connect();
    loop.loop();
}

int main(int argc, char* argv[]) {
    SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
