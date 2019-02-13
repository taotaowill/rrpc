#pragma once

#include <string>

#include "glog/logging.h"
#include "common/timer.h"
#include "google/protobuf/service.h"

namespace rrpc {

class RpcController : public ::google::protobuf::RpcController {
public:
    RpcController() : is_failed_(false) {}
    ~RpcController() {}

    virtual void Reset() {}

    virtual bool Failed() const {
        return is_failed_;
    }

    virtual std::string ErrorText() const {
        return reason_;
    }

    virtual void StartCancel() {
        return;
    }

    void SetFailed(const std::string &reason) {
        is_failed_ = true;
        reason_ = reason;
    }

    bool IsCanceled() const {
        return false;
    }

    void NotifyOnCancel(::google::protobuf::Closure* callback) {
        return;
    }

    void SetTimeout(int32_t timeout) {
        timeout_ = ::baidu::common::timer::get_micros() + timeout * 1000000L;
    }

    bool IsTimeout() {
        LOG(INFO) << "timeout: " << timeout_
                  << ", nowtime: " << ::baidu::common::timer::get_micros();
        long now_time = ::baidu::common::timer::get_micros();
        return timeout_ < now_time;
    }

private:
    bool is_failed_;
    std::string reason_;
    long timeout_;
};

}  // namespace rrpc

