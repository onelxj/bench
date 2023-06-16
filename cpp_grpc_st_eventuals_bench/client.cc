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

helloworld::HelloRequest MakeHelloRequest() {
  auto request = helloworld::HelloRequest();

  request.mutable_request()->set_name("Eva");
  return request;
}

int main() {
  stout::Borrowable<eventuals::grpc::ClientCompletionThreadPool> pool;

  eventuals::grpc::Client client(
      "127.0.0.1:50051",
      ::grpc::InsecureChannelCredentials(),
      pool.Borrow());
  auto context = std::make_optional<::grpc::ClientContext>();

  auto e = [&]() {
    return client.Call<
               helloworld::Greeter,
               helloworld::HelloRequest,
               helloworld::HelloReply>("SayHello", &context.value())
        >> Then(Let([](auto& call) {
             return call.Writer().WriteLast(MakeHelloRequest())
                 >> call.Reader().Read()
                 >> Map([](auto&& reply) {
                      std::cout << reply.DebugString();
                    })
                 >> Loop()
                 >> call.Finish();
           }));
  };

  *e();

  return 0;
}