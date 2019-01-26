#include <unistd.h>

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
    LOG(INFO) << "++++++++++ echo method called";
    if (request->has_message()) {
        LOG(INFO) << "++++++++++ message: " << request->message();
        response->set_message("message response by EchoServiceImpl");
    }
}

TEST(Server, main) {
    std::string proxy_ip = "127.0.0.1";
    int32_t proxy_port = 8988;
    int32_t rpc_server_id = 100045;
    rrpc::RpcServer* rpc_server = \
        new rrpc::RpcPbServer(proxy_ip, proxy_port, rpc_server_id);
    ASSERT_TRUE(rpc_server->Start());
    EchoService* service = new EchoServiceImpl();
    ASSERT_TRUE(rpc_server->RegisterService(static_cast<EchoService*>(service)));
    ASSERT_TRUE(rpc_server->Count() > 0);
    while (1) {
      sleep(1);
    }
}

int main(int argc, char* argv[]) {
    rrpc::SetupLog("test_server");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
