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

/*! \file nssf_config.hpp
 * \brief
 * \author Lionel Gauthier
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */

#ifndef FILE_nssf_config_HPP_SEEN
#define FILE_nssf_config_HPP_SEEN

#include "3gpp_29.510.h"
#include "NetworkSliceInformationDocumentApiImpl.h"
#include <libconfig.h++>
#include <mutex>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <stdbool.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

namespace nssf {

#define NSSF_CONFIG_STRING_NSSF_CONFIG "NSSF"
#define NSSF_CONFIG_STRING_FQDN "FQDN"
#define NSSF_CONFIG_STRING_NSSF_SLICE_CONFIG "NSSF_SLICE_CONFIG"
#define NSSF_CONFIG_STRING_INTERFACES "INTERFACES"
#define NSSF_CONFIG_STRING_INTERFACE_NAME "INTERFACE_NAME"
#define NSSF_CONFIG_STRING_IPV4_ADDRESS "IPV4_ADDRESS"
#define NSSF_CONFIG_STRING_SBI_PORT_HTTP1 "HTTP1_PORT"
#define NSSF_CONFIG_STRING_SBI_PORT_HTTP2 "HTTP2_PORT"
#define NSSF_CONFIG_STRING_SBI_INTERFACE "SBI"
#define NSSF_CONFIG_STRING_SBI_API_VERSION "API_VERSION"

#define NSSF_CONFIG_STRING_NETWORK_IPV4 "NETWORK_IPV4"
#define NSSF_CONFIG_STRING_NETWORK_IPV6 "NETWORK_IPV6"
#define NSSF_CONFIG_STRING_ADDRESS_PREFIX_DELIMITER "/"
#define NSSF_CONFIG_STRING_ITTI_TASKS "ITTI_TASKS"
#define NSSF_CONFIG_STRING_ITTI_TIMER_SCHED_PARAMS "ITTI_TIMER_SCHED_PARAMS"
#define NSSF_CONFIG_STRING_SBI_SCHED_PARAMS "SBI_SCHED_PARAMS"

#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES "SUPPORTED_FEATURES"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_REGISTER_NRF "REGISTER_NRF"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_NRF "NRF"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_NRF_IPV4_ADDRESS "IPV4_ADDRESS"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_NRF_PORT "PORT"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_NRF_API_VERSION "API_VERSION"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_NRF_HTTP_VERSION "HTTP_VERSION"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_HTTP_VERSION "HTTP_VERSION"
#define NSSF_CONFIG_STRING_SUPPORTED_FEATURES_USE_FQDN "USE_FQDN"

typedef struct interface_cfg_s {
  std::string if_name;
  struct in_addr addr4;
  struct in_addr network4;
  struct in6_addr addr6;
  unsigned int mtu;
  unsigned int http1_port;
  unsigned int http2_port;
} interface_cfg_t;

typedef struct nsi_info_s {
  Snssai snssai;
  NsiInformation nsiInfo;
} nsi_info_t;

typedef struct nssf_nsi_info_cfg_s {
  std::vector<nsi_info_t> nsiInfoList;
} nssf_nsi_info_t;

typedef struct ta_info_s {
  std::vector<Snssai> supoorted_snssai;
  Tai tai;
} ta_info_t;

typedef struct nssf_ta_info_cfg_s {
  std::vector<ta_info_t> taInfoList;
} nssf_ta_info_t;

class nssf_config {
 private:
  int load_interface(const libconfig::Setting& if_cfg, interface_cfg_t& cfg);

  static const bool ParseNsiInfo(
      const nlohmann::json& conf, nssf_nsi_info_t& cfg);

  static const bool ParseTaInfo(
      const nlohmann::json& conf, nssf_ta_info_t& cfg);
  static nlohmann::json nssf_slice_config;

 public:
  /* Reader/writer lock for this configuration */
  std::mutex m_rw_lock;
  std::string pid_dir;
  unsigned int instance;
  std::string fqdn;
  interface_cfg_t sbi;
  std::string sbi_api_version;

  std::string gateway;

  static nssf_nsi_info_t nssf_nsi_info;
  static nssf_ta_info_t nssf_ta_info;

  struct {
    bool register_nrf;
    bool use_fqdn;
    struct {
      struct in_addr ipv4_addr;
      unsigned int port;
      unsigned int http_version;
      std::string api_version;
      std::string fqdn;
    } nrf_addr;
  } nssf_features;

  static std::string slice_config_file;

  nssf_config() : m_rw_lock(), pid_dir(), instance(0) {
    sbi.http1_port = 9090;
    sbi.http2_port = 80;
  };

  void lock() { m_rw_lock.lock(); };
  void unlock() { m_rw_lock.unlock(); };
  int load(const std::string& config_file);
  int execute();
  void display();

  static bool ParseJson();
  static bool ValidateTA(Tai tai, std::vector<Snssai> rejected_snssai);
  static bool ValidateTA(const Tai& tai);
  static bool ValidateNSI(
      const SliceInfoForPDUSession& slice_info, NsiInformation& nsi_info);

  static bool get_slice_config(nlohmann::json& slice_config);
  static bool get_api_list(nlohmann::json& api_list);
};
}  // namespace nssf

#endif /* FILE_nssf_config_HPP_SEEN */
