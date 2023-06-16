#include <thread>

#include "eventuals/eventual.h"
#include "eventuals/just.h"
#include "eventuals/let.h"
#include "eventuals/lock.h"
#include "eventuals/loop.h"
#include "eventuals/map.h"
#include "eventuals/promisify.h"
#include "eventuals/repeat.h"
#include "eventuals/then.h"
#include "proto/helloworld/helloworld.eventuals.h"
#include "proto/helloworld/helloworld.grpc.pb.h"

using eventuals::Closure;
using eventuals::Eventual;
using eventuals::Head;
using eventuals::Just;
using eventuals::Let;
using eventuals::Loop;
using eventuals::Map;
using eventuals::Repeat;
using eventuals::Synchronizable;
using eventuals::Then;

// using eventuals::grpc::Server;
using eventuals::grpc::ServerBuilder;
// using eventuals::grpc::ServerReader;

class GreeterImpl final
  : public helloworld::eventuals::Greeter::Service<GreeterImpl>,
    public Synchronizable {
 public:
  helloworld::HelloReply SayHello(
      grpc::ServerContext* context,
      helloworld::HelloRequest&& request) {
    helloworld::HelloReply reply;
    *reply.mutable_response() = std::move(*request.mutable_request());

    return reply;
  }
};

// helloworld::HelloRequest MakeHelloRequest() {
//   auto request = helloworld::HelloRequest();

//   request.mutable_request()->set_name("name");
//   return request;
// }

int main() {
  const std::string server_address("127.0.0.1:50051");
  GreeterImpl service;
  std::unique_ptr<eventuals::grpc::Server> server;
  ServerBuilder builder;

  builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  auto build = builder.BuildAndStart();
  if (!build.status.ok()) {
    return 0;
  }
  server = std::move(build.server);

  // auto serve = [&]() {
  //   return server->Accept<
  //              helloworld::Greeter,
  //              helloworld::HelloRequest,
  //              helloworld::HelloReply>("SayHello")
  //       >> Head()
  //       >> Then(Let([](auto& call) {
  //            return call.Reader().Read()
  //                >> Head() // Only get the first element.
  //                >> Then([](helloworld::HelloRequest&& request) {
  //                     helloworld::HelloReply reply;
  //                     *reply.mutable_response() = std::move(*request.mutable_request());
  //                     return reply;
  //                   })
  //                >> UnaryEpilogue(call);
  //          }));
  // };

  // auto [cancelled, k] = eventuals::Promisify("serve", serve());

  // k.Start();
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
  return 0;
}