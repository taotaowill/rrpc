#include "connection_meta_parser.h"

#include "glog/logging.h"

#include "message_header.h"

namespace rrpc {

RpcConnectionMetaParser::RpcConnectionMetaParser(muduo::string* buff) :
    buff_(buff) {
}

RpcConnectionMetaParser::~RpcConnectionMetaParser() {
}

int RpcConnectionMetaParser::Parse() {
    size_t size = buff_->size();
    if (size < RPC_CONNECTION_META_SIZE) {
        return 0;
    }

    if (size > RPC_CONNECTION_META_SIZE) {
        LOG(WARNING) << "conn meta in invalid size: " << size;
        return -1;
    }

    const RpcConnectionMeta* meta = \
           reinterpret_cast<const RpcConnectionMeta*>(buff_->c_str());
    if (!meta->Check()) {
        LOG(WARNING) << "conn meta validate fail";
        return -1;
    }

    meta_ = *meta;
    return 1;
}

RpcConnectionMetaPtr RpcConnectionMetaParser::GetMeta() {
    return boost::make_shared<RpcConnectionMeta>(meta_);
}

}  // namespace rrpc
