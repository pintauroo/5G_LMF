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

/*! \file amf_n11.cpp
 \brief
 \author Keliang DU (BUPT), Tien-Thinh NGUYEN (EURECOM)
 \date 2020
 \email: contact@openairinterface.org
 */

#include "amf_n11.hpp"

#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include "3gpp_ts24501.hpp"
#include "3gpp_29.500.h"
#include "amf.hpp"
#include "amf_app.hpp"
#include "amf_config.hpp"
#include "amf_n1.hpp"
#include "itti.hpp"
#include "itti_msg_amf_app.hpp"
#include "nas_context.hpp"
// For smf_client
#include "ApiClient.h"
#include "ApiConfiguration.h"
#include "SMContextsCollectionApi.h"
#include "SmContextCreateData.h"
#include "mime_parser.hpp"
#include "ue_context.hpp"
#include "fqdn.hpp"

#include "conversions.hpp"
#include "comUt.hpp"
#include "AmfEventReport.h"

extern "C" {
#include "dynamic_memory_check.h"
}

using namespace oai::smf::model;
using namespace oai::smf::api;
using namespace web;
using namespace web::http;
// Common features like URIs.
using namespace web::http::client;
using namespace config;
using namespace amf_application;
extern itti_mw* itti_inst;
extern amf_config amf_cfg;
extern amf_n11* amf_n11_inst;
extern amf_n1* amf_n1_inst;
extern amf_app* amf_app_inst;
extern statistics stacs;

//------------------------------------------------------------------------------
std::size_t callback(
    const char* in, std::size_t size, std::size_t num, std::string* out) {
  const std::size_t totalBytes(size * num);
  out->append(in, totalBytes);
  return totalBytes;
}

//------------------------------------------------------------------------------
void octet_stream_2_hex_stream(uint8_t* buf, int len, std::string& out) {
  out       = "";
  char* tmp = (char*) calloc(1, 2 * len * sizeof(uint8_t) + 1);
  for (int i = 0; i < len; i++) {
    sprintf(tmp + 2 * i, "%02x", buf[i]);
  }
  tmp[2 * len] = '\0';
  out          = tmp;
  printf("n1sm buffer: %s\n", out.c_str());
}

//------------------------------------------------------------------------------
void amf_n11_task(void*);
//------------------------------------------------------------------------------
void amf_n11_task(void*) {
  const task_id_t task_id = TASK_AMF_N11;
  itti_inst->notify_task_ready(task_id);
  do {
    std::shared_ptr<itti_msg> shared_msg = itti_inst->receive_msg(task_id);
    auto* msg                            = shared_msg.get();
    switch (msg->msg_type) {
      case NSMF_PDU_SESSION_CREATE_SM_CTX: {
        Logger::amf_n11().info("Running ITTI_SMF_PDU_SESSION_CREATE_SM_CTX");
        itti_nsmf_pdusession_create_sm_context* m =
            dynamic_cast<itti_nsmf_pdusession_create_sm_context*>(msg);
        amf_n11_inst->handle_itti_message(ref(*m));
      } break;
      case NSMF_PDU_SESSION_UPDATE_SM_CTX: {
        Logger::amf_n11().info(
            "Receive Nsmf_PDUSessionUpdateSMContext, handling ...");
        itti_nsmf_pdusession_update_sm_context* m =
            dynamic_cast<itti_nsmf_pdusession_update_sm_context*>(msg);
        amf_n11_inst->handle_itti_message(ref(*m));
      } break;
      case PDU_SESSION_RESOURCE_SETUP_RESPONSE: {
        Logger::amf_n11().info(
            "Receive PDU Session Resource Setup response, handling ...");
        itti_pdu_session_resource_setup_response* m =
            dynamic_cast<itti_pdu_session_resource_setup_response*>(msg);
        amf_n11_inst->handle_itti_message(ref(*m));
      } break;
      case N11_REGISTER_NF_INSTANCE_REQUEST: {
        Logger::amf_n11().info(
            "Receive Register NF Instance Request, handling ...");
        itti_n11_register_nf_instance_request* m =
            dynamic_cast<itti_n11_register_nf_instance_request*>(msg);
        // TODO: Handle ITTI
      } break;
      case SBI_NOTIFY_SUBSCRIBED_EVENT: {
        Logger::amf_n11().info(
            "Receive Notify Subscribed Event Request, handling ...");
        itti_sbi_notify_subscribed_event* m =
            dynamic_cast<itti_sbi_notify_subscribed_event*>(msg);
        amf_n11_inst->handle_itti_message(ref(*m));
      } break;
      case TERMINATE: {
        if (itti_msg_terminate* terminate =
                dynamic_cast<itti_msg_terminate*>(msg)) {
          Logger::amf_n11().info("Received terminate message");
          return;
        }
      } break;
      default: {
        Logger::amf_n11().info(
            "Receive unknown message type %d", msg->msg_type);
      }
    }
  } while (true);
}

//------------------------------------------------------------------------------
amf_n11::amf_n11() {
  if (itti_inst->create_task(TASK_AMF_N11, amf_n11_task, nullptr)) {
    Logger::amf_n11().error("Cannot create task TASK_AMF_N11");
    throw std::runtime_error("Cannot create task TASK_AMF_N11");
  }
  Logger::amf_n11().startup("amf_n11 started");
}

//------------------------------------------------------------------------------
amf_n11::~amf_n11() {}

//------------------------------------------------------------------------------
void amf_n11::handle_itti_message(
    itti_pdu_session_resource_setup_response& itti_msg) {}

