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

/*! \file smf_config.cpp
 \brief
 \author  Lionel GAUTHIER, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2019
 \email: lionel.gauthier@eurecom.fr, tien-thinh.nguyen@eurecom.fr
 */

#include "smf_config.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "string.hpp"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "common_defs.h"
#include "epc.h"
#include "if.hpp"
#include "logger.hpp"
#include "fqdn.hpp"
#include "smf_app.hpp"

using namespace std;
using namespace libconfig;
using namespace smf;

extern smf_config smf_cfg;

//------------------------------------------------------------------------------
int smf_config::load_thread_sched_params(
    const Setting& thread_sched_params_cfg, util::thread_sched_params& cfg) {
  try {
    thread_sched_params_cfg.lookupValue(
        SMF_CONFIG_STRING_THREAD_RD_CPU_ID, cfg.cpu_id);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }
  try {
    std::string thread_rd_sched_policy;
    thread_sched_params_cfg.lookupValue(
        SMF_CONFIG_STRING_THREAD_RD_SCHED_POLICY, thread_rd_sched_policy);
    util::trim(thread_rd_sched_policy);
    if (boost::iequals(thread_rd_sched_policy, "SCHED_OTHER")) {
      cfg.sched_policy = SCHED_OTHER;
    } else if (boost::iequals(thread_rd_sched_policy, "SCHED_IDLE")) {
      cfg.sched_policy = SCHED_IDLE;
    } else if (boost::iequals(thread_rd_sched_policy, "SCHED_BATCH")) {
      cfg.sched_policy = SCHED_BATCH;
    } else if (boost::iequals(thread_rd_sched_policy, "SCHED_FIFO")) {
      cfg.sched_policy = SCHED_FIFO;
    } else if (boost::iequals(thread_rd_sched_policy, "SCHED_RR")) {
      cfg.sched_policy = SCHED_RR;
    } else {
      Logger::smf_app().error(
          "thread_rd_sched_policy: %s, unknown in config file",
          thread_rd_sched_policy.c_str());
      return RETURNerror;
    }
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    thread_sched_params_cfg.lookupValue(
        SMF_CONFIG_STRING_THREAD_RD_SCHED_PRIORITY, cfg.sched_priority);
    if ((cfg.sched_priority > 99) || (cfg.sched_priority < 1)) {
      Logger::smf_app().error(
          "thread_rd_sched_priority: %d, must be in interval [1..99] in config "
          "file",
          cfg.sched_priority);
      return RETURNerror;
    }
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }
  return RETURNok;
}
//------------------------------------------------------------------------------
int smf_config::load_itti(const Setting& itti_cfg, itti_cfg_t& cfg) {
  try {
    const Setting& itti_timer_sched_params_cfg =
        itti_cfg[SMF_CONFIG_STRING_ITTI_TIMER_SCHED_PARAMS];
    load_thread_sched_params(
        itti_timer_sched_params_cfg, cfg.itti_timer_sched_params);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& n4_sched_params_cfg =
        itti_cfg[SMF_CONFIG_STRING_N4_SCHED_PARAMS];
    load_thread_sched_params(n4_sched_params_cfg, cfg.n4_sched_params);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& smf_app_sched_params_cfg =
        itti_cfg[SMF_CONFIG_STRING_SMF_APP_SCHED_PARAMS];
    load_thread_sched_params(
        smf_app_sched_params_cfg, cfg.smf_app_sched_params);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& async_cmd_sched_params_cfg =
        itti_cfg[SMF_CONFIG_STRING_ASYNC_CMD_SCHED_PARAMS];
    load_thread_sched_params(
        async_cmd_sched_params_cfg, cfg.async_cmd_sched_params);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  return RETURNok;
}

//------------------------------------------------------------------------------
int smf_config::load_interface(const Setting& if_cfg, interface_cfg_t& cfg) {
  if_cfg.lookupValue(SMF_CONFIG_STRING_INTERFACE_NAME, cfg.if_name);
  util::trim(cfg.if_name);
  if (not boost::iequals(cfg.if_name, "none")) {
    std::string address = {};
    if_cfg.lookupValue(SMF_CONFIG_STRING_IPV4_ADDRESS, address);
    util::trim(address);
    if (boost::iequals(address, "read")) {
      if (get_inet_addr_infos_from_iface(
              cfg.if_name, cfg.addr4, cfg.network4, cfg.mtu)) {
        Logger::smf_app().error(
            "Could not read %s network interface configuration", cfg.if_name);
        return RETURNerror;
      }
    } else {
      std::vector<std::string> words = {};
      boost::split(
          words, address, boost::is_any_of("/"), boost::token_compress_on);
      if (words.size() != 2) {
        Logger::smf_app().error(
            "Bad value " SMF_CONFIG_STRING_IPV4_ADDRESS " = %s in config file",
            address.c_str());
        return RETURNerror;
      }
      unsigned char buf_in_addr[sizeof(struct in6_addr)];  // you never know...
      if (inet_pton(AF_INET, util::trim(words.at(0)).c_str(), buf_in_addr) ==
          1) {
        memcpy(&cfg.addr4, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::smf_app().error(
            "In conversion: Bad value " SMF_CONFIG_STRING_IPV4_ADDRESS
            " = %s in config file",
            util::trim(words.at(0)).c_str());
        return RETURNerror;
      }
      cfg.network4.s_addr = htons(
          ntohs(cfg.addr4.s_addr) &
          0xFFFFFFFF << (32 - std::stoi(util::trim(words.at(1)))));
    }
    if_cfg.lookupValue(SMF_CONFIG_STRING_PORT, cfg.port);

    try {
      const Setting& sched_params_cfg = if_cfg[SMF_CONFIG_STRING_SCHED_PARAMS];
      load_thread_sched_params(sched_params_cfg, cfg.thread_rd_sched_params);
    } catch (const SettingNotFoundException& nfex) {
      Logger::smf_app().info(
          "%s : %s, using defaults", nfex.what(), nfex.getPath());
    }
  }
  return RETURNok;
}

//------------------------------------------------------------------------------
int smf_config::load(const string& config_file) {
  Config cfg;
  unsigned char buf_in6_addr[sizeof(struct in6_addr)];

  // Read the file. If there is an error, report it and exit.
  try {
    cfg.readFile(config_file.c_str());
  } catch (const FileIOException& fioex) {
    Logger::smf_app().error(
        "I/O error while reading file %s - %s", config_file.c_str(),
        fioex.what());
    throw;
  } catch (const ParseException& pex) {
    Logger::smf_app().error(
        "Parse error at %s:%d - %s", pex.getFile(), pex.getLine(),
        pex.getError());
    throw;
  }

  const Setting& root = cfg.getRoot();

  try {
    const Setting& smf_cfg = root[SMF_CONFIG_STRING_SMF_CONFIG];
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  const Setting& smf_cfg = root[SMF_CONFIG_STRING_SMF_CONFIG];

  // Instance
  try {
    smf_cfg.lookupValue(SMF_CONFIG_STRING_INSTANCE, instance);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  // PID_DIR
  try {
    smf_cfg.lookupValue(SMF_CONFIG_STRING_PID_DIRECTORY, pid_dir);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  // FQDN
  try {
    smf_cfg.lookupValue(SMF_CONFIG_STRING_FQDN_DNS, fqdn);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& itti_cfg = smf_cfg[SMF_CONFIG_STRING_ITTI_TASKS];
    load_itti(itti_cfg, itti);
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().info(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& nw_if_cfg = smf_cfg[SMF_CONFIG_STRING_INTERFACES];

    const Setting& n4_cfg = nw_if_cfg[SMF_CONFIG_STRING_INTERFACE_N4];
    load_interface(n4_cfg, n4);

    const Setting& sbi_cfg = nw_if_cfg[SMF_CONFIG_STRING_INTERFACE_SBI];
    load_interface(sbi_cfg, sbi);

    // HTTP2 port
    if (!(sbi_cfg.lookupValue(
            SMF_CONFIG_STRING_SBI_HTTP2_PORT, sbi_http2_port))) {
      Logger::smf_app().error(SMF_CONFIG_STRING_SBI_HTTP2_PORT "failed");
      throw(SMF_CONFIG_STRING_SBI_HTTP2_PORT "failed");
    }

    // SBI API VERSION
    if (!(sbi_cfg.lookupValue(
            SMF_CONFIG_STRING_API_VERSION, sbi_api_version))) {
      Logger::smf_app().error(SMF_CONFIG_STRING_API_VERSION "failed");
      throw(SMF_CONFIG_STRING_API_VERSION "failed");
    }

  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  try {
    string astring = {};

    // DNN list
    const Setting& dnn_list_cfg = smf_cfg[SMF_CONFIG_STRING_DNN_LIST];
    int count                   = dnn_list_cfg.getLength();

    for (int i = 0; i < count; i++) {
      const Setting& dnn_cfg = dnn_list_cfg[i];
      dnn_cfg.lookupValue(SMF_CONFIG_STRING_DNN_NI, astring);
      dnn_t dnn_item                             = {};
      dnn_item.pdu_session_type.pdu_session_type = PDU_SESSION_TYPE_E_UNKNOWN;
      dnn_item.dnn                               = astring;
      dnn_item.dnn_label = EPC::Utility::dnn_label(astring);
      dnn_cfg.lookupValue(SMF_CONFIG_STRING_PDU_SESSION_TYPE, astring);
      if (boost::iequals(astring, "IPv4")) {
        dnn_item.pdu_session_type.pdu_session_type = PDU_SESSION_TYPE_E_IPV4;
      } else if (boost::iequals(astring, "IPv6")) {
        dnn_item.pdu_session_type.pdu_session_type = PDU_SESSION_TYPE_E_IPV6;
      } else if (boost::iequals(astring, "IPv4v6")) {
        dnn_item.pdu_session_type.pdu_session_type = PDU_SESSION_TYPE_E_IPV4V6;
      } else if (boost::iequals(astring, "Unstructured")) {
        dnn_item.pdu_session_type.pdu_session_type =
            PDU_SESSION_TYPE_E_UNSTRUCTURED;
      } else if (boost::iequals(astring, "Ethernet")) {
        dnn_item.pdu_session_type.pdu_session_type =
            PDU_SESSION_TYPE_E_ETHERNET;
      } else if (boost::iequals(astring, "Reserved")) {
        dnn_item.pdu_session_type.pdu_session_type =
            PDU_SESSION_TYPE_E_RESERVED;
      } else {
        Logger::smf_app().error(
            " " SMF_CONFIG_STRING_PDU_SESSION_TYPE " in %d'th DNN :%s", i + 1,
            astring.c_str());
        throw("Error PDU_SESSION_TYPE in config file");
      }

      string ipv4_range;
      unsigned char buf_in_addr[sizeof(struct in_addr)];

      dnn_cfg.lookupValue(SMF_CONFIG_STRING_IPV4_RANGE, ipv4_range);
      std::vector<std::string> ips;
      boost::split(
          ips, ipv4_range,
          boost::is_any_of(SMF_CONFIG_STRING_IPV4_ADDRESS_RANGE_DELIMITER),
          boost::token_compress_on);
      if (ips.size() != 2) {
        Logger::smf_app().error(
            "Bad value %s: %s in config file %s",
            SMF_CONFIG_STRING_IPV4_ADDRESS_RANGE_DELIMITER, ipv4_range.c_str(),
            config_file.c_str());
        throw(
            "Bad value %s: %s in config file %s",
            SMF_CONFIG_STRING_IPV4_ADDRESS_RANGE_DELIMITER, ipv4_range.c_str(),
            config_file.c_str());
      }

      memset(buf_in_addr, 0, sizeof(buf_in_addr));
      if (inet_pton(AF_INET, util::trim(ips.at(0)).c_str(), buf_in_addr) == 1) {
        memcpy(
            &dnn_item.ue_pool_range_low, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::smf_app().error(
            "CONFIG POOL ADDR IPV4: BAD LOWER ADDRESS "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST " pool %d",
            i);
        throw(
            "CONFIG POOL ADDR IPV4: BAD ADDRESS "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST);
      }

      memset(buf_in_addr, 0, sizeof(buf_in_addr));
      if (inet_pton(AF_INET, util::trim(ips.at(1)).c_str(), buf_in_addr) == 1) {
        memcpy(
            &dnn_item.ue_pool_range_high, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::smf_app().error(
            "CONFIG POOL ADDR IPV4: BAD HIGHER ADDRESS "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST " pool %d",
            i);
        throw(
            "CONFIG POOL ADDR IPV4: BAD ADDRESS "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST);
      }
      if (htonl(dnn_item.ue_pool_range_low.s_addr) >=
          htonl(dnn_item.ue_pool_range_high.s_addr)) {
        Logger::smf_app().error(
            "CONFIG POOL ADDR IPV4: BAD RANGE "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST " pool %d",
            i);
        throw(
            "CONFIG POOL ADDR IPV4: BAD RANGE "
            "in " SMF_CONFIG_STRING_IPV4_ADDRESS_LIST);
      }

      string ipv6_prefix = {};
      dnn_cfg.lookupValue(SMF_CONFIG_STRING_IPV6_PREFIX, ipv6_prefix);
      std::vector<std::string> ips6 = {};
      boost::split(
          ips6, ipv6_prefix,
          boost::is_any_of(SMF_CONFIG_STRING_IPV6_ADDRESS_PREFIX_DELIMITER),
          boost::token_compress_on);
      if (ips6.size() != 2) {
        Logger::smf_app().error(
            "Bad value %s: %s in config file %s", SMF_CONFIG_STRING_PREFIX,
            ipv6_prefix.c_str(), config_file.c_str());
        throw(
            "Bad value %s: %s in config file %s", SMF_CONFIG_STRING_PREFIX,
            ipv6_prefix.c_str(), config_file.c_str());
      }

      std::string addr = ips6.at(0);
      util::trim(addr);
      if (inet_pton(AF_INET6, addr.c_str(), buf_in6_addr) == 1) {
        memcpy(
            &dnn_item.paa_pool6_prefix, buf_in6_addr, sizeof(struct in6_addr));
      } else {
        Logger::smf_app().error(
            "CONFIG POOL ADDR IPV6: BAD ADDRESS "
            "in " SMF_CONFIG_STRING_IPV6_ADDRESS_LIST " pool %d",
            i);
        throw(
            "CONFIG POOL ADDR IPV6: BAD ADDRESS "
            "in " SMF_CONFIG_STRING_IPV6_ADDRESS_LIST);
      }

      std::string prefix = ips6.at(1);
      util::trim(prefix);
      dnn_item.paa_pool6_prefix_len = std::stoi(prefix);

      dnns.insert(std::pair<std::string, dnn_t>(dnn_item.dnn, dnn_item));
    }

    // DNS
    smf_cfg.lookupValue(SMF_CONFIG_STRING_DEFAULT_DNS_IPV4_ADDRESS, astring);
    IPV4_STR_ADDR_TO_INADDR(
        util::trim(astring).c_str(), default_dnsv4,
        "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS !");

    smf_cfg.lookupValue(
        SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV4_ADDRESS, astring);
    IPV4_STR_ADDR_TO_INADDR(
        util::trim(astring).c_str(), default_dns_secv4,
        "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS !");

    smf_cfg.lookupValue(SMF_CONFIG_STRING_DEFAULT_DNS_IPV6_ADDRESS, astring);
    if (inet_pton(AF_INET6, util::trim(astring).c_str(), buf_in6_addr) == 1) {
      memcpy(&default_dnsv6, buf_in6_addr, sizeof(struct in6_addr));
    } else {
      Logger::smf_app().error(
          "CONFIG : BAD ADDRESS in " SMF_CONFIG_STRING_DEFAULT_DNS_IPV6_ADDRESS
          " %s",
          astring.c_str());
      throw(
          "CONFIG : BAD ADDRESS in " SMF_CONFIG_STRING_DEFAULT_DNS_IPV6_ADDRESS
          " %s",
          astring.c_str());
    }
    smf_cfg.lookupValue(
        SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV6_ADDRESS, astring);
    if (inet_pton(AF_INET6, util::trim(astring).c_str(), buf_in6_addr) == 1) {
      memcpy(&default_dns_secv6, buf_in6_addr, sizeof(struct in6_addr));
    } else {
      Logger::smf_app().error(
          "CONFIG : BAD ADDRESS "
          "in " SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV6_ADDRESS " %s",
          astring.c_str());
      throw(
          "CONFIG : BAD ADDRESS "
          "in " SMF_CONFIG_STRING_DEFAULT_DNS_SEC_IPV6_ADDRESS " %s",
          astring.c_str());
    }

    // UE MTU
    smf_cfg.lookupValue(SMF_CONFIG_STRING_UE_MTU, ue_mtu);

    // Support features
    try {
      const Setting& support_features =
          smf_cfg[SMF_CONFIG_STRING_SUPPORT_FEATURES];
      string opt;
      unsigned int httpVersion = {0};
      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_REGISTER_NRF, opt);
      if (boost::iequals(opt, "yes")) {
        register_nrf = true;
      } else {
        register_nrf = false;
      }

      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_DISCOVER_UPF, opt);
      if (boost::iequals(opt, "yes")) {
        discover_upf = true;
      } else {
        discover_upf = false;
      }

      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_LOCAL_SUBSCRIPTION_INFO, opt);
      if (boost::iequals(opt, "yes")) {
        use_local_subscription_info = true;
      } else {
        use_local_subscription_info = false;
      }

      support_features.lookupValue(SMF_CONFIG_STRING_NAS_FORCE_PUSH_PCO, opt);
      if (boost::iequals(opt, "yes")) {
        force_push_pco = true;
      } else {
        force_push_pco = false;
      }

      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_FQDN_DNS, opt);
      if (boost::iequals(opt, "yes")) {
        use_fqdn_dns = true;
      } else {
        use_fqdn_dns = false;
      }

      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_SBI_HTTP_VERSION, httpVersion);
      http_version = httpVersion;

      support_features.lookupValue(
          SMF_CONFIG_STRING_SUPPORT_FEATURES_USE_NETWORK_INSTANCE, opt);
      if (boost::iequals(opt, "yes")) {
        use_nwi = true;
      } else {
        use_nwi = false;
      }

    } catch (const SettingNotFoundException& nfex) {
      Logger::smf_app().error(
          "%s : %s, using defaults", nfex.what(), nfex.getPath());
      return -1;
    }

    // AMF
    const Setting& amf_cfg       = smf_cfg[SMF_CONFIG_STRING_AMF];
    struct in_addr amf_ipv4_addr = {};
    unsigned int amf_port        = {0};
    std::string amf_api_version  = {};

    if (!use_fqdn_dns) {
      amf_cfg.lookupValue(SMF_CONFIG_STRING_AMF_IPV4_ADDRESS, astring);
      IPV4_STR_ADDR_TO_INADDR(
          util::trim(astring).c_str(), amf_ipv4_addr,
          "BAD IPv4 ADDRESS FORMAT FOR AMF !");
      amf_addr.ipv4_addr = amf_ipv4_addr;
      if (!(amf_cfg.lookupValue(SMF_CONFIG_STRING_AMF_PORT, amf_port))) {
        Logger::smf_app().error(SMF_CONFIG_STRING_AMF_PORT "failed");
        throw(SMF_CONFIG_STRING_AMF_PORT "failed");
      }
      amf_addr.port = amf_port;

      if (!(amf_cfg.lookupValue(
              SMF_CONFIG_STRING_API_VERSION, amf_api_version))) {
        Logger::smf_app().error(SMF_CONFIG_STRING_API_VERSION "failed");
        throw(SMF_CONFIG_STRING_API_VERSION "failed");
      }
      amf_addr.api_version = amf_api_version;
    } else {
      amf_cfg.lookupValue(SMF_CONFIG_STRING_FQDN_DNS, astring);
      uint8_t addr_type   = {0};
      std::string address = {};

      fqdn::resolve(astring, address, amf_port, addr_type);
      if (addr_type != 0) {  // IPv6
        // TODO:
        throw("DO NOT SUPPORT IPV6 ADDR FOR AMF!");
      } else {  // IPv4
        IPV4_STR_ADDR_TO_INADDR(
            util::trim(address).c_str(), amf_ipv4_addr,
            "BAD IPv4 ADDRESS FORMAT FOR AMF !");
        amf_addr.ipv4_addr = amf_ipv4_addr;
        // We hardcode amf port from config for the moment
        if (!(amf_cfg.lookupValue(SMF_CONFIG_STRING_AMF_PORT, amf_port))) {
          Logger::smf_app().error(SMF_CONFIG_STRING_AMF_PORT "failed");
          throw(SMF_CONFIG_STRING_AMF_PORT "failed");
        }
        amf_addr.port        = amf_port;
        amf_addr.api_version = "v1";  // TODO: to get API version from DNS
        amf_addr.fqdn        = astring;
      }
    }

    // UDM: Get UDM information if necessary
    if (!use_local_subscription_info) {
      const Setting& udm_cfg       = smf_cfg[SMF_CONFIG_STRING_UDM];
      struct in_addr udm_ipv4_addr = {};
      unsigned int udm_port        = {0};
      std::string udm_api_version  = {};

      if (!use_fqdn_dns) {
        udm_cfg.lookupValue(SMF_CONFIG_STRING_UDM_IPV4_ADDRESS, astring);
        IPV4_STR_ADDR_TO_INADDR(
            util::trim(astring).c_str(), udm_ipv4_addr,
            "BAD IPv4 ADDRESS FORMAT FOR UDM !");
        udm_addr.ipv4_addr = udm_ipv4_addr;
        if (!(udm_cfg.lookupValue(SMF_CONFIG_STRING_UDM_PORT, udm_port))) {
          Logger::smf_app().error(SMF_CONFIG_STRING_UDM_PORT "failed");
          throw(SMF_CONFIG_STRING_UDM_PORT "failed");
        }
        udm_addr.port = udm_port;

        if (!(udm_cfg.lookupValue(
                SMF_CONFIG_STRING_API_VERSION, udm_api_version))) {
          Logger::smf_app().error(SMF_CONFIG_STRING_API_VERSION "failed");
          throw(SMF_CONFIG_STRING_API_VERSION "failed");
        }
        udm_addr.api_version = udm_api_version;
      } else {
        udm_cfg.lookupValue(SMF_CONFIG_STRING_FQDN_DNS, astring);
        uint8_t addr_type   = {0};
        std::string address = {};

        fqdn::resolve(astring, address, udm_port, addr_type);
        if (addr_type != 0) {  // IPv6
          // TODO:
          throw("DO NOT SUPPORT IPV6 ADDR FOR UDM!");
        } else {  // IPv4
          IPV4_STR_ADDR_TO_INADDR(
              util::trim(address).c_str(), udm_ipv4_addr,
              "BAD IPv4 ADDRESS FORMAT FOR UDM !");
          udm_addr.ipv4_addr   = udm_ipv4_addr;
          udm_addr.port        = udm_port;
          udm_addr.api_version = "v1";  // TODO: to get API version from DNS
        }
      }
    }

    // UPF list
    if (!discover_upf) {
      unsigned char buf_in_addr[sizeof(struct in_addr) + 1];
      const Setting& upf_list_cfg = smf_cfg[SMF_CONFIG_STRING_UPF_LIST];
      count                       = upf_list_cfg.getLength();
      for (int i = 0; i < count; i++) {
        const Setting& upf_cfg = upf_list_cfg[i];
        // TODO FQDN
        string address    = {};
        pfcp::node_id_t n = {};
        if (!use_fqdn_dns) {
          if (upf_cfg.lookupValue(
                  SMF_CONFIG_STRING_UPF_IPV4_ADDRESS, address)) {
            // pfcp::node_id_t n = {};
            n.node_id_type = pfcp::NODE_ID_TYPE_IPV4_ADDRESS;  // actually
            if (inet_pton(AF_INET, util::trim(address).c_str(), buf_in_addr) ==
                1) {
              memcpy(&n.u1.ipv4_address, buf_in_addr, sizeof(struct in_addr));
            } else {
              Logger::smf_app().error(
                  "CONFIG: BAD IPV4 ADDRESS in " SMF_CONFIG_STRING_UPF_LIST
                  " item %d",
                  i);
              throw("CONFIG: BAD ADDRESS in " SMF_CONFIG_STRING_UPF_LIST);
            }
            upfs.push_back(n);
          } else {  // TODO IPV6_ADDRESS, FQDN
            throw(
                "Bad value in section %s: item no %d in config file %s",
                SMF_CONFIG_STRING_UPF_LIST, i, config_file.c_str());
          }

        } else {
          unsigned int upf_port = {0};

          upf_cfg.lookupValue(SMF_CONFIG_STRING_FQDN_DNS, astring);
          uint8_t addr_type   = {0};
          std::string address = {};
          fqdn::resolve(astring, address, upf_port, addr_type, "");
          if (addr_type != 0) {  // IPv6
            // TODO:
            throw("DO NOT SUPPORT IPV6 ADDR FOR NRF!");
          } else {                                             // IPv4
            n.node_id_type = pfcp::NODE_ID_TYPE_IPV4_ADDRESS;  // actually
            n.fqdn         = astring;
            if (inet_pton(AF_INET, util::trim(address).c_str(), buf_in_addr) ==
                1) {
              memcpy(&n.u1.ipv4_address, buf_in_addr, sizeof(struct in_addr));
            } else {
              Logger::smf_app().error(
                  "CONFIG: BAD IPV4 ADDRESS in " SMF_CONFIG_STRING_UPF_LIST
                  " item %d",
                  i);
              throw("CONFIG: BAD ADDRESS in " SMF_CONFIG_STRING_UPF_LIST);
            }
            upfs.push_back(n);
          }
        }
        // Network Instance
        if (upf_cfg.exists(SMF_CONFIG_STRING_NWI_LIST) & use_nwi) {
          const Setting& nwi_cfg = upf_cfg[SMF_CONFIG_STRING_NWI_LIST];
          count                  = nwi_cfg.getLength();
          // Check if NWI list for given UPF is present
          if (count > 0) {
            upf_nwi_list_t upf_nwi;
            nwi_cfg[0].lookupValue(
                SMF_CONFIG_STRING_DOMAIN_ACCESS, upf_nwi.domain_access);
            nwi_cfg[0].lookupValue(
                SMF_CONFIG_STRING_DOMAIN_CORE, upf_nwi.domain_core);
            upf_nwi.upf_id = n;
            Logger::smf_app().debug(
                "NWI config found for UP node:-\t Nwi access: %s , \t Nwi "
                "core: %s",
                upf_nwi.domain_access.c_str(), upf_nwi.domain_core.c_str());
            upf_nwi_list.push_back(upf_nwi);
          }
        }
      }
    }

    // NRF
    if (discover_upf or register_nrf) {
      const Setting& nrf_cfg       = smf_cfg[SMF_CONFIG_STRING_NRF];
      struct in_addr nrf_ipv4_addr = {};
      unsigned int nrf_port        = {0};
      std::string nrf_api_version  = {};

      if (!use_fqdn_dns) {
        nrf_cfg.lookupValue(SMF_CONFIG_STRING_NRF_IPV4_ADDRESS, astring);
        IPV4_STR_ADDR_TO_INADDR(
            util::trim(astring).c_str(), nrf_ipv4_addr,
            "BAD IPv4 ADDRESS FORMAT FOR NRF !");
        nrf_addr.ipv4_addr = nrf_ipv4_addr;
        if (!(nrf_cfg.lookupValue(SMF_CONFIG_STRING_NRF_PORT, nrf_port))) {
          Logger::smf_app().error(SMF_CONFIG_STRING_NRF_PORT "failed");
          throw(SMF_CONFIG_STRING_NRF_PORT "failed");
        }
        nrf_addr.port = nrf_port;

        if (!(nrf_cfg.lookupValue(
                SMF_CONFIG_STRING_API_VERSION, nrf_api_version))) {
          Logger::smf_app().error(SMF_CONFIG_STRING_API_VERSION "failed");
          throw(SMF_CONFIG_STRING_API_VERSION "failed");
        }
        nrf_addr.api_version = nrf_api_version;
      } else {
        nrf_cfg.lookupValue(SMF_CONFIG_STRING_FQDN_DNS, astring);
        uint8_t addr_type   = {0};
        std::string address = {};
        fqdn::resolve(astring, address, nrf_port, addr_type);
        if (addr_type != 0) {  // IPv6
          // TODO:
          throw("DO NOT SUPPORT IPV6 ADDR FOR NRF!");
        } else {  // IPv4
          IPV4_STR_ADDR_TO_INADDR(
              util::trim(address).c_str(), nrf_ipv4_addr,
              "BAD IPv4 ADDRESS FORMAT FOR NRF !");
          nrf_addr.ipv4_addr = nrf_ipv4_addr;
          // nrf_addr.port        = nrf_port;
          // We hardcode nrf port from config for the moment
          if (!(nrf_cfg.lookupValue(SMF_CONFIG_STRING_NRF_PORT, nrf_port))) {
            Logger::smf_app().error(SMF_CONFIG_STRING_NRF_PORT "failed");
            throw(SMF_CONFIG_STRING_NRF_PORT "failed");
          }
          nrf_addr.port        = nrf_port;
          nrf_addr.api_version = "v1";  // TODO: to get API version from DNS
          nrf_addr.fqdn        = astring;
        }
      }
    }

    // Local configuration
    if (use_local_subscription_info) {
      const Setting& local_cfg = smf_cfg[SMF_CONFIG_STRING_LOCAL_CONFIGURATION];

      const Setting& session_management_subscription_list_cfg =
          local_cfg[SMF_CONFIG_STRING_SESSION_MANAGEMENT_SUBSCRIPTION_LIST];
      count = session_management_subscription_list_cfg.getLength();
      for (int i = 0; i < count; i++) {
        const Setting& session_management_subscription_cfg =
            session_management_subscription_list_cfg[i];
        session_management_subscription_t sub_item = {};

        unsigned int nssai_sst                      = 0;
        string nssai_sd                             = {};
        string dnn                                  = {};
        string default_session_type                 = {};
        unsigned int default_ssc_mode               = 0;
        unsigned int qos_profile_5qi                = 0;
        unsigned int qos_profile_priority_level     = 0;
        unsigned int qos_profile_arp_priority_level = 0;
        string qos_profile_arp_preemptcap           = {};
        string qos_profile_arp_preemptvuln          = {};
        string session_ambr_ul                      = {};
        string session_ambr_dl                      = {};

        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_NSSAI_SST, nssai_sst);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_NSSAI_SD, nssai_sd);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_DNN, dnn);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_DEFAULT_SESSION_TYPE, default_session_type);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_DEFAULT_SSC_MODE, default_ssc_mode);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_QOS_PROFILE_5QI, qos_profile_5qi);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_QOS_PROFILE_PRIORITY_LEVEL,
            qos_profile_priority_level);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_QOS_PROFILE_ARP_PRIORITY_LEVEL,
            qos_profile_arp_priority_level);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTCAP,
            qos_profile_arp_preemptcap);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTVULN,
            qos_profile_arp_preemptvuln);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_SESSION_AMBR_UL, session_ambr_ul);
        session_management_subscription_cfg.lookupValue(
            SMF_CONFIG_STRING_SESSION_AMBR_DL, session_ambr_dl);

        sub_item.single_nssai.sST           = nssai_sst;
        sub_item.single_nssai.sD            = nssai_sd;
        sub_item.session_type               = default_session_type;
        sub_item.dnn                        = dnn;
        sub_item.ssc_mode                   = default_ssc_mode;
        sub_item.default_qos._5qi           = qos_profile_5qi;
        sub_item.default_qos.priority_level = qos_profile_priority_level;
        sub_item.default_qos.arp.priority_level =
            qos_profile_arp_priority_level;
        sub_item.default_qos.arp.preempt_cap  = qos_profile_arp_preemptcap;
        sub_item.default_qos.arp.preempt_vuln = qos_profile_arp_preemptvuln;
        sub_item.session_ambr.downlink        = session_ambr_dl;
        sub_item.session_ambr.uplink          = session_ambr_ul;
        session_management_subscriptions.push_back(sub_item);
      }
    }
  } catch (const SettingNotFoundException& nfex) {
    Logger::smf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }
  return RETURNok;
}

