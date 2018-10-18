#ifndef RRPC_RPC_PROXY_H_
#define RRPC_RPC_PROXY_H_

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
    void OnMetaMessage(const std::string& conn_name, Buffer* buf);
    // void OnClientConnection();
    // void OnServerConnection();
    // void OnClientMessage();
    // void OnServerMessage();

private:
    Mutex mutex_;
    Mutex conns_mutex_;
    EventLoop* loop_;
    ThreadPool parser_pool;
    boost::scoped_ptr<TcpServer> tcp_server_;
    std::map<std::string, TcpConnectionPtr> unconns_;
    std::map<std::string, TcpConnectionPtr> clients_;
    std::map<std::string, TcpConnectionPtr> servers_;
    std::map<std::string, std::string> buffs_;
};

}

#endif  // RRPC_RPC_PROXY_H_