//------------------------------------------------------------------------------
void amf_n11::handle_itti_message(
    itti_nsmf_pdusession_update_sm_context& itti_msg) {
  string ue_context_key = "app_ue_ranid_" + to_string(itti_msg.ran_ue_ngap_id) +
                          ":amfid_" + to_string(itti_msg.amf_ue_ngap_id);
  std::shared_ptr<ue_context> uc = {};
  if (!amf_app_inst->is_ran_amf_id_2_ue_context(ue_context_key)) {
    Logger::amf_n11().error(
        "No UE context for %s exit", ue_context_key.c_str());
    return;
  }

  uc = amf_app_inst->ran_amf_id_2_ue_context(ue_context_key);

  std::string supi = {};
  if (uc.get() != nullptr) {
    supi = uc->supi;
  } else {
    Logger::amf_n11().error(
        "Could not find UE context with key %s", ue_context_key.c_str());
    return;
  }

  Logger::amf_n11().debug(
      "Send PDU Session Update SM Context Request to SMF (SUPI %s, PDU Session "
      "ID %d)",
      supi.c_str(), itti_msg.pdu_session_id);

  std::shared_ptr<pdu_session_context> psc = {};
  if (!uc.get()->find_pdu_session_context(itti_msg.pdu_session_id, psc)) {
    Logger::amf_n11().error(
        "Could not find pdu_session_context with SUPI %s, Failed",
        supi.c_str());
    return;
  }

  std::string smf_addr        = {};
  std::string smf_port        = {};
  std::string smf_api_version = {};

  if (!psc.get()->smf_available) {
    Logger::amf_n11().error("No SMF is available for this PDU session");
  } else {
    smf_addr        = psc->smf_addr;
    smf_port        = psc->smf_port;
    smf_api_version = psc->smf_api_version;
  }

  std::string smf_ip_addr = {};
  std::string remote_uri  = {};

  // remove http port from the URI if existed
  std::size_t found_port = smf_addr.find(":");
  if (found_port != std::string::npos)
    smf_ip_addr = smf_addr.substr(0, found_port - 1);
  else
    smf_ip_addr = smf_addr;

  std::size_t found = psc.get()->smf_context_location.find(smf_ip_addr);
  if (found != std::string::npos)
    remote_uri = psc.get()->smf_context_location + "/modify";
  else
    remote_uri =
        smf_addr + ":" + smf_port + psc.get()->smf_context_location + "/modify";

  Logger::amf_n11().debug("SMF URI: %s", remote_uri.c_str());

  std::string n2SmMsg                       = {};
  nlohmann::json pdu_session_update_request = {};
  if (itti_msg.is_n2sm_set) {
    pdu_session_update_request["n2SmInfoType"] = itti_msg.n2sm_info_type;
    pdu_session_update_request["n2SmInfo"]["contentId"] = "n2msg";
    octet_stream_2_hex_stream(
        (uint8_t*) bdata(itti_msg.n2sm), blength(itti_msg.n2sm), n2SmMsg);
  }

  // For N2 HO
  if (itti_msg.ho_state.compare("PREPARING") == 0) {
    pdu_session_update_request["hoState"] = "PREPARING";
  } else if (itti_msg.ho_state.compare("PREPARED") == 0) {
    pdu_session_update_request["hoState"] = "PREPARED";
  } else if (itti_msg.ho_state.compare("COMPLETED") == 0) {
    pdu_session_update_request["hoState"] = "COMPLETED";
  }

  std::string json_part = pdu_session_update_request.dump();

  uint8_t http_version = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  curl_http_client(
      remote_uri, json_part, "", n2SmMsg, supi, itti_msg.pdu_session_id,
      http_version, itti_msg.promise_id);

  // stacs.display();
}

//------------------------------------------------------------------------------
void amf_n11::handle_itti_message(itti_nsmf_pdusession_create_sm_context& smf) {
  Logger::amf_n11().debug("Handle ITTI SMF_PDU_SESSION_CREATE_SM_CTX");

  if (!amf_n1_inst->is_amf_ue_id_2_nas_context(smf.amf_ue_ngap_id)) {
    Logger::amf_n2().error(
        "No UE NAS context with amf_ue_ngap_id (0x%x)", smf.amf_ue_ngap_id);
    return;
  }

  std::shared_ptr<nas_context> nc = {};
  nc               = amf_n1_inst->amf_ue_id_2_nas_context(smf.amf_ue_ngap_id);
  std::string supi = "imsi-" + nc.get()->imsi;
  string ue_context_key = "app_ue_ranid_" +
                          to_string(nc.get()->ran_ue_ngap_id) + ":amfid_" +
                          to_string(nc.get()->amf_ue_ngap_id);
  std::shared_ptr<ue_context> uc = {};
  Logger::amf_n11().info(
      "Find ue_context in amf_app using UE Context Key: %s",
      ue_context_key.c_str());
  if (!amf_app_inst->is_ran_amf_id_2_ue_context(ue_context_key)) {
    Logger::amf_n11().error(
        "No UE context for %s exit", ue_context_key.c_str());
    return;
  }

  uc = amf_app_inst->ran_amf_id_2_ue_context(ue_context_key);
  if (!uc.get()) {
    Logger::amf_n11().error(
        "No UE context for %s exit", ue_context_key.c_str());
    return;
  }

  // Create PDU Session Context if not available
  std::shared_ptr<pdu_session_context> psc = {};
  if (!uc.get()->find_pdu_session_context(smf.pdu_sess_id, psc)) {
    psc = std::shared_ptr<pdu_session_context>(new pdu_session_context());
    uc.get()->add_pdu_session_context(smf.pdu_sess_id, psc);
    Logger::amf_n11().debug("Create a PDU Session Context");
  }

  if (!psc.get()) {
    Logger::amf_n11().error("No PDU Session Context found");
    return;
  }

  // Store corresponding info in PDU Session Context
  psc.get()->amf_ue_ngap_id = nc.get()->amf_ue_ngap_id;
  psc.get()->ran_ue_ngap_id = nc.get()->ran_ue_ngap_id;
  psc.get()->req_type       = smf.req_type;
  psc.get()->pdu_session_id = smf.pdu_sess_id;
  psc.get()->snssai.sST     = smf.snssai.sST;
  psc.get()->snssai.sD      = smf.snssai.sD;
  psc.get()->plmn.mcc       = smf.plmn.mcc;
  psc.get()->plmn.mnc       = smf.plmn.mnc;

  Logger::amf_n1().debug(
      "PDU Session Context, NSSAI SST (0x%x) SD %s", psc.get()->snssai.sST,
      psc.get()->snssai.sD.c_str());

  // parse binary dnn and store
  std::string dnn = "default";  // If DNN doesn't available, use "default"
  if ((smf.dnn != nullptr) && (blength(smf.dnn) > 0)) {
    char* tmp = conv::bstring2charString(smf.dnn);
    dnn       = tmp;
    free_wrapper((void**) &tmp);
  }

  Logger::amf_n11().debug("Requested DNN: %s", dnn.c_str());
  psc.get()->dnn = dnn;

  std::string smf_addr        = {};
  std::string smf_api_version = {};
  std::string smf_port        = "80";  // Set to default port number
  if (!psc.get()->smf_available) {
    if (amf_cfg.support_features.enable_nrf_selection) {
      if (!discover_smf_from_nsi_info(
              smf_addr, smf_api_version, smf_port, psc.get()->snssai,
              psc.get()->plmn, psc.get()->dnn)) {
        Logger::amf_n11().error("NRF Selection, no NRF candidate is available");
        return;
      }
    } else if (amf_cfg.support_features.enable_smf_selection) {
      // use NRF to find suitable SMF based on snssai, plmn and dnn
      if (!discover_smf(
              smf_addr, smf_api_version, smf_port, psc.get()->snssai,
              psc.get()->plmn, psc.get()->dnn)) {
        Logger::amf_n11().error("SMF Selection, no SMF candidate is available");
        return;
      }
    } else if (!smf_selection_from_configuration(smf_addr, smf_api_version)) {
      Logger::amf_n11().error(
          "No SMF candidate is available (from configuration file)");
      return;
    }
    // store smf info to be used with this PDU session
    psc.get()->smf_available = true;
    psc->smf_addr            = smf_addr;
    psc->smf_port            = smf_port;
    psc->smf_api_version     = smf_api_version;
  } else {
    smf_addr        = psc->smf_addr;
    smf_api_version = psc->smf_api_version;
  }

  switch (smf.req_type & 0x07) {
    case PDU_SESSION_INITIAL_REQUEST: {
      // get pti
      uint8_t* sm_msg = (uint8_t*) bdata(smf.sm_msg);
      uint8_t pti     = sm_msg[2];
      Logger::amf_n11().debug(
          "Decoded PTI for PDUSessionEstablishmentRequest(0x%x)", pti);
      psc.get()->isn2sm_avaliable = false;
      handle_pdu_session_initial_request(
          supi, psc, smf_addr, smf_api_version, smf_port, smf.sm_msg, dnn);
    } break;
    case EXISTING_PDU_SESSION: {
      // TODO:
    } break;
    case PDU_SESSION_TYPE_MODIFICATION_REQUEST: {
      // TODO:
    } break;
    default: {
      // send Nsmf_PDUSession_UpdateSM_Context to SMF e.g., for PDU Session
      // release request
      send_pdu_session_update_sm_context_request(
          supi, psc, smf_addr, smf.sm_msg, dnn);
    }
  }
}

