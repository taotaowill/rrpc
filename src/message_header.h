#ifndef RRPC_MESSAGE_HEADER_H_
#define RRPC_MESSAGE_HEADER_H_

#include <stdint.h>

#define RPC_MESSAGE_HEADER_MAGIC 1129337426u  // RRPC
#define RPC_MESSAGE_HEADER_SIZE 24;

namespace rrpc {

struct RpcMessageHeader {
    union {
        char magic_str[4];
        uint32_t  magic_str_value;
    };
    uint32_t meta_size;
    uint64_t data_size;
    uint32_t conn_id;
    uint32_t crc;

    RpcMessageHeader()
        : magic_str_value(RPC_MESSAGE_HEADER_MAGIC)
        , meta_size(0), data_size(0), conn_id(0), crc(0) {}

    bool Check() const {
        if (magic_str_value == RPC_MESSAGE_HEADER_MAGIC
                || crc == 0) {
            return true;
        }

        return false;
    }
};

}  // namespace rrpc

#endif  // RRPC_MESSAGE_HEADER_H_
