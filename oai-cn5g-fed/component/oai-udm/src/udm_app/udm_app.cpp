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

/*! \file udm_app.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: Tien-Thinh.Nguyen@eurecom.fr
 */

#include "udm_app.hpp"

#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>

#include "logger.hpp"
#include "udm_client.hpp"
#include "udm_config.hpp"
#include "ProblemDetails.h"
#include "conversions.hpp"
#include "authentication_algorithms_with_5gaka.hpp"
#include "SequenceNumber.h"
#include "PatchItem.h"
#include "comUt.hpp"
#include "sha256.hpp"
#include "udm.h"
#include "api_conversions.hpp"
#include "3gpp_29.500.h"

using namespace oai::udm::app;
using namespace oai::udm::model;
using namespace std::chrono;
using namespace oai::udm::config;

extern udm_app* udm_app_inst;
extern udm_config udm_cfg;
udm_client* udm_client_inst = nullptr;

//------------------------------------------------------------------------------
udm_app::udm_app(const std::string& config_file) : event_sub() {
  Logger::udm_app().startup("Starting...");
  try {
    udm_client_inst = new udm_client();
  } catch (std::exception& e) {
    Logger::udm_app().error("Cannot create UDM APP: %s", e.what());
    throw;
  }
  // TODO: Register to NRF

  // Subscribe to UE Loss of Connectivity Status signal
  loss_of_connectivity_connection = event_sub.subscribe_loss_of_connectivity(
      boost::bind(&udm_app::handle_ee_loss_of_connectivity, this, _1, _2, _3));
  ue_reachability_for_data_connection =
      event_sub.subscribe_ue_reachability_for_data(boost::bind(
          &udm_app::handle_ee_ue_reachability_for_data, this, _1, _2, _3));

  Logger::udm_app().startup("Started");
}

//------------------------------------------------------------------------------
udm_app::~udm_app() {
  // Disconnect the boost connection
  if (loss_of_connectivity_connection.connected())
    loss_of_connectivity_connection.disconnect();
  if (ue_reachability_for_data_connection.connected())
    ue_reachability_for_data_connection.disconnect();
  Logger::udm_app().debug("Delete UDM APP instance...");
}

