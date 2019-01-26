#pragma once

#include <string>

#include "google/protobuf/service.h"

namespace rrpc {

class RpcController : public google::protobuf::RpcController {
public:
    RpcController() {};
    ~RpcController() {};
    virtual void Reset() {}
    virtual bool Failed() const { return is_failed_; }
    virtual std::string ErrorText() const { return "ok"; }
    virtual void StartCancel() { return ; }
    void SetFailed(const std::string &reason) {}
    bool IsCanceled() const { return false; }
    void NotifyOnCancel(google::protobuf::Closure* callback) { return; }
    void SetTimeout(int32_t timeout) { timeout_ = timeout; }
    int32_t GetTimeout() {
        return timeout_;
    }

private:
    bool is_failed_;
    std::string reason_;
    int32_t timeout_;
};

}  // namespace rrpc

