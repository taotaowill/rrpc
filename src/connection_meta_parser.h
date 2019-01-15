#pragma once

#include "muduo/base/Types.h"

#include "connection_meta.h"

namespace rrpc {

class RpcConnectionMetaParser {
public:
    RpcConnectionMetaParser(muduo::string* buff);
    ~RpcConnectionMetaParser();
    int Parse();
    RpcConnectionMetaPtr GetMeta();

private:
    muduo::string* buff_;
    RpcConnectionMeta meta_;
};

}  // namespace rrpc
