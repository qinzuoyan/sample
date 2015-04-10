#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sample/sofa-pbrpc/timeout/sleep_service.pb.h>

class SleepServerImpl : public sofa::pbrpc::test::SleepServer
{
public:
    SleepServerImpl() {}
    virtual ~SleepServerImpl() {}

    virtual void SleepWithServiceTimeout(google::protobuf::RpcController* controller,
                                         const sofa::pbrpc::test::SleepRequest* request,
                                         sofa::pbrpc::test::SleepResponse* response,
                                         google::protobuf::Closure* done)
    {
        LOG(INFO) << "SleepWithServiceTimeout(): request sleep time: " << request->sleep_time();
        Sleep(controller, request, response, done);
    }

    virtual void SleepWithMethodTimeout(google::protobuf::RpcController* controller,
                                        const sofa::pbrpc::test::SleepRequest* request,
                                        sofa::pbrpc::test::SleepResponse* response,
                                        google::protobuf::Closure* done)
    {
        LOG(INFO) << "SleepWithMethodTimeout(): request sleep time: " << request->sleep_time();
        Sleep(controller, request, response, done);
    }


private:
    void Sleep(google::protobuf::RpcController* controller,
               const sofa::pbrpc::test::SleepRequest* request,
               sofa::pbrpc::test::SleepResponse* response,
               google::protobuf::Closure* done)
    {
        if (controller->IsCanceled())
            done->Run();
        sleep(request->sleep_time());
        char tmp[100];
        sprintf(tmp, "sleep succeed for %d seconds", request->sleep_time());
        response->set_message(tmp);
        done->Run();
    }
};

volatile bool g_quit = false;

static void SignalIntHandler(int /* sig */)
{
    g_quit = true;
}

int main(int /*argc*/, char** /*argv*/)
{
    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start("0.0.0.0:12321")) {
        LOG(ERROR) << "start server failed";
        return EXIT_FAILURE;
    }
    
    sofa::pbrpc::test::SleepServer* sleep_service = new SleepServerImpl();
    if (!rpc_server.RegisterService(sleep_service)) {
        LOG(ERROR) << "export service failed";
        return EXIT_FAILURE;
    }

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    while (!g_quit) {
        sleep(1);
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
