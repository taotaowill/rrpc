#include "muduo/net/EventLoop.h"

#include "rpc_proxy.h"
#include "rpc_proxy_gflags.h"
#include "log_setting.h"

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    SetupLog("proxy");

    muduo::net::EventLoop loop;
    rrpc::RpcProxy proxy_server(&loop, FLAGS_rpc_proxy_port);
    if(!proxy_server.Start()) {
        exit(-1);
    }

    loop.loop();
    return 0;
}
