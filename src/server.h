#pragma once

#include <string>
#include <map>

#include <boost/algorithm/string.hpp>

#include "common/mutex.h"
#include "glog/logging.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

#include "context.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;

class RpcServer {
public:
    RpcServer() {};
    virtual ~RpcServer() {};
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual int32_t Count() {return 1;};

    bool RegisterService(google::protobuf::Service* service) {
        MutexLock lock(&mutex_);
        ServiceContext* service_context = new ServiceContext(service);
        std::string service_name = service_context->GetName();
        services_[service_name] = service_context;
        return true;
    }

    ServiceContext* RetrieveService(std::string& method_name) {
        MutexLock lock(&mutex_);
        std::vector<std::string> name_parts;
        int i = method_name.rfind('.');
        std::string service_name = method_name.substr(0, i);
        if ( i <= 0) {
            return NULL;
        }

        std::map<std::string, ServiceContext*>::iterator it = \
            services_.find(service_name);
        if (it == services_.end()) {
            return NULL;
        }

        LOG(INFO) << "service found +++++++++++++++++++";
        return it->second;
    }

protected:
    Mutex mutex_;
    std::map<std::string, ServiceContext*> services_;
};

}  // namespace rrpc
