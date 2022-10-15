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

#include "logger.hpp"
#include "ausf-api-server.h"
#include "ausf-http2-server.h"
#include "ausf_config.hpp"
#include "ausf_app.hpp"
#include "options.hpp"
#include "pid_file.hpp"

#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>  // srand
#include <unistd.h>  // get_pid(), pause()
#include <iostream>
#include <thread>

using namespace oai::ausf::app;
using namespace util;
using namespace std;

using namespace config;

ausf_config ausf_cfg;
ausf_app* ausf_app_inst              = nullptr;
AUSFApiServer* api_server            = nullptr;
ausf_http2_server* ausf_api_server_2 = nullptr;

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
  std::cout << "AUSF API Server memory done" << std::endl;

  if (ausf_app_inst) {
    delete ausf_app_inst;
    ausf_app_inst = nullptr;
  }

  std::cout << "AUSF APP memory done" << std::endl;
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
  Logger::init("ausf", Options::getlogStdout(), Options::getlogRotFilelog());
  Logger::ausf_server().startup("Options parsed");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_app_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Config
  ausf_cfg.load(Options::getlibconfigConfig());
  ausf_cfg.display();

  // AUSF application layer
  ausf_app_inst = new ausf_app(Options::getlibconfigConfig());

  // PID file
  // Currently hard-coded value. TODO: add as config option.
  string pid_file_name = get_exe_absolute_path("/var/run", ausf_cfg.instance);
  if (!is_pid_file_lock_success(pid_file_name.c_str())) {
    Logger::ausf_server().error(
        "Lock PID file %s failed\n", pid_file_name.c_str());
    exit(-EDEADLK);
  }

  // AUSF Pistache API server (HTTP1)
  Pistache::Address addr(
      std::string(inet_ntoa(*((struct in_addr*) &ausf_cfg.sbi.addr4))),
      Pistache::Port(ausf_cfg.sbi.port));
  api_server = new AUSFApiServer(addr, ausf_app_inst);
  api_server->init(2);
  std::thread ausf_manager(&AUSFApiServer::start, api_server);

  // AUSF NGHTTP API server (HTTP2)
  ausf_api_server_2 = new ausf_http2_server(
      conv::toString(ausf_cfg.sbi.addr4), ausf_cfg.sbi_http2_port,
      ausf_app_inst);
  std::thread ausf_http2_manager(&ausf_http2_server::start, ausf_api_server_2);

  ausf_manager.join();
  ausf_http2_manager.join();

  FILE* fp             = NULL;
  std::string filename = fmt::format("/tmp/ausf_{}.status", getpid());
  fp                   = fopen(filename.c_str(), "w+");
  fprintf(fp, "STARTED\n");
  fflush(fp);
  fclose(fp);

  pause();
  return 0;
}
