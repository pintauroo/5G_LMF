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

/*! \file udm_app.hpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: Tien-Thinh.Nguyen@eurecom.fr
 */

#ifndef FILE_UDM_APP_HPP_SEEN
#define FILE_UDM_APP_HPP_SEEN

#include <string>
#include <pistache/http.h>
#include <map>
#include <shared_mutex>

#include "AuthenticationInfoRequest.h"
#include "PlmnId.h"
#include "Amf3GppAccessRegistration.h"
#include "Snssai.h"
#include "PlmnId.h"
#include "SdmSubscription.h"
#include "AuthEvent.h"
#include "EeSubscription.h"
#include "CreatedEeSubscription.h"
#include "PatchItem.h"
#include "ProblemDetails.h"
#include "uint_generator.hpp"
#include "udm.h"
#include "udm_event.hpp"

namespace oai::udm::app {

// class ausf_config;
class udm_app {
 public:
  explicit udm_app(const std::string& config_file);
  udm_app(udm_app const&) = delete;
  void operator=(udm_app const&) = delete;

  virtual ~udm_app();
  void handle_generate_auth_data_request(
      const std::string& supiOrSuci,
      const oai::udm::model::AuthenticationInfoRequest&
          authenticationInfoRequest,
      nlohmann::json& auth_info_response, long& code);

  void handle_confirm_auth(
      const std::string& supi, const oai::udm::model::AuthEvent& authEvent,
      nlohmann::json& confirm_response, std::string& location, long& code);

  void handle_delete_auth(
      const std::string& supi, const std::string& authEventId,
      const oai::udm::model::AuthEvent& authEvent,
      nlohmann::json& auth_response, long& code);

  void handle_access_mobility_subscription_data_retrieval(
      const std::string& supi, nlohmann::json& response_data, long& code,
      oai::udm::model::PlmnId PlmnId = {});

  void handle_amf_registration_for_3gpp_access(
      const std::string& ue_id,
      const oai::udm::model::Amf3GppAccessRegistration&
          amf_3gpp_access_registration,
      nlohmann::json& response_data, long& code);

  void handle_session_management_subscription_data_retrieval(
      const std::string& supi, nlohmann::json& response_data, long& code,
      oai::udm::model::Snssai snssai = {}, std::string dnn = {},
      oai::udm::model::PlmnId plmn_id = {});

  void handle_slice_selection_subscription_data_retrieval(
      const std::string& supi, nlohmann::json& response_data, long& code,
      std::string supported_features  = {},
      oai::udm::model::PlmnId plmn_id = {});

  void handle_smf_selection_subscription_data_retrieval(
      const std::string& supi, nlohmann::json& response_data, long& code,
      std::string supported_features  = {},
      oai::udm::model::PlmnId plmn_id = {});

  void handle_subscription_creation(
      const std::string& supi,
      const oai::udm::model::SdmSubscription& sdmSubscription,
      nlohmann::json& response_data, long& code);

  evsub_id_t handle_create_ee_subscription(
      const std::string& ueIdentity,
      const oai::udm::model::EeSubscription& eeSubscription,
      oai::udm::model::CreatedEeSubscription& createdSub, long& code);

  void handle_delete_ee_subscription(
      const std::string& ueIdentity, const std::string& subscriptionId,
      oai::udm::model::ProblemDetails& problemDetails, long& code);

  void handle_update_ee_subscription(
      const std::string& ueIdentity, const std::string& subscriptionId,
      const std::vector<oai::udm::model::PatchItem>& patchItem,
      oai::udm::model::ProblemDetails& problemDetails, long& code);

  /*
   * Generate an unique ID for the new subscription
   * @return the generated ID
   */
  evsub_id_t generate_ev_subscription_id();

  /*
   * Add an Event Subscription to the list
   * @param [const evsub_id_t&] sub_id: Subscription ID
   * @param [std::string] ue_id: UE's identity
   * @param [std::shared_ptr<oai::udm::model::CreatedEeSubscription>] ces: a
   * shared pointer stored information of the created subscription
   * @return void
   */
  void add_event_subscription(
      const evsub_id_t& sub_id, const std::string& ue_id,
      std::shared_ptr<oai::udm::model::CreatedEeSubscription>& ces);

  /*
   * Delete an Event Subscription
   * @param [const std::string&] sub_id: Subscription ID
   * @param [std::string] ue_id: UE's identity
   * @return true if success, otherwise false
   */
  bool delete_event_subscription(
      const std::string& sub_id, const std::string& ue_id);

  /*
   * Update a new item for a subscription
   * @param [const std::string &] path: item name
   * @param [const std::string &] value: new value
   * @return true if success, otherwise false
   */
  bool replace_ee_subscription_item(
      const std::string& path, const std::string& value);

  /*
   * Add a new item for a subscription
   * @param [const std::string &] path: item name
   * @param [const std::string &] value: new value
   * @return true if success, otherwise false
   */
  bool add_ee_subscription_item(
      const std::string& path, const std::string& value);

  /*
   * Remove an item for a subscription
   * @param [const std::string &] path: item name
   * @return true if success, otherwise false
   */
  bool remove_ee_subscription_item(const std::string& path);

  /*
   * Handle Loss of Connectivity Event
   * @param [const std::string&] ue_id: UE's identity (e.g., SUPI)
   * @param [uint8_t] status: Connectivity status
   * @param [uint8_t] http_version: HTTP version
   * @return void
   */
  void handle_ee_loss_of_connectivity(
      const std::string& ue_id, uint8_t status, uint8_t http_version);

  /*
   * Handle UE Reachability For Data Event
   * @param [const std::string&] ue_id: UE's identity (e.g., SUPI)
   * @param [uint8_t] status: UE Reachability For Data status
   * @param [uint8_t] http_version: HTTP version
   * @return void
   */
  void handle_ee_ue_reachability_for_data(
      const std::string& ue_id, uint8_t status, uint8_t http_version);

 private:
  util::uint_generator<uint32_t> evsub_id_generator;
  std::map<evsub_id_t, std::shared_ptr<oai::udm::model::CreatedEeSubscription>>
      udm_event_subscriptions;
  std::map<std::string, std::vector<evsub_id_t>> udm_event_subscriptions_per_ue;
  mutable std::shared_mutex m_mutex_udm_event_subscriptions;

  // for Event Handling
  udm_event event_sub;
  bs2::connection loss_of_connectivity_connection;
  bs2::connection ue_reachability_for_data_connection;
};
}  // namespace oai::udm::app
#include "udm_config.hpp"

#endif /* FILE_UDM_APP_HPP_SEEN */
