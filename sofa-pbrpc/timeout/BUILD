proto_library(
    name = 'sleep_service',
    srcs = 'sleep_service.proto',
)

cc_binary(
    name = 'server',
    srcs = 'server.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':sleep_service',
    ]
)

cc_binary(
    name = 'client',
    srcs = 'client.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':sleep_service',
    ]
)
