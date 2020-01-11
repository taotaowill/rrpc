#protobuf 3.3
http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-3.3.0",
    url = "file:///home/tmp/protobuf-3.3.0.tar.gz",
)

local_repository(
    name = "muduo",
    path = "/home/tmp/muduo",
)

new_http_archive(
    name = "gtest",
    build_file = "third_party/gtest.BUILD",
    sha256 = "58a6f4277ca2bc8565222b3bbd58a177609e9c488e8a72649359ba51450db7d8",
    strip_prefix = "googletest-release-1.8.0",
    url = "file:///home/tmp/googletest-release-1.8.0.tar.gz",
)
