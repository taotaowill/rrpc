#include <unistd.h>

#include "proxy.h"

int main(int argc, char* argv[]) {
//    google::ParseCommandLineFlags(&argc, &argv, true);
//    rrpc::SetupLog("proxy");

//    rrpc::RpcProxy proxy_server(FLAGS_rpc_proxy_ip,
//                                FLAGS_rpc_proxy_port);
    rrpc::RpcProxy proxy_server("0.0.0.0",
                                8988);

    if(!proxy_server.Start()) {
        exit(-1);
    }

//    LOG(INFO) << "proxy server start ok on: " << FLAGS_rpc_proxy_port;

    while(1) {
      sleep(1);
    }

    return 0;
}
