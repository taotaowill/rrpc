#include "rrpc_gflags.h"

DEFINE_string(rpc_proxy_ip, "0.0.0.0", "RpcProxy listen ip");
DEFINE_int32(rpc_proxy_port, 8988, "RpcProxy listen port");
DEFINE_int32(rpc_client_id, 0, "RpcClient conn id");
DEFINE_int32(rpc_server_id, 0, "RpcServer conn id");
