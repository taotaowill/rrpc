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
        'src/rrpc.pb.cc' ], Glob('src/proto/*.proto')
)

env_gen.TestProtoc(
    [
        'test/echo.pb.h',
        'test/echo.pb.cc',
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
)

# library
env.Library(
    'rrpc_lib',
    [
        'src/connection_manager.cc',
        'src/connection_meta_parser.cc',
        'src/pb_message_parser.cc',
        'src/pb_server.cc',
        'src/proxy_gflags.cc',
        'src/proxy.cc',
        'src/rrpc.pb.cc',
        'src/channel.cc',
    ],
)

# program
env.Program(
    'proxy',
    'src/proxy_main.cc',
    LIBS=[
        'rrpc_lib',
    ] + env['LIBS']
)

# unittest
env.Program(
    'test_proxy',
    [
      'test/test_proxy.cc',
      'test/echo.pb.cc',
    ],
    LIBS=[
        'rrpc_lib',
    ] + env['LIBS']
)

env.Program(
    'test_server',
    [
        'test/test_server.cc',
        'test/echo.pb.cc',
    ],
    LIBS=[
        'rrpc_lib',
    ] + env['LIBS']
)

env.Program(
    'test_client',
    [
        'test/test_client.cc',
        'test/echo.pb.cc',
    ],
    LIBS=[
        'rrpc_lib',
    ] + env['LIBS']
)
