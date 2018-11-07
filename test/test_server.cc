#include "gtest/gtest.h"

int main(int argc, char* argv[]) {
    SetupLog("test_rpc_proxy");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