//------------------------------------------------------------------------------
void amf_n11::send_pdu_session_update_sm_context_request(
    std::string supi, std::shared_ptr<pdu_session_context> psc,
    std::string smf_addr, bstring sm_msg, std::string dnn) {
  Logger::amf_n11().debug(
      "Send PDU Session Update SM Context Request to SMF (SUPI %s, PDU Session "
      "ID %d, %s)",
      supi.c_str(), psc.get()->pdu_session_id, smf_addr.c_str());

  std::string smf_ip_addr = {};
  std::string remote_uri  = {};
  // remove http port from the URI if existed
  std::size_t found_port = smf_addr.find(":");
  if (found_port != std::string::npos)
    smf_ip_addr = smf_addr.substr(0, found_port - 1);
  else
    smf_ip_addr = smf_addr;

  std::size_t found = psc.get()->smf_context_location.find(smf_ip_addr);
  if (found != std::string::npos)
    remote_uri = psc.get()->smf_context_location + "/modify";
  else
    remote_uri = smf_addr + psc.get()->smf_context_location + "/modify";

  Logger::amf_n11().debug("SMF URI: %s", remote_uri.c_str());

  nlohmann::json pdu_session_update_request          = {};
  pdu_session_update_request["n1SmMsg"]["contentId"] = "n1SmMsg";
  std::string json_part = pdu_session_update_request.dump();

  std::string n1SmMsg = {};
  octet_stream_2_hex_stream((uint8_t*) bdata(sm_msg), blength(sm_msg), n1SmMsg);

  uint8_t http_version = 1;
  // if (amf_cfg.support_features.use_http2) http_version = 2;

  curl_http_client(
      remote_uri, json_part, n1SmMsg, "", supi, psc.get()->pdu_session_id,
      http_version);
}

//------------------------------------------------------------------------------
void amf_n11::handle_pdu_session_initial_request(
    std::string supi, std::shared_ptr<pdu_session_context> psc,
    std::string smf_addr, std::string smf_api_version, std::string smf_port,
    bstring sm_msg, std::string dnn) {
  Logger::amf_n11().debug(
      "Handle PDU Session Establishment Request (SUPI %s, PDU Session ID %d)",
      supi.c_str(), psc.get()->pdu_session_id);

  // remove http port from the URI if existed
  std::string smf_ip_addr = {};
  std::size_t found_port  = smf_addr.find(":");
  if (found_port != std::string::npos)
    smf_ip_addr = smf_addr.substr(0, found_port);
  else
    smf_ip_addr = smf_addr;
  // provide http2 port if enabled
  std::string amf_port = to_string(amf_cfg.n11.port);
  if (amf_cfg.support_features.use_http2)
    amf_port = to_string(amf_cfg.sbi_http2_port);

  // TODO: Remove hardcoded values
  std::string remote_uri = smf_ip_addr + ":" + smf_port + "/nsmf-pdusession/" +
                           smf_api_version + "/sm-contexts";
  nlohmann::json pdu_session_establishment_request;
  pdu_session_establishment_request["supi"]          = supi.c_str();
  pdu_session_establishment_request["pei"]           = "imei-200000000000001";
  pdu_session_establishment_request["gpsi"]          = "msisdn-200000000001";
  pdu_session_establishment_request["dnn"]           = dnn.c_str();
  pdu_session_establishment_request["sNssai"]["sst"] = psc.get()->snssai.sST;
  pdu_session_establishment_request["sNssai"]["sd"] =
      psc.get()->snssai.sD.c_str();
  pdu_session_establishment_request["pduSessionId"] = psc.get()->pdu_session_id;
  pdu_session_establishment_request["requestType"] =
      "INITIAL_REQUEST";  // TODO: from SM_MSG
  pdu_session_establishment_request["servingNfId"] = "servingNfId";
  pdu_session_establishment_request["servingNetwork"]["mcc"] =
      psc.get()->plmn.mcc.c_str();
  pdu_session_establishment_request["servingNetwork"]["mnc"] =
      psc.get()->plmn.mnc.c_str();
  pdu_session_establishment_request["anType"] = "3GPP_ACCESS";  // TODO
  pdu_session_establishment_request["smContextStatusUri"] =
      "http://" +
      std::string(inet_ntoa(*((struct in_addr*) &amf_cfg.n11.addr4))) + ":" +
      amf_port + "/nsmf-pdusession/callback/" + supi + "/" +
      std::to_string(psc.get()->pdu_session_id);

  pdu_session_establishment_request["n1MessageContainer"]["n1MessageClass"] =
      "SM";
  pdu_session_establishment_request["n1MessageContainer"]["n1MessageContent"]
                                   ["contentId"] = "n1SmMsg";

  std::string json_part = pdu_session_establishment_request.dump();

  Logger::amf_n11().debug("Message body %s", json_part.c_str());

  std::string n1SmMsg = {};
  octet_stream_2_hex_stream((uint8_t*) bdata(sm_msg), blength(sm_msg), n1SmMsg);

  uint8_t http_version = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  curl_http_client(
      remote_uri, json_part, n1SmMsg, "", supi, psc.get()->pdu_session_id,
      http_version);
}

