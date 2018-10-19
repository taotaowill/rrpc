#ifndef RRPC_RRPC_PB_REQUEST_PARSER_H_
#define RRPC_RRPC_PB_REQUEST_PARSER_H_

#include "boost/smart_ptr.hpp"

#include "rpc_request_parser.h"
#include "rpc_request.h"

namespace rrpc {

class RpcPbRequestParser: public RpcRequestParser {
public:
    ~RpcPbRequestParser();
    int Parse(char* buff);
    RpcRequestPtr GetRequest();
};

}

#endif  //RRPC_RRPC_PB_REQUEST_PARSER_H_
