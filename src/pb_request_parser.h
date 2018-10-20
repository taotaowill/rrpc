#ifndef RRPC_PB_REQUEST_PARSER_H_
#define RRPC_PB_REQUEST_PARSER_H_

#include "boost/smart_ptr.hpp"

#include "request_parser.h"
#include "request.h"

namespace rrpc {

class RpcPbRequestParser: public RpcRequestParser {
public:
    ~RpcPbRequestParser();
    int Parse(char* buff);
    RpcRequestPtr GetRequest();
};

}

#endif  //RRPC_PB_REQUEST_PARSER_H_
