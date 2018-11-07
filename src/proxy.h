#ifndef RRPC_PROXY_H_
#define RRPC_PROXY_H_

#include <map>

#include "boost/smart_ptr.hpp"
#include "common/mutex.h"
#include "common/thread_pool.h"
#include "muduo/base/Timestamp.h"
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
    RpcProxy(EventLoop* loop, int port);
    ~RpcProxy() {};
    bool Start();

private:
    void OnConnection(const TcpConnectionPtr &conn);
    void OnMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);
    void ParseMessage(RpcConnectionPtr rpc_conn);
    // void OnClientConnection();
    // void OnServerConnection();
    // void OnClientMessage();
    // void OnServerMessage();

private:
    Mutex mutex_;
    EventLoop* loop_;
    ThreadPool parse_pool_;
    boost::scoped_ptr<TcpServer> tcp_server_;
    boost::scoped_ptr<RpcConnectionManager> conn_manager_;
};

}

#endif  // RRPC_PROXY_H_