//------------------------------------------------------------------------------
void smf_config::display() {
  Logger::smf_app().info(
      "==== OAI-CN5G %s v%s ====", PACKAGE_NAME, PACKAGE_VERSION);
  Logger::smf_app().info("Configuration SMF:");
  Logger::smf_app().info("- Instance ..............: %d\n", instance);
  Logger::smf_app().info("- PID dir ...............: %s\n", pid_dir.c_str());

  Logger::smf_app().info("- N4 Networking:");
  Logger::smf_app().info("    Interface name ......: %s", n4.if_name.c_str());
  Logger::smf_app().info("    IPv4 Addr ...........: %s", inet_ntoa(n4.addr4));
  Logger::smf_app().info("    Port ................: %d", n4.port);

  Logger::smf_app().info("- SBI Networking:");
  Logger::smf_app().info("    Interface name ......: %s", sbi.if_name.c_str());
  Logger::smf_app().info("    IPv4 Addr ...........: %s", inet_ntoa(sbi.addr4));
  Logger::smf_app().info("    Port ................: %d", sbi.port);
  Logger::smf_app().info("    HTTP2 port ..........: %d", sbi_http2_port);
  Logger::smf_app().info(
      "    API version..........: %s", sbi_api_version.c_str());
  // TODO: Don't support threading/sched_policy for now
  /*
    Logger::smf_app().info("- N4 Threading:");
    Logger::smf_app().info(
        "    CPU id ..............: %d", n4.thread_rd_sched_params.cpu_id);
    Logger::smf_app().info(
        "    Scheduling policy ...: %d",
    n4.thread_rd_sched_params.sched_policy); Logger::smf_app().info( "
    Scheduling prio .....: %d", n4.thread_rd_sched_params.sched_priority);

    Logger::smf_app().info("- ITTI Timer Task Threading:");
    Logger::smf_app().info(
        "    CPU id ..............: %d", itti.itti_timer_sched_params.cpu_id);
    Logger::smf_app().info(
        "    Scheduling policy ...: %d",
        itti.itti_timer_sched_params.sched_policy);
    Logger::smf_app().info(
        "    Scheduling prio .....: %d",
        itti.itti_timer_sched_params.sched_priority);
    Logger::smf_app().info("- ITTI N4 Task Threading :");
    Logger::smf_app().info(
        "    CPU id ..............: %d", itti.n4_sched_params.cpu_id);
    Logger::smf_app().info(
        "    Scheduling policy ...: %d", itti.n4_sched_params.sched_policy);
    Logger::smf_app().info(
        "    Scheduling prio .....: %d", itti.n4_sched_params.sched_priority);
    Logger::smf_app().info("- ITTI SMF_APP task Threading:");
    Logger::smf_app().info(
        "    CPU id ..............: %d", itti.smf_app_sched_params.cpu_id);
    Logger::smf_app().info(
        "    Scheduling policy ...: %d",
    itti.smf_app_sched_params.sched_policy); Logger::smf_app().info( "
    Scheduling prio .....: %d", itti.smf_app_sched_params.sched_priority);
    Logger::smf_app().info("- ITTI ASYNC_CMD task Threading:");
    Logger::smf_app().info(
        "    CPU id ..............: %d", itti.async_cmd_sched_params.cpu_id);
    Logger::smf_app().info(
        "    Scheduling policy ...: %d",
        itti.async_cmd_sched_params.sched_policy);
    Logger::smf_app().info(
        "    Scheduling prio .....: %d",
        itti.async_cmd_sched_params.sched_priority);
  */

  Logger::smf_app().info("- AMF:");
  Logger::smf_app().info(
      "    IPv4 Addr ...........: %s",
      inet_ntoa(*((struct in_addr*) &amf_addr.ipv4_addr)));
  Logger::smf_app().info("    Port ................: %lu  ", amf_addr.port);
  Logger::smf_app().info(
      "    API version .........: %s", amf_addr.api_version.c_str());
  if (use_fqdn_dns)
    Logger::smf_app().info(
        "    FQDN ................: %s", amf_addr.fqdn.c_str());

  if (!use_local_subscription_info) {
    Logger::smf_app().info("- UDM:");
    Logger::smf_app().info(
        "    IPv4 Addr ...........: %s",
        inet_ntoa(*((struct in_addr*) &udm_addr.ipv4_addr)));
    Logger::smf_app().info("    Port ................: %lu  ", udm_addr.port);
    Logger::smf_app().info(
        "    API version .........: %s", udm_addr.api_version.c_str());
    if (use_fqdn_dns)
      Logger::smf_app().info(
          "    FQDN ................: %s", udm_addr.fqdn.c_str());
  }

  if (register_nrf or discover_upf) {
    Logger::smf_app().info("- NRF:");
    Logger::smf_app().info(
        "    IPv4 Addr ...........: %s",
        inet_ntoa(*((struct in_addr*) &nrf_addr.ipv4_addr)));
    Logger::smf_app().info("    Port ................: %lu  ", nrf_addr.port);
    Logger::smf_app().info(
        "    API version .........: %s", nrf_addr.api_version.c_str());
    if (use_fqdn_dns)
      Logger::smf_app().info(
          "    FQDN ................: %s", nrf_addr.fqdn.c_str());
  }

  if (!discover_upf) {
    Logger::smf_app().info("- UPF:");
    for (auto u : upfs) {
      if (u.node_id_type == pfcp::NODE_ID_TYPE_IPV4_ADDRESS)
        Logger::smf_app().info(
            "    IPv4 Addr ...........: %s",
            inet_ntoa(*((struct in_addr*) &u.u1.ipv4_address)));
      if (use_fqdn_dns)
        Logger::smf_app().info("    FQDN ................: %s", u.fqdn.c_str());
    }
  }

  char str_addr6[INET6_ADDRSTRLEN];
  Logger::smf_app().info("- DEFAULT DNS:");
  Logger::smf_app().info(
      "    Primary DNS .........: %s",
      inet_ntoa(*((struct in_addr*) &default_dnsv4)));
  Logger::smf_app().info(
      "    Secondary DNS .......: %s",
      inet_ntoa(*((struct in_addr*) &default_dns_secv4)));
  if (inet_ntop(AF_INET6, &default_dnsv6, str_addr6, sizeof(str_addr6))) {
    Logger::smf_app().info("    Primary DNS v6 ......: %s", str_addr6);
  }
  if (inet_ntop(AF_INET6, &default_dns_secv6, str_addr6, sizeof(str_addr6))) {
    Logger::smf_app().info("    Secondary DNS v6 ....: %s", str_addr6);
  }

  Logger::smf_app().info("- Default UE MTU: %d", ue_mtu);
  Logger::smf_app().info("- Supported Features:");
  Logger::smf_app().info(
      "    Register to NRF.....................: %s",
      register_nrf ? "Yes" : "No");
  Logger::smf_app().info(
      "    Discover UPF........................: %s",
      discover_upf ? "Yes" : "No");
  Logger::smf_app().info(
      "    Use Local Subscription Configuration: %s",
      use_local_subscription_info ? "Yes" : "No");
  Logger::smf_app().info(
      "    Push PCO (DNS+MTU)..................: %s",
      force_push_pco ? "Yes" : "No");
  Logger::smf_app().info(
      "    Use FQDN ...........................: %s",
      use_fqdn_dns ? "Yes" : "No");
  Logger::smf_app().info(
      "    Use NWI  ...........................: %s", use_nwi ? "Yes" : "No");

  Logger::smf_app().info("- DNN configurations:");

  for (std::map<string, dnn_t>::iterator it = dnns.begin(); it != dnns.end();
       it++) {
    Logger::smf_app().info(
        "    DNN..........: %s (%s)", it->second.dnn.c_str(),
        it->second.pdu_session_type.toString().c_str());

    if ((it->second.pdu_session_type.pdu_session_type ==
         pdu_session_type_e::PDU_SESSION_TYPE_E_IPV4) or
        (it->second.pdu_session_type.pdu_session_type ==
         pdu_session_type_e::PDU_SESSION_TYPE_E_IPV4V6)) {
      std::string range_low(inet_ntoa(it->second.ue_pool_range_low));
      std::string range_high(inet_ntoa(it->second.ue_pool_range_high));
      Logger::smf_app().info(
          "        IPv4 pool: %s - %s", range_low.c_str(), range_high.c_str());
    }

    if ((it->second.pdu_session_type.pdu_session_type ==
         pdu_session_type_e::PDU_SESSION_TYPE_E_IPV6) or
        (it->second.pdu_session_type.pdu_session_type ==
         pdu_session_type_e::PDU_SESSION_TYPE_E_IPV4V6)) {
      if (inet_ntop(
              AF_INET6, &it->second.paa_pool6_prefix, str_addr6,
              sizeof(str_addr6))) {
        Logger::smf_app().info(
            "        IPv6 pool: %s/%d", str_addr6,
            it->second.paa_pool6_prefix_len);
      }
    }
  }

  if (use_local_subscription_info) {
    Logger::smf_app().info("- Local Subscription Configuration:");
    uint8_t index = 0;
    for (auto sub : session_management_subscriptions) {
      Logger::smf_app().info(
          "    Session Management Subscription Data %d:", index);
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_NSSAI_SST
          ":  %d, " SMF_CONFIG_STRING_NSSAI_SD " %s",
          sub.single_nssai.sST, sub.single_nssai.sD.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_DNN ":  %s", sub.dnn.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_DEFAULT_SESSION_TYPE ":  %s",
          sub.session_type.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_DEFAULT_SSC_MODE ":  %d", sub.ssc_mode);
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_QOS_PROFILE_5QI ":  %d",
          sub.default_qos._5qi);
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_QOS_PROFILE_PRIORITY_LEVEL ":  %d",
          sub.default_qos.priority_level);
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_QOS_PROFILE_ARP_PRIORITY_LEVEL ":  %d",
          sub.default_qos.arp.priority_level);
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTCAP ":  %s",
          sub.default_qos.arp.preempt_cap.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_QOS_PROFILE_ARP_PREEMPTVULN ":  %s",
          sub.default_qos.arp.preempt_vuln.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_SESSION_AMBR_UL ":  %s",
          sub.session_ambr.uplink.c_str());
      Logger::smf_app().info(
          "        " SMF_CONFIG_STRING_SESSION_AMBR_DL ":  %s",
          sub.session_ambr.downlink.c_str());
      index++;
    }
  }
}

