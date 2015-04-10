#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/sofa-pbrpc/compress/echo_service.pb.h>

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
        LOG(INFO) << "Echo(): request message from "
                  << static_cast<sofa::pbrpc::RpcController*>(controller)->RemoteAddress()
                  << ": " << request->message();
        response->set_message("echo message: " + request->message());
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    LOG(INFO) << "Init work thread succeed";
    return true;
}

void thread_dest_func()
{
    LOG(INFO) << "Destroy work thread succeed";
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::string addr = argv[1] + std::string(":") + argv[2];

    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);
    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start(addr)) {
        LOG(ERROR) << "start server failed";
        return EXIT_FAILURE;
    }
    
    // Register service.
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
        LOG(ERROR) << "export service failed";
        return EXIT_FAILURE;
    }

    // Wait signal.
    rpc_server.Run();

    // Stop rpc server.
    rpc_server.Stop();

    // Delete closures.
    // Attention: should delete the closures after server stopped, or may be crash.
    delete options.work_thread_init_func;
    delete options.work_thread_dest_func;

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
