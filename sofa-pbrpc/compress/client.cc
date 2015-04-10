#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/sofa-pbrpc/compress/echo_service.pb.h>

int main(int /*argc*/, char** /*argv*/)
{
    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare params.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    //cntl->SetTimeout(3000);
    sofa::pbrpc::test::EchoRequest* request =
        new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    sofa::pbrpc::test::EchoResponse* response =
        new sofa::pbrpc::test::EchoResponse();

    // Sync call.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);
    stub->Echo(cntl, request, response, NULL);
    int ret = EXIT_SUCCESS;
    if (cntl->Failed())
    {
        LOG(ERROR) << "request failed: " << cntl->ErrorText();
        ret = EXIT_FAILURE;
    }
    else
    {
        LOG(INFO) << "request succeed: " << response->message();
    }

    delete request;
    delete response;
    delete cntl;
    delete stub;

    return ret;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
