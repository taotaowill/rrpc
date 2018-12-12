#ifndef RRPC_PB_SERVER_H_
#define RRPC_PB_SERVER_H_

#include "common/mutex.h"
#include "server.h"

namespace rrpc {

using baidu::common::Mutex;

class RpcPbServer : public RpcServer {
public:
    RpcPbServer(std::string proxy_endpoint);
    ~RpcPbServer() {};
    bool Start();
    void Run();
    void Stop();
    bool RegisterService(google::protobuf::Service* service);

private:
    Mutex mutex_;
    std::string proxy_endpoint_;
    bool stop_;
};

}  // namespace rrpc

#endif  // RRPC_PB_SERVER_H_
