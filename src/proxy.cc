#include "boost/bind.hpp"
#include "glog/logging.h"

#include "connection.h"
#include "connection_manager.h"
#include "message_header.h"
#include "pb_request_parser.h"
#include "proxy.h"
#include "request.h"

namespace rrpc {

RpcProxy::RpcProxy(EventLoop* loop, int port) :
    loop_(loop), conn_manager(new RpcConnectionManager()) {
    InetAddress addr("0.0.0.0", static_cast<uint16_t>(port));
    boost::scoped_ptr<TcpServer> t_server(
            new TcpServer(loop_, addr, "rpc_proxy_server"));
    tcp_server_.swap(t_server);
}

void RpcProxy::OnConnection(const TcpConnectionPtr &conn) {
    std::string conn_name(conn->name().c_str());
    if (conn->connected()) {
        LOG(INFO) << "New Connection, " << conn->name();
        RpcConnectionPtr rpc_conn(new RpcConnection());
        rpc_conn->conn_name = conn_name;
        rpc_conn->conn = conn;
        conn_manager->Insert(rpc_conn);
    } else {
        LOG(INFO) << "Del Connection, " << conn->name();
        conn_manager->Remove(conn_name);
    }
}

void RpcProxy::OnMessage(const TcpConnectionPtr &conn,
                         Buffer *buf,
                         Timestamp time) {
    std::string conn_name(conn->name().c_str());
    LOG(INFO) << "on message, conn_name: " << conn_name;
    RpcConnectionPtr rpc_conn = conn_manager->GetByName(conn_name);
    if (!rpc_conn) {
        LOG(INFO) << "rpc_conn not found";
        return;
    }

    if (!rpc_conn->checked) {
        parser_pool.AddTask(
                boost::bind(&RpcProxy::OnMetaMessage, this, rpc_conn, buf));
    } else {
        parser_pool.AddTask(
                boost::bind(&RpcProxy::OnRpcMessage, this, rpc_conn, buf));
    }
}

bool RpcProxy::Start() {
    tcp_server_->setConnectionCallback(boost::bind(&RpcProxy::OnConnection, this, _1));
    tcp_server_->setMessageCallback(boost::bind(&RpcProxy::OnMessage, this, _1, _2, _3));
    tcp_server_->start();
    return true;
}

void RpcProxy::OnMetaMessage(RpcConnectionPtr rpc_conn,
                             Buffer* buf) {
    MutexLock lock(&mutex_);
    muduo::string buf_str = buf->retrieveAllAsString();
    muduo::string& data = rpc_conn->buff;
    data += buf_str;
    size_t size = data.size();
    if (size < RPC_CONNECTION_META_SIZE) {
        LOG(INFO) << "meta message size not ready: " << size;
        return;
    }

    const RpcConnectionMeta* meta = reinterpret_cast<const RpcConnectionMeta*>(data.c_str());
    if (!meta->Check()) {
        LOG(WARNING) << "conn meta check fail";
        conn_manager->Remove(rpc_conn->conn_name);
        return;
    }

    // cut parsed buffer
    data = data.substr(RPC_CONNECTION_META_SIZE);
    LOG(INFO) << "left data size: " << data.size();
    //if (meta->conn_type == RPC_CONNECTION_TYPE_CLIENT ) {
    //} else {
    //}
}

void RpcProxy::OnRpcMessage(RpcConnectionPtr rpc_conn,
                            Buffer* buf) {
}

}  // namespace rrpc
