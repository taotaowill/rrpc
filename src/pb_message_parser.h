#pragma once

#include <deque>

#include "boost/smart_ptr.hpp"
#include "muduo/base/Types.h"

#include "message_parser.h"
#include "message.h"

namespace rrpc {

enum ParsingStage {
    PARSING_HEADER = 0,
    PARSING_BODY = 1
};

class RpcPbMessageParser: public RpcMessageParser {
public:
    RpcPbMessageParser(muduo::string* buff);
    ~RpcPbMessageParser();
    int Parse();
    RpcMessagePtr GetMessage();

private:
    muduo::string* buff_;
    ParsingStage stage_;
    RpcMessageHeader header_;
    RpcMessage message_;
    std::deque<RpcMessage> message_list_;
};

}
