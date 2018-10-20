#ifndef RRPC_REQUEST_PARSER_H_
#define RRPC_REQUEST_PARSER_H_

#include "boost/smart_ptr.hpp"

#include "request.h"

namespace rrpc {

class RpcRequestParser;
typedef boost::shared_ptr<RpcRequestParser> RpcRequestParserPtr;

class RpcRequestParser {
public:
    virtual ~RpcRequestParser() {}
    virtual int Parse(char* buff) = 0;
    virtual RpcRequestPtr GetRequest() = 0;
};

}

#endif  //RRPC_REQUEST_PARSER_H_
