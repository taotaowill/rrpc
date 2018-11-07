#ifndef RRPC_PB_REQUEST_PARSER_H_
#define RRPC_PB_REQUEST_PARSER_H_

#include <deque>

#include "boost/smart_ptr.hpp"
#include "muduo/base/Types.h"

#include "request_parser.h"
#include "request.h"

namespace rrpc {

enum ParsingStage {
    PARSING_HEADER = 0,
    PARSING_BODY = 1
};

class RpcPbRequestParser: public RpcRequestParser {
public:
    RpcPbRequestParser(muduo::string* buff);
    ~RpcPbRequestParser();
    int Parse();
    RpcRequestPtr GetRequest();


private:
    muduo::string* buff_;
    ParsingStage stage_;
    RpcMessageHeader header_;
    RpcRequest request_;
    std::deque<RpcRequest> request_list_;
};

}

#endif  //RRPC_PB_REQUEST_PARSER_H_
