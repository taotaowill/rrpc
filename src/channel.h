#pragma once

#include <map>
#include <string>

#include "common/mutex.h"
#include "common/thread_pool.h"
#include "google/protobuf/service.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"

#include "connection.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;
using baidu::common::ThreadPool;
using namespace muduo;
using namespace muduo::net;

class RpcClient;

class RpcChannel : public google::protobuf::RpcChannel {
public:
    RpcChannel(std::string proxy_ip,
               int32_t proxy_port,
               int32_t rpc_src_id,
               int32_t rpc_dst_id,
               RpcClient* rcp_client);
    ~RpcChannel() {}
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done);
    bool IsConnected() { return connected_; }

private:
    void StartLoop();
    void OnConnection(const TcpConnectionPtr &conn);
    void OnMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);
    void ParseMessage();
    void ProcessMessage(RpcMessagePtr message);
    int32_t GetSequenceId() {
        MutexLock lock(&mutex_);
        return sequence_id_++;
    }

private:
    Mutex mutex_;
    Mutex sequence_id_mutex_;
    int32_t sequence_id_;
    int32_t rpc_src_id_;
    int32_t rpc_dst_id_;
    ThreadPool loop_pool_;
    ThreadPool parse_pool_;
    ThreadPool process_pool_;
    InetAddress rpc_proxy_;
    RpcClient* rpc_client_;
    RpcConnectionPtr rpc_conn_;
    bool connected_;
    std::map<int32_t, const::google::protobuf::MethodDescriptor*> requests_;
    std::map<int32_t, google::protobuf::Message*> responses_;
};

}  // namespace rrpc
