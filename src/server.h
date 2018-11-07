#ifndef RRPC_SERVER_H_
#define RRPC_SERVER_H_

namespace rrpc {

class RpcServer {
public:
    RpcServer();
    virtual ~RpcServer();
    virtual bool RgisterService();
};

}  // namespace rrpc

#endif  // RRPC_SERVER_H_
