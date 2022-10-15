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

#ifndef _UDR_CONFIG_H_
#define _UDR_CONFIG_H_

#include <arpa/inet.h>
#include <libconfig.h++>
#include <netinet/in.h>
#include <string>

#define UDR_CONFIG_STRING_UDR_CONFIG "UDR"
#define UDR_CONFIG_STRING_INSTANCE_ID "INSTANCE_ID"
#define UDR_CONFIG_STRING_PID_DIRECTORY "PID_DIRECTORY"
#define UDR_CONFIG_STRING_INTERFACES "INTERFACES"
#define UDR_CONFIG_STRING_INTERFACE_NUDR "NUDR"
#define UDR_CONFIG_STRING_INTERFACE_NAME "INTERFACE_NAME"
#define UDR_CONFIG_STRING_IPV4_ADDRESS "IPV4_ADDRESS"
#define UDR_CONFIG_STRING_PORT "PORT"
#define UDR_CONFIG_STRING_HTTP2_PORT "HTTP2_PORT"
#define UDR_CONFIG_STRING_API_VERSION "API_VERSION"

#define UDR_CONFIG_STRING_MYSQL "MYSQL"
#define UDR_CONFIG_STRING_MYSQL_SERVER "MYSQL_SERVER"
#define UDR_CONFIG_STRING_MYSQL_USER "MYSQL_USER"
#define UDR_CONFIG_STRING_MYSQL_PASS "MYSQL_PASS"
#define UDR_CONFIG_STRING_MYSQL_DB "MYSQL_DB"

using namespace libconfig;

namespace oai::udr::config {

typedef struct {
  std::string mysql_server;
  std::string mysql_user;
  std::string mysql_pass;
  std::string mysql_db;
} mysql_conf_t;

typedef struct interface_cfg_s {
  std::string if_name;
  struct in_addr addr4;
  struct in_addr network4;
  struct in6_addr addr6;
  unsigned int mtu;
  unsigned int port;
  std::string api_version;

} interface_cfg_t;

class udr_config {
public:
  udr_config();
  ~udr_config();

  int load(const std::string &config_file);
  int load_interface(const Setting &if_cfg, interface_cfg_t &cfg);
  void display();

  unsigned int instance;
  std::string pid_dir;
  interface_cfg_t nudr;
  unsigned int nudr_http2_port;

  mysql_conf_t mysql;
};
} // namespace oai::udr::config

#endif
