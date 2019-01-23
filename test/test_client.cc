#include "gtest/gtest.h"

#include "rrpc.h"
#include "echo.pb.h"

TEST(Client, CallMethod) {
    rrpc::RpcClient client;
    rrpc::test::EchoService_Stub* stub;
    bool ret = client.GetStub(&stub);
    rrpc::test::EchoRequest request;
    rrpc::test::EchoResponse response;
    ret = client.SendRequest(stub, &rrpc::test::EchoService_Stub::Echo,
                             &request, &response, 5, 1);
    LOG(INFO) << "Test client done";
}

int main(int argc, char* argv[]) {
    rrpc::SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
