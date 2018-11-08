#ifndef RRPC_CONNECTION_META_H_
#define RRPC_CONNECTION_META_H_

#include "boost/smart_ptr.hpp"

#define RPC_CONNECTION_META_MAGIC 1096041805u  // META
#define RPC_CONNECTION_META_SIZE 16

namespace rrpc {

enum RpcConnectionType {
    RPC_CONNECTION_TYPE_CLIENT = 0,
    RPC_CONNECTION_TYPE_SERVER = 1,
};

struct RpcConnectionMeta {
    union {
        char magic_str[4];
        uint32_t magic_str_value;
    };
    uint32_t conn_type;
    uint32_t conn_id;
    uint32_t crc;

    RpcConnectionMeta() :
        magic_str_value(RPC_CONNECTION_META_MAGIC),
        conn_type(0), conn_id(0), crc(1) {}

    bool Check() const {
        if (magic_str_value == RPC_CONNECTION_META_MAGIC
                || crc == 1) {
            return true;
        }

        return false;
    }
};

typedef boost::shared_ptr<RpcConnectionMeta> RpcConnectionMetaPtr;

}  // namespace rrpc

#endif
