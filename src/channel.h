#pragma once

#include "common/mutex.h"
#include "google/protobuf/service.h"

namespace rrpc {

using baidu::common::Mutex;

class RrpcChannel : public google::protobuf::RpcChannel {
public:
  RrpcChannel() {}
  ~RrpcChannel() {}
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);

private:
    Mutex mutex_;
};

}  // namespace rrpc
