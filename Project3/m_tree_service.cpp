#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include <grpcpp/grpcpp.h>
#include "proto/m_tree.grpc.pb.h"
#include "entry.hpp"
#include "utils.hpp"
#include "m_tree.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using mtree::MTreeService;
using mtree::InsertRequest;
using mtree::InsertResponse;
using mtree::QueryRequest;
using mtree::QueryResponse;

using tree_t = m_tree<params>;



template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

// Utilidad para crear entradas desde los requests
entry_type make_entry_from_request(const std::string& id, const std::vector<double>& features) {
    return make_entry(features, id);
}


// Singleton para el M-Tree
std::shared_ptr<tree_t> global_tree = make_tree();

class MTreeServiceImpl final : public MTreeService::Service {
public:
    Status Insert(ServerContext* context, const InsertRequest* request, InsertResponse* reply) override {
        std::vector<double> features(request->features().begin(), request->features().end());
        entry_type entry = make_entry_from_request(request->id(), features);
        global_tree->insert(entry);

        reply->set_message("Inserted " + request->id());
        std::cout << "[INSERT] ID: " << request->id() << std::endl;
        return Status::OK;
    }

    Status Query(ServerContext* context, const QueryRequest* request, QueryResponse* reply) override {
        std::vector<double> query_vec(request->features().begin(), request->features().end());
        entry_type query_entry = make_entry_from_request("query", query_vec);

        std::cout << "[QUERY] Query vector: ";
        for (double v : query_vec) std::cout << v << " ";
        std::cout << std::endl;

        auto results = global_tree->simple_kNN_search(query_entry, request->k());

        for (const auto& [oid, distance] : results) {
            auto* song = reply->add_results();
            song->set_id(oid);
            song->set_distance(distance);
            std::cout << "Result: " << oid << " (distance: " << distance << ")\n";
        }

        return Status::OK;
    }
};

void RunServer(const std::string& port) {
    std::string server_address = "0.0.0.0:" + port;
    MTreeServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "MTree gRPC server running at " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    std::string port = "50051";
    if (argc > 1) {
        port = argv[1];
    } else {
        std::cout << "No se especificó puerto, por defecto: 50051" << std::endl;
    }

    RunServer(port);
    return 0;
}
