#pragma once

#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

namespace rrpc {

class ServiceContext {
public:
    ServiceContext(::google::protobuf::Service* service) {
        service_ = service;
        // get methods
        for (int i = 0; i < service_->GetDescriptor()->method_count(); ++i) {
            const ::google::protobuf::MethodDescriptor* method = \
                    service_->GetDescriptor()->method(i);
//            LOG(INFO) << "method: " << method->full_name();
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

    const ::google::protobuf::MethodDescriptor* GetMethod(std::string& method_name) {
        std::map<std::string, const ::google::protobuf::MethodDescriptor*>::iterator it = \
                methods_.find(method_name);
        if (it == methods_.end()) {
            return NULL;
        }

        return it->second;
    }

    ::google::protobuf::Service* GetService() {
        return service_;
    }

private:
    ::google::protobuf::Service* service_;
    std::map<std::string, const ::google::protobuf::MethodDescriptor*> methods_;
};

}  // namespace rrpc
