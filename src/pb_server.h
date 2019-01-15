#pragma once

#include "server.h"

#include "common/mutex.h"
#include "common/thread_pool.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpConnection.h"

#include "rrpc.h"
#include "connection.h"
#include "connection_manager.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::ThreadPool;
using namespace muduo;
using namespace muduo::net;

class RpcPbServer : public RpcServer {
public:
    RpcPbServer(std::string proxy_ip, int32_t proxy_port);
    ~RpcPbServer() {};
    bool Start();
    void Stop();
    bool RegisterService(google::protobuf::Service* service);

private:
    void StartLoop();
    void OnConnection(const TcpConnectionPtr &conn);
    void OnMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);
    void ParseMessage();
    void ProcessRequest(RpcMessagePtr message);

private:
    Mutex mutex_;
    InetAddress proxy_;
    ThreadPool loop_pool_;
    ThreadPool parse_pool_;
    ThreadPool dispatch_pool_;
    RpcConnectionPtr rpc_conn_;
};

}  // namespace rrpc
