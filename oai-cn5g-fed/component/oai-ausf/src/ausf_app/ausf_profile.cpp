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

/*! \file ausf_profile.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email: Tien-Thinh.Nguyen@eurecom.fr
 */

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "logger.hpp"
#include "ausf_profile.hpp"
#include "string.hpp"

// using namespace ausf;
using namespace oai::ausf::app;

//------------------------------------------------------------------------------
void ausf_profile::set_nf_instance_id(const std::string& instance_id) {
  nf_instance_id = instance_id;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_instance_id(std::string& instance_id) const {
  instance_id = nf_instance_id;
}

//------------------------------------------------------------------------------
std::string ausf_profile::get_nf_instance_id() const {
  return nf_instance_id;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_instance_name(const std::string& instance_name) {
  nf_instance_name = instance_name;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_instance_name(std::string& instance_name) const {
  instance_name = nf_instance_name;
}

//------------------------------------------------------------------------------
std::string ausf_profile::get_nf_instance_name() const {
  return nf_instance_name;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_type(const std::string& type) {
  nf_type = type;
}

//------------------------------------------------------------------------------
std::string ausf_profile::get_nf_type() const {
  return nf_type;
}
//------------------------------------------------------------------------------
void ausf_profile::set_nf_status(const std::string& status) {
  nf_status = status;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_status(std::string& status) const {
  status = nf_status;
}

//------------------------------------------------------------------------------
std::string ausf_profile::get_nf_status() const {
  return nf_status;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_heartBeat_timer(const int32_t& timer) {
  heartBeat_timer = timer;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_heartBeat_timer(int32_t& timer) const {
  timer = heartBeat_timer;
}

//------------------------------------------------------------------------------
int32_t ausf_profile::get_nf_heartBeat_timer() const {
  return heartBeat_timer;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_priority(const uint16_t& p) {
  priority = p;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_priority(uint16_t& p) const {
  p = priority;
}

//------------------------------------------------------------------------------
uint16_t ausf_profile::get_nf_priority() const {
  return priority;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_capacity(const uint16_t& c) {
  capacity = c;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_capacity(uint16_t& c) const {
  c = capacity;
}

//------------------------------------------------------------------------------
uint16_t ausf_profile::get_nf_capacity() const {
  return capacity;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_snssais(const std::vector<snssai_t>& s) {
  snssais = s;
}

//------------------------------------------------------------------------------
void ausf_profile::get_nf_snssais(std::vector<snssai_t>& s) const {
  s = snssais;
}

//------------------------------------------------------------------------------
void ausf_profile::add_snssai(const snssai_t& s) {
  snssais.push_back(s);
}

//------------------------------------------------------------------------------
void ausf_profile::set_fqdn(const std::string& fqdN) {
  fqdn = fqdN;
}

//------------------------------------------------------------------------------
std::string ausf_profile::get_fqdn() const {
  return fqdn;
}

//------------------------------------------------------------------------------
void ausf_profile::set_nf_ipv4_addresses(const std::vector<struct in_addr>& a) {
  ipv4_addresses = a;
}

//------------------------------------------------------------------------------
void ausf_profile::add_nf_ipv4_addresses(const struct in_addr& a) {
  ipv4_addresses.push_back(a);
}
//------------------------------------------------------------------------------
void ausf_profile::get_nf_ipv4_addresses(std::vector<struct in_addr>& a) const {
  a = ipv4_addresses;
}

//------------------------------------------------------------------------------
void ausf_profile::set_ausf_info(const ausf_info_t& s) {
  ausf_info = s;
}

//------------------------------------------------------------------------------
void ausf_profile::get_ausf_info(ausf_info_t& s) const {
  s = ausf_info;
}

//------------------------------------------------------------------------------
void ausf_profile::display() const {
  Logger::ausf_app().debug("- NF instance info");
  Logger::ausf_app().debug("    Instance ID: %s", nf_instance_id.c_str());
  Logger::ausf_app().debug("    Instance name: %s", nf_instance_name.c_str());
  Logger::ausf_app().debug("    Instance type: %s", nf_type.c_str());
  Logger::ausf_app().debug("    Instance fqdn: %s", fqdn.c_str());
  Logger::ausf_app().debug("    Status: %s", nf_status.c_str());
  Logger::ausf_app().debug("    HeartBeat timer: %d", heartBeat_timer);
  Logger::ausf_app().debug("    Priority: %d", priority);
  Logger::ausf_app().debug("    Capacity: %d", capacity);
  // SNSSAIs
  if (snssais.size() > 0) {
    Logger::ausf_app().debug("    SNSSAI:");
  }
  for (auto s : snssais) {
    Logger::ausf_app().debug("        SST, SD: %d, %s", s.sST, s.sD.c_str());
  }

  // IPv4 Addresses
  if (ipv4_addresses.size() > 0) {
    Logger::ausf_app().debug("    IPv4 Addr:");
  }
  for (auto address : ipv4_addresses) {
    Logger::ausf_app().debug("        %s", inet_ntoa(address));
  }

  Logger::ausf_app().debug("\tAUSF Info");
  Logger::ausf_app().debug("\t\tGroupId: %s", ausf_info.groupid);
  for (auto supi : ausf_info.supi_ranges) {
    Logger::ausf_app().debug(
        "\t\t SupiRanges: Start - %s, End - %s, Pattern - %s",
        supi.supi_range.start, supi.supi_range.end, supi.supi_range.pattern);
  }
  for (auto route_ind : ausf_info.routing_indicators) {
    Logger::ausf_app().debug("\t\t Routing Indicators: %s", route_ind);
  }
}

//------------------------------------------------------------------------------
void ausf_profile::to_json(nlohmann::json& data) const {
  data["nfInstanceId"]   = nf_instance_id;
  data["nfInstanceName"] = nf_instance_name;
  data["nfType"]         = nf_type;
  data["nfStatus"]       = nf_status;
  data["heartBeatTimer"] = heartBeat_timer;
  // SNSSAIs
  data["sNssais"] = nlohmann::json::array();
  for (auto s : snssais) {
    nlohmann::json tmp = {};
    tmp["sst"]         = s.sST;
    tmp["sd"]          = s.sD;
    data["sNssais"].push_back(tmp);
  }
  data["fqdn"] = fqdn;
  // ipv4_addresses
  data["ipv4Addresses"] = nlohmann::json::array();
  for (auto address : ipv4_addresses) {
    nlohmann::json tmp = inet_ntoa(address);
    data["ipv4Addresses"].push_back(tmp);
  }

  data["priority"] = priority;
  data["capacity"] = capacity;

  // AUSF Info
  data["ausfInfo"]["groupId"]           = ausf_info.groupid;
  data["ausfInfo"]["supiRanges"]        = nlohmann::json::array();
  data["ausfInfo"]["routingIndicators"] = nlohmann::json::array();
  for (auto supi : ausf_info.supi_ranges) {
    nlohmann::json tmp = {};
    tmp["start"]       = supi.supi_range.start;
    tmp["end"]         = supi.supi_range.end;
    tmp["pattern"]     = supi.supi_range.pattern;
    data["ausfInfo"]["supiRanges"].push_back(tmp);
  }
  for (auto route_ind : ausf_info.routing_indicators) {
    std::string tmp = route_ind;
    data["ausfInfo"]["routingIndicators"].push_back(route_ind);
  }

  Logger::ausf_app().debug("ausf profile to JSON:\n %s", data.dump().c_str());
}

//------------------------------------------------------------------------------
void ausf_profile::from_json(const nlohmann::json& data) {
  if (data.find("nfInstanceId") != data.end()) {
    nf_instance_id = data["nfInstanceId"].get<std::string>();
  }

  if (data.find("nfInstanceName") != data.end()) {
    nf_instance_name = data["nfInstanceName"].get<std::string>();
  }

  if (data.find("nfType") != data.end()) {
    nf_type = data["nfType"].get<std::string>();
  }

  if (data.find("nfStatus") != data.end()) {
    nf_status = data["nfStatus"].get<std::string>();
  }

  if (data.find("heartBeatTimer") != data.end()) {
    heartBeat_timer = data["heartBeatTimer"].get<int>();
  }
  // sNssais
  if (data.find("sNssais") != data.end()) {
    for (auto it : data["sNssais"]) {
      snssai_t s = {};
      s.sST      = it["sst"].get<int>();
      s.sD       = it["sd"].get<std::string>();
      snssais.push_back(s);
    }
  }

  if (data.find("ipv4Addresses") != data.end()) {
    nlohmann::json addresses = data["ipv4Addresses"];

    for (auto it : addresses) {
      struct in_addr addr4 = {};
      std::string address  = it.get<std::string>();
      unsigned char buf_in_addr[sizeof(struct in_addr)];
      if (inet_pton(AF_INET, util::trim(address).c_str(), buf_in_addr) == 1) {
        memcpy(&addr4, buf_in_addr, sizeof(struct in_addr));
      } else {
        Logger::ausf_app().warn(
            "Address conversion: Bad value %s", util::trim(address).c_str());
      }
      add_nf_ipv4_addresses(addr4);
    }
  }

  if (data.find("priority") != data.end()) {
    priority = data["priority"].get<int>();
  }

  if (data.find("capacity") != data.end()) {
    capacity = data["capacity"].get<int>();
  }

  // AUSF info
  if (data.find("ausfInfo") != data.end()) {
    nlohmann::json info = data["ausfInfo"];
    if (info.find("groupId") != info.end()) {
      ausf_info.groupid = info["groupId"].get<std::string>();
    }
    if (info.find("routingIndicators") != info.end()) {
      nlohmann::json routing_indicators_list =
          data["ausfInfo"]["routingIndicators"];
      for (auto d : routing_indicators_list) {
        ausf_info.routing_indicators.push_back(d);
      }
    }
    if (info.find("supiRanges") != info.end()) {
      nlohmann::json supi_ranges = data["ausfInfo"]["supiRanges"];
      for (auto d : supi_ranges) {
        supi_range_ausf_info_item_t supi;
        supi.supi_range.start   = d["start"];
        supi.supi_range.end     = d["end"];
        supi.supi_range.pattern = d["pattern"];
        ausf_info.supi_ranges.push_back(supi);
      }
    }
  }
  display();
}

//------------------------------------------------------------------------------
void ausf_profile::handle_heartbeart_timeout(uint64_t ms) {
  Logger::ausf_app().info(
      "Handle heartbeart timeout profile %s, time %d", nf_instance_id.c_str(),
      ms);
  set_nf_status("SUSPENDED");
}
