proto_library(
    name = 'proto',
    srcs = 'rocksdb_rpc.proto',
)

cc_binary(
    name = 'server',
    srcs = 'server.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/rocksdb:rocksdb',
        '//thirdparty/perftools:tcmalloc',
        ':proto',
    ]
)

cc_binary(
    name = 'client',
    srcs = 'client.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':proto',
    ]
)
