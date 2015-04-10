#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <toft/system/atomic/atomic.h>
#include <toft/system/time/clock.h>
#include <toft/system/threading/this_thread.h>
#include <gflags/gflags.h>
#include <perftools/malloc_extension.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sample/sofa-pbrpc/perf/echo_service.pb.h>

static toft::Atomic<int> s_succeed_count(0);

class EchoServerImpl : public sofa::pbrpc::test::EchoServer
{
public:
    EchoServerImpl() {}
    virtual ~EchoServerImpl() {}

private:
    virtual void Echo(google::protobuf::RpcController* controller,
                      const sofa::pbrpc::test::EchoRequest* request,
                      sofa::pbrpc::test::EchoResponse* response,
                      google::protobuf::Closure* done)
    {
        response->set_message(request->message());
        ++s_succeed_count;
        done->Run();
    }
};

volatile bool g_quit = false;

static void SignalIntHandler(int /* sig */)
{
    g_quit = true;
}

int main(int argc, char** argv)
{
    ::google::ParseCommandLineFlags(&argc, &argv, true);
    MallocExtension::instance()->SetMemoryReleaseRate(10.0);

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <host> <port> <thread_num>\n", argv[0]);
        return EXIT_FAILURE;
    }
    std::string address = argv[1] + std::string(":") + argv[2];
    int thread_num = atoi(argv[3]);

    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_num = thread_num;
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start(address)) {
        LOG(ERROR) << "start server failed";
        return EXIT_FAILURE;
    }
    
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
        LOG(ERROR) << "export service failed";
        return EXIT_FAILURE;
    }

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    int64_t print_interval_ms = 1000;
    int64_t release_interval_ms = 10000;
    int64_t last_print_time = toft::RealtimeClock.MilliSeconds();
    int64_t last_release_time = toft::RealtimeClock.MilliSeconds();
    long last_succeed_count = 0;
    while (!g_quit) {
        toft::ThisThread::Sleep(10);
        int64_t now = toft::RealtimeClock.MilliSeconds();
        if (now - last_print_time >= print_interval_ms) {
            long curr_succeed_count = static_cast<long>(s_succeed_count);
            LOG(INFO) << "QPS=" << (curr_succeed_count - last_succeed_count);
            last_succeed_count = curr_succeed_count;
            last_print_time = now;
        }
        if (now - last_release_time >= release_interval_ms) {
            LOG(INFO) << "ReleaseFreeMemory";
            MallocExtension::instance()->ReleaseFreeMemory();
            last_release_time = now;
        }
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
