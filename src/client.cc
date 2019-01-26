#include "client.h"

namespace rrpc {

RpcClient::RpcClient(std::string proxy_ip,
                     int32_t proxy_port,
                     int32_t rpc_id) :
        proxy_ip_(proxy_ip),
        proxy_port_(proxy_port),
        rpc_id_(rpc_id) {
}

RpcClient::~RpcClient() {
}

}  // namespace rrpc
