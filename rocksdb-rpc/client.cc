#include <iostream>
#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/rocksdb-rpc/rocksdb_rpc.pb.h>

void print_usage()
{
    std::cerr << "USAGE: client <get> <key>" << std::endl;
    std::cerr << "       client <put> <key> <value>" << std::endl;
    std::cerr << "       client <del> <key>" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "ERROR: miss operator" << std::endl;
        print_usage();
        return EXIT_FAILURE;
    }

    std::string opr = argv[1];
    if (opr == "get" || opr == "del") {
        if (argc < 3) {
            std::cerr << "ERROR: miss param" << std::endl;
            print_usage();
            return EXIT_FAILURE;
        }
    }
    else if (opr == "put") {
        if (argc < 4) {
            std::cerr << "ERROR: miss param" << std::endl;
            print_usage();
            return EXIT_FAILURE;
        }
    }
    else {
        std::cerr << "ERROR: invalid operator" << std::endl;
        print_usage();
        return EXIT_FAILURE;
    }

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:8080", channel_options);

    // Prepare stub.
    rocksdb::rpc::RocksDB_Stub stub(&rpc_channel);

    // Prepare controller.
    sofa::pbrpc::RpcController cntl;
    cntl.SetTimeout(3000);

    if (opr == "get") {
        rocksdb::rpc::GetRequest request;
        rocksdb::rpc::GetResponse response;
        request.set_key(argv[2]);
        stub.Get(&cntl, &request, &response, NULL);
        if (cntl.Failed()) {
            std::cerr << "ERROR: " << cntl.ErrorText() << std::endl;
            return EXIT_FAILURE;
        }
        if (!response.succeed()) {
            std::cerr << "ERROR: " << response.error() << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << response.value() << std::endl;
    }
    else if (opr == "put") {
        rocksdb::rpc::PutRequest request;
        rocksdb::rpc::PutResponse response;
        request.set_key(argv[2]);
        request.set_value(argv[3]);
        stub.Put(&cntl, &request, &response, NULL);
        if (cntl.Failed()) {
            std::cerr << "ERROR: " << cntl.ErrorText() << std::endl;
            return EXIT_FAILURE;
        }
        if (!response.succeed()) {
            std::cerr << "ERROR: " << response.error() << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Succeed" << std::endl;
    }
    else {
        rocksdb::rpc::DeleteRequest request;
        rocksdb::rpc::DeleteResponse response;
        request.set_key(argv[2]);
        stub.Delete(&cntl, &request, &response, NULL);
        if (cntl.Failed()) {
            std::cerr << "ERROR: " << cntl.ErrorText() << std::endl;
            return EXIT_FAILURE;
        }
        if (!response.succeed()) {
            std::cerr << "ERROR: " << response.error() << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Succeed" << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
