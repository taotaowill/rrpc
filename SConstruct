# gen protobuf
meta_protoc = Builder(
    action = 'third_party/bin/protoc --proto_path=src/ --cpp_out=src/ src/*.proto'
)

test_protoc = Builder(
    action = 'third_party/bin/protoc --proto_path=test/ --cpp_out=test/ test/*.proto'
)
env_gen = Environment(
    BUILDERS = {
        'MetaProtoc': meta_protoc,
        'TestProtoc': test_protoc
    }
)

env_gen.MetaProtoc(
    [
        'src/rrpc.pb.h',
        'src/rrpc.pb.cc'
    ],
    Glob('src/proto/*.proto')
)

env_gen.TestProtoc(
    [
        'test/echo.pb.h',
        'test/echo.pb.cc'
    ],
    Glob('test/proto/*.proto')
)

env = Environment(
    CPPPATH = [
        'src',
        'third_party/boost_1_57_0/',
        'third_party/include/',
    ] ,
    LIBS = [
        'glog',
        'gflags',
        'gtest',
        'tcmalloc',
        'unwind',
        'muduo_net',
        'muduo_base',
        'pthread',
        'rt',
        'boost_filesystem',
        'protobuf',
        'z',
        'snappy',
        'common',
    ],
    LIBPATH = [
        '.',
        'third_party/lib',
        'third_party/boost_1_57_0/stage/lib',
    ],
    CXXFLAGS = [
        '-g2',
        '-Wall',
        '-Wno-unused-function',
        '-Wno-deprecated-declarations',
        '-Wno-unused-but-set-variable',
    ],
    LINKFLAGS = [
        '-Wl'
    ]
)

# library
env.Library(
    'proto_lib',
    Glob('src/rrpc.pb.cc')
)

env.Library(
    'proxy_lib',
    [
        'src/proxy_gflags.cc',
        'src/connection_manager.cc',
        'src/connection_meta_parser.cc',
        'src/pb_request_parser.cc',
        'src/proxy.cc',
    ],
)

env.Library(
    'rrpc_lib',
    [
        'src/pb_server.cc',
    ],
)

# program
env.Program(
    'proxy',
    'src/proxy_main.cc',
    LIBS=[
        'proxy_lib',
        'proto_lib',
    ] + env['LIBS']
)

# unittest
env.Program(
    'test_rpc_proxy',
    'test/test_proxy.cc',
    LIBS=[
        'proxy_lib',
        'proto_lib',
    ] + env['LIBS']
)

env.Program(
    'test_rpc_server',
    [
        'test/test_server.cc',
        'test/echo.pb.cc',
    ],
    LIBS=[
        'rrpc_lib'
    ] + env['LIBS']
)