//------------------------------------------------------------------------------
int smf_config::get_pfcp_node_id(pfcp::node_id_t& node_id) {
  node_id = {};
  // TODO: support QFDN
  if (!fqdn.empty() and use_fqdn_dns) {
    node_id.node_id_type = pfcp::NODE_ID_TYPE_FQDN;
    node_id.fqdn         = fqdn;
    return RETURNok;
  }

  if (n4.addr4.s_addr) {
    node_id.node_id_type    = pfcp::NODE_ID_TYPE_IPV4_ADDRESS;
    node_id.u1.ipv4_address = n4.addr4;
    return RETURNok;
  }
  if (n4.addr6.s6_addr32[0] | n4.addr6.s6_addr32[1] | n4.addr6.s6_addr32[2] |
      n4.addr6.s6_addr32[3]) {
    node_id.node_id_type    = pfcp::NODE_ID_TYPE_IPV6_ADDRESS;
    node_id.u1.ipv6_address = n4.addr6;
    return RETURNok;
  }
  return RETURNerror;
}
//------------------------------------------------------------------------------
int smf_config::get_pfcp_fseid(pfcp::fseid_t& fseid) {
  int rc = RETURNerror;
  fseid  = {};
  if (n4.addr4.s_addr) {
    fseid.v4           = 1;
    fseid.ipv4_address = n4.addr4;
    rc                 = RETURNok;
  }
  if (n4.addr6.s6_addr32[0] | n4.addr6.s6_addr32[1] | n4.addr6.s6_addr32[2] |
      n4.addr6.s6_addr32[3]) {
    fseid.v6           = 1;
    fseid.ipv6_address = n4.addr6;
    rc                 = RETURNok;
  }
  return rc;
}

