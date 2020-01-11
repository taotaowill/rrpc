package(default_visibility = ["//visibility:public"])

proto_library(
    name = "echo_proto_lib",
    srcs = [
        "echo.proto",
    ],
)

cc_proto_library(
    name = "echo_proto",
    deps = [
        ":echo_proto_lib",
    ],
)

cc_test(
    name = "test_client",
    srcs = [
        "test_client.cc",
    ],
    deps = [
        ":echo_proto",
        "//:rrpc",
        "@gtest//:main",
    ],
)

cc_test(
    name = "test_proxy",
    srcs = [
        "test_server.cc",
    ],
    deps = [
        ":echo_proto",
        "//:rrpc",
        "@gtest//:main",
    ],
)

cc_test(
    name = "test_server",
    srcs = [
        "test_server.cc",
    ],
    deps = [
        ":echo_proto",
        "//:rrpc",
        "@gtest//:main",
    ],
)
