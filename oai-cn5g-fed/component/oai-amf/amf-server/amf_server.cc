#include <algorithm>

#include <exception>
#include <pistache/client.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <nlohmann/json.hpp>

using namespace Pistache;


using HttpClient = Http::Client;

static HttpClient client;

class StatsEndpoint {
public:
  explicit StatsEndpoint(Address addr)
      : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

  void init(size_t thr = 2) {
    auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
    httpEndpoint->init(opts);
    setupRoutes();
  }

  void start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
  }

  void set_params(std::string addr, int delay1, int delay2){
        this->addr = addr;
        this->delay1 = delay1;
        this->delay2 = delay2;
    }

private:
  std::string addr;
  int delay1;
  int delay2;

  std::shared_ptr<Http::Endpoint> httpEndpoint;
  Rest::Router router;



  void setupRoutes() {
    using namespace Rest;

    Routes::Get(router, "/determine_location/:name", Routes::bind(&StatsEndpoint::doDetermineLocation, this));

  }

  

  static auto getPeer(const Http::ResponseWriter &response)
      -> std::shared_ptr<Tcp::Peer> {
    try {
      return response.peer();
    } catch (...) {
    }
    return nullptr;
  }

  void doDetermineLocation(const Rest::Request &request,
                      Http::ResponseWriter response) {

    using namespace Pistache::Http;
    using json = nlohmann::json;

    auto name = request.param(":name").as<std::string>();
    if (std::stoi( name )%2){
        std::this_thread::sleep_for(std::chrono::milliseconds(delay1));
    }else{
        std::this_thread::sleep_for(std::chrono::milliseconds(delay2));
    }

    json j =
    {
        {"x",rand() % 30},
        {"y",rand() % 30},
        {"z",rand() % 30}
    };

    auto lmf_resp = client.post(addr).body(j.dump()).send(); 


    auto responseHeap = std::make_shared<ResponseWriter>(std::move(response));

    lmf_resp.then(
        [responseHeap](Response srvresponse) mutable {
          auto response = std::move(responseHeap);
          auto peerLocked = getPeer(*response);
          if (!peerLocked) {
            // Client disconnected
            return;
          }

          response->send(srvresponse.code(), srvresponse.body());
        },
        [responseHeap](std::exception_ptr exc) mutable {
          auto response = std::move(responseHeap);
          auto peerLocked = getPeer(*response);

          if (!peerLocked) {
            // Client disconnected
            return;
          }

          PrintException excPrinter;
          excPrinter(std::move(exc));

          // set mime type...
          response->send(Code::Internal_Server_Error, "{}");
        });
  }

};

int main(int argc, char *argv[]) {
  Port port = static_cast<uint16_t>(std::stol(argv[1]));

  client.init(HttpClient::options().threads(20));

  const std::string address (argv[3]); 
  int delay1 = std::stoi(argv[4]);
  int delay2 = std::stoi(argv[5]);

  int thr = std::stoi(argv[2]);


  Address addr(Ipv4::any(), port);

  std::cout << "Cores = " << hardware_concurrency() << std::endl;
  std::cout << "Using " << thr << " threads" << std::endl;
  std::cout << "Port: " << port << std::endl;
  std::cout << "address: " << address << std::endl;
  std::cout << "delay1: " << delay1 << std::endl;
  std::cout << "delay2: " << delay2 << std::endl;

  StatsEndpoint stats(addr);
  stats.set_params(address, delay1, delay2);

  stats.init(thr);
  stats.start();
}