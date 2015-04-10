#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/sofa-pbrpc/echo/echo_service.pb.h>

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
        LOG(INFO) << "Echo(): request message at local address ["
                  << static_cast<sofa::pbrpc::RpcController*>(controller)->LocalAddress()
                  << "] from remote address ["
                  << static_cast<sofa::pbrpc::RpcController*>(controller)->RemoteAddress()
                  << "]";
        response->set_message(request->message());
        done->Run();
    }
};

int main(int argc, char** argv)
{
    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start("127.0.0.1:12321")) {
        LOG(ERROR) << "start server failed";
        return EXIT_FAILURE;
    }
    
    // Register service.
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
        LOG(ERROR) << "export service failed";
        return EXIT_FAILURE;
    }

    // Wait signal to exit.
    rpc_server.Run();

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
