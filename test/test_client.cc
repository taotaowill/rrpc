#include "gtest/gtest.h"

#include <unistd.h>

#include "rrpc.h"
#include "echo.pb.h"

TEST(Client, CallMethod) {
    int32_t rpc_client_id = -1;
    int32_t rpc_server_id = 100045;
    rrpc::RpcClient client("127.0.0.1", 8988, rpc_client_id);
    rrpc::test::EchoService_Stub* stub;
    bool ret = client.GetStub(&stub, rpc_server_id);
    rrpc::test::EchoRequest request;
    request.set_message("hello rpc");
    rrpc::test::EchoResponse response;
    usleep(10000);
    ret = client.SendRequest(
            stub,
            &rrpc::test::EchoService_Stub::Echo,
            &request, &response,
            5,
            1);
    LOG(INFO) << "response: " << response.DebugString();
    LOG(INFO) << "Test client done";
    usleep(10000000);
}

int main(int argc, char* argv[]) {
    rrpc::SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
