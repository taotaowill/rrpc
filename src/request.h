#ifndef RRPC_REQUEST_H_
#define RRPC_REQUEST_H_

#include "boost/smart_ptr.hpp"

namespace rrpc {

class RpcRequest;
typedef boost::shared_ptr<RpcRequest> RpcRequestPtr;

class RpcRequest {
};

}

#endif  // RRPC_REQUEST_H_
