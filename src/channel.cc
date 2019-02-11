#include <string>

#include "boost/bind.hpp"
#include "common/timer.h"
#include "glog/logging.h"
#include "google/protobuf/descriptor.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpClient.h"

#include "channel.h"
#include "controller.h"

namespace rrpc {

RpcChannel::RpcChannel(std::string proxy_ip,
                       int32_t proxy_port,
                       int32_t rpc_src_id,
                       int32_t rpc_dst_id,
                       RpcClient* rpc_client) :
        sequence_id_(0),
        rpc_src_id_(rpc_src_id),
        rpc_dst_id_(rpc_dst_id),
        callback_pool_(1),
        rpc_proxy_(proxy_ip, proxy_port),
        rpc_client_(rpc_client),
        rpc_conn_(new RpcConnection()),
        connected_(false) {
    send_buff_ = malloc(SEND_BUFF_MAX_SIZE);
    loop_pool_.AddTask(boost::bind(&RpcChannel::StartLoop, this));
}

RpcChannel::~RpcChannel() {
    free(send_buff_);
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done) {
    RpcMessage message;
    RpcMeta meta;
    int32_t sequence_id = GetSequenceId();
    meta.set_sequence_id(sequence_id);
    meta.set_method(method->full_name());
    message.meta.CopyFrom(meta);
    std::string buff;
    request->SerializeToString(&buff);
    message.data = buff.c_str();
    message.src_id = rpc_src_id_;
    message.dst_id = rpc_dst_id_;
    uint32_t size;
    void* send_buff = message.Packaging(size);
    RpcController *rpc_controller = dynamic_cast<RpcController*>(controller);
        rpc_conn_->conn->send(send_buff, size);
        LOG(INFO) << "rpc_channel send message to proxy";
        requests_[sequence_id] = method;
        //free(send_buff);
        if (done != NULL) {
            closures_[sequence_id] = std::make_pair(rpc_controller, done);
        }

    // sync request
    if (NULL == done) {
        while (!dynamic_cast<RpcController*>(controller)->IsTimeout()) {
            {
                MutexLock lock(&mutex_);
                std::map<int32_t, ::google::protobuf::Message*>::iterator it = \
                        responses_.find(sequence_id);
                if (it != responses_.end()) {
                    response->CopyFrom(*(it->second));
                    responses_.erase(sequence_id);
                    return;
                }
            }
            usleep(10000);
        }
        controller->SetFailed("timeout");
        return;
    } else {
        responses_[sequence_id] = response;
    }
}

void RpcChannel::StartLoop() {
    ::muduo::net::EventLoop loop;
    ::muduo::net::TcpClient tcp_client(&loop, rpc_proxy_, "pb_client");
    tcp_client.setConnectionCallback(
            boost::bind(&RpcChannel::OnConnection, this, _1));
    tcp_client.setMessageCallback(
            boost::bind(&RpcChannel::OnMessage, this, _1, _2, _3));
    tcp_client.connect();
    loop.loop();
}

void RpcChannel::OnConnection(const TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        LOG(WARNING) << "connection closed by peer";
        connected_ = false;
        // TODO retry
        return;
    }

    // send conn meta
    rpc_conn_->conn = conn;
    RpcConnectionMeta meta;
    meta.conn_id = rpc_src_id_;
    meta.crc = 1;
    size_t size = sizeof(meta);
    char* data = new char[size];
    memset(data, 0, size);
    memcpy(data, &meta, size);
    conn->send(reinterpret_cast<void*>(data), size);
    LOG(INFO) << "RpcClient conn meta sended to proxy";
}

void RpcChannel::OnMessage(const ::muduo::net::TcpConnectionPtr &conn,
                           ::muduo::net::Buffer *buf,
                           ::muduo::Timestamp time) {
    MutexLock lock(&mutex_);
    ::muduo::string data = buf->retrieveAllAsString();
    rpc_conn_->buff += data;
    parse_pool_.AddTask(boost::bind(&RpcChannel::ParseMessage, this));
}

void RpcChannel::ParseMessage() {
    MutexLock lock(&mutex_);
    LOG(INFO) << "parse message";
    // deal with meta message
    if (!rpc_conn_->checked) {
        int ret = rpc_conn_->meta_parser->Parse();
        LOG(INFO) << "meta_parser: " << ret;
        switch (ret) {
        case -1:
            break;
        case 1:
            connected_ = true;
            rpc_conn_->checked = true;
            LOG(INFO) << "conn meta parsed ok";

            // reset data
            rpc_conn_->buff.clear();
            break;
        default:
            break;
        }

        return;
    }

    // deal with rpc message
    LOG(INFO) << "parse with rpc message";
    int ret = rpc_conn_->message_parser->Parse();
    if (ret == 1) {
        RpcMessagePtr message = rpc_conn_->message_parser->GetMessage();
        process_pool_.AddTask(
            boost::bind(&RpcChannel::ProcessMessage, this, message));
    }
}

void RpcChannel::ProcessMessage(RpcMessagePtr message) {
    MutexLock lock(&mutex_);
    int32_t sequence_id = message->meta.sequence_id();
    std::map<int32_t, const ::google::protobuf::MethodDescriptor*>::iterator it = \
            requests_.find(sequence_id);
    if (it == requests_.end()) {
        return;
    }

    ::google::protobuf::Message* response = NULL;
    const ::google::protobuf::MethodDescriptor* method = it->second;
    const ::google::protobuf::Message* prototype =
        ::google::protobuf::MessageFactory::generated_factory()->GetPrototype(
                method->output_type());
    if (prototype) {
        response = prototype->New();
    }

    std::string data(message->data.c_str(), message->data.size());
    response->ParseFromString(data);
    //LOG(INFO) << "response debug_string: " << response->DebugString();
    // remove from requests_
    requests_.erase(sequence_id);
    if (closures_.find(sequence_id) == closures_.end()) {
        responses_[sequence_id] = response;
    } else {
        responses_[sequence_id]->CopyFrom(*response);
        callback_pool_.AddTask(
                boost::bind(&RpcChannel::ProcessCallback, this, sequence_id));
    }
}

void RpcChannel::ProcessCallback(
        int32_t sequence_id) {
    LOG(INFO) << "++++ ProcessCallback, id: " << sequence_id;
    ::google::protobuf::Closure* done = closures_[sequence_id].second;
    RpcController* controller = closures_[sequence_id].first;
    if (controller->IsTimeout()) {
        controller->SetFailed("timeout");
    }

    done->Run();
}

}  // namespace rrpc
