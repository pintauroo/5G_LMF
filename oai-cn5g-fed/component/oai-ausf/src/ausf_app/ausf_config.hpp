/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this
 *file except in compliance with the License. You may obtain a copy of the
 *License at
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

/*! \file ausf_config.hpp
 \brief
 \author  Fengjiao He, BUPT
 \date 2021
 \email: contact@openairinterface.org
 */

#ifndef _AUSF_CONFIG_H_
#define _AUSF_CONFIG_H_

#include "ausf_config.hpp"

#include <arpa/inet.h>
#include <libconfig.h++>
#include <netinet/in.h>
#include <sys/socket.h>
#include <mutex>
#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#define AUSF_CONFIG_STRING_AUSF_CONFIG "AUSF"
#define AUSF_CONFIG_STRING_PID_DIRECTORY "PID_DIRECTORY"
#define AUSF_CONFIG_STRING_INSTANCE_ID "INSTANCE_ID"
#define AUSF_CONFIG_STRING_AUSF_NAME "AUSF_NAME"

#define AUSF_CONFIG_STRING_INTERFACES "INTERFACES"
#define AUSF_CONFIG_STRING_INTERFACE_SBI "SBI"
#define AUSF_CONFIG_STRING_SBI_HTTP2_PORT "HTTP2_PORT"

#define AUSF_CONFIG_STRING_INTERFACE_NAME "INTERFACE_NAME"
#define AUSF_CONFIG_STRING_IPV4_ADDRESS "IPV4_ADDRESS"
#define AUSF_CONFIG_STRING_PORT "PORT"
#define AUSF_CONFIG_STRING_API_VERSION "API_VERSION"

#define AUSF_CONFIG_STRING_UDM "UDM"
#define AUSF_CONFIG_STRING_UDM_IPV4_ADDRESS "IPV4_ADDRESS"
#define AUSF_CONFIG_STRING_UDM_PORT "PORT"

#define AUSF_CONFIG_STRING_NRF "NRF"
#define AUSF_CONFIG_STRING_NRF_IPV4_ADDRESS "IPV4_ADDRESS"
#define AUSF_CONFIG_STRING_NRF_PORT "PORT"

#define AUSF_CONFIG_STRING_SUPPORT_FEATURES "SUPPORT_FEATURES"
#define AUSF_CONFIG_STRING_SUPPORT_FEATURES_USE_FQDN_DNS "USE_FQDN_DNS"
#define AUSF_CONFIG_STRING_SUPPORT_FEATURES_USE_HTTP2 "USE_HTTP2"
#define AUSF_CONFIG_STRING_SUPPORTED_FEATURES_REGISTER_NRF "REGISTER_NRF"
#define AUSF_CONFIG_STRING_FQDN_DNS "FQDN"

using namespace libconfig;

namespace config {

typedef struct interface_cfg_s {
  std::string if_name;
  struct in_addr addr4;
  struct in_addr network4;
  struct in6_addr addr6;
  unsigned int mtu;
  unsigned int port;
} interface_cfg_t;

class ausf_config {
 public:
  ausf_config();
  ~ausf_config();
  int load(const std::string& config_file);
  int load_interface(const Setting& if_cfg, interface_cfg_t& cfg);
  void display();

  unsigned int instance;
  std::string pid_dir;
  std::string ausf_name;

  interface_cfg_t sbi;
  unsigned int sbi_http2_port;
  std::string sbi_api_version;

  struct {
    struct in_addr ipv4_addr;
    unsigned int port;
    std::string api_version;
    std::string fqdn;
  } udm_addr;

  struct {
    struct in_addr ipv4_addr;
    unsigned int port;
    std::string api_version;
    std::string fqdn;
  } nrf_addr;

  bool register_nrf;
  ;
  bool use_fqdn_dns;
  bool use_http2;
};

}  // namespace config

#endif
