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

/*! \file ausf_app.cpp
 \brief
 \author  Jian Yang, Fengjiao He, Hongxin Wang, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email: contact@openairinterface.org
 */

#include "ausf_app.hpp"
#include "ausf_nrf.hpp"

#include <unistd.h>
#include "logger.hpp"
#include "ausf_client.hpp"
#include "ProblemDetails.h"

#include "conversions.hpp"
#include "sha256.hpp"
#include "UEAuthenticationCtx.h"
#include "ConfirmationDataResponse.h"
#include "AuthenticationInfo.h"
#include "authentication_algorithms_with_5gaka.hpp"
#include <string>
#include "iostream"
#include <algorithm>
#include <iterator>

using namespace std;
using namespace oai::ausf::app;

extern ausf_app* ausf_app_inst;
ausf_client* ausf_client_inst = nullptr;
using namespace config;
extern ausf_config ausf_cfg;
ausf_nrf* ausf_nrf_inst = nullptr;

//------------------------------------------------------------------------------
ausf_app::ausf_app(const std::string& config_file)
    : contextId2security_context(),
      supi2security_context(),
      imsi2security_context() {
  Logger::ausf_app().startup("Starting...");
  try {
    ausf_client_inst = new ausf_client();
  } catch (std::exception& e) {
    Logger::ausf_app().error("Cannot create AUSF APP: %s", e.what());
    throw;
  }
  // Register to NRF
  if (ausf_cfg.register_nrf) {
    try {
      ausf_nrf_inst = new ausf_nrf();
      ausf_nrf_inst->register_to_nrf();
      Logger::ausf_app().info("NRF TASK Created ");
    } catch (std::exception& e) {
      Logger::ausf_app().error("Cannot create NRF TASK: %s", e.what());
      throw;
    }
  }
  Logger::ausf_app().startup("Started");
}

//------------------------------------------------------------------------------
ausf_app::~ausf_app() {
  Logger::ausf_app().debug("Delete AUSF_APP instance...");
}

//------------------------------------------------------------------------------
bool ausf_app::is_supi_2_security_context(const std::string& supi) const {
  std::shared_lock lock(m_supi2security_context);
  return bool{supi2security_context.count(supi) > 0};
}

//------------------------------------------------------------------------------
std::shared_ptr<security_context> ausf_app::supi_2_security_context(
    const std::string& supi) const {
  std::shared_lock lock(m_supi2security_context);
  return supi2security_context.at(supi);
}

//------------------------------------------------------------------------------
void ausf_app::set_supi_2_security_context(
    const std::string& supi, std::shared_ptr<security_context> sc) {
  std::unique_lock lock(m_supi2security_context);
  supi2security_context[supi] = sc;
}

//------------------------------------------------------------------------------
bool ausf_app::is_contextId_2_security_context(
    const std::string& contextId) const {
  std::shared_lock lock(m_contextId2security_context);
  return bool{contextId2security_context.count(contextId) > 0};
}

//------------------------------------------------------------------------------
std::shared_ptr<security_context> ausf_app::contextId_2_security_context(
    const std::string& contextId) const {
  std::shared_lock lock(m_contextId2security_context);
  return contextId2security_context.at(contextId);
}

//------------------------------------------------------------------------------
void ausf_app::set_contextId_2_security_context(
    const std::string& contextId, std::shared_ptr<security_context> sc) {
  std::unique_lock lock(m_contextId2security_context);
  contextId2security_context[contextId] = sc;
}

