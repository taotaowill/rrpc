#pragma once

#include <stdint.h>
#include <string>

#include "muduo/net/TcpConnection.h"

#include "connection_meta_parser.h"
#include "pb_message_parser.h"


namespace rrpc {

using muduo::net::TcpConnectionPtr;

struct RpcConnection {
    std::string conn_name;
    uint32_t conn_id;
    uint32_t conn_type;
    bool checked;
    TcpConnectionPtr conn;
    muduo::string buff;
    RpcConnectionMetaParser* meta_parser;
    RpcPbMessageParser* message_parser;

    RpcConnection() :
        conn_id(0),
        checked(false),
        meta_parser(new RpcConnectionMetaParser(&buff)),
        message_parser(new RpcPbMessageParser(&buff)) {}

    ~RpcConnection() {
        delete message_parser;
        delete meta_parser;
    }
};

typedef boost::shared_ptr<RpcConnection> RpcConnectionPtr;

}  // namespace rrpc
