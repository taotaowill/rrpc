package(default_visibility = ["//visibility:public"])

proto_library(
    name = "rrpc_proto_lib",
    srcs = [
        "rrpc.proto",
    ],
)

cc_proto_library(
    name = "rrpc_proto",
    deps = [
        ":rrpc_proto_lib"
    ],
)

cc_library(
    name = "rrpc",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    deps = [
        ":rrpc_proto",
        "@com_google_protobuf//:protobuf",
        "@muduo//muduo/net",
    ]
)
