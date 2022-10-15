#include <algorithm>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include "locator.h"
#include <nlohmann/json.hpp>
#include <random>

using namespace loc;
using namespace Pistache;


class StatsEndpoint
{
int count=0;
loc::locator locator;
arma::colvec distances; //distances column for each anchor
arma::mat anc_pos; // anchors (x,y) positions
arma::mat res;

public:
    explicit StatsEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    void init(size_t thr = 2)
    {
        auto opts = Http::Endpoint::options()
                        .threads(static_cast<int>(thr));
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start()
    {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }
    void initLocatorr(loc::locator locator){

        this->locator = locator;
        this->distances = arma::colvec(3, arma::fill::zeros); //column with distance? for each anchor
        this->anc_pos = arma::mat(3,2,arma::fill::zeros); // one row for each anchor to store (x,y) values       
        this->res = arma::mat(2,1,arma::fill::zeros);
        distances={3,3,3};
        anc_pos={{0,0},{5,5},{0,5}};

        this->locator.first_lls_init(distances, anc_pos);
    }
private:

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;


    void setupRoutes()
    {
        using namespace Rest;

        Routes::Post(router, "/localize-ue", Routes::bind(&StatsEndpoint::doLocalizeUE, this));
    }



    void doLocalizeUE(const Rest::Request& request, Http::ResponseWriter response)
    {


        using json = nlohmann::json;
        json body = json::parse(request.body());  


        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        // distances={(double)(rand() % 30),(double)(rand() % 30),(double)(rand() % 30)};
        distances={(double)(body.at("x")),(double)(body.at("y")),(double)(body.at("z"))};
        res = locator.localize(distances);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
 
        if(res.is_zero()){
            response.send(Http::Code::Not_Acceptable);
        }else{
            response.send(Http::Code::Ok,
                "{\"x\": \""+ std::to_string(res(0)) +  
                "\", \"y\": \""+ std::to_string(res(1)) +  "\"}", 
                MIME(Application, Json));
        }




        // std::this_thread::sleep_for(std::chrono::milliseconds(3));

        // response.send(Http::Code::Ok);

    }
};

int main(int argc, char* argv[])
{
    Port port(80);

    int thr = 20;

    if (argc >= 2)
    {
        port = static_cast<uint16_t>(std::stol(argv[1]));

        if (argc == 3)
            thr = std::stoi(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    std::cout << "Cores = " << hardware_concurrency() << std::endl;
    std::cout << "Using " << thr << " threads" << std::endl;

    StatsEndpoint stats(addr);

    locator locator;
    stats.initLocatorr(locator);
    stats.init(thr);
    stats.start();

}
