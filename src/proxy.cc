#include "boost/bind.hpp"
#include "glog/logging.h"

#include <string>

#include "connection.h"
#include "connection_manager.h"
#include "message_header.h"
#include "pb_request_parser.h"
#include "proxy.h"
#include "request.h"

namespace rrpc {

RpcProxy::RpcProxy(EventLoop* loop, int port) :
    loop_(loop), conn_manager_(new RpcConnectionManager()) {
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
        conn_manager_->Insert(rpc_conn);
    } else {
        LOG(INFO) << "Del Connection, " << conn->name();
        conn_manager_->Remove(conn_name);
    }
}

void RpcProxy::OnMessage(const TcpConnectionPtr &conn,
                         Buffer *buf,
                         Timestamp time) {
    std::string conn_name(conn->name().c_str());
    LOG(INFO) << "on message, conn_name: " << conn_name;
    RpcConnectionPtr rpc_conn = conn_manager_->GetByName(conn_name);
    if (!rpc_conn) {
        LOG(INFO) << "rpc_conn not found";
        return;
    }

    // append data
    MutexLock lock(&mutex_);
    muduo::string data = buf->retrieveAllAsString();
    rpc_conn->buff += data;
    parse_pool_.AddTask(boost::bind(&RpcProxy::ParseMessage, this, rpc_conn));
}

bool RpcProxy::Start() {
    tcp_server_->setConnectionCallback(
            boost::bind(&RpcProxy::OnConnection, this, _1));
    tcp_server_->setMessageCallback(
            boost::bind(&RpcProxy::OnMessage, this, _1, _2, _3));
    tcp_server_->start();
    return true;
}

void RpcProxy::ParseMessage(RpcConnectionPtr rpc_conn) {
    MutexLock lock(&mutex_);
    // deal with meta message
    RpcConnectionMetaPtr meta;
    if (!rpc_conn->checked) {
        int ret = rpc_conn->meta_parser->Parse();
        LOG(INFO) << "meta_parser: " << ret;
        switch (ret) {
            case -1:
                conn_manager_->Remove(rpc_conn->conn_name);
                break;
            case 1:
                meta = rpc_conn->meta_parser->GetMeta();
                // echo back
                rpc_conn->conn->send(meta.get(), RPC_CONNECTION_META_SIZE);
                // modify rpc_conn
                rpc_conn->conn_id = meta->conn_id;
                rpc_conn->conn_type = meta->conn_type;
                rpc_conn->checked = true;
                LOG(INFO) << "conn meta parsed ok"
                          << ", conn_id: " << rpc_conn->conn_id
                          << ", conn_type: " << rpc_conn->conn_type;

                // reset data
                rpc_conn->buff.clear();
                break;
            default:
                break;
        }
        return;
    }

    // deal with rpc message
    LOG(INFO) << "parse message";
    int ret = rpc_conn->request_parser->Parse();
    if (ret == 1) {
        RpcRequestPtr request = rpc_conn->request_parser->GetRequest();
        LOG(INFO) << "request_meta: " << request->meta.DebugString();
        LOG(INFO) << "request_data: " << request->data;
    }
}

}  // namespace rrpc
