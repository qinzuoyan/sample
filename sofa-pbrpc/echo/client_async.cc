#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/closure.h>
#include <sample/sofa-pbrpc/echo/echo_service.pb.h>

void EchoCallback(sofa::pbrpc::RpcController* cntl,
        sofa::pbrpc::test::EchoRequest* request,
        sofa::pbrpc::test::EchoResponse* response,
        bool* callbacked)
{
    // Check if the request has been sent.
    // If has been sent, then can get the sent bytes.
    LOG(INFO) << "RemoteAddress=" << cntl->RemoteAddress();
    LOG(INFO) << "IsRequestSent=" << (cntl->IsRequestSent() ? "true" : "false");
    if (cntl->IsRequestSent())
    {
        LOG(INFO) << "LocalAddress=" << cntl->LocalAddress();
        LOG(INFO) << "SentBytes=" << cntl->SentBytes();
    }

    // Check if failed.
    if (cntl->Failed()) {
        LOG(ERROR) << "request failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "request succeed: " << response->message();
    }

    *callbacked = true;
    delete cntl;
    delete request;
    delete response;
}

int main(int argc, char** argv)
{
    // Define an rpc server.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare parameters.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    cntl->SetTimeout(3000);
    sofa::pbrpc::test::EchoRequest* request =
        new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    sofa::pbrpc::test::EchoResponse* response =
        new sofa::pbrpc::test::EchoResponse();
    bool callbacked = false;
    google::protobuf::Closure* done = sofa::pbrpc::NewClosure(
            &EchoCallback, cntl, request, response, &callbacked);

    // Async call.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);
    stub->Echo(cntl, request, response, done);

    // Wait callback done.
    while (!callbacked) {
        sleep(1);
    }

    return EXIT_SUCCESS;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
