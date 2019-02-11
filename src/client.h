#pragma once

#include <map>
#include <string>

#include "boost/function.hpp"
#include "common/mutex.h"
#include "common/thread_pool.h"
#include "glog/logging.h"


#include "channel.h"
#include "closure.h"
#include "controller.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;
using baidu::common::ThreadPool;
using namespace muduo;
using namespace muduo::net;

class RpcClient {
public:
    RpcClient(std::string proxy_ip,
              int32_t proxy_port,
              int32_t rpc_id);
    virtual ~RpcClient();

    template <class T>
    bool GetStub(T** stub, int32_t rpc_id) {
        MutexLock lock(&mutex_);
        if (channels_.find(rpc_id) == channels_.end()) {
            RpcChannel* channel = new RpcChannel(
                    proxy_ip_, proxy_port_, rpc_id_, rpc_id, this);
            channels_[rpc_id] = channel;
        }
        *stub = new T(channels_[rpc_id]);
        return true;
    }

    template <class Stub, class Request, class Response, class Callback>
    bool SendRequest(
            Stub* stub,
            void(Stub::*func) (::google::protobuf::RpcController*,
                               const Request*, Response*, Callback*),
            const Request* request,
            Response* response,
            int32_t rpc_timeout,
            int retry_times) {
        RpcController controller;
        controller.SetTimeout(rpc_timeout);
        for (int32_t retry = 0; retry < retry_times; ++retry) {
            (stub->*func)(&controller, request, response, NULL);
            LOG(INFO) << "SendRequest after call";
            if (!controller.Failed()) {
                LOG(INFO) << "SendRequest success, response: "
                          << response->DebugString();
                return true;
            }

           if (retry < retry_times - 1) {
               LOG(INFO) << "Send failed, retry ...";
               usleep(100000);
           } else {
               LOG(WARNING) <<  "SendRequest fail: "
                            << controller.ErrorText().c_str();
           }

            controller.Reset();
        }

        LOG(WARNING) << "SendRequest failed: " << controller.ErrorText();
        return false;
    }


    template <class Stub, class Request, class Response, class Callback>
    void AsyncRequest(
            Stub* stub,
            void(Stub::*func) (::google::protobuf::RpcController*,
                               const Request*, Response*, Callback*),
            const Request* request, Response* response,
            boost::function<void (const Request*, Response*, bool, int)> callback,
            int32_t rpc_timeout, int /*retry_times*/) {
        RpcController* controller = new RpcController();
        ::google::protobuf::Closure* done = NewClosure(
                &RpcClient::template RpcCallback<Request, Response, Callback>,
                controller, request, response, callback);
        (stub->*func)(controller, request, response, done);
    }

    template <class Request, class Response, class Callback>
    static void RpcCallback(
            RpcController* rpc_controller,
            const Request* request,
            Response* response,
            boost::function<void (const Request*, Response*, bool, int)> callback) {
        bool failed = rpc_controller->Failed();
        if (failed) {
            LOG(WARNING) << "RpcCallback: "
                         << rpc_controller->ErrorText().c_str();
        }

        delete rpc_controller;
        callback(request, response, failed, 1);
    }

private:
    void StartLoop();

private:
    Mutex mutex_;
    std::string proxy_ip_;
    int32_t proxy_port_;
    int32_t rpc_id_;
    std::map<int32_t, RpcChannel*> channels_;
};

}  // namespace rrpc
