#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sample/sofa-pbrpc/compress/echo_service.pb.h>

class EchoServerImpl : public sofa::pbrpc::test::EchoServer
{
public:
    EchoServerImpl() {}
    virtual ~EchoServerImpl() {}

private:
    virtual void Echo(google::protobuf::RpcController* /*controller*/,
                      const sofa::pbrpc::test::EchoRequest* request,
                      sofa::pbrpc::test::EchoResponse* response,
                      google::protobuf::Closure* done)
    {
        LOG(INFO) << "Echo(): request message: " << request->message();
        response->set_message("echo message: " + request->message());
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
    
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
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
