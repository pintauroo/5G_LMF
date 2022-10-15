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

/*! \file ausf_nrf.cpp
 \brief
 \author  Jian Yang, Fengjiao He, Hongxin Wang, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email:
 */

#include "ausf_nrf.hpp"
#include "ausf_app.hpp"
#include "ausf_profile.hpp"
#include "ausf_client.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <curl/curl.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "logger.hpp"
#include "ausf.h"

using namespace config;
// using namespace ausf;
using namespace oai::ausf::app;

using json = nlohmann::json;

extern ausf_config ausf_cfg;
extern ausf_nrf* ausf_nrf_inst;
ausf_client* ausf_client_instance = nullptr;

//------------------------------------------------------------------------------
ausf_nrf::ausf_nrf() {}
//---------------------------------------------------------------------------------------------
void ausf_nrf::get_ausf_api_root(std::string& api_root) {
  api_root = std::string(
                 inet_ntoa(*((struct in_addr*) &ausf_cfg.nrf_addr.ipv4_addr))) +
             ":" + std::to_string(ausf_cfg.nrf_addr.port) + NNRF_NFM_BASE +
             ausf_cfg.nrf_addr.api_version;
}

//---------------------------------------------------------------------------------------------
void ausf_nrf::generate_ausf_profile(
    ausf_profile& ausf_nf_profile, std::string& ausf_instance_id) {
  // TODO: remove hardcoded values
  ausf_nf_profile.set_nf_instance_id(ausf_instance_id);
  ausf_nf_profile.set_nf_instance_name("OAI-AUSF");
  ausf_nf_profile.set_nf_type("AUSF");
  ausf_nf_profile.set_nf_status("REGISTERED");
  ausf_nf_profile.set_nf_heartBeat_timer(50);
  ausf_nf_profile.set_nf_priority(1);
  ausf_nf_profile.set_nf_capacity(100);
  // ausf_nf_profile.set_fqdn(ausf_cfg.fqdn);
  ausf_nf_profile.add_nf_ipv4_addresses(ausf_cfg.sbi.addr4);  // N4's Addr

  // AUSF info (Hardcoded for now)
  ausf_info_t ausf_info_item;
  supi_range_ausf_info_item_t supi_ranges;
  ausf_info_item.groupid = "oai-ausf-testgroupid";
  ausf_info_item.routing_indicators.push_back("0210");
  ausf_info_item.routing_indicators.push_back("9876");
  supi_ranges.supi_range.start   = "109238210938";
  supi_ranges.supi_range.pattern = "209238210938";
  supi_ranges.supi_range.start   = "q0930j0c80283ncjf";
  ausf_info_item.supi_ranges.push_back(supi_ranges);
  ausf_nf_profile.set_ausf_info(ausf_info_item);
  // AUSF info item end

  ausf_nf_profile.display();
}
//---------------------------------------------------------------------------------------------
void ausf_nrf::register_to_nrf() {
  // generate UUID
  std::string ausf_instance_id;  // AUSF instance id
  ausf_instance_id             = to_string(boost::uuids::random_generator()());
  nlohmann::json response_data = {};

  // Generate NF Profile
  ausf_profile ausf_nf_profile;
  generate_ausf_profile(ausf_nf_profile, ausf_instance_id);

  // Send NF registeration request
  std::string ausf_api_root = {};
  std::string response      = {};
  std::string method        = {"PUT"};
  get_ausf_api_root(ausf_api_root);
  std::string remoteUri =
      ausf_api_root + AUSF_NF_REGISTER_URL + ausf_instance_id;
  nlohmann::json json_data = {};
  ausf_nf_profile.to_json(json_data);

  Logger::ausf_nrf().info("Sending NF registeration request");
  ausf_client_instance->curl_http_client(
      remoteUri, method, json_data.dump().c_str(), response);

  try {
    response_data = nlohmann::json::parse(response);
    if (response_data["nfStatus"].dump().c_str() == "REGISTERED") {
      // ToDo Trigger NF heartbeats
    }
  } catch (nlohmann::json::exception& e) {
    Logger::ausf_nrf().info("NF registeration procedure failed");
  }
}
//---------------------------------------------------------------------------------------------
