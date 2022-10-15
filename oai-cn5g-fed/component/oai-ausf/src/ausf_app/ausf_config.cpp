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

/*! \file ausf_config.cpp
 \brief
 \author  Fengjiao He, BUPT
 \date 2021
 \email: contact@openairinterface.org
 */

#include "ausf_config.hpp"

#include <iostream>
#include <libconfig.h++>
#include "string.hpp"

#include "logger.hpp"
#include "if.hpp"
#include "fqdn.hpp"

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

using namespace libconfig;

namespace config {

//------------------------------------------------------------------------------
ausf_config::ausf_config() : sbi(), ausf_name(), pid_dir(), instance() {
  udm_addr.ipv4_addr.s_addr = INADDR_ANY;
  udm_addr.port             = 80;
  udm_addr.api_version      = "v1";
  udm_addr.fqdn             = {};
  use_fqdn_dns              = false;
  use_http2                 = false;
}

//------------------------------------------------------------------------------
ausf_config::~ausf_config() {}

//------------------------------------------------------------------------------
int ausf_config::load(const std::string& config_file) {
  Logger::config().debug(
      "\nLoad AUSF system configuration file(%s)", config_file.c_str());
  Config cfg;
  unsigned char buf_in6_addr[sizeof(struct in6_addr)];

  try {
    cfg.readFile(config_file.c_str());
  } catch (const FileIOException& fioex) {
    Logger::config().error(
        "I/O error while reading file %s - %s", config_file.c_str(),
        fioex.what());
    throw;
  } catch (const ParseException& pex) {
    Logger::config().error(
        "Parse error at %s:%d - %s", pex.getFile(), pex.getLine(),
        pex.getError());
    throw;
  }
  const Setting& root = cfg.getRoot();

  try {
    const Setting& ausf_cfg = root[AUSF_CONFIG_STRING_AUSF_CONFIG];
  } catch (const SettingNotFoundException& nfex) {
    Logger::config().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }
  const Setting& ausf_cfg = root[AUSF_CONFIG_STRING_AUSF_CONFIG];
  try {
    ausf_cfg.lookupValue(AUSF_CONFIG_STRING_INSTANCE_ID, instance);
  } catch (const SettingNotFoundException& nfex) {
    Logger::config().error(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }

  try {
    ausf_cfg.lookupValue(AUSF_CONFIG_STRING_PID_DIRECTORY, pid_dir);
  } catch (const SettingNotFoundException& nfex) {
    Logger::config().error(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }
  try {
    ausf_cfg.lookupValue(AUSF_CONFIG_STRING_AUSF_NAME, ausf_name);
  } catch (const SettingNotFoundException& nfex) {
    Logger::config().error(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
  }
  // AUSF SBI interface
  try {
    const Setting& new_if_cfg = ausf_cfg[AUSF_CONFIG_STRING_INTERFACES];

    const Setting& sbi_cfg = new_if_cfg[AUSF_CONFIG_STRING_INTERFACE_SBI];
    load_interface(sbi_cfg, sbi);
    // HTTP2 port
    if (!(sbi_cfg.lookupValue(
            AUSF_CONFIG_STRING_SBI_HTTP2_PORT, sbi_http2_port))) {
      Logger::ausf_app().error(AUSF_CONFIG_STRING_SBI_HTTP2_PORT "failed");
      throw(AUSF_CONFIG_STRING_SBI_HTTP2_PORT " failed");
    }

    // API Version
    if (!(sbi_cfg.lookupValue(
            AUSF_CONFIG_STRING_API_VERSION, sbi_api_version))) {
      Logger::ausf_app().error(AUSF_CONFIG_STRING_API_VERSION " failed");
      throw(AUSF_CONFIG_STRING_API_VERSION " failed");
    }

  } catch (const SettingNotFoundException& nfex) {
    Logger::config().error(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  // Support features
  try {
    const Setting& support_features =
        ausf_cfg[AUSF_CONFIG_STRING_SUPPORT_FEATURES];
    std::string opt = {};

    support_features.lookupValue(
        AUSF_CONFIG_STRING_SUPPORT_FEATURES_USE_FQDN_DNS, opt);
    if (boost::iequals(opt, "yes")) {
      use_fqdn_dns = true;
    } else {
      use_fqdn_dns = false;
    }

    support_features.lookupValue(
        AUSF_CONFIG_STRING_SUPPORT_FEATURES_USE_HTTP2, opt);
    if (boost::iequals(opt, "yes")) {
      use_http2 = true;
    } else {
      use_http2 = false;
    }

    support_features.lookupValue(
        AUSF_CONFIG_STRING_SUPPORTED_FEATURES_REGISTER_NRF, opt);
    if (boost::iequals(opt, "yes")) {
      register_nrf = true;
    } else {
      register_nrf = false;
    }

  } catch (const SettingNotFoundException& nfex) {
    Logger::ausf_app().error(
        "%s : %s, using defaults", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  // UDM
  try {
    std::string astring = {};

    const Setting& udm_cfg       = ausf_cfg[AUSF_CONFIG_STRING_UDM];
    struct in_addr udm_ipv4_addr = {};
    unsigned int udm_port        = 0;
    std::string udm_api_version  = {};

    if (!use_fqdn_dns) {
      udm_cfg.lookupValue(AUSF_CONFIG_STRING_UDM_IPV4_ADDRESS, astring);
      IPV4_STR_ADDR_TO_INADDR(
          util::trim(astring).c_str(), udm_ipv4_addr,
          "BAD IPv4 ADDRESS FORMAT FOR UDM !");
      udm_addr.ipv4_addr = udm_ipv4_addr;
      if (!(udm_cfg.lookupValue(AUSF_CONFIG_STRING_UDM_PORT, udm_port))) {
        Logger::ausf_app().error(AUSF_CONFIG_STRING_UDM_PORT "failed");
        throw(AUSF_CONFIG_STRING_UDM_PORT "failed");
      }
      udm_addr.port = udm_port;

      if (!(udm_cfg.lookupValue(
              AUSF_CONFIG_STRING_API_VERSION, udm_api_version))) {
        Logger::ausf_app().error(AUSF_CONFIG_STRING_API_VERSION "failed");
        throw(AUSF_CONFIG_STRING_API_VERSION "failed");
      }
      udm_addr.api_version = udm_api_version;

    } else {
      udm_cfg.lookupValue(AUSF_CONFIG_STRING_FQDN_DNS, astring);
      uint8_t addr_type   = {0};
      std::string address = {};
      fqdn::resolve(astring, address, udm_port, addr_type);
      if (addr_type != 0) {  // IPv6
        // TODO:
        throw("DO NOT SUPPORT IPV6 ADDR FOR UDM!");
      } else {  // IPv4
        IPV4_STR_ADDR_TO_INADDR(
            util::trim(address).c_str(), udm_ipv4_addr,
            "BAD IPv4 ADDRESS FORMAT FOR NRF !");
        udm_addr.ipv4_addr = udm_ipv4_addr;
        // udm_addr.port               = udm_port;
        // We hardcode udm port from config for the moment
        if (!(udm_cfg.lookupValue(AUSF_CONFIG_STRING_UDM_PORT, udm_port))) {
          Logger::ausf_app().error(AUSF_CONFIG_STRING_UDM_PORT "failed");
          throw(AUSF_CONFIG_STRING_UDM_PORT "failed");
        }
        udm_addr.port               = udm_port;
        std::string udm_api_version = {};
        if (!(udm_cfg.lookupValue(
                AUSF_CONFIG_STRING_API_VERSION, udm_api_version))) {
          Logger::ausf_app().error(AUSF_CONFIG_STRING_API_VERSION "failed");
          throw(AUSF_CONFIG_STRING_API_VERSION "failed");
        }
        udm_addr.api_version =
            udm_api_version;  // TODO: to get API version from DNS
        udm_addr.fqdn = astring;
      }
    }

  } catch (const SettingNotFoundException& nfex) {
    Logger::ausf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  // NRF
  if (register_nrf) {
    try {
      std::string astring = {};

      const Setting& nrf_cfg       = ausf_cfg[AUSF_CONFIG_STRING_NRF];
      struct in_addr nrf_ipv4_addr = {};
      unsigned int nrf_port        = 0;
      std::string nrf_api_version  = {};

      if (!use_fqdn_dns) {
        nrf_cfg.lookupValue(AUSF_CONFIG_STRING_NRF_IPV4_ADDRESS, astring);
        IPV4_STR_ADDR_TO_INADDR(
            util::trim(astring).c_str(), nrf_ipv4_addr,
            "BAD IPv4 ADDRESS FORMAT FOR NRF !");
        nrf_addr.ipv4_addr = nrf_ipv4_addr;
        if (!(nrf_cfg.lookupValue(AUSF_CONFIG_STRING_NRF_PORT, nrf_port))) {
          Logger::ausf_app().error(AUSF_CONFIG_STRING_NRF_PORT "failed");
          throw(AUSF_CONFIG_STRING_NRF_PORT "failed");
        }
        nrf_addr.port = nrf_port;

        if (!(nrf_cfg.lookupValue(
                AUSF_CONFIG_STRING_API_VERSION, nrf_api_version))) {
          Logger::ausf_app().error(AUSF_CONFIG_STRING_API_VERSION "failed");
          throw(AUSF_CONFIG_STRING_API_VERSION "failed");
        }
        nrf_addr.api_version = nrf_api_version;

      } else {
        nrf_cfg.lookupValue(AUSF_CONFIG_STRING_FQDN_DNS, astring);
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
          nrf_addr.api_version = "v1";  // TODO: to get API version from DNS
          nrf_addr.fqdn        = astring;
          // We hardcode nrf port from config for the moment
          if (!(nrf_cfg.lookupValue(AUSF_CONFIG_STRING_NRF_PORT, nrf_port))) {
            Logger::ausf_app().error(AUSF_CONFIG_STRING_NRF_PORT "failed");
            throw(AUSF_CONFIG_STRING_NRF_PORT "failed");
          }
          nrf_addr.port = nrf_port;
        }
      }
    } catch (const SettingNotFoundException& nfex) {
      Logger::ausf_app().error("%s : %s", nfex.what(), nfex.getPath());
      return RETURNerror;
    }
  }
  return RETURNok;
}

//------------------------------------------------------------------------------
void ausf_config::display() {
  Logger::config().info(
      "==== OAI-CN5G %s v%s ====", PACKAGE_NAME, PACKAGE_VERSION);
  Logger::config().info("================= AUSF =================");
  Logger::config().info("Configuration AUSF:");
  Logger::config().info("- Instance ...............: %d", instance);
  Logger::config().info("- PID Dir ................: %s", pid_dir.c_str());
  Logger::config().info("- AUSF Name ..............: %s", ausf_name.c_str());

  Logger::config().info("- SBI Networking:");
  Logger::config().info("    Iface ................: %s", sbi.if_name.c_str());
  Logger::config().info("    IPv4 Addr ............: %s", inet_ntoa(sbi.addr4));
  Logger::config().info("    HTTP1 Port ...........: %d", sbi.port);
  Logger::config().info("    HTTP2 Port............: %d", sbi_http2_port);
  Logger::config().info(
      "    API Version...........: %s", sbi_api_version.c_str());
  Logger::config().info("- Supported Features:");
  Logger::config().info(
      "    Register NRF ..........: %s", register_nrf ? "Yes" : "No");
  Logger::config().info(
      "    Use FQDN ..............: %s", use_fqdn_dns ? "Yes" : "No");
  Logger::config().info(
      "    Use HTTP2..............: %s", use_http2 ? "Yes" : "No");

  Logger::config().info("- UDM:");
  Logger::config().info(
      "    IPv4 Addr ............: %s",
      inet_ntoa(*((struct in_addr*) &udm_addr.ipv4_addr)));
  Logger::config().info("    Port .................: %lu  ", udm_addr.port);
  Logger::config().info(
      "    API version ..........: %s", udm_addr.api_version.c_str());

  Logger::config().info("- NRF:");
  Logger::config().info(
      "    IPv4 Addr ............: %s",
      inet_ntoa(*((struct in_addr*) &nrf_addr.ipv4_addr)));
  Logger::config().info("    Port .................: %lu  ", nrf_addr.port);
  Logger::config().info(
      "    API version ..........: %s", nrf_addr.api_version.c_str());

  if (use_fqdn_dns)
    Logger::config().info(
        "    FQDN .................: %s", udm_addr.fqdn.c_str());
}

//------------------------------------------------------------------------------
int ausf_config::load_interface(
    const libconfig::Setting& if_cfg, interface_cfg_t& cfg) {
  if_cfg.lookupValue(AUSF_CONFIG_STRING_INTERFACE_NAME, cfg.if_name);
  util::trim(cfg.if_name);
  if (not boost::iequals(cfg.if_name, "none")) {
    std::string address = {};
    if_cfg.lookupValue(AUSF_CONFIG_STRING_IPV4_ADDRESS, address);
    util::trim(address);
    if (boost::iequals(address, "read")) {
      if (get_inet_addr_infos_from_iface(
              cfg.if_name, cfg.addr4, cfg.network4, cfg.mtu)) {
        Logger::config().error(
            "Could not read %s network interface configuration", cfg.if_name);
        return RETURNerror;
      }
    } else {
      std::vector<std::string> words;
      boost::split(
          words, address, boost::is_any_of("/"), boost::token_compress_on);
      if (words.size() != 2) {
        Logger::config().error(
            "Bad value " AUSF_CONFIG_STRING_IPV4_ADDRESS " = %s in config file",
            address.c_str());
        return RETURNerror;
      }
      unsigned char buf_in_addr[sizeof(struct in6_addr)];  // you never know...
      if (inet_pton(AF_INET, util::trim(words.at(0)).c_str(), buf_in_addr) ==
          1) {
        memcpy(&cfg.addr4, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::config().error(
            "In conversion: Bad value " AUSF_CONFIG_STRING_IPV4_ADDRESS
            " = %s in config file",
            util::trim(words.at(0)).c_str());
        return RETURNerror;
      }
      cfg.network4.s_addr = htons(
          ntohs(cfg.addr4.s_addr) &
          0xFFFFFFFF << (32 - std::stoi(util::trim(words.at(1)))));
    }
    if_cfg.lookupValue(AUSF_CONFIG_STRING_PORT, cfg.port);
  }
  return RETURNok;
}

}  // namespace config
