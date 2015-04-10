#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <toft/system/atomic/atomic.h>
#include <toft/system/time/clock.h>
#include <toft/system/threading/this_thread.h>
#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/closure.h>
#include <sample/sofa-pbrpc/perf/echo_service.pb.h>

static volatile bool s_is_running = true;
static volatile bool s_should_wait = false;

static toft::Atomic<int> s_print_token(0);
static toft::Atomic<int> s_succeed_count(0);
static toft::Atomic<int> s_timeout_count(0);
static toft::Atomic<int> s_pending_full_count(0);
static toft::Atomic<int> s_pending_count(0);

void sigcatcher(int sig)
{
    LOG(INFO) << "signal catched: " << sig;
    s_is_running = false;
}

void EchoCallback(
        sofa::pbrpc::RpcController* cntl,
        const sofa::pbrpc::test::EchoRequest* request,
        sofa::pbrpc::test::EchoResponse* response)
{
    --s_pending_count;

    if (cntl->Failed()) {
        if (cntl->ErrorCode() == sofa::pbrpc::RPC_ERROR_SEND_BUFFER_FULL) {
            s_should_wait = true;
            ++s_pending_full_count;
        }
        else if (cntl->ErrorCode() == sofa::pbrpc::RPC_ERROR_REQUEST_TIMEOUT) {
            s_should_wait = true;
            ++s_timeout_count;
        }
        else {
            s_is_running = false;
            if (++s_print_token == 1)
            {
                LOG(ERROR) << "request failed: " << cntl->ErrorText();
            }
        }
    }
    else if (request->message().size() != response->message().size()) {
        s_is_running = false;
        if (++s_print_token == 1)
        {
            LOG(ERROR) << "response not matched";
        }
    }
    else {
        ++s_succeed_count;
    }

    delete response;
    delete cntl;
}

int main(int argc, char **argv)
{
    // check command line arguments.
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <host> <port> <message_size> [is_grace_exit]\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::string address = argv[1] + std::string(":") + argv[2];
    int message_size = atoi(argv[3]);
    std::string message_str;
    message_str.resize(message_size, 'z');
    bool is_grace_exit = true;
    if (argc > 4)
    {
        if (strcmp(argv[4], "true") == 0)
            is_grace_exit = true;
        else if (strcmp(argv[4], "false") == 0)
            is_grace_exit = false;
        else
        {
            fprintf(stderr, "Invalid param 'is_grace_exit': should be 'true' or 'false'");
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT,  &sigcatcher);
    signal(SIGTERM, &sigcatcher);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel and stub.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, address, channel_options);
    sofa::pbrpc::test::EchoServer_Stub stub(&rpc_channel);
    sofa::pbrpc::test::EchoRequest echo_request;
    echo_request.set_message(message_str);

    int64_t print_interval_ms = 1000;
    int64_t last_time = toft::RealtimeClock.MilliSeconds();
    long last_succeed_count = 0;
    while (s_is_running) {
        sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
        const sofa::pbrpc::test::EchoRequest* request = &echo_request;
        sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
        google::protobuf::Closure* done = sofa::pbrpc::NewClosure(
                &EchoCallback, cntl, request, response);
        ++s_pending_count;
        stub.Echo(cntl, request, response, done);

        if (s_should_wait || s_pending_count > 300000) {
            s_should_wait = false;
            usleep(100000);
        }

        int64_t now = toft::RealtimeClock.MilliSeconds();
        if (now - last_time >= print_interval_ms) {
            long curr_succeed_count = s_succeed_count;
            LOG(INFO) << "QPS=" << (curr_succeed_count - last_succeed_count)
                      << ", pending_count=" << static_cast<long>(s_pending_count)
                      << ", timeout_count=" << static_cast<long>(s_timeout_count)
                      << ", pending_full_count=" << static_cast<long>(s_pending_full_count);
            last_succeed_count = curr_succeed_count;
            last_time = now;
        }
    }

    if (is_grace_exit) {
        LOG(INFO) << "gracely exiting ...";
        while (s_pending_count > 0) {
            LOG(INFO) << "pending count: " << static_cast<long>(s_pending_count);
            toft::ThisThread::Sleep(1000);
        }
    }

    rpc_client.Shutdown(); // should call Shutdown here!

    fprintf(stderr, "Succeed %ld\n", static_cast<long>(s_succeed_count));

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
