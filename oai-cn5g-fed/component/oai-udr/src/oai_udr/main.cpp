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
#include <stdlib.h> // srand
#include <unistd.h> // get_pid(), pause()

#include <iostream>
#include <thread>

#include "conversions.hpp"
#include "logger.hpp"
#include "options.hpp"
#include "pid_file.hpp"
#include "udr-api-server.h"
#include "udr-http2-server.h"
#include "udr_app.hpp"
#include "udr_config.hpp"

using namespace util;
using namespace std;
using namespace oai::udr::app;
using namespace oai::udr::config;

udr_config udr_cfg;
udr_app *udr_app_inst = nullptr;
UDRApiServer *api_server = nullptr;
udr_http2_server *udr_api_server_2 = nullptr;

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
  std::cout << "UDR API Server memory done" << std::endl;

  if (udr_app_inst) {
    delete udr_app_inst;
    udr_app_inst = nullptr;
  }

  std::cout << "UDR APP memory done" << std::endl;
  std::cout << "Freeing allocated memory done" << std::endl;

  exit(0);
}

//------------------------------------------------------------------------------
int main(int argc, char **argv) {
  srand(time(NULL));

  // Command line options
  if (!Options::parse(argc, argv)) {
    std::cout << "Options::parse() failed" << std::endl;
    return 1;
  }

  // Logger
  Logger::init("udr", Options::getlogStdout(), Options::getlogRotFilelog());
  Logger::udr_server().startup("Options parsed");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_app_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Config
  udr_cfg.load(Options::getlibconfigConfig());
  udr_cfg.display();

  // UDR application layer
  udr_app_inst = new udr_app(Options::getlibconfigConfig());

  // PID file
  // Currently hard-coded value. TODO: add as config option.
  string pid_file_name = get_exe_absolute_path("/var/run", udr_cfg.instance);
  if (!is_pid_file_lock_success(pid_file_name.c_str())) {
    Logger::udr_server().error("Lock PID file %s failed\n",
                               pid_file_name.c_str());
    exit(-EDEADLK);
  }

  // UDR Pistache API server (HTTP1)
  Pistache::Address addr(
      std::string(inet_ntoa(*((struct in_addr *)&udr_cfg.nudr.addr4))),
      Pistache::Port(udr_cfg.nudr.port));

  api_server = new UDRApiServer(addr, udr_app_inst);
  api_server->init(2);
  std::thread udr_manager(&UDRApiServer::start, api_server);

  // UDM NGHTTP API server (HTTP2)
  udr_api_server_2 =
      new udr_http2_server(conv::toString(udr_cfg.nudr.addr4),
                           udr_cfg.nudr_http2_port, udr_app_inst);
  std::thread udr_http2_manager(&udr_http2_server::start, udr_api_server_2);

  udr_manager.join();
  udr_http2_manager.join();

  FILE *fp = NULL;
  std::string filename = fmt::format("/tmp/udr_{}.status", getpid());
  fp = fopen(filename.c_str(), "w+");
  fprintf(fp, "STARTED\n");
  fflush(fp);
  fclose(fp);

  pause();
  return 0;
}