//------------------------------------------------------------------------------
void udm_app::handle_generate_auth_data_request(
    const std::string& supiOrSuci,
    const oai::udm::model::AuthenticationInfoRequest& authenticationInfoRequest,
    nlohmann::json& auth_info_response, long& code) {
  Logger::udm_ueau().info("Handle Generate Auth Data Request");
  uint8_t rand[16] = {0};
  uint8_t opc[16]  = {0};
  uint8_t key[16]  = {0};
  uint8_t sqn[6]   = {0};
  uint8_t amf[2]   = {0};

  uint8_t* r_sqn        = nullptr;  // for resync
  std::string r_sqnms_s = {};       // for resync
  uint8_t r_rand[16]    = {0};      // for resync
  uint8_t r_auts[14]    = {0};      // for resync

  uint8_t mac_a[8]     = {0};
  uint8_t ck[16]       = {0};
  uint8_t ik[16]       = {0};
  uint8_t ak[6]        = {0};
  uint8_t xres[8]      = {0};
  uint8_t xresStar[16] = {0};
  uint8_t autn[16]     = {0};
  uint8_t kausf[32]    = {0};

  std::string rand_s     = {};
  std::string autn_s     = {};
  std::string xresStar_s = {};
  std::string kausf_s    = {};
  std::string sqn_s      = {};
  std::string amf_s      = {};
  std::string key_s      = {};
  std::string opc_s      = {};

  std::string snn  = authenticationInfoRequest.getServingNetworkName();
  std::string supi = supiOrSuci;

  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port  = std::to_string(udm_cfg.udr_addr.port);
  std::string remoteUri = {};
  std::string Method    = {};
  std::string msgBody   = {};
  std::string Response  = {};

  nlohmann::json j_ProblemDetails = {};
  ProblemDetails m_ProblemDetails = {};

  // UDR GET interface ----- get authentication related info--------------------
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              NUDR_AUTHENTICATION_SUBSCRIPTION_ENDPOINT;
  Logger::udm_ueau().debug("GET Request:" + remoteUri);
  Method = "GET";

  udm_client::curl_http_client(remoteUri, Method, Response);

  nlohmann::json response_data = {};
  try {
    response_data = nlohmann::json::parse(Response.c_str());
  } catch (nlohmann::json::exception& e) {  // error handling
    Logger::udm_ueau().info("Could not get Json content from UDR response");

    m_ProblemDetails.setCause("USER_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("User " + supi + " not found");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("User " + supi + " not found");
    Logger::udm_ueau().info("Send 404 Not_Found response to AUSF");
    auth_info_response = j_ProblemDetails;
    code               = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }

  std::string authMethod_s = response_data.at("authenticationMethod");
  if (!authMethod_s.compare("5G_AKA") ||
      !authMethod_s.compare("AuthenticationVector")) {
    try {
      key_s = response_data.at("encPermanentKey");
      conv::hex_str_to_uint8(key_s.c_str(), key);
      comUt::print_buffer("udm_ueau", "Result For F1-Alg Key", key, 16);

      opc_s = response_data.at("encOpcKey");
      conv::hex_str_to_uint8(opc_s.c_str(), opc);
      comUt::print_buffer("udm_ueau", "Result For F1-Alg OPC", opc, 16);

      amf_s = response_data.at("authenticationManagementField");
      conv::hex_str_to_uint8(amf_s.c_str(), amf);
      comUt::print_buffer("udm_ueau", "Result For F1-Alg AMF", amf, 2);

      sqn_s = response_data["sequenceNumber"].at("sqn");
      conv::hex_str_to_uint8(sqn_s.c_str(), sqn);
      comUt::print_buffer("udm_ueau", "Result For F1-Alg SQN: ", sqn, 6);
    } catch (nlohmann::json::exception& e) {
      // error handling
      m_ProblemDetails.setCause("AUTHENTICATION_REJECTED");
      m_ProblemDetails.setStatus(403);
      m_ProblemDetails.setDetail(
          "Missing authentication parameter in UDR response");
      to_json(j_ProblemDetails, m_ProblemDetails);

      Logger::udm_ueau().error(
          "Missing authentication parameter in UDR response");
      Logger::udm_ueau().info("Send 403 Forbidden response to AUSF");
      auth_info_response = j_ProblemDetails;
      code               = HTTP_RESPONSE_CODE_FORBIDDEN;
      return;
    }
  } else {
    // error handling
    m_ProblemDetails.setCause("UNSUPPORTED_PROTECTION_SCHEME");
    m_ProblemDetails.setStatus(501);
    m_ProblemDetails.setDetail(
        "Non 5G_AKA authenticationMethod configuration in database");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error(
        "Non 5G_AKA authenticationMethod configuration in "
        "database, method set = " +
        authMethod_s);
    Logger::udm_ueau().info("Send 501 Not_Implemented response to AUSF");
    auth_info_response = j_ProblemDetails;
    code               = HTTP_RESPONSE_CODE_NOT_IMPLEMENTED;
    return;
  }

  if (authenticationInfoRequest.resynchronizationInfoIsSet()) {
    // Resync procedure
    Logger::udm_ueau().info("Start resynchronization procedure");
    ResynchronizationInfo m_ResynchronizationInfo =
        authenticationInfoRequest.getResynchronizationInfo();
    std::string r_rand_s = m_ResynchronizationInfo.getRand();
    std::string r_auts_s = m_ResynchronizationInfo.getAuts();

    Logger::udm_ueau().info("[resync] r_rand = " + r_rand_s);
    Logger::udm_ueau().info("[resync] r_auts = " + r_auts_s);

    conv::hex_str_to_uint8(r_rand_s.c_str(), r_rand);
    conv::hex_str_to_uint8(r_auts_s.c_str(), r_auts);

    r_sqn = Authentication_5gaka::sqn_ms_derive(opc, key, r_auts, r_rand, amf);

    if (r_sqn) {  // Not NULL (validate auts)
      Logger::udm_ueau().info("Valid AUTS, generate new AV with SQNms");

      // UDR PATCH interface
      // replace SQNhe with SQNms
      remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
                  udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
                  NUDR_AUTHENTICATION_SUBSCRIPTION_ENDPOINT;
      Logger::udm_ueau().debug("PATCH Request:" + remoteUri);
      Method = "PATCH";

      nlohmann::json j_SequenceNumber;
      SequenceNumber m_SequenceNumber;
      m_SequenceNumber.setSqnScheme("NON_TIME_BASED");
      r_sqnms_s = conv::uint8_to_hex_string(r_sqn, 6);
      m_SequenceNumber.setSqn(r_sqnms_s);
      std::map<std::string, int32_t> index;
      index["ausf"] = 0;
      m_SequenceNumber.setLastIndexes(index);
      to_json(j_SequenceNumber, m_SequenceNumber);

      Logger::udm_ueau().info(
          "Sequence Number %s", j_SequenceNumber.dump().c_str());

      nlohmann::json j_PatchItem = {};
      PatchItem m_PatchItem      = {};
      m_PatchItem.setValue(j_SequenceNumber.dump());
      m_PatchItem.setOp("replace");
      m_PatchItem.setFrom("");
      m_PatchItem.setPath("");
      to_json(j_PatchItem, m_PatchItem);

      msgBody = "[" + j_PatchItem.dump() + "]";
      Logger::udm_ueau().info("PATCH Request body: %s", msgBody.c_str());

      udm_client::curl_http_client(remoteUri, Method, Response, msgBody);

      // replace SQNhe with SQNms
      int i = 0;
      for (i; i < 6; i++) sqn[i] = r_sqn[i];  // generate first, increase later
      sqn_s = conv::uint8_to_hex_string(sqn, 16);
      // Logger::udm_ueau().debug("sqn string = "+sqn_s);
      sqn_s[12] = '\0';

      comUt::print_buffer("udm_ueau", "SQNms", sqn, 6);

      if (r_sqn) {  // free
        free(r_sqn);
        r_sqn = NULL;
      }
    } else {
      Logger::udm_ueau().error(
          "Invalid AUTS, generate new AV with SQNhe = " + sqn_s);
    }
  }

  // 5GAKA functions
  Authentication_5gaka::generate_random(rand, 16);  // generate rand
  Authentication_5gaka::f1(
      opc, key, rand, sqn, amf,
      mac_a);  // to compute mac_a
  Authentication_5gaka::f2345(
      opc, key, rand, xres, ck, ik,
      ak);  // to compute XRES, CK, IK, AK
  Authentication_5gaka::generate_autn(
      sqn, ak, amf, mac_a,
      autn);  // generate AUTN
  Authentication_5gaka::annex_a_4_33501(
      ck, ik, xres, rand, snn,
      xresStar);  // generate xres*
  Authentication_5gaka::derive_kausf(
      ck, ik, snn, sqn, ak,
      kausf);  // derive Kausf

  // convert uint8_t to string
  rand_s     = conv::uint8_to_hex_string(rand, 16);
  autn_s     = conv::uint8_to_hex_string(autn, 16);
  xresStar_s = conv::uint8_to_hex_string(xresStar, 16);
  kausf_s    = conv::uint8_to_hex_string(kausf, 32);

  // convert to json
  nlohmann::json AuthInfoResult                      = {};
  AuthInfoResult["authType"]                         = "5G_AKA";
  AuthInfoResult["authenticationVector"]["avType"]   = "5G_HE_AKA";
  AuthInfoResult["authenticationVector"]["rand"]     = rand_s;
  AuthInfoResult["authenticationVector"]["autn"]     = autn_s;
  AuthInfoResult["authenticationVector"]["xresStar"] = xresStar_s;
  AuthInfoResult["authenticationVector"]["kausf"]    = kausf_s;

  // TODO: Separate into a new function
  // Do it after send ok to AUSF (to be verified)

  // Calculate new sqn
  unsigned long long sqn_value;
  std::stringstream s1;
  s1 << std::hex << sqn_s;
  s1 >> sqn_value;  // hex string to decimal value
  sqn_value += 32;
  std::stringstream s2;
  s2 << std::hex << std::setw(12) << std::setfill('0')
     << sqn_value;  // decimal value to hex string
  std::string new_sqn(s2.str());

  Logger::udm_ueau().info("new_sqn = " + new_sqn);

  // UDR PATCH interface
  // Increase sqn
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              NUDR_AUTHENTICATION_SUBSCRIPTION_ENDPOINT;
  Logger::udm_ueau().debug("PATCH Request:" + remoteUri);
  Method = "PATCH";

  nlohmann::json j_SequenceNumber;
  SequenceNumber m_SequenceNumber;
  m_SequenceNumber.setSqnScheme("NON_TIME_BASED");
  m_SequenceNumber.setSqn(new_sqn);
  std::map<std::string, int32_t> index;
  index["ausf"] = 0;
  m_SequenceNumber.setLastIndexes(index);
  to_json(j_SequenceNumber, m_SequenceNumber);

  nlohmann::json j_PatchItem;
  PatchItem m_PatchItem;
  m_PatchItem.setValue(j_SequenceNumber.dump());
  m_PatchItem.setOp("replace");
  m_PatchItem.setFrom("");
  m_PatchItem.setPath("");
  to_json(j_PatchItem, m_PatchItem);

  msgBody = "[" + j_PatchItem.dump() + "]";
  Logger::udm_ueau().info(
      "Update UDR with PATCH message, body:  %s", msgBody.c_str());

  udm_client::curl_http_client(remoteUri, Method, Response, msgBody);

  Logger::udm_ueau().info("Send 200 Ok response to AUSF");
  Logger::udm_ueau().info("AuthInfoResult %s", AuthInfoResult.dump().c_str());
  auth_info_response = AuthInfoResult;
  code               = HTTP_RESPONSE_CODE_OK;
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_confirm_auth(
    const std::string& supi, const oai::udm::model::AuthEvent& authEvent,
    nlohmann::json& confirm_response, std::string& location, long& code) {
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string remoteUri;
  std::string Method;
  std::string msgBody;
  std::string Response;
  std::string Location;
  std::string authEventId;

  nlohmann::json j_ProblemDetails;
  ProblemDetails m_ProblemDetails;

  // UDR GET interface
  // get user info
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              NUDR_AUTHENTICATION_SUBSCRIPTION_ENDPOINT;
  Logger::udm_ueau().debug("GET Request:" + remoteUri);
  Method = "GET";

  udm_client::curl_http_client(remoteUri, Method, Response);

  nlohmann::json response_data = {};
  try {
    response_data = nlohmann::json::parse(Response.c_str());
  } catch (nlohmann::json::exception& e) {  // error handling
    Logger::udm_ueau().info("Could not get Json content from UDR response");

    m_ProblemDetails.setCause("USER_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("User " + supi + " not found");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("User " + supi + " not found");
    Logger::udm_ueau().info("Send 404 Not_Found response to AUSF");
    confirm_response = j_ProblemDetails;
    code             = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }

  if (authEvent.isAuthRemovalInd()) {
    // error handling
    m_ProblemDetails.setStatus(400);
    m_ProblemDetails.setDetail("authRemovalInd should be false");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("authRemovalInd should be false");
    Logger::udm_ueau().info("Send 400 Bad_Request response to AUSF");
    confirm_response = j_ProblemDetails;
    code             = HTTP_RESPONSE_CODE_BAD_REQUEST;
    return;
  }

  // UDR PUT interface
  // Put authentication status
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              "/authentication-data/authentication-status";

  Logger::udm_ueau().debug("PUT Request:" + remoteUri);
  Method = "PUT";

  nlohmann::json j_authEvent;
  to_json(j_authEvent, authEvent);

  msgBody = j_authEvent.dump();
  Logger::udm_ueau().debug("PATCH Request body = " + msgBody);

  udm_client::curl_http_client(remoteUri, Method, Response, msgBody);

  std::string hash_value = sha256(supi + authEvent.getServingNetworkName());
  // Logger::udm_ueau().debug("\n\nauthEventId=" +
  // hash_value.substr(0,hash_value.length()/2));
  Logger::udm_ueau().debug("authEventId=" + hash_value);

  authEventId = hash_value;  // Represents the authEvent Id per UE per serving
                             // network assigned by the UDM during
                             // ResultConfirmation service operation.
  location = std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.sbi.addr4))) +
             ":" + std::to_string(udm_cfg.sbi.port) + "/nudm-ueau/" +
             udm_cfg.sbi.api_version + "/" + supi + "/auth-events/" +
             authEventId;

  Logger::udm_ueau().info("Send 201 Created response to AUSF");
  confirm_response = j_authEvent;
  code             = HTTP_RESPONSE_CODE_CREATED;
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_delete_auth(
    const std::string& supi, const std::string& authEventId,
    const oai::udm::model::AuthEvent& authEvent, nlohmann::json& auth_response,
    long& code) {
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string remoteUri;
  std::string Method;
  std::string msgBody;
  std::string Response;
  std::string Location;

  nlohmann::json j_ProblemDetails;
  ProblemDetails m_ProblemDetails;

  // UDR GET interface
  // get user info
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              NUDR_AUTHENTICATION_SUBSCRIPTION_ENDPOINT;
  Logger::udm_ueau().debug("GET Request:" + remoteUri);
  Method = "GET";

  udm_client::curl_http_client(remoteUri, Method, Response);

  nlohmann::json response_data = {};
  try {
    response_data = nlohmann::json::parse(Response.c_str());
  } catch (nlohmann::json::exception& e) {  // error handling
    Logger::udm_ueau().info("Could not get Json content from UDR response");

    m_ProblemDetails.setCause("USER_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("User " + supi + " not found");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("User " + supi + " not found");
    Logger::udm_ueau().info("Send 404 Not_Found response to AUSF");
    auth_response = j_ProblemDetails;
    code          = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }

  if (!authEvent.isAuthRemovalInd()) {
    // error handling
    m_ProblemDetails.setStatus(400);
    m_ProblemDetails.setDetail("authRemovalInd should be true");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("authRemovalInd should be true");
    Logger::udm_ueau().info("Send 400 Bad_Request response to AUSF");
    auth_response = j_ProblemDetails;
    code          = HTTP_RESPONSE_CODE_BAD_REQUEST;
    return;
  }

  std::string hash_value = sha256(supi + authEvent.getServingNetworkName());
  // Logger::udm_ueau().debug("\n\nauthEventId=" +
  // hash_value.substr(0,hash_value.length()/2));
  Logger::udm_ueau().debug("authEventId=" + hash_value);

  if (!hash_value.compare(authEventId)) {
    // UDR DELETE interface
    // delete authentication status
    remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
                udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
                "/authentication-data/authentication-status";

    Logger::udm_ueau().debug("DELETE Request:" + remoteUri);
    Method = "DELETE";

    nlohmann::json j_authEvent;
    to_json(j_authEvent, authEvent);

    udm_client::curl_http_client(remoteUri, Method, Response);

    Logger::udm_ueau().info("Send 204 No_Content response to AUSF");
    auth_response = {};
    code          = HTTP_RESPONSE_CODE_NO_CONTENT;
    return;
  } else {
    // error handling
    // wrong AuthEventId
    m_ProblemDetails.setCause("DATA_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("Wrong authEventId");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_ueau().error("Wrong authEventId, should be = " + hash_value);
    Logger::udm_ueau().info("Send 404 Not_Found response to AUSF");
    auth_response = j_ProblemDetails;
    code          = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }
}

//------------------------------------------------------------------------------
void udm_app::handle_access_mobility_subscription_data_retrieval(
    const std::string& supi, nlohmann::json& response_data, long& code,
    oai::udm::model::PlmnId plmn_id) {
  // TODO: remove hardcoded path
  // TODO: check if plmn_id available
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string remote_uri =
      udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
      udm_cfg.udr_addr.api_version + "/subscription-data/" + supi + "/" +
      plmn_id.getMcc() + plmn_id.getMnc() + "/provisioned-data/am-data";

  std::string method("GET");
  std::string body("");
  std::string response_get;
  Logger::udm_sdm().debug("UDR: GET Request: " + remote_uri);
  // Use curl to get response from UDR
  udm_client::curl_http_client(remote_uri, method, response_get, body);
  try {
    Logger::udm_sdm().debug("subscription-data: GET Response: " + response_get);
    response_data = nlohmann::json::parse(response_get.c_str());
  } catch (nlohmann::json::exception& e) {
    Logger::udm_sdm().info("Could not get json content from UDR response");
    ProblemDetails problem_details;
    nlohmann::json json_problem_details;
    problem_details.setCause("USER_NOT_FOUND");
    problem_details.setStatus(404);
    problem_details.setDetail("User " + supi + " not found");
    to_json(json_problem_details, problem_details);
    Logger::udm_sdm().error("User " + supi + " not found");
    Logger::udm_sdm().info("Send 404 Not_Found response to client");

    response_data = json_problem_details;
    code          = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }
}

//------------------------------------------------------------------------------
void udm_app::handle_amf_registration_for_3gpp_access(
    const std::string& ue_id,
    const oai::udm::model::Amf3GppAccessRegistration&
        amf_3gpp_access_registration,
    nlohmann::json& response_data, long& code) {
  // TODO: to be completed
  std::string remoteUri;
  std::string response;
  nlohmann::json j_ProblemDetails;
  ProblemDetails m_ProblemDetails;

  // UDR GET interface
  // get 3gpp_registration related info
  remoteUri =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr))) +
      ":" + std::to_string(udm_cfg.udr_addr.port) + NUDR_DATA_REPOSITORY +
      udm_cfg.udr_addr.api_version + "/subscription-data/" + ue_id +
      "/context-data/amf-3gpp-access";
  Logger::udm_uecm().debug("PUT Request:" + remoteUri);

  nlohmann::json amf_registration_json;
  to_json(amf_registration_json, amf_3gpp_access_registration);
  long http_code;
  http_code = udm_client::curl_http_client(
      remoteUri, "PUT", response, amf_registration_json.dump());

  try {
    Logger::udm_uecm().debug("PUT Response:" + response);
    response_data = nlohmann::json::parse(response.c_str());

  } catch (nlohmann::json::exception& e) {  // error handling
    Logger::udm_uecm().info("Could not get Json content from UDR response");

    m_ProblemDetails.setCause("USER_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("User " + ue_id + " not found");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_uecm().error("User " + ue_id + " not found");
    Logger::udm_uecm().info("Send 404 Not_Found response to client");
    response_data = j_ProblemDetails;
    return;
  }
  Logger::udm_uecm().debug("HTTP response code %d", http_code);

  response_data = amf_registration_json;
  // code          = static_cast<Pistache::Http::Code>(http_code);
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_session_management_subscription_data_retrieval(
    const std::string& supi, nlohmann::json& response_data, long& code,
    oai::udm::model::Snssai snssai, std::string dnn,
    oai::udm::model::PlmnId plmn_id) {
  // UDR's URL
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string serving_plmn_id =
      plmn_id.getMcc() +
      plmn_id.getMnc();  // TODO: get serving PLMN when plmn_is is not present
  std::string remote_uri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
                           udm_cfg.udr_addr.api_version +
                           "/subscription-data/" + supi + "/" +
                           serving_plmn_id + "/provisioned-data/sm-data";

  std::string query_str = {};

  if (snssai.getSst() > 0) {
    query_str += "?single-nssai={\"sst\":" + std::to_string(snssai.getSst()) +
                 ",\"sd\":\"" + snssai.getSd() + "\"}";
    if (!dnn.empty()) {
      query_str += "&dnn=" + dnn;
    }
  } else if (!dnn.empty()) {
    query_str += "?dnn=" + dnn;
  }

  // URI with Optional SNSSAI/DNN
  remote_uri += query_str;

  std::string response_str = {};
  Logger::udm_sdm().debug("Request URI: " + remote_uri);

  // Send curl to UDM
  code = udm_client::curl_http_client(remote_uri, "GET", response_str);

  Logger::udm_sdm().debug("HTTP response code %ld", code);

  // Process response
  try {
    Logger::udm_sdm().debug("Response: " + response_str);
    response_data = nlohmann::json::parse(response_str.c_str());
  } catch (nlohmann::json::exception& e) {
    Logger::udm_sdm().info("Could not get JSON content from UDR response");
    ProblemDetails problem_details      = {};
    nlohmann::json json_problem_details = {};
    problem_details.setCause("USER_NOT_FOUND");
    problem_details.setStatus(404);
    problem_details.setDetail("User " + supi + " not found");
    to_json(json_problem_details, problem_details);
    Logger::udm_sdm().error("User " + supi + " not found");
    response_data = json_problem_details;
    code          = 404;
    return;
  }
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_slice_selection_subscription_data_retrieval(
    const std::string& supi, nlohmann::json& response_data, long& code,
    std::string supported_features, oai::udm::model::PlmnId plmn_id) {
  // 1. populate remote uri for udp request
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string remote_uri =
      udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
      udm_cfg.udr_addr.api_version + "/subscription-data/" + supi + "/" +
      plmn_id.getMcc() + plmn_id.getMnc() + "/provisioned-data/sm-data";
  std::string body("");
  std::string response_get;
  Logger::udm_sdm().debug("UDR: GET Request: " + remote_uri);
  // 2. invoke curl to get response from udr
  long http_code =
      udm_client::curl_http_client(remote_uri, "GET", response_get, body);
  // 3. process response

  nlohmann::json return_response_data_json = {};
  try {
    Logger::udm_sdm().debug("subscription-data: GET Response: " + response_get);

    response_data = nlohmann::json::parse(response_get.c_str());
    // TODO: 1. shall check if "singleNassai" is existing or not, if not, raise
    // exception
    // TODO: 2. return_response_data_json: need to check if here is required to
    // allocate memory first. Or check json code to confirm, otherwise codedump
    // might happen
    return_response_data_json["singleNssai"] = response_data["singleNssai"];
  } catch (nlohmann::json::exception& e) {
    Logger::udm_sdm().info("Could not get json content from UDR response");
    ProblemDetails problem_details;
    nlohmann::json json_problem_details;
    problem_details.setCause("USER_NOT_FOUND");
    problem_details.setStatus(404);
    problem_details.setDetail("User " + supi + " not found");
    to_json(json_problem_details, problem_details);
    Logger::udm_sdm().error("User " + supi + " not found");
    Logger::udm_sdm().info("Send 404 Not_Found response to client");
    response_data = json_problem_details;
    // code          = Pistache::Http::Code::Not_Found;

    return;
  }
  Logger::udm_sdm().debug("HTTP response code %d", http_code);
  response_data = return_response_data_json;
  // code          = static_cast<Pistache::Http::Code>(http_code);
}

//------------------------------------------------------------------------------
void udm_app::handle_smf_selection_subscription_data_retrieval(
    const std::string& supi, nlohmann::json& response_data, long& code,
    std::string supported_features, oai::udm::model::PlmnId plmn_id) {
  // 1. populate remote uri for udp request
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port   = std::to_string(udm_cfg.udr_addr.port);
  std::string remote_uri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
                           udm_cfg.udr_addr.api_version +
                           "/subscription-data/" + supi + "/" +
                           plmn_id.getMcc() + plmn_id.getMnc() +
                           "/provisioned-data/smf-selection-subscription-data";

  std::string body("");
  std::string response_get;
  Logger::udm_sdm().debug("UDR: GET Request: " + remote_uri);
  // 2. invoke curl to get response from udr
  code = udm_client::curl_http_client(remote_uri, "GET", response_get, body);
  // 3. process response
  try {
    Logger::udm_sdm().debug("subscription-data: GET Response: " + response_get);
    response_data = nlohmann::json::parse(response_get.c_str());
  } catch (nlohmann::json::exception& e) {
    Logger::udm_sdm().info("Could not get json content from UDR response");
    ProblemDetails problem_details;
    nlohmann::json json_problem_details;
    problem_details.setCause("USER_NOT_FOUND");
    problem_details.setStatus(404);
    problem_details.setDetail("User " + supi + " not found");
    to_json(json_problem_details, problem_details);
    Logger::udm_sdm().error("User " + supi + " not found");
    Logger::udm_sdm().info("Send 404 Not_Found response to client");
    response_data = json_problem_details;
    code          = 404;
    return;
  }
  Logger::udm_sdm().debug("HTTP response code %d", code);
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_subscription_creation(
    const std::string& supi,
    const oai::udm::model::SdmSubscription& sdmSubscription,
    nlohmann::json& response_data, long& code) {
  std::string udr_ip =
      std::string(inet_ntoa(*((struct in_addr*) &udm_cfg.udr_addr.ipv4_addr)));
  std::string udr_port = std::to_string(udm_cfg.udr_addr.port);
  std::string remoteUri;
  std::string Method;
  std::string msgBody;
  std::string Response;
  nlohmann::json j_ProblemDetails;
  ProblemDetails m_ProblemDetails;

  // UDR GET interface
  // get 3gpp_registration related info
  remoteUri = udr_ip + ":" + udr_port + NUDR_DATA_REPOSITORY +
              udm_cfg.udr_addr.api_version + "/subscription-data/" + supi +
              "/context-data/sdm-subscriptions";
  Logger::udm_uecm().debug("POST Request:" + remoteUri);

  nlohmann::json sdmSubscription_j;
  to_json(sdmSubscription_j, sdmSubscription);
  long http_code;
  http_code = udm_client::curl_http_client(
      remoteUri, "POST", Response, sdmSubscription_j.dump());

  nlohmann::json response_data_json = {};
  try {
    Logger::udm_uecm().debug("POST Response:" + Response);
    response_data_json = nlohmann::json::parse(Response.c_str());

  } catch (nlohmann::json::exception& e) {  // error handling
    Logger::udm_uecm().info("Could not get JSON content from UDR response");

    m_ProblemDetails.setCause("USER_NOT_FOUND");
    m_ProblemDetails.setStatus(404);
    m_ProblemDetails.setDetail("User " + supi + " not found");
    to_json(j_ProblemDetails, m_ProblemDetails);

    Logger::udm_uecm().error("User " + supi + " not found");
    Logger::udm_uecm().info("Send 404 Not_Found response to client");
    response_data = j_ProblemDetails;
    code          = HTTP_RESPONSE_CODE_NOT_FOUND;
    return;
  }
  Logger::udm_uecm().debug("HTTP response code %d", http_code);
  response_data = sdmSubscription_j;  // to be verified
}

//------------------------------------------------------------------------------
evsub_id_t udm_app::handle_create_ee_subscription(
    const std::string& ueIdentity,
    const oai::udm::model::EeSubscription& eeSubscription,
    oai::udm::model::CreatedEeSubscription& createdSub, long& code) {
  Logger::udm_ee().info("Handle Create EE Subscription");

  // Generate a subscription ID Id and store the corresponding information in a
  // map (subscription id, info)
  evsub_id_t evsub_id = generate_ev_subscription_id();

  oai::udm::model::EeSubscription es = eeSubscription;
  // TODO: Update Subscription

  // MonitoringConfiguration

  es.setSubscriptionId(std::to_string(evsub_id));
  std::shared_ptr<CreatedEeSubscription> ces =
      std::make_shared<CreatedEeSubscription>(createdSub);
  ces->setEeSubscription(es);

  if (!ueIdentity.empty()) {
    ces->setNumberOfUes(1);
  } else {
    // TODO: For group of UEs
  }
  // TODO: MonitoringReport

  add_event_subscription(evsub_id, ueIdentity, ces);
  code = HTTP_RESPONSE_CODE_CREATED;

  return evsub_id;
}

//------------------------------------------------------------------------------
void udm_app::handle_delete_ee_subscription(
    const std::string& ueIdentity, const std::string& subscriptionId,
    oai::udm::model::ProblemDetails& problemDetails, long& code) {
  Logger::udm_ee().info("Handle Delete EE Subscription");

  if (!delete_event_subscription(subscriptionId, ueIdentity)) {
    // Set ProblemDetails
    // Code
    code = HTTP_RESPONSE_CODE_NOT_FOUND;
  }
  code = HTTP_RESPONSE_CODE_NO_CONTENT;
  return;
}

//------------------------------------------------------------------------------
void udm_app::handle_update_ee_subscription(
    const std::string& ueIdentity, const std::string& subscriptionId,
    const std::vector<oai::udm::model::PatchItem>& patchItem,
    oai::udm::model::ProblemDetails& problemDetails, long& code) {
  Logger::udm_ee().info("Handle Update EE Subscription");
  // TODO:
  bool op_success = false;

  for (auto p : patchItem) {
    patch_op_type_t op = util::api_conv::string_to_patch_operation(p.getOp());
    // Verify Path
    if ((p.getPath().substr(0, 1).compare("/") != 0) or
        (p.getPath().length() < 2)) {
      Logger::udm_ee().warn(
          "Bad value for operation path: %s ", p.getPath().c_str());
      code = HTTP_RESPONSE_CODE_BAD_REQUEST;
      problemDetails.setCause(
          protocol_application_error_e2str[MANDATORY_IE_INCORRECT]);
      return;
    }

    std::string path = p.getPath().substr(1);

    switch (op) {
      case PATCH_OP_REPLACE: {
        if (replace_ee_subscription_item(path, p.getValue())) {
          code = HTTP_RESPONSE_CODE_OK;
        } else {
          op_success = false;
        }
      } break;

      case PATCH_OP_ADD: {
        if (add_ee_subscription_item(path, p.getValue())) {
          code = HTTP_RESPONSE_CODE_OK;
        } else {
          op_success = false;
        }
      } break;

      case PATCH_OP_REMOVE: {
        if (remove_ee_subscription_item(path)) {
          code = HTTP_RESPONSE_CODE_OK;
        } else {
          op_success = false;
        }
      } break;

      default: {
        Logger::udm_ee().warn("Requested operation is not valid!");
        op_success = false;
      }
    }

    if (!op_success) {
      code = HTTP_RESPONSE_CODE_BAD_REQUEST;
      problemDetails.setCause(
          protocol_application_error_e2str[INVALID_QUERY_PARAM]);  // TODO:
    } else {
    }
  }
}

//------------------------------------------------------------------------------
evsub_id_t udm_app::generate_ev_subscription_id() {
  return evsub_id_generator.get_uid();
}

//------------------------------------------------------------------------------
void udm_app::add_event_subscription(
    const evsub_id_t& sub_id, const std::string& ue_id,
    std::shared_ptr<oai::udm::model::CreatedEeSubscription>& ces) {
  std::unique_lock lock(m_mutex_udm_event_subscriptions);
  udm_event_subscriptions[sub_id] = ces;
  std::vector<evsub_id_t> ev_subs;

  if (udm_event_subscriptions_per_ue.count(ue_id) > 0) {
    ev_subs = udm_event_subscriptions_per_ue.at(ue_id);
  }
  ev_subs.push_back(sub_id);
  udm_event_subscriptions_per_ue[ue_id] = ev_subs;
  return;
}

//------------------------------------------------------------------------------
bool udm_app::delete_event_subscription(
    const std::string& subscription_id, const std::string& ue_id) {
  std::unique_lock lock(m_mutex_udm_event_subscriptions);
  bool result     = true;
  uint32_t sub_id = 0;
  try {
    sub_id = std::stoul(subscription_id);
  } catch (std::exception e) {
    Logger::udm_ee().warn(
        "Bad value for subscription id %s ", subscription_id.c_str());
    return false;
  }

  if (udm_event_subscriptions.count(sub_id)) {
    udm_event_subscriptions.erase(sub_id);
  } else {
    result = false;
  }

  if (udm_event_subscriptions_per_ue.count(ue_id) > 0) {
    udm_event_subscriptions_per_ue.erase(ue_id);
  } else {
    result = false;
  }

  return result;
}

//------------------------------------------------------------------------------
bool udm_app::replace_ee_subscription_item(
    const std::string& path, const std::string& value) {
  Logger::udm_ee().debug(
      "Replace member %s with new value %s", path.c_str(), value.c_str());
  // TODO:

  return true;
}

//------------------------------------------------------------------------------
bool udm_app::add_ee_subscription_item(
    const std::string& path, const std::string& value) {
  Logger::udm_ee().debug(
      "Add member %s with value %s", path.c_str(), value.c_str());
  // TODO:
  return true;
}

//------------------------------------------------------------------------------
bool udm_app::remove_ee_subscription_item(const std::string& path) {
  Logger::udm_ee().debug("Remove member %s", path.c_str());
  // TODO:
  return true;
}

//------------------------------------------------------------------------------
void udm_app::handle_ee_loss_of_connectivity(
    const std::string& ue_id, uint8_t status, uint8_t http_version) {
  // TODO:
}

//------------------------------------------------------------------------------
void udm_app::handle_ee_ue_reachability_for_data(
    const std::string& ue_id, uint8_t status, uint8_t http_version) {
  // TODO:
}
