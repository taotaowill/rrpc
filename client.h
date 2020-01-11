#pragma once

#include <map>
#include <string>

#include "boost/function.hpp"

#include "channel.h"
#include "closure.h"
#include "controller.h"
#include "mutex.h"
#include "thread_pool.h"

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
        long timeout = ::baidu::common::timer::get_micros() + 2 * 1000000L;
        while (true) {
            long nowtime = ::baidu::common::timer::get_micros();
            if (channels_[rpc_id]->IsConnected()) {
                return true;
            }

            if (nowtime > timeout) {
                return false;
            }

            usleep(10000);
        }

        return false;
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
        MutexLock lock(&mutex_);
        RpcController controller;
        controller.SetTimeout(rpc_timeout);
        for (int32_t retry = 0; retry < retry_times; ++retry) {
            (stub->*func)(&controller, request, response, NULL);
//            LOG(INFO) << "SendRequest after call";
            if (!controller.Failed()) {
//                LOG(INFO) << "SendRequest success, response: "
//                          << response->DebugString();
                return true;
            }

           if (retry < retry_times - 1) {
//               LOG(INFO) << "Send failed, retry ...";
               usleep(100000);
           }
        }

        controller.Reset();
//        LOG(WARNING) << "SendRequest failed: " << controller.ErrorText();
        return false;
    }


    template <class Stub, class Request, class Response, class Callback>
    void AsyncRequest(
            Stub* stub,
            void(Stub::*func) (::google::protobuf::RpcController*,
                               const Request*, Response*, Callback*),
            const Request* request,
            Response* response,
            boost::function<void (const Request*, Response*, bool, int)> callback,
            int32_t rpc_timeout,
            int /*retry_times*/) {
        MutexLock lock(&mutex_);
        RpcController* controller = new RpcController();
        controller->SetTimeout(rpc_timeout);
        ::google::protobuf::Closure* done = NewClosure(
                &RpcClient::template RpcCallback<Request, Response, Callback>,
                controller, request, response, callback);
        (stub->*func)(controller, request, response, done);
//        LOG(INFO) << "+++ async requst end";
    }

    template <class Request, class Response, class Callback>
    static void RpcCallback(
            RpcController* rpc_controller,
            const Request* request,
            Response* response,
            boost::function<void (const Request*, Response*, bool, int)> callback) {
        bool failed = rpc_controller->Failed();
        if (failed) {
//            LOG(WARNING) << "RpcCallback: "
//                         << rpc_controller->ErrorText().c_str();
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
