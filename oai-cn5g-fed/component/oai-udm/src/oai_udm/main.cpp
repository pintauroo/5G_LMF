/*
 * Copyright (c) 2017 Sprint
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>  // srand
#include <unistd.h>  // get_pid(), pause()

#include <iostream>
#include <thread>

#include "logger.hpp"
#include "options.hpp"
#include "pid_file.hpp"
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include "udm-api-server.h"
#include "udm-http2-server.h"
#include "udm_app.hpp"
#include "udm_config.hpp"

using namespace oai::udm::app;
using namespace oai::udm::config;
using namespace util;
using namespace std;

udm_config udm_cfg;
udm_app* udm_app_inst              = nullptr;
UDMApiServer* api_server           = nullptr;
udm_http2_server* udm_api_server_2 = nullptr;

//------------------------------------------------------------------------------
void my_app_signal_handler(int s) {
  std::cout << "Caught signal " << s << std::endl;
  Logger::system().startup("exiting");
  std::cout << "Freeing Allocated memory..." << std::endl;
  if (api_server) {
    api_server->shutdown();
    delete api_server;
    api_server = nullptr;
  }
  std::cout << "UDM API Server memory done" << std::endl;

  if (udm_app_inst) {
    delete udm_app_inst;
    udm_app_inst = nullptr;
  }

  std::cout << "UDM APP memory done" << std::endl;
  std::cout << "Freeing allocated memory done" << std::endl;

  exit(0);
}

//------------------------------------------------------------------------------
int main(int argc, char** argv) {
  srand(time(NULL));

  // Command line options
  if (!Options::parse(argc, argv)) {
    std::cout << "Options::parse() failed" << std::endl;
    return 1;
  }

  // Logger
  Logger::init("udm", Options::getlogStdout(), Options::getlogRotFilelog());
  Logger::udm_server().startup("Options parsed");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_app_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Config
  udm_cfg.load(Options::getlibconfigConfig());
  udm_cfg.display();

  // UDM application layer
  udm_app_inst = new udm_app(Options::getlibconfigConfig());

  // PID file
  // Currently hard-coded value. TODO: add as config option.
  string pid_file_name = get_exe_absolute_path("/var/run", udm_cfg.instance);
  if (!is_pid_file_lock_success(pid_file_name.c_str())) {
    Logger::udm_server().error(
        "Lock PID file %s failed\n", pid_file_name.c_str());
    exit(-EDEADLK);
  }

  // UDM Pistache API server (HTTP1)
  Pistache::Address addr(
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.sbi.addr4))),
      Pistache::Port(udm_cfg.sbi.port));
  api_server = new UDMApiServer(addr, udm_app_inst);
  api_server->init(2);
  std::thread udm_manager(&UDMApiServer::start, api_server);

  // UDM NGHTTP API server (HTTP2)
  udm_api_server_2 = new udm_http2_server(
      conv::toString(udm_cfg.sbi.addr4), udm_cfg.sbi_http2_port, udm_app_inst);
  std::thread udm_http2_manager(&udm_http2_server::start, udm_api_server_2);

  udm_manager.join();
  udm_http2_manager.join();

  FILE* fp             = NULL;
  std::string filename = fmt::format("/tmp/udm_{}.status", getpid());
  fp                   = fopen(filename.c_str(), "w+");
  fprintf(fp, "STARTED\n");
  fflush(fp);
  fclose(fp);

  pause();
  return 0;
}
