#pragma once

#include <string>
#include "google/protobuf/service.h"

namespace rrpc {

class RrpcController : public google::protobuf::RpcController {
public:
    RrpcController() {};
    ~RrpcController() {};
    virtual void Reset() {}
    virtual bool Failed() const { return false; }
    virtual std::string ErrorText() const { return "ok"; }
    virtual void StartCancel() { return ; }
    void SetFailed(const std::string &reason) {}
    bool IsCanceled() const { return false; }
    void NotifyOnCancel(google::protobuf::Closure* callback) { return; }

private:
    bool is_failed;
    std::string reason;
};

}  // namespace rrpc

