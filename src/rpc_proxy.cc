#include "boost/bind.hpp"
#include "glog/logging.h"

#include "rpc_message_header.h"
#include "rpc_pb_request_parser.h"
#include "rpc_proxy.h"
#include "rpc_request.h"
#include "tcp_connection_meta.h"

namespace rrpc {

RpcProxy::RpcProxy(EventLoop* loop, int port) : loop_(loop) {
    InetAddress addr("0.0.0.0", static_cast<uint16_t>(port));
    boost::scoped_ptr<TcpServer> t_server(
            new TcpServer(loop_, addr, "rpc_proxy_server"));
    tcp_server_.swap(t_server);
}

void RpcProxy::OnConnection(const TcpConnectionPtr &conn) {
    LOG(INFO) << "OnConnection, " << conn->peerAddress().toIpPort()
              << ", " << conn->name();
    MutexLock lock(&conns_mutex_);
    unconns_[conn->name().c_str()] = conn;
}

void RpcProxy::OnMessage(const TcpConnectionPtr &conn,
                         Buffer *buf,
                         Timestamp time) {
    LOG(INFO) << "+++ message";
    std::string conn_name(conn->name().c_str());
    // conn is unauthorzied
    std::map<std::string, TcpConnectionPtr>::iterator uit = unconns_.find(conn_name);
    if (uit != unconns_.end()) {
        parser_pool.AddTask(
                boost::bind(&RpcProxy::OnMetaMessage, this, conn_name, buf));
    }
}

bool RpcProxy::Start() {
    tcp_server_->setConnectionCallback(boost::bind(&RpcProxy::OnConnection, this, _1));
    tcp_server_->setMessageCallback(boost::bind(&RpcProxy::OnMessage, this, _1, _2, _3));
    tcp_server_->start();
    return true;
}

void RpcProxy::OnMetaMessage(const std::string& conn_name, Buffer* buf) {
    MutexLock lock(&mutex_);
    if (buffs_.find(conn_name) == buffs_.end()) {
        buffs_[conn_name] = "";
    }
    std::map<std::string, std::string>::iterator bit = buffs_.find(conn_name);
    int n = buf->retrieveAllAsString().size();
    LOG(INFO) << "size_t: " << n;
    //bit->second += data;
    //LOG(INFO) << "buf_size: " << data.size();
    //if (bit->second.size() < TCP_CONNECTION_META_SIZE) {
    //    return;
    //}

    //char magic_str[5];
    //memcpy(magic_str, bit->second.c_str(), 4);
    //magic_str[4] = '\0';
    //uint32_t* a = reinterpret_cast<uint32_t*>(magic_str);
    //LOG(INFO) << "magic_str: " << magic_str;
}

}  // namespace rrpc
