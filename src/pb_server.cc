#include "boost/bind.hpp"

#include "message.h"
#include "pb_server.h"

namespace rrpc {

RpcPbServer::RpcPbServer(std::string proxy_ip,
                         int32_t proxy_port,
                         int32_t rpc_id) :
        loop_pool_(1),
        rpc_proxy_(proxy_ip, proxy_port),
        rpc_conn_(new RpcConnection()),
        rpc_id_(rpc_id) {
//    LOG(INFO) << "RpcPbServer rpc_id: " << rpc_id;
}

void RpcPbServer::StartLoop() {
    EventLoop loop;
    TcpClient client(&loop, rpc_proxy_, "pb_server");
    client.setConnectionCallback(
          boost::bind(&RpcPbServer::OnConnection, this, _1));
    client.setMessageCallback(
          boost::bind(&RpcPbServer::OnMessage, this, _1, _2, _3));
    client.connect();
    loop.loop();
}

bool RpcPbServer::Start() {
    loop_pool_.AddTask(boost::bind(&RpcPbServer::StartLoop, this));
//    LOG(INFO) << "Start() end";
}

void RpcPbServer::Stop() {
    loop_pool_.Stop(false);
}

void RpcPbServer::OnConnection(const TcpConnectionPtr &conn) {
    std::string conn_name(conn->name().c_str());
    // lost connection
    if (!conn->connected()) {
//      LOG(ERROR) << "========== lost connection to proxy ============";
      // TODO reconnect
    }

    rpc_conn_->conn_name = conn_name;
    conn->setTcpNoDelay(true);
    rpc_conn_->conn = conn;
    rpc_conn_->buff = "";
    RpcConnectionMeta meta;
    meta.conn_id = rpc_id_;
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
//    LOG(INFO) << "on message, conn_name: " << conn_name;

    // append data
    muduo::string data = buf->retrieveAllAsString();
//    LOG(INFO) << "message data_size: " << data.size();
    {
        MutexLock lock(&mutex_);
        rpc_conn_->buff += data;
    }
    parse_pool_.AddTask(
        boost::bind(&RpcPbServer::ParseMessage, this));;
//    LOG(INFO) << "OnMessage end";
}

void RpcPbServer::ParseMessage() {
    // deal with meta message
    RpcConnectionMetaPtr meta;

    // deal with rpc message
    int ret = rpc_conn_->message_parser->Parse();
//    LOG(INFO) << "message_parser->Parse(): " << ret;
    if (ret == 1) {
        RpcMessagePtr message = rpc_conn_->message_parser->GetMessage();
        while (message) {
            process_pool_.AddTask(
                  boost::bind(&RpcPbServer::ProcessMessage, this, message));
            message = rpc_conn_->message_parser->GetMessage();
        }
    }
}

void RpcPbServer::ProcessMessage(RpcMessagePtr message) {
    MutexLock lock(&mutex_);
//    LOG(INFO) << "message_meta: " << message->meta.DebugString();
//    LOG(INFO) << "message_data: " << message->data;
    std::string method_name = message->meta.method();
    ServiceContext* service = RetrieveService(method_name);
    if (service != NULL) {
        const ::google::protobuf::MethodDescriptor* method = service->GetMethod(method_name);
        if (method != NULL) {
            ::google::protobuf::Message* request = \
                    service->GetService()->GetRequestPrototype(method).New();
            std::string data(message->data.c_str(), message->data.size());
            request->ParseFromString(data);
            ::google::protobuf::Message* response = \
                    service->GetService()->GetResponsePrototype(method).New();
            service->GetService()->CallMethod(
                    method, NULL, request, response, NULL);

//            LOG(INFO) << "------ response " << response->DebugString();

            // send response to proxy
            RpcMessage ret_message;
            ret_message.src_id = message->dst_id;
            ret_message.dst_id = message->src_id;
            ret_message.meta.CopyFrom(message->meta);
            std::string buff;
            response->SerializeToString(&buff);
            ret_message.data = buff.c_str();
            uint32_t size;
            void* send_buff = ret_message.Packaging(size);
            rpc_conn_->conn->send(send_buff, size);
//            LOG(INFO) << "response send to proxy ok";
        }
    }
}

}  // namespace rrpc
