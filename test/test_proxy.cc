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

#include "rrpc.h"

using namespace muduo;
using namespace muduo::net;
using namespace rrpc;

void onConnection(const TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        LOG(WARNING) << "connection closed by peer";
        exit(-1);
        return;
    }

    RpcConnectionMeta meta;
    meta.conn_type = RPC_CONNECTION_TYPE_CLIENT;
    meta.conn_id = 100045;
    meta.crc = 1;

    char* data = new char[16];
    memset(data, 0, sizeof(16));
    memcpy(data, &meta, sizeof(meta));
    conn->send(reinterpret_cast<void*>(data), 16);
    LOG(INFO) << "EchoClient conn meta sended to proxy";
}

void onMessage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG(INFO) << conn->name() << " echo " << msg.size() << " bytes, "
              << "data received at " << time.toString();
    if (msg.size() != 16) {
        return;
    }

    RpcRequest request;
    RpcMeta* meta = new RpcMeta();
    meta->set_sequence_id(123);
    meta->set_method("rrpc::hello::Service::GetAgent");
    request.meta.CopyFrom(*meta);
    request.data = "1234567890";
    request.src_id = 1000000001;
    request.dst_id = 100045;
    uint32_t size;
    void* send_buff = request.Packaging(size);
    LOG(INFO) << "message request size: " << size;
    conn->send(send_buff, size);
    LOG(INFO) << "hhhhhh";
    free(send_buff);
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
    rrpc::SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