//------------------------------------------------------------------------------
void amf_n11::handle_itti_message(
    itti_nsmf_pdusession_release_sm_context& itti_msg) {
  // TODO: Need PDU session ID
  uint8_t pdu_session_id                   = 1;  // Hardcoded
  std::shared_ptr<pdu_session_context> psc = {};
  if (!amf_app_inst->find_pdu_session_context(
          itti_msg.supi, pdu_session_id, psc)) {
    Logger::amf_n11().warn(
        "PDU Session context for SUPI %s doesn't exit!", itti_msg.supi.c_str());
    return;
  }

  string smf_addr             = {};
  std::string smf_api_version = {};

  if (!psc.get()->smf_available) {
    Logger::amf_n11().error("No SMF is available for this PDU session");
  } else {
    smf_addr        = psc->smf_addr;
    smf_api_version = psc->smf_api_version;
  }

  string remote_uri = psc.get()->location + "release";
  nlohmann::json pdu_session_release_request;
  pdu_session_release_request["supi"]          = itti_msg.supi.c_str();
  pdu_session_release_request["dnn"]           = psc.get()->dnn.c_str();
  pdu_session_release_request["sNssai"]["sst"] = 1;
  pdu_session_release_request["sNssai"]["sd"]  = "0";
  pdu_session_release_request["pduSessionId"]  = psc.get()->pdu_session_id;
  pdu_session_release_request["cause"]         = "REL_DUE_TO_REACTIVATION";
  pdu_session_release_request["ngApCause"]     = "radioNetwork";
  std::string json_part = pdu_session_release_request.dump();
  uint8_t http_version  = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  curl_http_client(
      remote_uri, json_part, "", "", itti_msg.supi, psc.get()->pdu_session_id,
      http_version);
}

//------------------------------------------------------------------------------
void amf_n11::handle_itti_message(itti_sbi_notify_subscribed_event& itti_msg) {
  Logger::amf_n11().debug("Send notification for the subscribed events");

  for (auto i : itti_msg.event_notifs) {
    // Fill the json part
    nlohmann::json json_data         = {};
    json_data["notifyCorrelationId"] = i.get_notify_correlation_id();
    auto report_lists                = nlohmann::json::array();
    nlohmann::json report            = {};

    std::vector<oai::amf::model::AmfEventReport> event_reports = {};
    i.get_reports(event_reports);
    for (auto r : event_reports) {
      report["type"]            = r.getType().get_value();
      report["state"]["active"] = "TRUE";
      if (r.supiIsSet()) {
        report["supi"] = r.getSupi();
      }
      if (r.reachabilityIsSet()) {
        report["reachability"] = r.getReachability().get_value();
      }

      // timestamp
      std::time_t time_epoch_ntp = std::time(nullptr);
      uint64_t tv_ntp            = time_epoch_ntp + SECONDS_SINCE_FIRST_EPOCH;
      report["timeStamp"]        = std::to_string(tv_ntp);
      report_lists.push_back(report);
    }

    json_data["reportList"] = report_lists;

    std::string body = json_data.dump();
    std::string response_data;

    std::string url = i.get_notify_uri();
    curl_http_client(url, "POST", body, response_data);
    // TODO: process the response
  }
  return;
}

