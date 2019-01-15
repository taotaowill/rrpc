#include "pb_server.h"

#include "boost/bind.hpp"

#include "rrpc.h"

namespace rrpc {

RpcPbServer::RpcPbServer(std::string proxy_ip, int32_t proxy_port) :
    proxy_(proxy_ip, proxy_port),
    loop_pool_(1),
    rpc_conn_(new RpcConnection()) {
}

void RpcPbServer::StartLoop() {
    EventLoop loop;
    TcpClient client(&loop, proxy_, "pb_server");
    client.setConnectionCallback(
          boost::bind(&RpcPbServer::OnConnection, this, _1));
    client.setMessageCallback(
          boost::bind(&RpcPbServer::OnMessage, this, _1, _2, _3));
    client.connect();
    loop.loop();
}

bool RpcPbServer::Start() {
    loop_pool_.AddTask(boost::bind(&RpcPbServer::StartLoop, this));
}

void RpcPbServer::Stop() {
    loop_pool_.Stop(false);
}

void RpcPbServer::OnConnection(const TcpConnectionPtr &conn) {
    std::string conn_name(conn->name().c_str());
    // lost connection
    if (!conn->connected()) {
      LOG(ERROR) << "========== lost connection to proxy";
      // TODO reconnect
    }

    // send meta
    rpc_conn_->conn_name = conn_name;
    rpc_conn_->conn = conn;
    rpc_conn_->buff = "";
    RpcConnectionMeta meta;
    meta.conn_type = RPC_CONNECTION_TYPE_SERVER;
    meta.conn_id = 100045;
    meta.crc = 1;
    size_t size = sizeof(meta);
    char* data = new char[size];
    memset(data, 0, size);
    memcpy(data, &meta, sizeof(meta));
    conn->send(reinterpret_cast<void*>(data), size);
}

void RpcPbServer::OnMessage(const TcpConnectionPtr &conn,
                            Buffer *buf,
                            Timestamp time) {
    std::string conn_name(conn->name().c_str());
    LOG(INFO) << "on message, conn_name: " << conn_name;

    // append data
    MutexLock lock(&mutex_);
    muduo::string data = buf->retrieveAllAsString();
    rpc_conn_->buff += data;
    parse_pool_.AddTask(
        boost::bind(&RpcPbServer::ParseMessage, this));
}

void RpcPbServer::ParseMessage() {
    MutexLock lock(&mutex_);
    // deal with meta message
    RpcConnectionMetaPtr meta;

    // deal with rpc message
    LOG(INFO) << "parse message";
    int ret = rpc_conn_->message_parser->Parse();
    if (ret == 1) {
        RpcMessagePtr message = rpc_conn_->message_parser->GetMessage();
        dispatch_pool_.AddTask(
              boost::bind(&RpcPbServer::ProcessRequest, this, message));
    }
}

void RpcPbServer::ProcessRequest(RpcMessagePtr message) {
    LOG(INFO) << "=========== start process request";
    LOG(INFO) << "message_meta: " << message->meta.DebugString();
    LOG(INFO) << "message_data: " << message->data;
}

}  // namespace rrpc