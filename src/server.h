#ifndef RRPC_SERVER_H_
#define RRPC_SERVER_H_

#include <string>
#include <map>

#include <boost/algorithm/string.hpp>

#include "glog/logging.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

#include "context.h"

namespace rrpc {

class RpcServer {
public:
    RpcServer() {};
    virtual ~RpcServer() {};
    virtual bool Start() = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual int32_t Count() {return 1;};

    bool RegisterService(google::protobuf::Service* service) {
        ServiceContext* service_context = new ServiceContext(service);
        std::string service_name = service_context->GetName();
        services_[service_name] = service_context;
        return true;
    }

    ServiceContext* GetService(std::string& method_name) {
        std::vector<std::string> name_parts;
        int i = method_name.rfind('.');
        std::string service_name = method_name.substr(0, i);
        if ( i <= 0) {
            return NULL;
        }

        std::map<std::string, ServiceContext*>::iterator it = services_.find(service_name);
        if (it == services_.end()) {
            return NULL;
        }

        return it->second;
    }

protected:
    std::map<std::string, ServiceContext*> services_;
};

}  // namespace rrpc

#endif  // RRPC_SERVER_H_
