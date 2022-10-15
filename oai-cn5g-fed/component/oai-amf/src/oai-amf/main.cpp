/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the
 * License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include "AMFApiServer.hpp"
#include "amf-http2-server.hpp"
#include "amf_app.hpp"
#include "amf_config.hpp"
#include "amf_module_from_config.hpp"
#include "amf_statistics.hpp"
#include "itti.hpp"
#include "logger.hpp"
#include "ngap_app.hpp"
#include "normalizer.hh"
#include "options.hpp"
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"

using namespace config;
using namespace amf_application;

amf_config amf_cfg;
amf_modules modules;
itti_mw* itti_inst    = nullptr;
amf_app* amf_app_inst = nullptr;
statistics stacs;

AMFApiServer* amf_api_server_1     = nullptr;
amf_http2_server* amf_api_server_2 = nullptr;

//------------------------------------------------------------------------------
void amf_signal_handler(int s) {
  std::cout << "Caught signal " << s << std::endl;
  Logger::system().startup("exiting");
  if (itti_inst) {
    itti_inst->send_terminate_msg(TASK_AMF_APP);
    itti_inst->wait_tasks_end();
  }
  std::cout << "Freeing Allocated memory..." << std::endl;
  if (amf_app_inst) {
    delete amf_app_inst;
    amf_app_inst = nullptr;
    std::cout << "AMF APP memory done." << std::endl;
  }
  if (amf_api_server_1) {
    amf_api_server_1->shutdown();
    delete amf_api_server_1;
    amf_api_server_1 = nullptr;
  }
  if (amf_api_server_2) {
    amf_api_server_2->stop();
    delete amf_api_server_2;
    amf_api_server_2 = nullptr;
  }
  std::cout << "AMF API Server memory done." << std::endl;

  if (itti_inst) {
    delete itti_inst;
    itti_inst = nullptr;
    std::cout << "ITTI memory done." << std::endl;
  }
  std::cout << "Freeing Allocated memory done" << std::endl;
  exit(s);
}

//------------------------------------------------------------------------------
int main(int argc, char** argv) {
  srand(time(NULL));

  if (!Options::parse(argc, argv)) {
    std::cout << "Options::parse() failed" << std::endl;
    return 1;
  }

  Logger::init("AMF", Options::getlogStdout(), Options::getlogRotFilelog());
  Logger::amf_app().startup("Options parsed!");

  // TODO: to be optimized
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = amf_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  amf_cfg.load(Options::getlibconfigConfig());
  amf_cfg.display();
  modules.load(Options::getlibconfigConfig());
  modules.display();

  itti_inst = new itti_mw();
  itti_inst->start(amf_cfg.itti.itti_timer_sched_params);

  amf_app_inst = new amf_app(amf_cfg);
  amf_app_inst->allRegistredModulesInit(modules);

  Logger::amf_app().debug("Initiating AMF server endpoints");
  // AMF HTTP1 server
  Pistache::Address addr(
      std::string(inet_ntoa(*((struct in_addr*) &amf_cfg.n11.addr4))),
      Pistache::Port(amf_cfg.n11.port));
  amf_api_server_1 = new AMFApiServer(addr, amf_app_inst);
  amf_api_server_1->init(2);
  // std::thread amf_http1_manager(&AMFApiServer::start, amf_api_server_1);
  amf_api_server_1->start();
  // AMF HTTP2 server
  amf_api_server_2 = new amf_http2_server(
      conv::toString(amf_cfg.n11.addr4), amf_cfg.sbi_http2_port, amf_app_inst);
  amf_api_server_2->init(1);
  // std::thread amf_http2_manager(&amf_http2_server::start, amf_api_server_2);
  amf_api_server_2->start();

  // amf_http1_manager.join();
  // amf_http2_manager.join();

  Logger::amf_app().debug("Initiation Done!");
  pause();
  return 0;
}
