#pragma once

#include "boost/smart_ptr.hpp"

#include "message.h"

namespace rrpc {

class RpcMessageParser;
typedef boost::shared_ptr<RpcMessageParser> RpcMessageParserPtr;

class RpcMessageParser {
public:
    virtual ~RpcMessageParser() {}
    virtual int Parse() = 0;
    virtual RpcMessagePtr GetMessage() = 0;
};

}
