proto_library(
    name = 'echo_service',
    srcs = 'echo_service.proto',
)

cc_binary(
    name = 'server',
    srcs = 'server.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)

cc_binary(
    name = 'client',
    srcs = 'client.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)
