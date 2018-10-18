## gen protobuf
#protoc = Builder(
#    action = 'third_party/bin/protoc --proto_path=../proto --cpp_out=src/proto/ ../proto/*.proto'
#)
#env_gen = Environment(BUILDERS = {'Protoc': protoc})
#env_gen.Protoc(
#    [
#        'src/proto/gandalf.pb.h',
#        'src/proto/gandalf.pb.cc'
#    ],
#    Glob('../proto/*.proto')
#)

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

## library
#env.Library(
#    'gandalf_master_common_lib',
#    Glob('src/common/*.cc')
#)
#env.Library(
#    'gandalf_proto_lib',
#    Glob('src/proto/*.cc')
#)
#env.Library(
#    'gandalf_master_lib',
#    'src/gandalf_master.cc',
#    LIBS=[
#        'gandalf_proto_lib',
#        'gandalf_master_common_lib',
#    ] + env['LIBS']
#)

# program
env.Program(
    'build/bin/proxy',
    'src/proxy/main.cc',
    #LIBS=[
    #] + env['LIBS']
)

# unittest
env.Program(
    'build/test/test_proxy',
    'test/proxy/test_proxy.cc',
)

