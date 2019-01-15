#pragma once

#include <stdint.h>

#define RPC_MESSAGE_HEADER_MAGIC 1129337426u  // RRPC
#define RPC_MESSAGE_HEADER_SIZE 28

namespace rrpc {

struct RpcMessageHeader {
    union {
        char magic_str[4];
        uint32_t  magic_str_value;
    };
    uint32_t meta_size;
    uint64_t data_size;
    uint32_t src_id;
    uint32_t dst_id;
    uint32_t crc;

    RpcMessageHeader()
        : magic_str_value(RPC_MESSAGE_HEADER_MAGIC)
        , meta_size(0), data_size(0), src_id(0), dst_id(0), crc(0) {}

    bool Check() const {
        if (magic_str_value == RPC_MESSAGE_HEADER_MAGIC
                && crc == 0) {
            return true;
        }

        return false;
    }
};

}  // namespace rrpc
