#include "channel.h"

#include "glog/logging.h"
#include "google/protobuf/descriptor.h"

namespace rrpc {

void RrpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                             google::protobuf::RpcController* controller,
                             const ::google::protobuf::Message* request,
                             ::google::protobuf::Message* response,
                             ::google::protobuf::Closure* done) {
    LOG(INFO) << "CallMethod from rrpc_channel, method: " << method->full_name();
}

}  // namespace rrpc
