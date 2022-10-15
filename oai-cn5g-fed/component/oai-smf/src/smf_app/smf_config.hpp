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

/*! \file smf_config.hpp
 * \brief
 \author  Lionel GAUTHIER, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2019
 \email: lionel.gauthier@eurecom.fr, tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_SMF_CONFIG_HPP_SEEN
#define FILE_SMF_CONFIG_HPP_SEEN

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <libconfig.h++>
#include <mutex>
#include <vector>
#include "thread_sched.hpp"

#include "3gpp_29.244.h"
#include "pfcp.hpp"
#include "smf.h"

#define SMF_CONFIG_STRING_SMF_CONFIG "SMF"
#define SMF_CONFIG_STRING_PID_DIRECTORY "PID_DIRECTORY"
#define SMF_CONFIG_STRING_INSTANCE "INSTANCE"
#define SMF_CONFIG_STRING_FQDN_DNS "FQDN"
#define SMF_CONFIG_STRING_INTERFACES "INTERFACES"
#define SMF_CONFIG_STRING_INTERFACE_NAME "INTERFACE_NAME"
#define SMF_CONFIG_STRING_IPV4_ADDRESS "IPV4_ADDRESS"
#define SMF_CONFIG_STRING_PORT "PORT"
#define SMF_CONFIG_STRING_INTERFACE_N4 "N4"
#define SMF_CONFIG_STRING_INTERFACE_SBI "SBI"
#define SMF_CONFIG_STRING_SBI_HTTP2_PORT "HTTP2_PORT"
#define SMF_CONFIG_STRING_API_VERSION "API_VERSION"

#define SMF_CONFIG_STRING_IP_ADDRESS_POOL "IP_ADDRESS_POOL"
#define SMF_CONFIG_STRING_ARP_UE "ARP_UE"
#define SMF_CONFIG_STRING_ARP_UE_CHOICE_NO "NO"
#define SMF_CONFIG_STRING_ARP_UE_CHOICE_LINUX "LINUX"
#define SMF_CONFIG_STRING_ARP_UE_CHOICE_OAI "OAI"
#define SMF_CONFIG_STRING_IPV4_ADDRESS_LIST "IPV4_LIST"
#define SMF_CONFIG_STRING_IPV6_ADDRESS_LIST "IPV6_LIST"
#define SMF_CONFIG_STRING_RANGE "RANGE"
#define SMF_CONFIG_STRING_PREFIX "PREFIX"
#define SMF_CONFIG_STRING_IPV4_ADDRESS_RANGE_DELIMITER "-"
#define SMF_CONFIG_STRING_IPV6_ADDRESS_PREFIX_DELIMITER "/"
#define SMF_CONFIG_STRING_DEFAULT_DNS_IPV4_ADDRESS "DEFAULT_DNS_IPV4_ADDRESS"
#define SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV4_ADDRESS                         \
  "DEFAULT_DNS_SEC_IPV4_ADDRESS"
#define SMF_CONFIG_STRING_DEFAULT_DNS_IPV6_ADDRESS "DEFAULT_DNS_IPV6_ADDRESS"
#define SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV6_ADDRESS                         \
  "DEFAULT_DNS_SEC_IPV6_ADDRESS"
#define SMF_CONFIG_STRING_UE_MTU "UE_MTU"

#define SMF_CONFIG_STRING_INTERFACE_DISABLED "none"

#define SMF_CONFIG_STRING_DNN_LIST "DNN_LIST"
#define SMF_CONFIG_STRING_DNN_NI "DNN_NI"
#define SMF_CONFIG_STRING_PDU_SESSION_TYPE "PDU_SESSION_TYPE"
#define SMF_CONFIG_STRING_IPV4_POOL "IPV4_POOL"
#define SMF_CONFIG_STRING_IPV6_POOL "IPV6_POOL"
#define SMF_CONFIG_STRING_IPV4_RANGE "IPV4_RANGE"
#define SMF_CONFIG_STRING_IPV6_PREFIX "IPV6_PREFIX"

#define SMF_ABORT_ON_ERROR true
#define SMF_WARN_ON_ERROR false

#define SMF_CONFIG_STRING_SCHED_PARAMS "SCHED_PARAMS"
#define SMF_CONFIG_STRING_THREAD_RD_CPU_ID "CPU_ID"
#define SMF_CONFIG_STRING_THREAD_RD_SCHED_POLICY "SCHED_POLICY"
#define SMF_CONFIG_STRING_THREAD_RD_SCHED_PRIORITY "SCHED_PRIORITY"

#define SMF_CONFIG_STRING_ITTI_TASKS "ITTI_TASKS"
#define SMF_CONFIG_STRING_ITTI_TIMER_SCHED_PARAMS "ITTI_TIMER_SCHED_PARAMS"
#define SMF_CONFIG_STRING_S11_SCHED_PARAMS "S11_SCHED_PARAMS"
#define SMF_CONFIG_STRING_N4_SCHED_PARAMS "N4_SCHED_PARAMS"
#define SMF_CONFIG_STRING_SMF_APP_SCHED_PARAMS "SMF_APP_SCHED_PARAMS"
#define SMF_CONFIG_STRING_ASYNC_CMD_SCHED_PARAMS "ASYNC_CMD_SCHED_PARAMS"

#define SMF_CONFIG_STRING_AMF "AMF"
#define SMF_CONFIG_STRING_AMF_IPV4_ADDRESS "IPV4_ADDRESS"
#define SMF_CONFIG_STRING_AMF_PORT "PORT"
#define SMF_CONFIG_STRING_UDM "UDM"
#define SMF_CONFIG_STRING_UDM_IPV4_ADDRESS "IPV4_ADDRESS"
#define SMF_CONFIG_STRING_UDM_PORT "PORT"

#define SMF_CONFIG_STRING_UPF_LIST "UPF_LIST"
#define SMF_CONFIG_STRING_UPF_IPV4_ADDRESS "IPV4_ADDRESS"

#define SMF_CONFIG_STRING_NRF "NRF"
#define SMF_CONFIG_STRING_NRF_IPV4_ADDRESS "IPV4_ADDRESS"
#define SMF_CONFIG_STRING_NRF_PORT "PORT"

#define SMF_CONFIG_STRING_NWI_LIST "NWI_LIST"
#define SMF_CONFIG_STRING_DOMAIN_ACCESS "DOMAIN_ACCESS"
#define SMF_CONFIG_STRING_DOMAIN_CORE "DOMAIN_CORE"
#define SMF_CONFIG_STRING_DOMAIN_SGI_LAN "DOMAIN_SGI_LAN"

#define SMF_CONFIG_STRING_LOCAL_CONFIGURATION "LOCAL_CONFIGURATION"
#define SMF_CONFIG_STRING_SESSION_MANAGEMENT_SUBSCRIPTION_LIST                 \
  "SESSION_MANAGEMENT_SUBSCRIPTION_LIST"
#define SMF_CONFIG_STRING_NSSAI_SST "NSSAI_SST"
#define SMF_CONFIG_STRING_NSSAI_SD "NSSAI_SD"
#define SMF_CONFIG_STRING_DNN "DNN"
#define SMF_CONFIG_STRING_DEFAULT_SESSION_TYPE "DEFAULT_SESSION_TYPE"
#define SMF_CONFIG_STRING_DEFAULT_SSC_MODE "DEFAULT_SSC_MODE"
#define SMF_CONFIG_STRING_QOS_PROFILE_5QI "QOS_PROFILE_5QI"
#define SMF_CONFIG_STRING_QOS_PROFILE_PRIORITY_LEVEL                           \
  "QOS_PROFILE_PRIORITY_LEVEL"
#define SMF_CONFIG_STRING_QOS_PROFILE_ARP_PRIORITY_LEVEL                       \
  "QOS_PROFILE_ARP_PRIORITY_LEVEL"
#define SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTCAP                           \
  "QOS_PROFILE_ARP_PREEMPTCAP"
#define SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTVULN                          \
  "QOS_PROFILE_ARP_PREEMPTVULN"
#define SMF_CONFIG_STRING_SESSION_AMBR_UL "SESSION_AMBR_UL"
#define SMF_CONFIG_STRING_SESSION_AMBR_DL "SESSION_AMBR_DL"

#define SMF_CONFIG_STRING_SUPPORT_FEATURES "SUPPORT_FEATURES"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_REGISTER_NRF "REGISTER_NRF"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_DISCOVER_UPF "DISCOVER_UPF"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_LOCAL_SUBSCRIPTION_INFO         \
  "USE_LOCAL_SUBSCRIPTION_INFO"
#define SMF_CONFIG_STRING_NAS_FORCE_PUSH_PCO                                   \
  "FORCE_PUSH_PROTOCOL_CONFIGURATION_OPTIONS"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_FQDN_DNS "USE_FQDN_DNS"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_SBI_HTTP_VERSION "HTTP_VERSION"
#define SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_NETWORK_INSTANCE                \
  "USE_NETWORK_INSTANCE"

#define SMF_MAX_ALLOCATED_PDN_ADDRESSES 1024

namespace smf {

typedef struct interface_cfg_s {
  std::string if_name;
  struct in_addr addr4;
  struct in_addr network4;
  struct in6_addr addr6;
  unsigned int mtu;
  unsigned int port;
  util::thread_sched_params thread_rd_sched_params;
} interface_cfg_t;

typedef struct itti_cfg_s {
  util::thread_sched_params itti_timer_sched_params;
  util::thread_sched_params n4_sched_params;
  util::thread_sched_params smf_app_sched_params;
  util::thread_sched_params async_cmd_sched_params;
} itti_cfg_t;

typedef struct dnn_s {
  std::string dnn;
  std::string dnn_label;
  bool is_ipv4;
  bool is_ipv6;
  int pool_id_iv4;
  int pool_id_iv6;
  struct in_addr ue_pool_range_low;
  struct in_addr ue_pool_range_high;
  struct in6_addr paa_pool6_prefix;
  uint8_t paa_pool6_prefix_len;
  pdu_session_type_t pdu_session_type;
} dnn_t;

typedef struct session_management_subscription_s {
  snssai_t single_nssai;
  std::string session_type;
  std::string dnn;
  uint8_t ssc_mode;
  subscribed_default_qos_t default_qos;
  session_ambr_t session_ambr;
} session_management_subscription_t;

class smf_config {
 private:
  int load_itti(const libconfig::Setting& itti_cfg, itti_cfg_t& cfg);
  int load_interface(const libconfig::Setting& if_cfg, interface_cfg_t& cfg);
  int load_thread_sched_params(
      const libconfig::Setting& thread_sched_params_cfg,
      util::thread_sched_params& cfg);

 public:
  /* Reader/writer lock for this configuration */
  std::mutex m_rw_lock;
  std::string pid_dir;
  unsigned int instance = 0;
  std::string fqdn      = {};

  interface_cfg_t n4;
  interface_cfg_t sbi;
  unsigned int sbi_http2_port;
  std::string sbi_api_version;
  itti_cfg_t itti;

  struct in_addr default_dnsv4;
  struct in_addr default_dns_secv4;
  struct in6_addr default_dnsv6;
  struct in6_addr default_dns_secv6;

  std::map<std::string, dnn_t> dnns;

  bool force_push_pco;
  uint ue_mtu;

  bool register_nrf;
  bool discover_upf;
  bool use_local_subscription_info;
  bool use_fqdn_dns;
  unsigned int http_version;
  bool use_nwi;

  struct {
    struct in_addr ipv4_addr;
    unsigned int port;
    std::string api_version;
    std::string fqdn;
  } amf_addr;

  struct {
    struct in_addr ipv4_addr;
    unsigned int port;
    std::string api_version;
    std::string fqdn;
  } udm_addr;

  std::vector<pfcp::node_id_t> upfs;

  struct {
    struct in_addr ipv4_addr;
    unsigned int port;
    unsigned int http_version;
    std::string api_version;
    std::string fqdn;
  } nrf_addr;

  // Network instance
  // bool network_instance_configuration;
  struct upf_nwi_list_s {
    pfcp::node_id_t upf_id;
    std::string domain_access;
    std::string domain_core;
    //      std::string domain_sgi_lan;
  };
  typedef struct upf_nwi_list_s upf_nwi_list_t;

  std::vector<upf_nwi_list_t> upf_nwi_list;

  std::vector<session_management_subscription_t>
      session_management_subscriptions;

  smf_config()
      : m_rw_lock(), pid_dir(), instance(0), n4(), sbi(), itti(), upfs() {
    default_dnsv4.s_addr     = INADDR_ANY;
    default_dns_secv4.s_addr = INADDR_ANY;
    default_dnsv6            = in6addr_any;
    default_dns_secv6        = in6addr_any;

    force_push_pco = true;
    ue_mtu         = 1358;

    itti.itti_timer_sched_params.sched_priority = 85;
    itti.n4_sched_params.sched_priority         = 84;
    itti.smf_app_sched_params.sched_priority    = 84;
    itti.async_cmd_sched_params.sched_priority  = 84;

    n4.thread_rd_sched_params.sched_priority = 90;
    n4.port                                  = pfcp::default_port;

    sbi.thread_rd_sched_params.sched_priority = 90;
    sbi.port                                  = 80;

    amf_addr.ipv4_addr.s_addr = INADDR_ANY;
    amf_addr.port             = 80;
    amf_addr.api_version      = "v1";
    amf_addr.fqdn             = {};

    udm_addr.ipv4_addr.s_addr = INADDR_ANY;
    udm_addr.port             = 80;
    udm_addr.api_version      = "v1";
    udm_addr.fqdn             = {};

    nrf_addr.ipv4_addr.s_addr = INADDR_ANY;
    nrf_addr.port             = 80;
    nrf_addr.api_version      = "v1";
    nrf_addr.fqdn             = {};

    sbi_http2_port  = 8080;
    sbi_api_version = "v1";
    http_version    = 1;

    use_local_subscription_info = false;
    register_nrf                = false;
    discover_upf                = false;
    use_fqdn_dns                = false;
    use_nwi                     = false;
  };
  ~smf_config();
  void lock() { m_rw_lock.lock(); };
  void unlock() { m_rw_lock.unlock(); };
  int load(const std::string& config_file);
  void display();
  int get_pfcp_node_id(pfcp::node_id_t& node_id);
  int get_pfcp_fseid(pfcp::fseid_t& fseid);
  bool is_dotted_dnn_handled(
      const std::string& dnn, const pdu_session_type_t& pdn_session_type);
  std::string get_default_dnn();
  bool get_nwi_list_index(
      bool nwi_enabled, uint8_t nwi_list_index, pfcp::node_id_t node_id);
  std::string get_nwi(
      const std::vector<interface_upf_info_item_t>& int_list,
      const std::string& int_type);
};

}  // namespace smf

#endif /* FILE_SMF_CONFIG_HPP_SEEN */