//------------------------------------------------------------------------------
void ausf_app::handle_ue_authentications(
    const AuthenticationInfo& authenticationInfo, nlohmann::json& json_data,
    std::string& location, Pistache::Http::Code& code, uint8_t http_version) {
  Logger::ausf_app().info("Handle UE Authentication Request");
  std::string snn =
      authenticationInfo.getServingNetworkName();  // serving network name
  std::string supi = authenticationInfo.getSupiOrSuci();  // supi
  // TODO: supi64_t supi64 = {};

  Logger::ausf_app().info("ServingNetworkName %s", snn.c_str());
  Logger::ausf_app().info("supiOrSuci %s", supi.c_str());

  // 5g he av from udm
  // get authentication related info
  std::string udm_uri  = {};
  std::string method   = "POST";
  std::string response = {};
  udm_uri              = "http://" +
            std::string(
                inet_ntoa(*((struct in_addr*) &ausf_cfg.udm_addr.ipv4_addr))) +
            ":" + std::to_string(ausf_cfg.udm_addr.port) + "/nudm-ueau/" +
            ausf_cfg.udm_addr.api_version + "/" + supi +
            "/security-information/generate-auth-data";
  Logger::ausf_app().debug("UDM's URI %s", udm_uri.c_str());

  // Create AuthInfo to send to UDM
  nlohmann::json AuthInfo =
      {};  // model AuthenticationInfo do not have ausfInstanceId field
  AuthInfo["servingNetworkName"] = snn;
  AuthInfo["ausfInstanceId"] =
      "400346f4-087e-40b1-a4cd-00566953999d";  // TODO: need to be generated
                                               // automatically

  if (authenticationInfo
          .resynchronizationInfoIsSet())  // set ResynchronizationInfo
  {
    ResynchronizationInfo resynInfo =
        authenticationInfo.getResynchronizationInfo();

    AuthInfo["resynchronizationInfo"]["rand"] = resynInfo.getRand();
    AuthInfo["resynchronizationInfo"]["auts"] = resynInfo.getAuts();
    Logger::ausf_app().info(
        "Received authInfo from AMF with ResynchronizationInfo IE");
  } else {
    Logger::ausf_app().info(
        "Received authInfo from AMF without ResynchronizationInfo IE");
  }

  // Send request to UDM
  ausf_client_inst->curl_http_client(
      udm_uri, method, AuthInfo.dump(), response);

  Logger::ausf_app().info("Response from UDM: %s", response.c_str());

  ProblemDetails problemDetails;
  nlohmann::json problemDetails_json = {};

  nlohmann::json response_data = {};
  std::string authType_udm     = {};
  std::string autn_udm         = {};
  std::string avType_udm       = {};
  std::string kausf_udm        = {};
  std::string rand_udm         = {};
  std::string xresStar_udm     = {};
  try {
    response_data = nlohmann::json::parse(response.c_str());
    // Get security context
    authType_udm = response_data.at("authType");  // AuthType
    Logger::ausf_app().debug("authType %s", authType_udm.c_str());
    autn_udm = response_data["authenticationVector"].at("autn");  // autn
    Logger::ausf_app().debug("autn_udm %s", autn_udm.c_str());
    avType_udm = response_data["authenticationVector"].at("avType");  // avType
    Logger::ausf_app().debug("avType_udm %s", avType_udm.c_str());
    kausf_udm = response_data["authenticationVector"].at("kausf");  // kausf
    Logger::ausf_app().debug("kausf_udm %s", kausf_udm.c_str());
    rand_udm = response_data["authenticationVector"].at("rand");  // rand
    Logger::ausf_app().debug("rand_udm %s", rand_udm.c_str());
    xresStar_udm =
        response_data["authenticationVector"].at("xresStar");  // xres*
    Logger::ausf_app().debug("xres*_udm %s", xresStar_udm.c_str());

  } catch (nlohmann::json::exception& e) {
    // TODO: Catch parse_error exception
    // TODO: Catch out_of_range exception
    Logger::ausf_app().info("Could not Parse JSON content from UDM response");

    // TODO: error handling
    problemDetails.setCause("CONTEXT_NOT_FOUND");
    problemDetails.setStatus(404);
    problemDetails.setDetail(
        "Resource corresponding to User " + supi + " not found");
    to_json(problemDetails_json, problemDetails);

    Logger::ausf_app().error(
        "Resource corresponding to User %s not found", supi.c_str());
    Logger::ausf_app().info("Send 404 Not_Found response");
    code      = Pistache::Http::Code::Not_Found;
    json_data = problemDetails_json;
    return;
  }

  // 5G HE AV
  uint8_t autn[16]     = {0};
  uint8_t rand[16]     = {0};
  uint8_t xresStar[16] = {0};
  uint8_t kausf[32]    = {0};

  conv::hex_str_to_uint8(autn_udm.c_str(), autn);          // autn
  conv::hex_str_to_uint8(rand_udm.c_str(), rand);          // rand
  conv::hex_str_to_uint8(xresStar_udm.c_str(), xresStar);  // xres*
  conv::hex_str_to_uint8(kausf_udm.c_str(), kausf);        // kausf

  // Generating 5G AV from 5G HE AV
  //  HXRES* <-- XRES*
  //  KSEAF  <-- KAUSF
  //  5G HE AV HXRES* XRES*，KSEAF KAUSF
  //  KSEAF，SEAF 5G SE AV（RAND, AUTN, HXRES*）
  //  A.5, 3gpp ts33.501
  Logger::ausf_app().debug("Generating 5G AV");

  // Generating hxres*
  uint8_t rand_ausf[16]     = {0};
  uint8_t autn_ausf[16]     = {0};
  uint8_t xresStar_ausf[16] = {0};
  uint8_t kausf_ausf[32]    = {0};
  uint8_t hxresStar[16]     = {0};

  // Getting params from UDM 5G HE AV
  std::copy(
      std::begin(xresStar), std::end(xresStar), std::begin(xresStar_ausf));
  std::copy(std::begin(rand), std::end(rand), std::begin(rand_ausf));
  std::copy(std::begin(autn), std::end(autn), std::begin(autn_ausf));
  std::copy(std::begin(kausf), std::end(kausf), std::begin(kausf_ausf));

  // Generate_Hxres*
  Authentication_5gaka::generate_Hxres(rand_ausf, xresStar_ausf, hxresStar);
  Logger::ausf_app().debug(
      "HXresStar calculated:\n %s",
      (conv::uint8_to_hex_string(hxresStar, 16)).c_str());

  uint8_t kseaf[32] = {0};
  Authentication_5gaka::derive_kseaf(snn, kausf, kseaf);
  Logger::ausf_app().debug(
      "Kseaf calculated:\n %s", (conv::uint8_to_hex_string(kseaf, 32)).c_str());

  // Store the security context
  std::shared_ptr<security_context> sc = {};
  if (is_supi_2_security_context(supi)) {
    Logger::ausf_app().debug(
        "Update security context with SUPI: ", supi.c_str());
    sc = supi_2_security_context(supi);
  } else {
    Logger::ausf_app().debug(
        "Create a new security context with SUPI ", supi.c_str());
    sc = std::make_shared<security_context>();
    set_supi_2_security_context(supi, sc);
  }

  // Update information
  sc->supi_ausf = supi;  // TODO: setter/getter
  std::copy(
      std::begin(rand_ausf), std::end(rand_ausf),
      std::begin(sc->ausf_av_s.rand));
  std::copy(
      std::begin(autn_ausf), std::end(autn_ausf),
      std::begin(sc->ausf_av_s.autn));
  std::copy(
      std::begin(hxresStar), std::end(hxresStar),
      std::begin(sc->ausf_av_s.hxresStar));
  std::copy(
      std::begin(kseaf), std::end(kseaf), std::begin(sc->ausf_av_s.kseaf));
  // store xres* in ausf
  std::copy(
      std::begin(xresStar), std::end(xresStar), std::begin(sc->xres_star));

  sc->supi_ausf  = authenticationInfo.getSupiOrSuci();  // store supi in ausf
  sc->serving_nn = snn;                                 // store snn in ausf
  sc->auth_type  = authType_udm;  // store authType in ausf
  sc->kausf_tmp =
      conv::uint8_to_hex_string(kausf_ausf, 32);  // store kausf_tmp in ausf

  // Send authentication context to SEAF (AUSF->SEAF)
  UEAuthenticationCtx UEAuthCtx;
  string rand_s      = conv::uint8_to_hex_string(rand_ausf, 16);
  string autn_s      = conv::uint8_to_hex_string(autn_ausf, 16);
  string hxresStar_s = conv::uint8_to_hex_string(hxresStar, 16);
  UEAuthCtx.setAuthType(authType_udm);  // authType(string)

  std::map<std::string, LinksValueSchema> ausf_links;  // links(std::map)
  LinksValueSchema ausf_Href;
  std::string resourceURI;

  std::string authCtxId_s;
  authCtxId_s = autn_s;  // authCtxId = autn
  // Store the security context
  set_contextId_2_security_context(authCtxId_s, sc);

  std::string ausf_port = std::to_string(ausf_cfg.sbi.port);
  if (http_version == 2) ausf_port = std::to_string(ausf_cfg.sbi_http2_port);

  resourceURI =
      "http://" +
      std::string(inet_ntoa(*((struct in_addr*) &ausf_cfg.sbi.addr4))) + ":" +
      ausf_port + "/nausf-auth/v1/ue-authentications/" + authCtxId_s +
      "/5g-aka-confirmation";
  ausf_Href.setHref(resourceURI);

  ausf_links["5G_AKA"] = ausf_Href;
  UEAuthCtx.setLinks(ausf_links);

  // 5gAuthData(Av5gAka):rand autn hxresStar
  Av5gAka ausf_5gAuthData;
  ausf_5gAuthData.setRand(rand_s);
  ausf_5gAuthData.setAutn(autn_s);
  ausf_5gAuthData.setHxresStar(hxresStar_s);
  UEAuthCtx.setR5gAuthData(ausf_5gAuthData);

  to_json(json_data, UEAuthCtx);
  code = Pistache::Http::Code::Created;
  Logger::ausf_app().debug("Auth Response:\n %s", json_data.dump().c_str());
  return;
}

