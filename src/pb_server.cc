#include "pb_server.h"

namespace rrpc {

RpcPbServer::RpcPbServer(std::string proxy_endpoint) :
    proxy_endpoint_(proxy_endpoint) {
}

bool RpcPbServer::Start() {
    return true;
}

void RpcPbServer::Run() {
}

void RpcPbServer::Stop() {
}

}  // namespace rrpc
