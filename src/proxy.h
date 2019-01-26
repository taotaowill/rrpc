#pragma once

#include <map>

#include "boost/smart_ptr.hpp"
#include "common/mutex.h"
#include "common/thread_pool.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/net/TcpServer.h"

#include "connection.h"
#include "connection_manager.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;
using baidu::common::ThreadPool;
using muduo::Timestamp;
using muduo::net::Buffer;
using muduo::net::EventLoop;
using muduo::net::InetAddress;
using muduo::net::TcpServer;
using muduo::net::TcpConnectionPtr;

class RpcProxy {
public:
    RpcProxy(std::string ip, int port);
    ~RpcProxy() {};
    bool Start();
    void Stop();

private:
    void StartLoop();
    void OnConnection(const TcpConnectionPtr &conn);
    void OnMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);
    void ParseMessage(RpcConnectionPtr rpc_conn);
    void ProcessMessage(RpcMessagePtr message);

private:
    Mutex mutex_;
    std::string ip_;
    int32_t port_;
    int32_t rpc_id_;

    ThreadPool loop_pool_;
    ThreadPool parse_pool_;
    ThreadPool dispatch_pool_;
    boost::scoped_ptr<TcpServer> tcp_server_;
    boost::scoped_ptr<RpcConnectionManager> conn_manager_;
};

}
