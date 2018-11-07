#ifndef RRPC_CONNECTION_META_PARSER_H_
#define RRPC_CONNECTION_META_PARSER_H_

#include "muduo/base/Types.h"

namespace rrpc {

class RpcConnectionMetaParser {
public:
    RpcConnectionMetaParser(muduo::string* buff);
    ~RpcConnectionMetaParser();
    int Parse();

private:
    muduo::string* buff_;
};

}  // namespace rrpc

#endif
