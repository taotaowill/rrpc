#include "gtest/gtest.h"

#include "rrpc.h"
#include "echo.pb.h"

using namespace rrpc;
using namespace rrpc::test;

class EchoServiceImpl : public EchoService {
public:
    void Echo(::google::protobuf::RpcController* controller,
              const EchoRequest* request,
              EchoResponse* response,
              ::google::protobuf::Closure* done);
};

void EchoServiceImpl::Echo(
        ::google::protobuf::RpcController* controller,
        const EchoRequest* request,
        EchoResponse* response,
        ::google::protobuf::Closure* done) {
    LOG(INFO) << "echo method called";
}

TEST(Server, main) {
    std::string endpoint = "127.0.0.1:8888";
    rrpc::RpcServer* rpc_server = new rrpc::RpcPbServer(endpoint);
    ASSERT_TRUE(rpc_server->Start());
    EchoService* service = new EchoServiceImpl();
    ASSERT_TRUE(rpc_server->RegisterService(static_cast<EchoService*>(service)));
    ASSERT_TRUE(rpc_server->Count() > 0);
}

int main(int argc, char* argv[]) {
    SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
