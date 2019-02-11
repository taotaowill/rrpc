#include <unistd.h>

#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "common/timer.h"
#include "gtest/gtest.h"

#include "echo.pb.h"
#include "rrpc.h"

void tcallback(const rrpc::test::EchoRequest* request,
               rrpc::test::EchoResponse* response,
               bool failed,
               int /*error*/) {
    if (failed) {
        LOG(WARNING) << "rpc async_request failed";
    } else {
        LOG(INFO) << "callback from test_client, response message: "
                  << response->message();
    }
}

TEST(Client, CallMethod) {
    int32_t rpc_client_id = -1;
    int32_t rpc_server_id = 100045;
    std::string rpc_proxy_ip = "127.0.0.1";
    int32_t rpc_proxy_port = 8988;
    rrpc::RpcClient client(rpc_proxy_ip,
                           rpc_proxy_port,
                           rpc_client_id);
    rrpc::test::EchoService_Stub* stub;
    bool ret = client.GetStub(&stub, rpc_server_id);
    usleep(500000);

    // 1st sync request
    rrpc::test::EchoRequest sync_request;
    //sync_request.set_message("hello rpc: sync request");
    rrpc::test::EchoResponse sync_response;
    long start_time = ::baidu::common::timer::get_micros();
    ret = client.SendRequest(
            stub,
            &rrpc::test::EchoService_Stub::Echo,
            &sync_request,
            &sync_response,
            5,
            1);
    long end_time = ::baidu::common::timer::get_micros();
    LOG(INFO) << "time elaspe: " << end_time - start_time;
    if (!ret) {
        LOG(WARNING) << "SendRequest failed";
    } else {
        LOG(INFO) << "rpc sync_response: " << sync_response.DebugString();
    }

    // 2nd async request
    rrpc::test::EchoRequest async_request;
    async_request.set_message("async request: hello rpc");
    rrpc::test::EchoResponse async_response;
    boost::function<
        void (const rrpc::test::EchoRequest*,
              rrpc::test::EchoResponse*,
              bool,
              int)> callback;
    callback = boost::bind(&tcallback, _1, _2, _3, _4);
    client.AsyncRequest(
            stub,
            &rrpc::test::EchoService_Stub::Echo,
            &async_request,
            &async_response,
            callback,
            5,
            0);
    LOG(INFO) << "Test client done";
    usleep(1000000);
}

int main(int argc, char* argv[]) {
    rrpc::SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
