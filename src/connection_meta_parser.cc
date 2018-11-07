#include "connection_meta_parser.h"

namespace rrpc {

RpcConnectionMetaParser::RpcConnectionMetaParser(muduo::string* buff) :
    buff_(buff) {
}

RpcConnectionMetaParser::~RpcConnectionMetaParser() {
}

int RpcConnectionMetaParser::Parse() {
    return 0;
}

}  // namespace rrpc
