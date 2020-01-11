#include <unistd.h>
#include <stdlib.h>

#include "gtest/gtest.h"

#include "rrpc.h"

#include "test/echo.pb.h"

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
    if (request->has_message()) {
//        LOG(INFO) << "request message: " << request->message();
        response->set_message(request->message());
    }
}

TEST(Server, main) {
    //std::string proxy_ip = "127.0.0.1";
    std::string proxy_ip = getenv("RRPC_PROXY");
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

//int main(int argc, char* argv[]) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
