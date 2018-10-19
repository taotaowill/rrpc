#ifndef RRPC_RPC_MESSAGE_HEADER_H_
#define RRPC_RPC_MESSAGE_HEADER_H_

#include <stdint.h>

#define RRPC_MAGIC 1129337426u  // RRPC
#define RPC_MESSAGE_HEADER_SIZE 24;

namespace rrpc {

struct RpcMessageHeader {
    union {
        char magic_str[4];
        uint32_t  magic_str_value;
    };
    int32_t meta_size;
    uint64_t data_size;
    uint64_t message_size;

    RpcMessageHeader()
        : magic_str_value(RRPC_MAGIC)
        , meta_size(0), data_size(0), message_size(0) {}

    bool CheckMagicStr() const {
        return magic_str_value == RRPC_MAGIC;
    }
};

}  // namespace rrpc

#endif  // RRPC_RPC_MESSAGE_HEADER_H_
