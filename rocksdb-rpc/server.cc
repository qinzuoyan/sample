#include <signal.h>
#include <unistd.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <thirdparty/rocksdb/db.h>
#include <thirdparty/rocksdb/slice.h>
#include <thirdparty/rocksdb/options.h>
#include <sample/rocksdb-rpc/rocksdb_rpc.pb.h>

using namespace rocksdb;

class ServerImpl : public rocksdb::rpc::RocksDB
{
public:
    ServerImpl() : _db(NULL) {}

    virtual ~ServerImpl() {
        if (_db != NULL) {
            delete _db;
        }
    }

    bool Init(const std::string& dbname) {
        Options options;
        // Optimize RocksDB.
        options.IncreaseParallelism();
        options.OptimizeLevelStyleCompaction();
        // create the DB if it's not already present
        options.create_if_missing = true;
        // open DB
        Status s = DB::Open(options, dbname, &_db);
        if (!s.ok()) {
            LOG(ERROR) << "open db failed: " << s.ToString();
            return false;
        }
        return true;
    }

private:
    virtual void Get(::google::protobuf::RpcController* controller,
		    const ::rocksdb::rpc::GetRequest* request,
		    ::rocksdb::rpc::GetResponse* response,
		    ::google::protobuf::Closure* done) {
        sofa::pbrpc::RpcController* cntl = static_cast<sofa::pbrpc::RpcController*>(controller);
        LOG(INFO) << "GET from [" << cntl->RemoteAddress() << "]";

        if (!request->has_key()) {
            response->set_succeed(false);
            response->set_error("key not set");
            done->Run();
            return;
        }

        Status s = _db->Get(ReadOptions(), request->key(), response->mutable_value());
        if (!s.ok()) {
            response->set_succeed(false);
            response->clear_value();
            response->set_error(s.ToString());
            done->Run();
            return;
        }

        response->set_succeed(true);
        done->Run();
    }

    virtual void Put(::google::protobuf::RpcController* controller,
		    const ::rocksdb::rpc::PutRequest* request,
		    ::rocksdb::rpc::PutResponse* response,
		    ::google::protobuf::Closure* done) {
        sofa::pbrpc::RpcController* cntl = static_cast<sofa::pbrpc::RpcController*>(controller);
        LOG(INFO) << "PUT from [" << cntl->RemoteAddress() << "]";

        if (!request->has_key() || !request->has_value()) {
            response->set_succeed(false);
            response->set_error("key or value not set");
            done->Run();
            return;
        }

        Status s = _db->Put(WriteOptions(), request->key(), request->value());
        if (!s.ok()) {
            response->set_succeed(false);
            response->set_error(s.ToString());
            done->Run();
            return;
        }

        response->set_succeed(true);
        done->Run();
    }

    virtual void Delete(::google::protobuf::RpcController* controller,
		    const ::rocksdb::rpc::DeleteRequest* request,
		    ::rocksdb::rpc::DeleteResponse* response,
		    ::google::protobuf::Closure* done)
    {
        sofa::pbrpc::RpcController* cntl = static_cast<sofa::pbrpc::RpcController*>(controller);
        LOG(INFO) << "DEL from [" << cntl->RemoteAddress() << "]";

        if (!request->has_key()) {
            response->set_succeed(false);
            response->set_error("key not set");
            done->Run();
            return;
        }

        Status s = _db->Delete(WriteOptions(), request->key());
        if (!s.ok()) {
            response->set_succeed(false);
            response->set_error(s.ToString());
            done->Run();
            return;
        }

        response->set_succeed(true);
        done->Run();
    }

private:
    rocksdb::DB* _db;
};

int main(int argc, char** argv)
{
    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start("127.0.0.1:8080")) {
        LOG(ERROR) << "start server failed";
        return EXIT_FAILURE;
    }
    
    // Register service.
    ServerImpl* service = new ServerImpl();
    if (!service->Init("/tmp/rocksdb-rpc-test")) {
        LOG(ERROR) << "init service failed";
        return EXIT_FAILURE;
    }

    if (!rpc_server.RegisterService(service)) {
        LOG(ERROR) << "export service failed";
        return EXIT_FAILURE;
    }

    // Wait signal to exit.
    rpc_server.Run();

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
