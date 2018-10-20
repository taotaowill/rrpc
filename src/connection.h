#ifndef RRPC_CONNECTION_H
#define RRPC_CONNECTION_H

#include <stdint.h>
#include <string>

#include "muduo/net/TcpConnection.h"

#define RPC_CONNECTION_META_MAGIC 1096041805u  // META
#define RPC_CONNECTION_META_SIZE 16

namespace rrpc {

using muduo::net::TcpConnectionPtr;

enum RpcConnectionType {
    RPC_CONNECTION_TYPE_CLIENT = 0,
    RPC_CONNECTION_TYPE_SERVER = 1,
};

class RpcConnectionMeta;
typedef boost::shared_ptr<RpcConnectionMeta> RpcConnectionMetaPtr;

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
                || crc == 1) {\
            return true;
        }

        return false;
    }
};

struct RpcConnection {
    std::string conn_name;
    uint32_t conn_id;
    uint32_t conn_type;
    bool checked;
    TcpConnectionPtr conn;
    muduo::string buff;

    RpcConnection() : conn_id(0), checked(false) {}
};

typedef boost::shared_ptr<RpcConnection> RpcConnectionPtr;

}  // namespace rrpc

#endif  // RRPC_CONNECTION_H
