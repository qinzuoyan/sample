proto_library(
    name = 'echo_service',
    srcs = 'echo_service.proto',
)

cc_binary(
    name = 'server',
    srcs = 'server.cc',
    deps = [
        '//toft/system/atomic:atomic',
        '//toft/system/time:time',
        '//toft/system/threading:threading',
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)

cc_binary(
    name = 'client',
    srcs = 'client.cc',
    deps = [
        '//toft/system/time:time',
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)

cc_binary(
    name = 'client_continuous',
    srcs = 'client_continuous.cc',
    deps = [
        '//toft/system/atomic:atomic',
        '//toft/system/time:time',
        '//toft/system/threading:threading',
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)

cc_binary(
    name = 'client_dynamic',
    srcs = 'client_dynamic.cc',
    deps = [
        '//toft/system/time:time',
        '//sofa/pbrpc:sofa-pbrpc',
        '//thirdparty/perftools:tcmalloc',
        ':echo_service',
    ]
)
