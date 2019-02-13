#pragma once

#include <map>
#include <pthread.h>
#include <string>

#include "common/mutex.h"
#include "common/thread_pool.h"
#include "google/protobuf/service.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"

#include "controller.h"
#include "connection.h"

const int32_t SEND_BUFF_MAX_SIZE = 10 * 1024 * 1024; // 10M

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;
using baidu::common::ThreadPool;

class RpcClient;

class RpcChannel : public ::google::protobuf::RpcChannel {
public:
    RpcChannel(std::string proxy_ip,
               int32_t proxy_port,
               int32_t rpc_src_id,
               int32_t rpc_dst_id,
               RpcClient* rcp_client);
    ~RpcChannel();
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done);
    bool IsConnected() { return rpc_conn_->checked; }

private:
    void StartLoop();
    void OnConnection(const ::muduo::net::TcpConnectionPtr &conn);
    void OnMessage(const ::muduo::net::TcpConnectionPtr &conn,
                   ::muduo::net::Buffer *buf,
                   ::muduo::Timestamp time);
    void ParseMessage();
    void ProcessMessage(RpcMessagePtr message);
    int32_t GetSequenceId() {
        return sequence_id_++;
    }
    void ProcessCallback(int32_t sequence_id);

private:
    Mutex mutex_;
    Mutex sequence_id_mutex_;
    int32_t sequence_id_;
    int32_t rpc_src_id_;
    int32_t rpc_dst_id_;
    void* send_buff_;

    ThreadPool loop_pool_;
    ThreadPool parse_pool_;
    ThreadPool process_pool_;
    ThreadPool callback_pool_;
    ::muduo::net::InetAddress rpc_proxy_;
    RpcClient* rpc_client_;
    RpcConnectionPtr rpc_conn_;
    std::map<int32_t, const ::google::protobuf::MethodDescriptor*> requests_;
    std::map<int32_t, ::google::protobuf::Message*> responses_;
    std::map<int32_t, std::pair<RpcController*, ::google::protobuf::Closure*> > closures_;
};

}  // namespace rrpc