//------------------------------------------------------------------------------
bool amf_n11::smf_selection_from_configuration(
    std::string& smf_addr, std::string& smf_api_version) {
  for (int i = 0; i < amf_cfg.smf_pool.size(); i++) {
    if (amf_cfg.smf_pool[i].selected) {
      if (!amf_cfg.support_features.use_fqdn_dns) {
        if (!amf_cfg.support_features.use_http2)
          smf_addr = amf_cfg.smf_pool[i].ipv4 + ":" + amf_cfg.smf_pool[i].port;
        else
          smf_addr = amf_cfg.smf_pool[i].ipv4 + ":" +
                     std::to_string(amf_cfg.smf_pool[i].http2_port);
        smf_api_version = amf_cfg.smf_pool[i].version;
        return true;
      } else {
        // resolve IP addr from a FQDN/DNS name
        uint8_t addr_type = 0;
        uint32_t smf_port = 0;
        fqdn::resolve(
            amf_cfg.smf_pool[i].fqdn, amf_cfg.smf_pool[i].ipv4, smf_port,
            addr_type);
        // TODO for HTTP2
        if (amf_cfg.support_features.use_http2) smf_port = 8080;
        if (addr_type != 0) {  // IPv6: TODO
          Logger::amf_n11().warn("Do not support IPv6 Addr for SMF");
          return false;
        } else {  // IPv4
          smf_addr = amf_cfg.smf_pool[i].ipv4 + ":" + std::to_string(smf_port);
          smf_api_version = "v1";  // TODO: get API version
          return true;
        }
      }
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
void amf_n11::handle_post_sm_context_response_error_400() {}

//------------------------------------------------------------------------------
void amf_n11::handle_post_sm_context_response_error(
    long code, std::string cause, bstring n1sm, std::string supi,
    uint8_t pdu_session_id) {
  comUt::print_buffer(
      "amf_n11", "n1 sm", (uint8_t*) bdata(n1sm), blength(n1sm));
  itti_n1n2_message_transfer_request* itti_msg =
      new itti_n1n2_message_transfer_request(TASK_AMF_N11, TASK_AMF_APP);
  itti_msg->n1sm           = n1sm;
  itti_msg->is_n2sm_set    = false;
  itti_msg->supi           = supi;
  itti_msg->pdu_session_id = pdu_session_id;
  itti_msg->is_ppi_set     = false;
  std::shared_ptr<itti_n1n2_message_transfer_request> i =
      std::shared_ptr<itti_n1n2_message_transfer_request>(itti_msg);
  int ret = itti_inst->send_msg(i);
  if (0 != ret) {
    Logger::amf_n11().error(
        "Could not send ITTI message %s to task TASK_AMF_APP",
        i->get_msg_name());
  }
}

//------------------------------------------------------------------------------
void amf_n11::curl_http_client(
    std::string remote_uri, std::string jsonData, std::string n1SmMsg,
    std::string n2SmMsg, std::string supi, uint8_t pdu_session_id,
    uint8_t http_version, uint32_t promise_id) {
  Logger::amf_n11().debug("Call SMF service: %s", remote_uri.c_str());

  uint8_t number_parts                     = 0;
  mime_parser parser                       = {};
  std::string body                         = {};
  std::shared_ptr<pdu_session_context> psc = {};
  bool is_multipart                        = true;

  if (!amf_app_inst->find_pdu_session_context(supi, pdu_session_id, psc)) {
    Logger::amf_n11().warn(
        "PDU Session context for SUPI %s doesn't exit!", supi.c_str());
    // TODO:
    return;
  }

  if ((n1SmMsg.size() > 0) and (n2SmMsg.size() > 0)) {
    // prepare the body content for Curl
    parser.create_multipart_related_content(
        body, jsonData, CURL_MIME_BOUNDARY, n1SmMsg, n2SmMsg);
  } else if (n1SmMsg.size() > 0) {  // only N1 content
    // prepare the body content for Curl
    parser.create_multipart_related_content(
        body, jsonData, CURL_MIME_BOUNDARY, n1SmMsg,
        multipart_related_content_part_e::NAS);
  } else if (n2SmMsg.size() > 0) {  // only N2 content
    // prepare the body content for Curl
    parser.create_multipart_related_content(
        body, jsonData, CURL_MIME_BOUNDARY, n2SmMsg,
        multipart_related_content_part_e::NGAP);
  } else {
    body         = jsonData;
    is_multipart = false;
  }

  Logger::amf_n11().debug(
      "Send HTTP message to SMF with body %s", body.c_str());

  uint32_t str_len = body.length();
  char* body_data  = (char*) malloc(str_len + 1);
  memset(body_data, 0, str_len + 1);
  memcpy((void*) body_data, (void*) body.c_str(), str_len);

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();

  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    std::string content_type   = {};
    if (is_multipart) {
      content_type = "content-type: multipart/related; boundary=" +
                     std::string(CURL_MIME_BOUNDARY);
    } else {
      content_type = "content-type: application/json";
    }
    headers = curl_slist_append(headers, content_type.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, remote_uri.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    std::unique_ptr<std::string> httpHeaderData(new std::string());

    // Hook up data handling function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, httpHeaderData.get());

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_data);

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    // get cause from the response
    std::string response           = *httpData.get();
    std::string json_data_response = {};
    std::string n1sm               = {};
    std::string n2sm               = {};
    nlohmann::json response_data   = {};
    bstring n1sm_hex, n2sm_hex;

    Logger::amf_n11().info("Get response with HTTP code (%d)", httpCode);
    Logger::amf_n11().info("Response body %s", response.c_str());

    if (static_cast<http_response_codes_e>(httpCode) ==
        http_response_codes_e::HTTP_RESPONSE_CODE_0) {
      // TODO: should be removed
      Logger::amf_n11().error(
          "Cannot get response when calling %s", remote_uri.c_str());
      // free curl before returning
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      curl_global_cleanup();
      free_wrapper((void**) &body_data);
      return;
    }

    if (response.size() > 0) {
      number_parts = parser.parse(response, json_data_response, n1sm, n2sm);
    }

    if (number_parts == 0) {
      json_data_response = response;
    }

    Logger::amf_n11().info("JSON part %s", json_data_response.c_str());

    if ((static_cast<http_response_codes_e>(httpCode) !=
         http_response_codes_e::HTTP_RESPONSE_CODE_200_OK) &&
        (static_cast<http_response_codes_e>(httpCode) !=
         http_response_codes_e::HTTP_RESPONSE_CODE_201_CREATED) &&
        (static_cast<http_response_codes_e>(httpCode) !=
         http_response_codes_e::HTTP_RESPONSE_CODE_204_UPDATED)) {
      // ERROR
      if (response.size() < 1) {
        Logger::amf_n11().error("There's no content in the response");
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        free_wrapper((void**) &body_data);
        // TODO: send context response error
        return;
      }
      // TODO: HO

      // Transfer N1 to gNB/UE if available
      if (number_parts > 1) {
        try {
          response_data = nlohmann::json::parse(json_data_response);
        } catch (nlohmann::json::exception& e) {
          Logger::amf_n11().warn(
              "Could not get JSON content from the response");
          // Set the default Cause
          response_data["error"]["cause"] = "504 Gateway Timeout";
        }

        Logger::amf_n11().debug(
            "Get response with jsonData: %s", json_data_response.c_str());
        conv::msg_str_2_msg_hex(n1sm, n1sm_hex);
        comUt::print_buffer(
            "amf_n11", "Get response with n1sm:", (uint8_t*) bdata(n1sm_hex),
            blength(n1sm_hex));

        std::string cause = response_data["error"]["cause"];
        Logger::amf_n11().debug(
            "Network Function services failure (with cause %s)", cause.c_str());
        //         if (!cause.compare("DNN_DENIED"))
        handle_post_sm_context_response_error(
            httpCode, cause, n1sm_hex, supi, pdu_session_id);
      }

    } else {  // Response with success code
      // Store location of the created context in case of PDU Session
      // Establishment
      std::string header_response = *httpHeaderData.get();
      std::string CRLF            = "\r\n";
      std::size_t location_pos    = header_response.find("Location");
      if (location_pos == std::string::npos)
        location_pos = header_response.find("location");

      if (location_pos != std::string::npos) {
        std::size_t crlf_pos = header_response.find(CRLF, location_pos);
        if (crlf_pos != std::string::npos) {
          std::string location = header_response.substr(
              location_pos + 10, crlf_pos - (location_pos + 10));
          Logger::amf_n11().info(
              "Location of the created SMF context: %s", location.c_str());
          psc.get()->smf_context_location = location;
        }
      }

      try {
        response_data = nlohmann::json::parse(json_data_response);
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().warn("Could not get JSON content from the response");
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        free_wrapper((void**) &body_data);
        // TODO:
        return;
      }

      // For N2 HO
      bool is_ho_procedure       = false;
      std::string promise_result = {};
      if (response_data.find("hoState") != response_data.end()) {
        is_ho_procedure = true;

        std::string ho_state = {};
        response_data.at("hoState").get_to(ho_state);
        if (ho_state.compare("COMPLETED") == 0) {
          if (response_data.find("pduSessionId") != response_data.end())
            response_data.at("pduSessionId").get_to(promise_result);
        } else if (number_parts > 1) {
          promise_result = n1sm;  // actually, N2 SM Info
        }
      }
      // Notify to the result
      if ((promise_id > 0) and (is_ho_procedure)) {
        amf_app_inst->trigger_process_response(promise_id, promise_result);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        free_wrapper((void**) &body_data);
        return;
      }

      // Transfer N1/N2 to gNB/UE if available
      if (number_parts > 1) {
        itti_n1n2_message_transfer_request* itti_msg =
            new itti_n1n2_message_transfer_request(TASK_AMF_N11, TASK_AMF_APP);

        itti_msg->is_n1sm_set = false;
        itti_msg->is_n2sm_set = false;
        itti_msg->is_ppi_set  = false;

        if (n1sm.size() > 0) {
          conv::msg_str_2_msg_hex(n1sm, n1sm_hex);
          comUt::print_buffer(
              "amf_n11", "Get response n1sm:", (uint8_t*) bdata(n1sm_hex),
              blength(n1sm_hex));
          itti_msg->n1sm        = n1sm_hex;
          itti_msg->is_n1sm_set = true;
        }
        if (n2sm.size() > 0) {
          conv::msg_str_2_msg_hex(n2sm, n2sm_hex);
          comUt::print_buffer(
              "amf_n11", "Get response n2sm:", (uint8_t*) bdata(n2sm_hex),
              blength(n2sm_hex));
          itti_msg->n2sm           = n2sm_hex;
          itti_msg->is_n2sm_set    = true;
          itti_msg->n2sm_info_type = response_data
              ["n2SmInfoType"];  // response_data["n2InfoContainer"]["smInfo"]["n2InfoContent"]["ngapIeType"];
        }

        itti_msg->supi           = supi;
        itti_msg->pdu_session_id = pdu_session_id;
        std::shared_ptr<itti_n1n2_message_transfer_request> i =
            std::shared_ptr<itti_n1n2_message_transfer_request>(itti_msg);
        int ret = itti_inst->send_msg(i);
        if (0 != ret) {
          Logger::amf_n11().error(
              "Could not send ITTI message %s to task TASK_AMF_APP",
              i->get_msg_name());
        }
      }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();
  free_wrapper((void**) &body_data);
}
//-----------------------------------------------------------------------------------------------------
bool amf_n11::discover_smf_from_nsi_info(
    std::string& smf_addr, std::string& smf_api_version, std::string& smf_port,
    const snssai_t snssai, const plmn_t plmn, const std::string dnn) {
  Logger::amf_n11().debug(
      "Send NS Selection to NSSF to discover the appropriate NRF");

  bool result                 = true;
  std::string nrf_addr        = {};
  std::string nrf_port        = {};
  std::string nrf_api_version = {};

  uint8_t http_version = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  // Get NSI information from NSSF
  nlohmann::json slice_info  = {};
  nlohmann::json snssai_info = {};
  snssai_info["sst"]         = snssai.sST;
  if (!snssai.sD.empty()) snssai_info["sd"] = snssai.sD;
  slice_info["sNssai"]            = snssai_info;
  slice_info["roamingIndication"] = "NON_ROAMING";
  // ToDo Add TAI

  std::string url = std::string(inet_ntoa(
                        *((struct in_addr*) &amf_cfg.nssf_addr.ipv4_addr))) +
                    ":" + std::to_string(amf_cfg.nssf_addr.port) +
                    "/nnssf-nsselection/" + amf_cfg.nssf_addr.api_version +
                    "/network-slice-information?nf-type=AMF&nf-id=abc&slice-"
                    "info-request-for-pdu-session=" +
                    slice_info.dump();

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl = curl_easy_init();
  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    // headers = curl_slist_append(headers, "charsets: utf-8");
    headers = curl_slist_append(headers, "content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    Logger::amf_n11().debug(
        "NS Selection, response from NSSF, HTTP Code: %d", httpCode);

    if (httpCode == 200) {
      Logger::amf_n11().debug(
          "NS Selection, got successful response from NSSF");

      nlohmann::json response_data = {};
      try {
        response_data = nlohmann::json::parse(*httpData.get());
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().warn(
            "NS Selection, could not parse json from the NRF "
            "response");
      }
      Logger::amf_n11().debug(
          "NS Selection, response from NSSF, json data: \n %s",
          response_data.dump().c_str());

      // Process data to obtain NRF info
      if (response_data.find("nsiInformation") != response_data.end()) {
        std::string nrf_id = response_data["nsiInformation"]["nrfId"];
        std::string nsi_id = response_data["nsiInformation"]["nsiId"];

        std::vector<std::string> split_result;
        boost::split(split_result, nrf_id, boost::is_any_of("/"));
        nrf_api_version = split_result[split_result.size() - 2].c_str();

        std::string nrf_endpoint =
            split_result[split_result.size() - 4].c_str();
        std::vector<std::string> split_nrf_endpoint;
        boost::split(split_nrf_endpoint, nrf_endpoint, boost::is_any_of(":"));
        nrf_port = split_nrf_endpoint[split_nrf_endpoint.size() - 1].c_str();
        nrf_addr = split_nrf_endpoint[split_nrf_endpoint.size() - 2].c_str();
      }
    } else {
      Logger::amf_n11().warn("NS Selection, could not get response from NSSF");
      result = false;
    }

    Logger::amf_n11().debug(
        "NS Selection, NRF Addr: %s, NRF Port: %s, NRF Api Version: %s",
        nrf_addr.c_str(), nrf_port.c_str(), nrf_api_version.c_str());

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  if (!result) return result;

  Logger::amf_n11().debug("NSI Inforation is successfully retrieved from NSSF");
  if (!discover_smf(
          smf_addr, smf_api_version, smf_port, snssai, plmn, dnn, nrf_addr,
          nrf_port, nrf_api_version))
    return false;
  return true;
}

//-----------------------------------------------------------------------------------------------------
bool amf_n11::discover_smf(
    std::string& smf_addr, std::string& smf_api_version, std::string& smf_port,
    const snssai_t snssai, const plmn_t plmn, const std::string dnn,
    const std::string& nrf_addr, const std::string& nrf_port,
    const std::string& nrf_api_version) {
  Logger::amf_n11().debug(
      "Send NFDiscovery to NRF to discover the available SMFs");
  bool result = false;

  uint8_t http_version = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  nlohmann::json json_data = {};
  std::string url          = {};
  // TODO: remove hardcoded values
  if (!nrf_addr.empty() & !nrf_port.empty() & !nrf_api_version.empty()) {
    url = nrf_addr + ":" + nrf_port + "/nnrf-disc/" + nrf_api_version +
          "/nf-instances?target-nf-type=SMF&requester-nf-type=AMF";
  } else
    url = std::string(
              inet_ntoa(*((struct in_addr*) &amf_cfg.nrf_addr.ipv4_addr))) +
          ":" + std::to_string(amf_cfg.nrf_addr.port) + "/nnrf-disc/" +
          amf_cfg.nrf_addr.api_version +
          "/nf-instances?target-nf-type=SMF&requester-nf-type=AMF";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl = curl_easy_init();

  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    // headers = curl_slist_append(headers, "charsets: utf-8");
    headers = curl_slist_append(headers, "content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    Logger::amf_n11().debug(
        "NFDiscovery, response from NRF, HTTP Code: %d", httpCode);

    if (httpCode == 200) {
      Logger::amf_n11().debug("NFDiscovery, got successful response from NRF");

      nlohmann::json response_data = {};
      try {
        response_data = nlohmann::json::parse(*httpData.get());
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().warn(
            "NFDiscovery, could not parse json from the NRF "
            "response");
      }
      Logger::amf_n11().debug(
          "NFDiscovery, response from NRF, json data: \n %s",
          response_data.dump().c_str());

      // Process data to obtain SMF info
      if (response_data.find("nfInstances") != response_data.end()) {
        for (auto& it : response_data["nfInstances"].items()) {
          nlohmann::json instance_json = it.value();
          // TODO: convert instance_json to SMF profile
          // TODO: add SMF to the list of available SMF
          // check with sNSSAI
          if (instance_json.find("sNssais") != instance_json.end()) {
            for (auto& s : instance_json["sNssais"].items()) {
              nlohmann::json Snssai = s.value();
              if (Snssai["sst"].get<int>() == snssai.sST) {
                // Match SD (optional) only if it is provided
                if (Snssai["sd"].empty() or
                    (snssai.sD.compare(Snssai["sd"].get<std::string>()) == 0)) {
                  Logger::amf_n11().debug(
                      "S-NSSAI [SST- %d, SD -%s] is matched for SMF profile",
                      snssai.sST, snssai.sD.c_str());
                  result = true;
                  break;  // NSSAI is included in the list of supported slices
                          // from SMF
                }
              }
            }
          }

          if (!result) {
            Logger::amf_n11().debug("S-NSSAI is not matched for SMF profile");
            // continue;
          }

          // TODO: check DNN
          // TODO: PLMN (need to add plmnList into NRF profile, SMF profile)
          // for now, just IP addr of SMF of the first NF instance
          if (instance_json.find("ipv4Addresses") != instance_json.end()) {
            if (instance_json["ipv4Addresses"].size() > 0)
              smf_addr =
                  instance_json["ipv4Addresses"].at(0).get<std::string>();
          }
          if (instance_json.find("nfServices") != instance_json.end()) {
            if (instance_json["nfServices"].size() > 0) {
              nlohmann::json nf_service = instance_json["nfServices"].at(0);
              if (nf_service.find("versions") != nf_service.end()) {
                nlohmann::json nf_version = nf_service["versions"].at(0);
                if (nf_version.find("apiVersionInUri") != nf_version.end()) {
                  smf_api_version =
                      nf_version["apiVersionInUri"].get<std::string>();
                }
              }
              if (nf_service.find("ipEndPoints") != nf_service.end()) {
                nlohmann::json nf_ip_end = nf_service["ipEndPoints"].at(0);
                if (nf_ip_end.find("port") != nf_ip_end.end()) {
                  smf_port = to_string(nf_ip_end["port"].get<int>()).c_str();
                }
              }
            }
          }

          // Break after first matching SMF instance for requested S-NSSAI
          if (result) break;
        }
      }
      Logger::amf_n11().debug(
          "NFDiscovery, SMF Addr: %s, SMF Api Version: %s, SMF Port: %s",
          smf_addr.c_str(), smf_api_version.c_str(), smf_port.c_str());
    } else {
      Logger::amf_n11().warn("NFDiscovery, could not get response from NRF");
      result = false;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return result;
}

//-----------------------------------------------------------------------------------------------------
void amf_n11::register_nf_instance(
    std::shared_ptr<itti_n11_register_nf_instance_request> msg) {
  Logger::amf_n11().debug(
      "Send NF Instance Registration to NRF (HTTP version %d)",
      msg->http_version);
  nlohmann::json json_data = {};
  msg->profile.to_json(json_data);

  std::string url =
      std::string(inet_ntoa(*((struct in_addr*) &amf_cfg.nrf_addr.ipv4_addr))) +
      ":" + std::to_string(amf_cfg.nrf_addr.port) + "/nnrf-nfm/" +
      amf_cfg.nrf_addr.api_version + "/nf-instances/" +
      msg->profile.get_nf_instance_id();

  Logger::amf_n11().debug(
      "Send NF Instance Registration to NRF, NRF URL %s", url.c_str());

  std::string body = json_data.dump();
  Logger::amf_n11().debug(
      "Send NF Instance Registration to NRF, msg body: \n %s", body.c_str());

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl = curl_easy_init();

  uint8_t http_version = 1;
  if (amf_cfg.support_features.use_http2) http_version = 2;

  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    // headers = curl_slist_append(headers, "charsets: utf-8");
    headers = curl_slist_append(headers, "content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information.
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    Logger::amf_n11().debug(
        "NFDiscovery, response from NRF, HTTP Code: %d", httpCode);

    if (httpCode == 200) {
      Logger::amf_n11().debug(
          "NFRegistration, got successful response from NRF");

      nlohmann::json response_data = {};
      try {
        response_data = nlohmann::json::parse(*httpData.get());
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().warn(
            "NFDiscovery, could not parse json from the NRF "
            "response");
      }
      Logger::amf_n11().debug(
          "NFDiscovery, response from NRF, json data: \n %s",
          response_data.dump().c_str());

      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
  }
}

//-----------------------------------------------------------------------------------------------------
bool amf_n11::send_ue_authentication_request(
    oai::amf::model::AuthenticationInfo& auth_info,
    oai::amf::model::UEAuthenticationCtx& ue_auth_ctx, uint8_t http_version) {
  Logger::amf_n11().debug(
      "Send UE Authentication Request to AUSF (HTTP version %d)", http_version);

  nlohmann::json json_data = {};
  to_json(json_data, auth_info);
  std::string url =
      std::string(
          inet_ntoa(*((struct in_addr*) &amf_cfg.ausf_addr.ipv4_addr))) +
      ":" + std::to_string(amf_cfg.ausf_addr.port) + "/nausf-auth/" +
      amf_cfg.ausf_addr.api_version + "/ue-authentications";

  Logger::amf_n11().debug(
      "Send UE Authentication Request to AUSF, URL %s", url.c_str());

  std::string body = json_data.dump();
  Logger::amf_n11().debug(
      "Send UE Authentication Request to AUSF, msg body: \n %s", body.c_str());

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl = curl_easy_init();

  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    // headers = curl_slist_append(headers, "charsets: utf-8");
    headers = curl_slist_append(headers, "content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    Logger::amf_n11().debug(
        "UE Authentication, response from AUSF, HTTP Code: %d", httpCode);

    if ((httpCode == 200) or
        (httpCode == 201)) {  // TODO: remove hardcoded value
      try {
        std::string tmp = *httpData.get();
        nlohmann::json::parse(tmp.c_str()).get_to(ue_auth_ctx);
        Logger::amf_n11().debug(
            "UE Authentication, response from AUSF\n, %s ", tmp.c_str());
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().warn(
            "UE Authentication, could not parse JSON from the AUSF "
            "response");
        // TODO: error handling
        return false;
      }

    } else {
      Logger::amf_n11().warn(
          "UE Authentication, could not get response from AUSF");
      return false;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return true;
}

//-----------------------------------------------------------------------------------------------------
// From AMF_N1, need to be reworked
void amf_n11::curl_http_client(
    std::string remote_uri, std::string method, std::string msg_body,
    std::string& response, uint8_t http_version) {
  Logger::amf_n11().info("Send HTTP message to %s", remote_uri.c_str());
  Logger::amf_n11().info("HTTP message Body: %s", msg_body.c_str());

  uint32_t str_len = msg_body.length();
  char* body_data  = (char*) malloc(str_len + 1);
  memset(body_data, 0, str_len + 1);
  memcpy((void*) body_data, (void*) msg_body.c_str(), str_len);

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();

  if (curl) {
    CURLcode res               = {};
    struct curl_slist* headers = nullptr;
    if ((method.compare("POST") == 0) or (method.compare("PATCH") == 0) or
        (method.compare("PUT") == 0)) {
      std::string content_type = "Content-Type: application/json";
      headers = curl_slist_append(headers, content_type.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    curl_easy_setopt(curl, CURLOPT_URL, remote_uri.c_str());
    if (method.compare("POST") == 0)
      curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
    else if (method.compare("PATCH") == 0)
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    else if (method.compare("PUT") == 0) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    } else
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);
    curl_easy_setopt(curl, CURLOPT_INTERFACE, amf_cfg.n11.if_name.c_str());

    if (http_version == 2) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      // we use a self-signed test server, skip verification during debugging
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(
          curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    }

    // Response information.
    long httpCode = {0};
    std::unique_ptr<std::string> httpData(new std::string());
    std::unique_ptr<std::string> httpHeaderData(new std::string());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, httpHeaderData.get());
    if ((method.compare("POST") == 0) or (method.compare("PATCH") == 0) or
        (method.compare("PUT") == 0)) {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, msg_body.length());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_data);
    }

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    // get the response
    response                       = *httpData.get();
    std::string json_data_response = {};
    std::string resMsg             = {};
    bool is_response_ok            = true;
    Logger::amf_n11().info("Get response with HTTP code (%d)", httpCode);

    if (httpCode == 0) {
      Logger::amf_n11().info(
          "Cannot get response when calling %s", remote_uri.c_str());
      // free curl before returning
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      return;
    }

    nlohmann::json response_data = {};

    if (httpCode != 200 && httpCode != 201 && httpCode != 204) {
      is_response_ok = false;
      if (response.size() < 1) {
        Logger::amf_n11().info("There's no content in the response");
        // TODO: send context response error
        return;
      }
      Logger::amf_n11().debug("Error with response code %d", httpCode);
      return;
    }

    if (!is_response_ok) {
      try {
        response_data = nlohmann::json::parse(json_data_response);
      } catch (nlohmann::json::exception& e) {
        Logger::amf_n11().info("Could not get JSON content from the response");
        // Set the default Cause
        response_data["error"]["cause"] = "504 Gateway Timeout";
      }

      Logger::amf_n11().info(
          "Get response with jsonData: %s", json_data_response.c_str());

      std::string cause = response_data["error"]["cause"];
      Logger::amf_n11().info("Call Network Function services failure");
      Logger::amf_n11().info("Cause value: %s", cause.c_str());
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  if (body_data) {
    free(body_data);
    body_data = NULL;
  }
  fflush(stdout);
}