//------------------------------------------------------------------------------
void ausf_app::handle_ue_authentications_confirmation(
    const std::string& authCtxId, const ConfirmationData& confirmationData,
    nlohmann::json& json_data, Pistache::Http::Code& code) {
  // SEAF-> AUSF
  ProblemDetails problemDetails;
  nlohmann::json problemDetails_json = {};
  Logger::ausf_app().debug("Handling 5g-aka-confirmation");

  // Get the security context
  std::shared_ptr<security_context> sc = {};
  if (is_contextId_2_security_context(authCtxId)) {
    Logger::ausf_app().debug(
        "Retrieve security context with authCtxId: ", authCtxId.c_str());
    sc = contextId_2_security_context(authCtxId);
  } else {  // No ue-authentications request before
    Logger::ausf_app().debug(
        "Security context with authCtxId  ", authCtxId.c_str(),
        "does not exist");

    problemDetails.setCause("SERVING_NETWORK_NOT_AUTHORIZED");
    problemDetails.setStatus(403);
    problemDetails.setDetail("Serving Network Not Authorized");
    to_json(problemDetails_json, problemDetails);

    Logger::ausf_app().error("Serving Network Not Authorized");
    Logger::ausf_app().info("Send 403 Forbidden response");
    code      = Pistache::Http::Code::Forbidden;
    json_data = problemDetails_json;
    return;
  }

  Logger::ausf_app().info(
      "Received authCtxId %s", authCtxId.c_str());  // authCtxId
  Logger::ausf_app().info(
      "Received res* %s", confirmationData.getResStar().c_str());

  uint8_t resStar[16] = {0};
  conv::hex_str_to_uint8(confirmationData.getResStar().c_str(), resStar);

  ConfirmationDataResponse confirmResponse;
  uint8_t authCtxId_seaf[16];
  conv::hex_str_to_uint8(
      authCtxId.c_str(), authCtxId_seaf);  // authCtxId in SEAF

  Logger::ausf_app().debug(
      "authCtxId in AUSF: %s",
      (conv::uint8_to_hex_string(sc->ausf_av_s.autn, 16)).c_str());

  bool is_auth_vectors_present =
      Authentication_5gaka::equal_uint8(sc->ausf_av_s.autn, authCtxId_seaf, 16);
  if (!is_auth_vectors_present)  // AV expired
  {
    Logger::ausf_app().error(
        "Authentication failure by home network with authCtxId %s: AV expired",
        authCtxId.c_str());
    confirmResponse.setAuthResult(is_auth_vectors_present);
    sc->kausf_tmp = "invalid";
  } else  // AV valid
  {
    Logger::ausf_app().info("AV is up to date, handling received res*...");
    // Get stored xres* and compare with res*
    uint8_t xresStar[16] = {0};
    // xres* stored for 5g-aka-confirmation
    std::copy(
        std::begin(sc->xres_star), std::end(sc->xres_star),
        std::begin(xresStar));

    Logger::ausf_app().debug(
        "xres* in AUSF: %s", (conv::uint8_to_hex_string(xresStar, 16)).c_str());
    Logger::ausf_app().debug(
        "xres in AMF: %s", (conv::uint8_to_hex_string(resStar, 16)).c_str());

    bool authResult = Authentication_5gaka::equal_uint8(xresStar, resStar, 16);
    confirmResponse.setAuthResult(authResult);

    if (!authResult)  // Authentication failed
    {
      Logger::ausf_app().error(
          "Authentication failure by home network with authCtxId %s: res* != "
          "xres*",
          authCtxId.c_str());
    } else  // Authentication success
    {
      Logger::ausf_app().info("Authentication successful by home network!");
      // Send Kseaf to SEAF
      string kseaf_s;
      kseaf_s = conv::uint8_to_hex_string(sc->ausf_av_s.kseaf, 32);
      confirmResponse.setKseaf(kseaf_s);
      // Send SUPI when supi_ausf exists
      if (!sc->supi_ausf.empty()) {
        confirmResponse.setSupi(sc->supi_ausf);
      }
      // Send authResult to UDM (authentication result info)
      std::string udm_uri  = {};
      std::string method   = "POST";
      std::string response = {};
      udm_uri =
          "http://" +
          std::string(
              inet_ntoa(*((struct in_addr*) &ausf_cfg.udm_addr.ipv4_addr))) +
          ":" + std::to_string(ausf_cfg.udm_addr.port) + "/nudm-ueau/" +
          ausf_cfg.udm_addr.api_version + "/" + sc->supi_ausf + "/auth-events";

      Logger::ausf_app().debug("UDM's URI: %s", udm_uri.c_str());

      // Form request body
      nlohmann::json confirmResultInfo = {};
      confirmResultInfo["nfInstanceId"] =
          "400346f4-087e-40b1-a4cd-00566953999d";  // TODO: remove hardcoded
                                                   // value
      confirmResultInfo["success"] = true;

      // TODO: Update timestamp
      time_t rawtime;
      time(&rawtime);
      char buf[32];
      strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&rawtime));
      confirmResultInfo["timeStamp"] = buf;  // timestamp generated

      // Get info from the security context (stored in AUSF)
      confirmResultInfo["authType"]           = sc->auth_type;
      confirmResultInfo["servingNetworkName"] = sc->serving_nn;
      confirmResultInfo["authRemovalInd"]     = false;

      Logger::ausf_app().debug(
          "confirmResultInfo: %s", confirmResultInfo.dump().c_str());
      ausf_client_inst->curl_http_client(
          udm_uri, method, confirmResultInfo.dump(), response);
    }
  }

  to_json(json_data, confirmResponse);
  code = Pistache::Http::Code::Ok;
  return;
}
