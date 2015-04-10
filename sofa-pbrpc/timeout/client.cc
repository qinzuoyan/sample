#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sample/sofa-pbrpc/timeout/sleep_service.pb.h>

int main(int /*argc*/, char** /*argv*/)
{
    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare objects.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    sofa::pbrpc::test::SleepRequest* request =
        new sofa::pbrpc::test::SleepRequest();
    sofa::pbrpc::test::SleepResponse* response =
        new sofa::pbrpc::test::SleepResponse();
    sofa::pbrpc::test::SleepServer_Stub* stub =
        new sofa::pbrpc::test::SleepServer_Stub(&rpc_channel);

    // Call 1
    LOG(INFO) << "----------- Call 1 ---------------------------------------";
    LOG(INFO) << "Sync call SleepWithServiceTimeout(), timeout is 2 seconds.";
    LOG(INFO) << "Sleep for 1 seconds.";
    cntl->Reset();
    request->set_sleep_time(1);
    stub->SleepWithServiceTimeout(cntl, request, response, NULL);
    LOG(INFO) << "Actual timeout is " << cntl->Timeout() << " milli-seconds.";
    if (cntl->Failed()) {
        LOG(ERROR) << "Failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "Succeed: " << response->message().c_str();
    }

    // Call 2
    LOG(INFO) << "----------- Call 2 ---------------------------------------";
    LOG(INFO) << "Sync call SleepWithServiceTimeout(), timeout is 2 seconds.";
    LOG(INFO) << "Sleep for 3 seconds.";
    cntl->Reset();
    request->set_sleep_time(3);
    stub->SleepWithServiceTimeout(cntl, request, response, NULL);
    LOG(INFO) << "Actual timeout is " << cntl->Timeout() << " milli-seconds.";
    if (cntl->Failed()) {
        LOG(ERROR) << "Failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "Succeed: " << response->message();
    }

    // Call 3
    LOG(INFO) << "----------- Call 3 ---------------------------------------";
    LOG(INFO) << "Sync call SleepWithMethodTimeout(), timeout is 4 seconds.";
    LOG(INFO) << "Sleep for 3 seconds.";
    cntl->Reset();
    request->set_sleep_time(3);
    stub->SleepWithMethodTimeout(cntl, request, response, NULL);
    LOG(INFO) << "Actual timeout is " << cntl->Timeout() << " milli-seconds.";
    if (cntl->Failed()) {
        LOG(ERROR) << "Failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "Succeed: " << response->message();
    }

    // Call 4
    LOG(INFO) << "----------- Call 4 ---------------------------------------";
    LOG(INFO) << "Sync call SleepWithMethodTimeout(), timeout is 4 seconds.";
    LOG(INFO) << "Sleep for 5 seconds.";
    cntl->Reset();
    request->set_sleep_time(5);
    stub->SleepWithMethodTimeout(cntl, request, response, NULL);
    LOG(INFO) << "Actual timeout is ld milli-seconds." << cntl->Timeout();
    LOG(INFO) << "Actual timeout is " << cntl->Timeout() << " milli-seconds.";
    if (cntl->Failed()) {
        LOG(ERROR) << "Failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "Succeed: " << response->message();
    }

    // Call 5
    LOG(INFO) << "----------- Call 5 ---------------------------------------";
    LOG(INFO) << "Sync call SleepWithMethodTimeout(), timeout is 4 seconds.";
    LOG(INFO) << "Set timeout of RpcController to 1 seconds.";
    LOG(INFO) << "Sleep for 3 seconds.";
    cntl->Reset();
    cntl->SetTimeout(1000);
    request->set_sleep_time(3);
    stub->SleepWithMethodTimeout(cntl, request, response, NULL);
    LOG(INFO) << "Actual timeout is " << cntl->Timeout() << " milli-seconds.";
    if (cntl->Failed()) {
        LOG(ERROR) << "Failed: " << cntl->ErrorText();
    }
    else {
        LOG(INFO) << "Succeed: " << response->message();
    }

    delete cntl;
    delete request;
    delete response;
    delete stub;
    return EXIT_SUCCESS;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
