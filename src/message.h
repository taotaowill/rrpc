#pragma once

#include "boost/smart_ptr.hpp"
#include "muduo/base/Types.h"

#include "message_header.h"
#include "rrpc.pb.h"

namespace rrpc {

class RpcMessage;
typedef boost::shared_ptr<RpcMessage> RpcMessagePtr;

struct RpcMessage {
    uint32_t src_id;
    uint32_t dst_id; // dst_id > 0 means message client -> server
    RpcMeta meta;
    muduo::string data;

    void* Packaging(uint32_t& size) {
        RpcMessageHeader header;
        std::string meta_string;
        meta.SerializeToString(&meta_string);
        header.meta_size = meta_string.size();
        header.data_size = data.size();
        header.src_id = src_id;
        header.dst_id = dst_id;
        header.crc = 0;
        uint32_t body_size = header.meta_size + header.data_size;
        size = body_size + RPC_MESSAGE_HEADER_SIZE;
        void* buff = malloc(size);
        memset(buff, 0, size);
        memcpy(buff, &header, RPC_MESSAGE_HEADER_SIZE);
        memcpy(buff + RPC_MESSAGE_HEADER_SIZE,
               meta_string.c_str(),
               header.meta_size);
        memcpy(buff + RPC_MESSAGE_HEADER_SIZE + header.meta_size,
               data.c_str(),
               header.data_size);
        return buff;
    }
};

}