//------------------------------------------------------------------------------
smf_config::~smf_config() {}

//------------------------------------------------------------------------------
bool smf_config::is_dotted_dnn_handled(
    const std::string& dnn, const pdu_session_type_t& pdn_session_type) {
  Logger::smf_app().debug("Requested DNN: %s", dnn.c_str());

  for (std::map<std::string, dnn_t>::iterator it = dnns.begin();
       it != dnns.end(); it++) {
    Logger::smf_app().debug(
        "DNN label: %s, dnn: %s", it->second.dnn_label.c_str(),
        it->second.dnn.c_str());
    if (0 == dnn.compare(it->second.dnn)) {
      Logger::smf_app().debug("DNN matched!");
      Logger::smf_app().debug(
          "PDU Session Type %d, PDN Type %d", pdn_session_type.pdu_session_type,
          it->second.pdu_session_type.pdu_session_type);
      if (pdn_session_type.pdu_session_type ==
          it->second.pdu_session_type.pdu_session_type) {
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
std::string smf_config::get_default_dnn() {
  for (std::map<std::string, dnn_t>::iterator it = dnns.begin();
       it != dnns.end(); it++) {
    Logger::smf_app().debug("Default DNN: %s", it->second.dnn.c_str());
    return it->second.dnn;
  }
  return "default";  // default DNN
}

//------------------------------------------------------------------------------
bool smf_config::get_nwi_list_index(
    bool nwi_enabled, uint8_t nwi_list_index, pfcp::node_id_t node_id) {
  if (node_id.node_id_type == pfcp::NODE_ID_TYPE_IPV4_ADDRESS) {
    for (int i = 0; i < upf_nwi_list.size(); i++) {
      if (node_id.u1.ipv4_address.s_addr ==
          upf_nwi_list[i].upf_id.u1.ipv4_address.s_addr) {
        nwi_list_index = i;
        nwi_enabled    = true;
        return true;
      }
    }
    nwi_enabled = false;
    return false;
  }
  if (node_id.node_id_type == pfcp::NODE_ID_TYPE_FQDN) {
    // Resove FQDN here because, node id type is always IPV4_ADDRESS in
    // upf_nwi_list
    unsigned char buf_in_addr[sizeof(struct in_addr) + 1];
    unsigned int upf_port = {0};
    uint8_t addr_type     = {0};
    std::string address   = {};
    struct in_addr ipv4_Address;
    fqdn::resolve(node_id.fqdn, address, upf_port, addr_type, "");
    if (inet_pton(AF_INET, util::trim(address).c_str(), buf_in_addr) == 1) {
      memcpy(&ipv4_Address, buf_in_addr, sizeof(struct in_addr));
    } else {
      Logger::smf_app().error("FQDN resolve failed for get_nwi_list_index");
    }

    for (int i = 0; i < upf_nwi_list.size(); i++) {
      if (ipv4_Address.s_addr ==
          upf_nwi_list[i].upf_id.u1.ipv4_address.s_addr) {
        nwi_list_index = i;
        nwi_enabled    = true;
        return true;
      }
    }
    nwi_enabled = false;
    return false;
  }
  return false;
}

//------------------------------------------------------------------------------
std::string smf_config::get_nwi(
    const std::vector<interface_upf_info_item_t>& int_list,
    const std::string& int_type) {
  std::string nwi = {};
  for (auto ui : int_list) {
    if (!ui.interface_type.compare(int_type)) nwi = ui.network_instance;
  }
  Logger::smf_app().debug(
      "Interface Type - %s, NWI - %s", int_type.c_str(), nwi.c_str());
  return nwi;
}
//------------------------------------------------------------------------------
