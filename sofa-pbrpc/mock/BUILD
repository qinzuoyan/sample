proto_library(
    name = 'echo_service',
    srcs = 'echo_service.proto',
)

cc_test(
    name = 'mock_test',
    srcs = 'mock_test.cc',
    deps = [
        '//sofa/pbrpc:sofa-pbrpc',
        ':echo_service',
    ]
)
