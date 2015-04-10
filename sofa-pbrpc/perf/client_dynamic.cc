#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <toft/system/time/clock.h>
#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/sofa-pbrpc/perf/echo_service.pb.h>

volatile bool g_quit = false;

static void SignalIntHandler(int /* sig */)
{
    g_quit = true;
}

int main(int argc, char** argv)
{
    // check command line arguments.
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-address> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int message_size = 100;
    std::string message_str;
    message_str.resize(message_size, 'z');

    std::vector<std::string> address_list;
    for (int i = 1; i < argc; ++i) {
        address_list.push_back(argv[i]);
    }

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, address_list, channel_options);

    // Prepare objects.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
    request->set_message(message_str);
    sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
    sofa::pbrpc::test::EchoServer_Stub* stub = new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    int count = 0;
    while (!g_quit) {
        ++count;
        cntl->Reset();
        cntl->SetTimeout(3000);
        int64_t start_time = toft::RealtimeClock.MicroSeconds();
        stub->Echo(cntl, request, response, NULL);
        int64_t end_time = toft::RealtimeClock.MicroSeconds();
        if (cntl->Failed()) {
            LOG(ERROR) << "* " << count << " * [" << cntl->RemoteAddress()
                << "] request failed: " << cntl->ErrorText();
        }
        else if (response->message().size() != request->message().size()) {
            LOG(ERROR) << "* " << count << " * [" << cntl->RemoteAddress()
                << "] response not matched";
        }
        else {
            LOG(INFO) << "* " << count << " * [" << cntl->RemoteAddress()
                << "] request succeed, elapsed time in us: " << (end_time - start_time);
        }
        usleep(200000);
    }
    
    delete request;
    delete response;
    delete cntl;
    delete stub;

    rpc_client.Shutdown();

    return EXIT_SUCCESS;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
