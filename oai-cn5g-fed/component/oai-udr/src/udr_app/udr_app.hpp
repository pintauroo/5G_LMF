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

/*! \file udr_app.hpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: Tien-Thinh.Nguyen@eurecom.fr
 */

#ifndef FILE_UDR_APP_HPP_SEEN
#define FILE_UDR_APP_HPP_SEEN

#include <mysql/mysql.h>
#include <pistache/http.h>

#include <nlohmann/json.hpp>
#include <string>

#include "Amf3GppAccessRegistration.h"
#include "AuthEvent.h"
#include "PatchItem.h"
#include "SdmSubscription.h"
#include "SmfRegistration.h"

using namespace oai::udr::model;

namespace oai {
namespace udr {
namespace app {

class udr_app {
public:
  explicit udr_app(const std::string &config_file);
  udr_app(udr_app const &) = delete;
  void operator=(udr_app const &) = delete;

  virtual ~udr_app();

  /*
   * Handle a query for AccessandMobilitySubscriptionData
   * (AccessAndMobilitySubscriptionDataDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string& ] serving_plmn_id: Serving PLMN ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_am_data(const std::string &ue_id,
                            const std::string &serving_plmn_id,
                            nlohmann::json &response_data, long &code);

  /*
   * Handle a request to create AMF3GPPAccessRegistration Context
   * (AMF3GPPAccessRegistrationDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [Amf3GppAccessRegistration& ] amf3GppAccessRegistration:
   * Amf3GppAccessRegistration
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_create_amf_context_3gpp(
      const std::string &ue_id,
      Amf3GppAccessRegistration &amf3GppAccessRegistration,
      nlohmann::json &response_data, long &code);

  /*
   * Handle a query for AMF3GPPAccessRegistration
   * (AMF3GPPAccessRegistrationDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_amf_context_3gpp(const std::string &ue_id,
                                     nlohmann::json &response_data, long &code);

  /*
   * Handle a request to create AuthenticationStatus
   * (AuthenticationStatusDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const AuthEvent& ] authEvent:
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_create_authentication_status(const std::string &ue_id,
                                           const AuthEvent &authEvent,
                                           nlohmann::json &response_data,
                                           long &code);

  /*
   * Handle a request to delete AuthenticationStatus
   * (AuthenticationStatusDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_delete_authentication_status(const std::string &ue_id,
                                           nlohmann::json &response_data,
                                           long &code);

  /*
   * Handle a request to retrieve AuthenticationStatus
   * (AuthenticationStatusDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_authentication_status(const std::string &ue_id,
                                          nlohmann::json &response_data,
                                          long &code);

  /*
   * Handle a request to modify AuthenticationSubscription
   * (AuthenticationSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::vector<PatchItem>&] patchItem: PATCH message
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_modify_authentication_subscription(
      const std::string &ue_id, const std::vector<PatchItem> &patchItem,
      nlohmann::json &response_data, long &code);

  /*
   * Handle a request to get AuthenticationSubscription
   * (AuthenticationSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_read_authentication_subscription(const std::string &ue_id,
                                               nlohmann::json &response_data,
                                               long &code);

  /*
   * Handle a request to retrieve SDMSubscription
   * (SDMSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string&] subs_id: subscription ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_sdm_subscription(const std::string &ue_id,
                                     const std::string &subs_id,
                                     nlohmann::json &response_data, long &code);

  /*
   * Handle a request to remove SDMSubscription (SDMSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string&] subs_id: subscription ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_remove_sdm_subscription(const std::string &ue_id,
                                      const std::string &subs_id,
                                      nlohmann::json &response_data,
                                      long &code);

  /*
   * Handle a request to update SDMSubscription (SDMSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string&] subs_id: subscription ID
   * @param [SdmSubscription&] sdmSubscription: subscription information
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_update_sdm_subscription(const std::string &ue_id,
                                      const std::string &subs_id,
                                      SdmSubscription &sdmSubscription,
                                      nlohmann::json &response_data,
                                      long &code);

  /*
   * Handle a request to create SDMSubscriptions
   * (SDMSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [SdmSubscription&] sdmSubscription: subscription information
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_create_sdm_subscriptions(const std::string &ue_id,
                                       SdmSubscription &sdmSubscription,
                                       nlohmann::json &response_data,
                                       long &code);

  /*
   * Handle a request to retrieve SDMSubscriptions
   * (SDMSubscriptionDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_sdm_subscriptions(const std::string &ue_id,
                                      nlohmann::json &response_data,
                                      long &code);

  /*
   * Handle a request to retrieve SessionManagementSubscription
   * (SessionManagementSubscriptionDataApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string&] serving_plmn_id: Serving PLMN ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_sm_data(const std::string &ue_id,
                            const std::string &serving_plmn_id,
                            nlohmann::json &response_data, long &code,
                            oai::udr::model::Snssai snssai = {},
                            std::string dnn = {});

  /*
   * Handle a request to create SMFRegistration (SMFRegistrationDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const int32_t&] pdu_session_id: PDU Session ID
   * @param [const SmfRegistration&] smfRegistration: SMF registration info
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void
  handle_create_smf_context_non_3gpp(const std::string &ue_id,
                                     const int32_t &pdu_session_id,
                                     const SmfRegistration &smfRegistration,
                                     nlohmann::json &response_data, long &code);

  /*
   * Handle a request to delete SMFRegistration (SMFRegistrationDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const int32_t&] pdu_session_id: PDU Session ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_delete_smf_context(const std::string &ue_id,
                                 const int32_t &pdu_session_id,
                                 nlohmann::json &response_data, long &code);

  /*
   * Handle a request to retrieve SMFRegistration
   * (SMFRegistrationDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const int32_t&] pdu_session_id: PDU Session ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_smf_registration(const std::string &ue_id,
                                     const int32_t &pdu_session_id,
                                     nlohmann::json &response_data, long &code);

  /*
   * Handle a request to retrieve SMFRegistrationsCollection
   * (SMFRegistrationsCollectionApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_smf_reg_list(const std::string &ue_id,
                                 nlohmann::json &response_data, long &code);

  /*
   * Handle a request to retrieve SMFSelectionSubscription
   * (SMFSelectionSubscriptionDataDocumentApiImpl)
   * @param [const std::string&] ue_id: UE Identity
   * @param [const std::string&] serving_plmn_id: Serving PLMN ID
   * @param [nlohmann::json&] response_data: Response in Json format
   * @param [long code] code: HTTP response code
   * @return void
   */
  void handle_query_smf_select_data(const std::string &ue_id,
                                    const std::string &serving_plmn_id,
                                    nlohmann::json &response_data, long &code);

private:
  MYSQL mysql;
};
} // namespace app
} // namespace udr
} // namespace oai
#include "udr_config.hpp"

#endif /* FILE_UDR_APP_HPP_SEEN */
