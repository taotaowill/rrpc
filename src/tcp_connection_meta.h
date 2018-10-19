#ifndef RRPC_TCP_CONNECTION_META_H
#define RRPC_TCP_CONNECTION_META_H

#include <stdint.h>

#define META_MAGIC 1096041805u  // META
#define TCP_CONNECTION_META_SIZE 12

namespace rrpc {

enum TcpConnectionType {
    CLIENT = 0,
    SERVER = 1,
};

class TcpConnectionMeta;

struct TcpConnectionMeta {
    union {
        char magic_str[4];
        uint32_t magic_str_value;
    };
    int32_t conn_type;
    int32_t crc;

    TcpConnectionMeta() : magic_str_value(META_MAGIC)
                        , conn_type(0), crc(0) {}

    bool CheckMagicStr() const {
        return magic_str_value == META_MAGIC;
    }
};

}  // namespace rrpc

#endif  // RRPC_TCP_CONNECTION_META_H
