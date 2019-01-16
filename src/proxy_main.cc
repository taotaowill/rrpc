#include <unistd.h>

#include "muduo/net/EventLoop.h"

#include "proxy.h"
#include "proxy_gflags.h"
#include "log_setting.h"

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    rrpc::SetupLog("proxy");

    rrpc::RpcProxy proxy_server(FLAGS_rpc_proxy_port);
    if(!proxy_server.Start()) {
        exit(-1);
    }
    LOG(INFO) << "proxy server start ok on: " << FLAGS_rpc_proxy_port;

    while(1) {
      sleep(1);
    }

    return 0;
}
