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

/*! \file nssf_http2-server.h
 \brief
 \author  Rohan Kharade
 \company Openairinterface Software Allianse
 \date 2021
 \email: rohan.kharade@openairinterface.org
 */

#include "nssf_config.hpp"
#include "common_defs.h"
#include "conversions.hpp"
#include "if.hpp"
#include "logger.hpp"
#include "string.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace libconfig;
using namespace nssf;

#define kJsonFileBuffer (1024)

nssf_nsi_info_t nssf_config::nssf_nsi_info;
nssf_ta_info_t nssf_config::nssf_ta_info;
std::string nssf_config::slice_config_file;
nlohmann::json nssf_config::nssf_slice_config;

// C includes
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

//------------------------------------------------------------------------------
int nssf_config::execute() {
  return RETURNok;
}

//------------------------------------------------------------------------------
int nssf_config::load_interface(const Setting& if_cfg, interface_cfg_t& cfg) {
  if_cfg.lookupValue(NSSF_CONFIG_STRING_INTERFACE_NAME, cfg.if_name);
  util::trim(cfg.if_name);
  if (not boost::iequals(cfg.if_name, "none")) {
    std::string address = {};
    if_cfg.lookupValue(NSSF_CONFIG_STRING_IPV4_ADDRESS, address);
    util::trim(address);
    if (boost::iequals(address, "read")) {
      if (get_inet_addr_infos_from_iface(
              cfg.if_name, cfg.addr4, cfg.network4, cfg.mtu)) {
        Logger::nssf_app().error(
            "Could not read %s network interface configuration", cfg.if_name);
        return RETURNerror;
      }
    } else {
      std::vector<std::string> words;
      boost::split(
          words, address, boost::is_any_of("/"), boost::token_compress_on);
      if (words.size() != 2) {
        Logger::nssf_app().error(
            "Bad value " NSSF_CONFIG_STRING_IPV4_ADDRESS " = %s in config file",
            address.c_str());
        return RETURNerror;
      }
      unsigned char buf_in_addr[sizeof(struct in6_addr)];  // you never know...
      if (inet_pton(AF_INET, util::trim(words.at(0)).c_str(), buf_in_addr) ==
          1) {
        memcpy(&cfg.addr4, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::nssf_app().error(
            "In conversion: Bad value " NSSF_CONFIG_STRING_IPV4_ADDRESS
            " = %s in config file",
            util::trim(words.at(0)).c_str());
        return RETURNerror;
      }
      cfg.network4.s_addr = htons(
          ntohs(cfg.addr4.s_addr) &
          0xFFFFFFFF << (32 - std::stoi(util::trim(words.at(1)))));
    }
    if_cfg.lookupValue(NSSF_CONFIG_STRING_SBI_PORT_HTTP1, cfg.http1_port);
    if_cfg.lookupValue(NSSF_CONFIG_STRING_SBI_PORT_HTTP2, cfg.http2_port);
  }
  return RETURNok;
}

//------------------------------------------------------------------------------
int nssf_config::load(const string& config_file) {
  Config cfg;
  unsigned char buf_in_addr[sizeof(struct in_addr) + 1];
  unsigned char buf_in6_addr[sizeof(struct in6_addr) + 1];

  // Read the file. If there is an error, report it and exit.
  try {
    cfg.readFile(config_file.c_str());
  } catch (const FileIOException& fioex) {
    Logger::nssf_app().error(
        "I/O error while reading file %s - %s", config_file.c_str(),
        fioex.what());
    throw;
  } catch (const ParseException& pex) {
    Logger::nssf_app().error(
        "Parse error at %s:%d - %s", pex.getFile(), pex.getLine(),
        pex.getError());
    throw;
  }

  const Setting& root = cfg.getRoot();

  try {
    const Setting& nssf_cfg = root[NSSF_CONFIG_STRING_NSSF_CONFIG];
  } catch (const SettingNotFoundException& nfex) {
    Logger::nssf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }

  const Setting& nssf_cfg = root[NSSF_CONFIG_STRING_NSSF_CONFIG];

  try {
    nssf_cfg.lookupValue(
        NSSF_CONFIG_STRING_NSSF_SLICE_CONFIG, slice_config_file);
  } catch (const SettingNotFoundException& nfex) {
    Logger::nssf_app().info(
        "%s : %s, No slice_config_file configured", nfex.what(),
        nfex.getPath());
  }

  try {
    nssf_cfg.lookupValue(NSSF_CONFIG_STRING_FQDN, fqdn);
    util::trim(fqdn);
  } catch (const SettingNotFoundException& nfex) {
    Logger::nssf_app().info(
        "%s : %s, No FQDN configured", nfex.what(), nfex.getPath());
  }

  try {
    const Setting& nw_if_cfg = nssf_cfg[NSSF_CONFIG_STRING_INTERFACES];

    const Setting& sbi_cfg = nw_if_cfg[NSSF_CONFIG_STRING_SBI_INTERFACE];
    load_interface(sbi_cfg, sbi);

    sbi_cfg.lookupValue(NSSF_CONFIG_STRING_SBI_API_VERSION, sbi_api_version);
  } catch (const SettingNotFoundException& nfex) {
    Logger::nssf_app().error("%s : %s", nfex.what(), nfex.getPath());
    return RETURNerror;
  }
  return RETURNok;
}

//------------------------------------------------------------------------------
void nssf_config::display() {
  Logger::nssf_app().info(
      "==== OPENAIRINTERFACE %s v%s ====", PACKAGE_NAME, PACKAGE_VERSION);
  Logger::nssf_app().info("Configuration:");
  Logger::nssf_app().info("- FQDN ..................: %s", fqdn.c_str());
  Logger::nssf_app().info("- SBI:");
  Logger::nssf_app().info("    iface ............: %s", sbi.if_name.c_str());
  Logger::nssf_app().info("    ipv4.addr ........: %s", inet_ntoa(sbi.addr4));
  Logger::nssf_app().info(
      "    ipv4.mask ........: %s", inet_ntoa(sbi.network4));
  Logger::nssf_app().info("    mtu ..............: %d", sbi.mtu);
  Logger::nssf_app().info("    http1_port .......: %u", sbi.http1_port);
  Logger::nssf_app().info("    http2_port .......: %u", sbi.http2_port);
  Logger::nssf_app().info(
      "    api_version ......: %s", sbi_api_version.c_str());
}

//------------------------------------------------------------------------------
bool nssf_config::ValidateNSI(
    const SliceInfoForPDUSession& slice_info, NsiInformation& nsi_info) {
  Logger::nssf_app().debug("Validating S-NSSAI for NSI");

  Snssai requested_snssai = slice_info.getSNssai();

  for (int i = 0; i < nssf_nsi_info.nsiInfoList.size(); i++) {
    Snssai target_snssai = nssf_nsi_info.nsiInfoList[i].snssai;

    if (requested_snssai.getSst() == target_snssai.getSst()) {
      if (requested_snssai.sdIsSet() & target_snssai.sdIsSet()) {
        if (requested_snssai.getSd() != target_snssai.getSd()) return false;
      }

      nsi_info.setNrfId(nssf_nsi_info.nsiInfoList[i].nsiInfo.getNrfId());

      if (nssf_nsi_info.nsiInfoList[i].nsiInfo.nsiIdIsSet())
        nsi_info.setNsiId(nssf_nsi_info.nsiInfoList[i].nsiInfo.getNsiId());

      if (nssf_nsi_info.nsiInfoList[i].nsiInfo.nrfNfMgtUriIsSet())
        nsi_info.setNrfNfMgtUri(
            nssf_nsi_info.nsiInfoList[i].nsiInfo.getNrfNfMgtUri());

      return true;
    }
  }

  Logger::nssf_app().warn(
      "NS Selection: S-NSSAI from SliceInfoForPDUSession "
      "is not authorised !!!");
  Logger::nssf_app().info(
      "//---------------------------------------------------------");
  Logger::nssf_app().info("");
  return false;
}
//------------------------------------------------------------------------------

bool nssf_config::ValidateTA(const Tai& tai) {
  Logger::nssf_app().debug("Validating TA");
  PlmnId requested_plmn     = tai.getPlmnId();
  std::string requested_tac = tai.getTac();

  for (int i = 0; i < nssf_ta_info.taInfoList.size(); i++) {
    PlmnId target_plmn     = nssf_ta_info.taInfoList[i].tai.getPlmnId();
    std::string target_tac = nssf_ta_info.taInfoList[i].tai.getTac();

    if (requested_plmn.getMcc() == target_plmn.getMcc() &&
        requested_plmn.getMnc() == target_plmn.getMnc() &&
        requested_tac == target_tac)
      return true;
  }
  Logger::nssf_app().warn("NS Selection: TAI is not authorised !!!");
  Logger::nssf_app().info(
      "//---------------------------------------------------------");
  Logger::nssf_app().info("");
  return false;
}
//------------------------------------------------------------------------------
const bool nssf_config::ParseTaInfo(
    const nlohmann::json& conf, nssf_ta_info_t& cfg) {
  if (!conf.is_array()) {
    Logger::nssf_app().error(
        "Error parsing json value: nsiInfoList is not array");
    return false;
  }
  static std::mutex mutex;
  for (auto it : conf) {
    ta_info_t ta_info;
    PlmnId plmn_id;
    nlohmann::json tai   = it["tai"];
    nlohmann::json nssai = it["supportedSnssaiList"];

    // Set Tai
    plmn_id.setMcc(tai["plmnId"]["mcc"]);
    plmn_id.setMnc(tai["plmnId"]["mnc"]);
    ta_info.tai.setPlmnId(plmn_id);
    ta_info.tai.setTac(tai["tac"]);

    // Set Supported Snssai List
    // if (!nssai.is_array()) {
    //   Logger::nssf_app().error(
    //       "Error parsing json value: supportedSnssaiList is not array");
    //   return false;
    // } else {
    //   //ToDo
    // }
    cfg.taInfoList.push_back(ta_info);
  }
  return true;
}

//------------------------------------------------------------------------------
const bool nssf_config::ParseNsiInfo(
    const nlohmann::json& conf, nssf_nsi_info_t& cfg) {
  if (!conf.is_array()) {
    Logger::nssf_app().error(
        "Error parsing json value: nsiInfoList is not array");
    return false;
  }
  static std::mutex mutex;
  for (auto it : conf) {
    nsi_info_t nsi_info;
    nlohmann::json snssai = it["snssai"];
    nlohmann::json nsi    = it["nsiInformationList"];

    // Set S-NSSAI
    nsi_info.snssai.setSst(snssai["sst"]);
    if (!snssai["sd"].empty()) nsi_info.snssai.setSd(snssai["sd"]);

    // Set NSI Info List
    nsi_info.nsiInfo.setNrfId(nsi["nrfId"]);
    nsi_info.nsiInfo.setNsiId(nsi["nsiId"]);

    std::lock_guard<std::mutex> lock(mutex);
    cfg.nsiInfoList.push_back(nsi_info);
  }
  return true;
}

//------------------------------------------------------------------------------
bool nssf_config::ParseJson() {
  nlohmann::json data = {};
  try {
    std::ifstream ifs(slice_config_file.c_str());
    data = nlohmann::json::parse(ifs);
  } catch (nlohmann::detail::exception& e) {
    std::cout << "The json config file specified does not exists" << std::endl;
    return false;
  }

  nssf_slice_config = data["configuration"];
  try {
    if (!nssf_slice_config.empty()) {
      nlohmann::json& nsi_info = nssf_slice_config["nsiInfoList"];
      if (!nsi_info.empty()) {
        if (!ParseNsiInfo(nsi_info, nssf_nsi_info)) {
          Logger::nssf_app().error("Error parsing json section: nsiInfoList");
          return false;
        }
      }
      nlohmann::json& ta_info = nssf_slice_config["taInfoList"];
      if (!ta_info.empty()) {
        if (!ParseTaInfo(ta_info, nssf_ta_info)) {
          Logger::nssf_app().error("Error parsing json section: taInfoList");
          return false;
        }
      }
      return true;
    }
  } catch (nlohmann::detail::exception& e) {
    Logger::nssf_sbi().warn(
        "Can not parse the json data (error: %s)!", e.what());
    return false;
  }
  return false;
}

//------------------------------------------------------------------------------
bool nssf_config::get_slice_config(nlohmann::json& slice_config) {
  slice_config = nssf_slice_config;
  return true;
}

//------------------------------------------------------------------------------
bool nssf_config::get_api_list(nlohmann::json& api_list) {
  api_list["OAI-NSSF"] = {
      {"Organisation", "Openairinterface Software Aliance"},
      {"Description", "OAI-NSSF initial Release"},
      {"Version", "1.0.0"},
      {"Supported APIs",
       {{"API", "Network Slice Information (Document)"},
        {"Method", "GET"},
        {"URI Path",
         "/nnssf-nsselection/<api_version>/network-slice-information"},
        {"Details",
         "Retrieve the Network Slice Selection Information (PDU Session)"}}}};
  return true;
}
//------------------------------------------------------------------------------
