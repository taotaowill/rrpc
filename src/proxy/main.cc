#include <map>

#include "muduo/base/Logging.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpConnection.h"

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
                     << "data received at " << time.toString();
    conn->send(msg);
}

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8988);
    TcpServer server(&loop, addr, "echo_server");
    server.setConnectionCallback(&onConnection);
    server.setMessageCallback(&onMessage);
    server.start();
    loop.loop();

    return 0;
}
