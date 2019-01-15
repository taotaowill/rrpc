#ifndef RRPC_CONTEXT_H_
#define RRPC_CONTEXT_H_

#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

namespace rrpc {

using google::protobuf::MethodDescriptor;
using google::protobuf::ServiceDescriptor;
using google::protobuf::Service;

class ServiceContext {
public:
    ServiceContext(google::protobuf::Service* service) {
        service_ = service;
        // get methods
        for (int i = 0; i < service_->GetDescriptor()->method_count(); ++i) {
            const MethodDescriptor* method = service_->GetDescriptor()->method(i);
            LOG(INFO) << "method: " << method->full_name();
            methods_[method->full_name()] = method;
        }
    }

    std::string GetName() {
        if (service_ != NULL) {
            return service_->GetDescriptor()->full_name();
        }

        return "";
    }

    size_t GetMehodCount() {
        return methods_.size();
    }

    const MethodDescriptor* GetMethod(std::string& method_name) {
        std::map<std::string, const MethodDescriptor*>::iterator it = methods_.find(method_name);
        if (it == methods_.end()) {
            return NULL;
        }

        return NULL;
    }

private:
    Service* service_;
    std::map<std::string, const MethodDescriptor*> methods_;
};

}  // namespace rrpc

#endif  // RRPC_CONTEXT_H_

