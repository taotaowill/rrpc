#include "boost/bind.hpp"
#include "glog/logging.h"

#include "connection.h"
#include "connection_manager.h"
#include "message.h"
#include "pb_message_parser.h"
#include "proxy.h"

namespace rrpc {

RpcProxy::RpcProxy(std::string ip, int port) :
    ip_(ip),
    port_(port),
    conn_manager_(new RpcConnectionManager()) {
}

void RpcProxy::OnConnection(const TcpConnectionPtr &conn) {
    std::string conn_name(conn->name().c_str());
    if (conn->connected()) {
        LOG(INFO) << "on connection, " << conn->name();
        RpcConnectionPtr rpc_conn(new RpcConnection());
        rpc_conn->conn_name = conn_name;
        rpc_conn->conn = conn;
        conn_manager_->Insert(rpc_conn);
    } else {
        LOG(INFO) << "!!!on lost connection, " << conn->name();
        conn_manager_->Remove(conn_name);
    }
}

void RpcProxy::OnMessage(const TcpConnectionPtr &conn,
                         Buffer *buf,
                         Timestamp time) {
    std::string conn_name(conn->name().c_str());
    LOG(INFO) << "on message, conn_name: " << conn_name
              << ", time: " << time.toString();
    RpcConnectionPtr rpc_conn = conn_manager_->Get(conn_name);
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
    loop_pool_.AddTask(boost::bind(&RpcProxy::StartLoop, this));
    return true;
}

void RpcProxy::StartLoop() {
    InetAddress addr(ip_, static_cast<uint16_t>(port_));
    EventLoop loop;
    boost::scoped_ptr<TcpServer> t_server(
            new TcpServer(&loop, addr, "proxy"));
    tcp_server_.swap(t_server);
    tcp_server_->setConnectionCallback(
            boost::bind(&RpcProxy::OnConnection, this, _1));
    tcp_server_->setMessageCallback(
            boost::bind(&RpcProxy::OnMessage, this, _1, _2, _3));
    tcp_server_->start();
    loop.loop();
}

void RpcProxy::Stop() {
    loop_pool_.Stop(false);
}

void RpcProxy::ParseMessage(RpcConnectionPtr rpc_conn) {
    MutexLock lock(&mutex_);
    // deal with meta message
    RpcConnectionMetaPtr meta;
    if (!rpc_conn->checked) {
        int ret = rpc_conn->meta_parser->Parse();
        // LOG(INFO) << "meta_parser: " << ret;
        switch (ret) {
        case -1:
            conn_manager_->Remove(rpc_conn->conn_name);
            break;
        case 1:
            meta = rpc_conn->meta_parser->GetMeta();
            LOG(INFO) << "conn meta parsed ok"
                      << ", conn_id: " << meta->conn_id;
            // echo back to client
            if (meta->conn_id < 0) {
                LOG(INFO) << "### client connection, and echo meta back";
                rpc_conn->conn->send(meta.get(), RPC_CONNECTION_META_SIZE);
            } else {
                LOG(INFO) << "### server connection";
            }

            // modify rpc_conn
            rpc_conn->conn_id = meta->conn_id;
            rpc_conn->checked = true;

            // reset data
            rpc_conn->buff.clear();
            break;
        default:
            break;
        }
        return;
    }

    // deal with rpc message
    // LOG(INFO) << "parse message";
    int ret = rpc_conn->message_parser->Parse();
    LOG(INFO) << "message parse ret: " << ret;
    if (ret == 1) {
        RpcMessagePtr message = rpc_conn->message_parser->GetMessage();
        while (message) {
            process_pool_.AddTask(
                boost::bind(&RpcProxy::ProcessMessage, this, message));
            message = rpc_conn->message_parser->GetMessage();
        }
    }
}

void RpcProxy::ProcessMessage(RpcMessagePtr message) {
    MutexLock lock(&mutex_);
    RpcConnectionPtr rpc_conn = conn_manager_->Get(message->dst_id);
    if (rpc_conn) {
        uint32_t size;
        void* send_buff = message->Packaging(size);
        rpc_conn->conn->send(send_buff, size);
        LOG(INFO) << "send to dst ok, dst_id: " << message->dst_id
                  << ", data_size: " << size;
    } else {
        LOG(WARNING) << "dst_id is not exit, dst_id: " << message->dst_id;
    }
}

}  // namespace rrpc
