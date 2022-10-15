/*
 * Copyright (c) 2019 EURECOM
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

#include "common_defs.h"
#include "logger.hpp"
#include "options.hpp"
#include "pid_file.hpp"
#include "nssf_app.hpp"
#include "nssf_config.hpp"
#include "nssf-api-server.h"
#include "nssf-http2-server.h"
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"

#include <boost/asio.hpp>
#include <iostream>
#include <algorithm>
#include <thread>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>  // get_pid(), pause()
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace nssf;
using namespace util;
using namespace std;
using namespace oai::nssf_server::api;

nssf_app* nssf_app_inst = nullptr;
nssf_config nssf_cfg;
boost::asio::io_service io_service;
NSSFApiServer* nssf_api_server_1     = nullptr;
nssf_http2_server* nssf_api_server_2 = nullptr;

//------------------------------------------------------------------------------
void my_app_signal_handler(int s) {
  std::cout << "Caught signal " << s << std::endl;
  Logger::system().startup("exiting");
  std::cout << "Freeing Allocated memory..." << std::endl;

  if (nssf_api_server_1) {
    nssf_api_server_1->shutdown();
    delete nssf_api_server_1;
    nssf_api_server_1 = nullptr;
  }
  std::cout << "NSSF API Server memory done." << std::endl;
  if (nssf_app_inst) delete nssf_app_inst;
  nssf_app_inst = nullptr;
  std::cout << "NSSF APP memory done." << std::endl;
  // if (itti_inst) delete itti_inst;
  // itti_inst = nullptr;
  // std::cout << "ITTI memory done." << std::endl;
  std::cout << "Freeing Allocated memory done" << std::endl;
  exit(0);
}
//------------------------------------------------------------------------------
// We are doing a check to see if an existing process already runs this program.
// We have seen that running at least twice this program in a container may lead
// to the container host to crash.
int my_check_redundant_process(char* exec_name) {
  FILE* fp;
  char* cmd = new char[200];
  std::vector<std::string> words;
  int result     = 0;
  size_t retSize = 0;

  // Retrieving only the executable name
  boost::split(words, exec_name, boost::is_any_of("/"));
  memset(cmd, 0, 200);
  sprintf(
      cmd, "ps aux | grep -v grep | grep -v nohup | grep -c %s || true",
      words[words.size() - 1].c_str());
  fp = popen(cmd, "r");

  // clearing the buffer
  memset(cmd, 0, 200);
  retSize = fread(cmd, 1, 200, fp);
  fclose(fp);

  // if something wrong, then we don't know
  if (retSize == 0) {
    delete[] cmd;
    return 10;
  }

  result = atoi(cmd);
  delete[] cmd;
  return result;
}
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
  // Checking if another instance of nssf is running
  int nb_processes = my_check_redundant_process(argv[0]);
  if (nb_processes > 1) {
    std::cout << "An instance of " << argv[0] << " is maybe already called!"
              << std::endl;
    std::cout << "  " << nb_processes << " were detected" << std::endl;
    return -1;
  }

  // Command line options
  if (!Options::parse(argc, argv)) {
    std::cout << "Options::parse() failed" << std::endl;
    return 1;
  }

  // Logger
  Logger::init("nssf", Options::getlogStdout(), Options::getlogRotFilelog());
  Logger::nssf_app().startup("Options parsed");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_app_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // Config
  nssf_cfg.load(Options::getlibconfigConfig());
  nssf_cfg.display();

  if (!nssf_cfg.ParseJson()) {
    std::cout << "nssf_cfg::ParseJson() failed" << std::endl;
    return 1;
  }

  // PID file
  // Currently hard-coded value. TODO: add as config option.
  string pid_file_name = get_exe_absolute_path("/var/run", nssf_cfg.instance);
  if (!is_pid_file_lock_success(pid_file_name.c_str())) {
    Logger::nssf_app().error(
        "Lock PID file %s failed\n", pid_file_name.c_str());
    exit(-EDEADLK);
  }

  // NSSF Pistache API server (HTTP1)
  Pistache::Address addr(
      std::string(inet_ntoa(*((struct in_addr*) &nssf_cfg.sbi.addr4))),
      Pistache::Port(nssf_cfg.sbi.http1_port));
  nssf_api_server_1 = new NSSFApiServer(addr, nssf_app_inst);
  nssf_api_server_1->init(2);
  std::thread nssf_http1_manager(&NSSFApiServer::start, nssf_api_server_1);

  // NSSF NGHTTP API server (HTTP2)
  nssf_api_server_2 = new nssf_http2_server(
      conv::toString(nssf_cfg.sbi.addr4), nssf_cfg.sbi.http2_port,
      nssf_app_inst);
  std::thread nssf_http2_manager(&nssf_http2_server::start, nssf_api_server_2);

  nssf_http1_manager.join();
  nssf_http2_manager.join();

  FILE* fp             = NULL;
  std::string filename = fmt::format("/tmp/nssf_{}.status", getpid());
  fp                   = fopen(filename.c_str(), "w+");
  fprintf(fp, "STARTED\n");
  fflush(fp);
  fclose(fp);

  pause();
  return 0;
}
