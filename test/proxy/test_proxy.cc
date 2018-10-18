#include <gtest/gtest.h>
#include <unistd.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>

#include "boost/bind.hpp"
#include "boost/smart_ptr.hpp"
#include "common/thread.h"

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << "EchoClient connected";
}

void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
                     << "data received at " << time.toString();
}

void loop_func(boost::shared_ptr<EventLoop> loop) {
    loop->loop();
}

TEST(Proxy, main) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8988);
    TcpClient client(&loop, addr, "test_echo_client");
    client.setConnectionCallback(&onConnection);
    client.setMessageCallback(&onMessage);
    client.connect();
    baidu::common::Thread t;
    boost::shared_ptr<EventLoop> loop_ptr(&loop);
    t.Start(boost::bind(&loop_func, loop_ptr));
    LOG_INFO << "exit";
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
