#pragma once

#include <assert.h>
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "common/mutex.h"
#include "common/thread_pool.h"
#include "glog/logging.h"

#include "channel.h"
#include "controller.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;

class RpcClient {
public:
    RpcClient() {};
    virtual ~RpcClient() {}

    template <class T>
    bool GetStub(T** stub) {
        MutexLock lock(&mutex_);
        RrpcChannel* channel = new RrpcChannel();
        *stub = new T(channel);
        return true;
    }

    template <class Stub, class Request, class Response, class Callback>
    bool SendRequest(Stub* stub,
                     void(Stub::*func) (google::protobuf::RpcController*,
                                        const Request*, Response*, Callback*),
                     const Request* request,
                     Response* response,
                     int32_t rpc_timeout,
                     int retry_times) {
        RrpcController controller;
        for (int32_t retry = 0; retry < retry_times; ++retry) {
            (stub->*func)(&controller, request, response, NULL);
            if (!controller.Failed()) {
                return true;
            }

            if (retry < retry_times - 1) {
                LOG(INFO) << "Send failed, retry ...";
                usleep(1000000);
            } else {
                LOG(WARNING) <<  "SendRequest fail: " << controller.ErrorText().c_str();
            }

            controller.Reset();
        }

        return false;
    }

    template <class Stub, class Request, class Response, class Callback>
    void AsyncRequest(Stub* stub,
                      void(Stub::*func) (google::protobuf::RpcController*,
                                         const Request*, Response*, Callback*),
                      const Request* request, Response* response,
                      boost::function<void (const Request*, Response*, bool, int)> callback,
                      int32_t rpc_timeout, int /*retry_times*/) {
        RrpcController* controller = new RrpcController();
        google::protobuf::Closure* done = NULL;
        (stub->*func)(controller, request, response, done);
    }

//    template <class Request, class Response, class Callback>
//    static void RpcCallback(sofa::pbrpc::RpcController* rpc_controller,
//                            const Request* request,
//                            Response* response,
//                            boost::function<void (const Request*, Response*, bool, int)> callback) {
//        bool failed = rpc_controller->Failed();
//        int error = rpc_controller->ErrorCode();
//        if (failed || error) {
//            assert(failed && error);
//            if (error != sofa::pbrpc::RPC_ERROR_SEND_BUFFER_FULL) {
//                LOG(WARNING) << "RpcCallback: " << rpc_controller->ErrorText().c_str();
//            } else {
//                ///TODO: Retry
//            }
//        }
//        delete rpc_controller;
//        callback(request, response, failed, error);
//    }
private:
    Mutex mutex_;
};

}  // namespace rrpc
