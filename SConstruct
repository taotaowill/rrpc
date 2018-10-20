# gen protobuf
protoc = Builder(
    action = 'third_party/bin/protoc --proto_path=src/ --cpp_out=src/ src/*.proto'
)
env_gen = Environment(BUILDERS = {'Protoc': protoc})
env_gen.Protoc(
    [
        'src/rpc.pb.h',
        'src/rpc.pb.cc'
    ],
    Glob('src/proto/*.proto')
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
    'rpc_proto_lib',
    Glob('src/rpc.pb.cc')
)

env.Library(
    'rpc_proxy_lib',
    [
        'src/proxy_gflags.cc',
        'src/connection_manager.cc',
        'src/proxy.cc',
    ],
)

# program
env.Program(
    'proxy',
    'src/proxy_main.cc',
    LIBS=[
        'rpc_proxy_lib',
    ] + env['LIBS']
)

# unittest
env.Program(
    'test_rpc_proxy',
    'test/test_proxy.cc',
    LIBS=[
        'rpc_proxy_lib',
    ] + env['LIBS']
)
