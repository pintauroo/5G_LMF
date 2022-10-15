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

/*! \file smf_sbi.cpp
 \brief
 \author  Lionel GAUTHIER, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2019
 \email: lionel.gauthier@eurecom.fr, tien-thinh.nguyen@eurecom.fr
 */

#include "smf_sbi.hpp"

#include <stdexcept>

#include <curl/curl.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <nlohmann/json.hpp>
#include <boost/algorithm/string/split.hpp>
//#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "common_defs.h"
#include "itti.hpp"
#include "logger.hpp"
#include "mime_parser.hpp"
#include "smf.h"
#include "smf_app.hpp"
#include "smf_config.hpp"

extern "C" {
#include "dynamic_memory_check.h"
}

using namespace Pistache::Http;
using namespace Pistache::Http::Mime;

using namespace smf;
using json = nlohmann::json;

extern itti_mw* itti_inst;
extern smf_sbi* smf_sbi_inst;
extern smf_config smf_cfg;
void smf_sbi_task(void*);

// To read content of the response from AMF
static std::size_t callback(
    const char* in, std::size_t size, std::size_t num, std::string* out) {
  const std::size_t totalBytes(size * num);
  out->append(in, totalBytes);
  return totalBytes;
}

//------------------------------------------------------------------------------
void smf_sbi_task(void* args_p) {
  const task_id_t task_id = TASK_SMF_SBI;
  itti_inst->notify_task_ready(task_id);

  do {
    std::shared_ptr<itti_msg> shared_msg = itti_inst->receive_msg(task_id);
    auto* msg                            = shared_msg.get();
    switch (msg->msg_type) {
      case N11_SESSION_CREATE_SM_CONTEXT_RESPONSE:
        smf_sbi_inst->send_n1n2_message_transfer_request(
            std::static_pointer_cast<itti_n11_create_sm_context_response>(
                shared_msg));
        break;

      case NX_TRIGGER_SESSION_MODIFICATION:
        smf_sbi_inst->send_n1n2_message_transfer_request(
            std::static_pointer_cast<itti_nx_trigger_pdu_session_modification>(
                shared_msg));
        break;

      case N11_SESSION_REPORT_RESPONSE:
        smf_sbi_inst->send_n1n2_message_transfer_request(
            std::static_pointer_cast<itti_n11_session_report_request>(
                shared_msg));
        break;

      case N11_SESSION_NOTIFY_SM_CONTEXT_STATUS:
        smf_sbi_inst->send_sm_context_status_notification(
            std::static_pointer_cast<itti_n11_notify_sm_context_status>(
                shared_msg));
        break;

      case N11_NOTIFY_SUBSCRIBED_EVENT:
        smf_sbi_inst->notify_subscribed_event(
            std::static_pointer_cast<itti_n11_notify_subscribed_event>(
                shared_msg));
        break;

      case N11_REGISTER_NF_INSTANCE_REQUEST:
        smf_sbi_inst->register_nf_instance(
            std::static_pointer_cast<itti_n11_register_nf_instance_request>(
                shared_msg));
        break;

      case N11_UPDATE_NF_INSTANCE_REQUEST:
        smf_sbi_inst->update_nf_instance(
            std::static_pointer_cast<itti_n11_update_nf_instance_request>(
                shared_msg));
        break;

      case N11_DEREGISTER_NF_INSTANCE:
        smf_sbi_inst->deregister_nf_instance(
            std::static_pointer_cast<itti_n11_deregister_nf_instance>(
                shared_msg));
        break;

      case N11_SUBSCRIBE_UPF_STATUS_NOTIFY:
        smf_sbi_inst->subscribe_upf_status_notify(
            std::static_pointer_cast<itti_n11_subscribe_upf_status_notify>(
                shared_msg));
        break;

      case N10_SESSION_GET_SESSION_MANAGEMENT_SUBSCRIPTION:
        break;

      case TERMINATE:
        if (itti_msg_terminate* terminate =
                dynamic_cast<itti_msg_terminate*>(msg)) {
          Logger::smf_sbi().info("Received terminate message");
          return;
        }
        break;

      default:
        Logger::smf_sbi().info("no handler for msg type %d", msg->msg_type);
    }

  } while (true);
}

//------------------------------------------------------------------------------
smf_sbi::smf_sbi() {
  Logger::smf_sbi().startup("Starting...");
  if (itti_inst->create_task(TASK_SMF_SBI, smf_sbi_task, nullptr)) {
    Logger::smf_sbi().error("Cannot create task TASK_SMF_SBI");
    throw std::runtime_error("Cannot create task TASK_SMF_SBI");
  }
  CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
  curl_multi    = curl_multi_init();
  handles       = {};
  headers       = nullptr;
  headers       = curl_slist_append(headers, "Accept: application/json");
  headers       = curl_slist_append(headers, "Content-Type: application/json");
  headers       = curl_slist_append(headers, "charsets: utf-8");

  if ((code < 0) or (curl_multi == nullptr) or (headers == nullptr)) {
    Logger::smf_sbi().error("Cannot initialize Curl Multi Interface");
    throw std::runtime_error("Cannot create task TASK_SMF_SBI");
  }
  Logger::smf_sbi().startup("Started");
}

//------------------------------------------------------------------------------
smf_sbi::~smf_sbi() {
  Logger::smf_sbi().debug("Delete SMF SBI instance...");
  // Remove handle, free memory
  for (auto h : handles) {
    curl_multi_remove_handle(curl_multi, h);
    curl_easy_cleanup(h);
  }

  handles.clear();
  curl_multi_cleanup(curl_multi);
  curl_global_cleanup();
  curl_slist_free_all(headers);
}

//------------------------------------------------------------------------------
void smf_sbi::send_n1n2_message_transfer_request(
    std::shared_ptr<itti_n11_create_sm_context_response> sm_context_res) {
  Logger::smf_sbi().debug(
      "Send Communication_N1N2MessageTransfer to AMF (HTTP version %d)",
      sm_context_res->http_version);

  nlohmann::json json_data = {};
  std::string body         = {};

  sm_context_res->res.get_json_data(json_data);
  std::string json_part = json_data.dump();
  // Add N2 content if available
  auto n2_sm_found = json_data.count("n2InfoContainer");
  if (n2_sm_found > 0) {
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY,
        sm_context_res->res.get_n1_sm_message(),
        sm_context_res->res.get_n2_sm_information());
  } else {
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY,
        sm_context_res->res.get_n1_sm_message(),
        multipart_related_content_part_e::NAS);
  }

  Logger::smf_sbi().debug(
      "Send Communication_N1N2MessageTransfer to AMF, body %s", body.c_str());

  uint32_t str_len           = body.length();
  char data_str[str_len + 1] = {};
  body.copy(data_str, str_len);
  data_str[str_len] = '\0';

  std::string response_data = {};

  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          sm_context_res->res.get_amf_url(), data_str, str_len, response_data,
          pid_ptr, "POST", true, sm_context_res->http_version)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t response_code = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());

  // Get cause from the response
  json response_data_json = {};
  try {
    response_data_json = json::parse(response_data);
  } catch (json::exception& e) {
    Logger::smf_sbi().warn("Could not get the cause from the response");
    // Set the default Cause
    response_data_json["cause"] = "504 Gateway Timeout";
  }
  Logger::smf_sbi().debug(
      "Response from AMF, Http Code: %d, cause %s", response_code,
      response_data_json["cause"].dump().c_str());

  // Send response to APP to process
  std::shared_ptr<itti_n11_n1n2_message_transfer_response_status> itti_msg =
      std::make_shared<itti_n11_n1n2_message_transfer_response_status>(
          TASK_SMF_SBI, TASK_SMF_APP);

  itti_msg->set_response_code(response_code);
  itti_msg->set_scid(sm_context_res->scid);
  itti_msg->set_procedure_type(session_management_procedures_type_e::
                                   PDU_SESSION_ESTABLISHMENT_UE_REQUESTED);
  itti_msg->set_cause(response_data_json["cause"]);
  if (sm_context_res->res.get_cause() ==
      static_cast<uint8_t>(cause_value_5gsm_e::CAUSE_255_REQUEST_ACCEPTED)) {
    itti_msg->set_msg_type(PDU_SESSION_ESTABLISHMENT_ACCEPT);
  } else {
    itti_msg->set_msg_type(PDU_SESSION_ESTABLISHMENT_REJECT);
  }

  int ret = itti_inst->send_msg(itti_msg);
  if (RETURNok != ret) {
    Logger::smf_sbi().error(
        "Could not send ITTI message %s to task TASK_SMF_APP",
        itti_msg->get_msg_name());
  }
  return;
}

//------------------------------------------------------------------------------
void smf_sbi::send_n1n2_message_transfer_request(
    std::shared_ptr<itti_nx_trigger_pdu_session_modification>
        sm_session_modification) {
  Logger::smf_sbi().debug("Send Communication_N1N2MessageTransfer to AMF");

  std::string body         = {};
  nlohmann::json json_data = {};
  std::string json_part    = {};
  sm_session_modification->msg.get_json_data(json_data);
  json_part = json_data.dump();

  // add N2 content if available
  auto n2_sm_found = json_data.count("n2InfoContainer");
  if (n2_sm_found > 0) {
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY,
        sm_session_modification->msg.get_n1_sm_message(),
        sm_session_modification->msg.get_n2_sm_information());
  } else {
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY,
        sm_session_modification->msg.get_n1_sm_message(),
        multipart_related_content_part_e::NAS);
  }

  uint32_t str_len           = body.length();
  char data_str[str_len + 1] = {};
  body.copy(data_str, str_len);
  data_str[str_len] = '\0';

  std::string response_data = {};

  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new Curl Easy Handle and add to the Multi Handle
  if (!curl_create_handle(
          sm_session_modification->msg.get_amf_url(), data_str, str_len,
          response_data, pid_ptr, "POST", true)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t response_code = get_available_response(f);
  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());

  json response_data_json = {};
  try {
    response_data_json = json::parse(response_data);
  } catch (json::exception& e) {
    Logger::smf_sbi().warn("Could not get the cause from the response");
  }
  Logger::smf_sbi().debug("Response from AMF, Http Code: %u", response_code);
}

//------------------------------------------------------------------------------
void smf_sbi::send_n1n2_message_transfer_request(
    std::shared_ptr<itti_n11_session_report_request> report_msg) {
  Logger::smf_sbi().debug(
      "Send Communication_N1N2MessageTransfer to AMF (Network-initiated "
      "Service Request)");

  std::string n2_message   = report_msg->res.get_n2_sm_information();
  nlohmann::json json_data = {};
  std::string body         = {};
  report_msg->res.get_json_data(json_data);
  std::string json_part = json_data.dump();

  // Add N1 content if available
  auto n1_sm_found = json_data.count("n1MessageContainer");
  if (n1_sm_found > 0) {
    std::string n1_message = report_msg->res.get_n1_sm_message();
    // prepare the body content for Curl
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY, n1_message, n2_message);
  } else {
    mime_parser::create_multipart_related_content(
        body, json_part, CURL_MIME_BOUNDARY, n2_message,
        multipart_related_content_part_e::NGAP);
  }

  uint32_t str_len           = body.length();
  char data_str[str_len + 1] = {};
  body.copy(data_str, str_len);
  data_str[str_len] = '\0';

  std::string response_data = {};

  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          report_msg->res.get_amf_url(), data_str, str_len, response_data,
          pid_ptr, "POST", true)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);
  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());

  json response_data_json = {};
  try {
    response_data_json = json::parse(response_data);
  } catch (json::exception& e) {
    Logger::smf_sbi().warn("Could not get the cause from the response");
    // Set the default Cause
    response_data_json["cause"] = "504 Gateway Timeout";
  }
  Logger::smf_sbi().debug(
      "Response from AMF, Http Code: %d, cause %s", httpCode,
      response_data_json["cause"].dump().c_str());

  // Send response to APP to process
  std::shared_ptr<itti_n11_n1n2_message_transfer_response_status> itti_msg =
      std::make_shared<itti_n11_n1n2_message_transfer_response_status>(
          TASK_SMF_SBI, TASK_SMF_APP);

  itti_msg->set_response_code(httpCode);
  itti_msg->set_procedure_type(
      session_management_procedures_type_e::SERVICE_REQUEST_NETWORK_TRIGGERED);
  itti_msg->set_cause(response_data_json["cause"]);
  itti_msg->set_seid(report_msg->res.get_seid());
  itti_msg->set_trxn_id(report_msg->res.get_trxn_id());

  int ret = itti_inst->send_msg(itti_msg);
  if (RETURNok != ret) {
    Logger::smf_sbi().error(
        "Could not send ITTI message %s to task TASK_SMF_APP",
        itti_msg->get_msg_name());
  }
}

//------------------------------------------------------------------------------
void smf_sbi::send_sm_context_status_notification(
    std::shared_ptr<itti_n11_notify_sm_context_status> sm_context_status) {
  Logger::smf_sbi().debug(
      "Send SM Context Status Notification to AMF(HTTP version %d)",
      sm_context_status->http_version);
  Logger::smf_sbi().debug(
      "AMF URI: %s", sm_context_status->amf_status_uri.c_str());

  nlohmann::json json_data = {};
  // Fill the json part
  json_data["statusInfo"]["resourceStatus"] =
      sm_context_status->sm_context_status;
  std::string body = json_data.dump();

  std::string response_data;
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          sm_context_status->amf_status_uri, body, response_data, pid_ptr,
          "POST")) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t response_code = get_available_response(f);
  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response code %u", response_code);
  // TODO: in case of "307 temporary redirect"
}

//-----------------------------------------------------------------------------------------------------
void smf_sbi::notify_subscribed_event(
    std::shared_ptr<itti_n11_notify_subscribed_event> msg) {
  Logger::smf_sbi().debug(
      "Send notification for the subscribed event to the subscription");

  // Create and add an easy handle to a  multi curl request
  for (auto i : msg->event_notifs) {
    // Fill the json part
    nlohmann::json json_data   = {};
    json_data["notifId"]       = i.get_notif_id();
    auto event_notifs          = nlohmann::json::array();
    nlohmann::json event_notif = {};
    event_notif["event"]       = smf_event_from_enum(i.get_smf_event());
    event_notif["pduSeId"]     = i.get_pdu_session_id();
    event_notif["supi"]        = std::to_string(i.get_supi());

    if (i.is_ad_ipv4_addr_is_set()) {
      event_notif["adIpv4Addr"] = i.get_ad_ipv4_addr();
    }
    if (i.is_re_ipv4_addr_is_set()) {
      event_notif["reIpv4Addr"] = i.get_re_ipv4_addr();
    }

    // add support for plmn change.
    if (i.is_plmnid_is_set()) {
      event_notif["plmnId"] = i.get_plmnid();
    }

    // add support for ddds
    if (i.is_ddds_is_set()) {
      // TODO: change this one to the real value when finished the event for
      // ddds
      // event_notif["dddStatus"] = i.get_ddds();
      event_notif["dddStatus"] = "TRANSMITTED";
    }

    // customized data
    nlohmann::json customized_data = {};
    i.get_custom_info(customized_data);
    if (!customized_data.is_null())
      event_notif["customized_data"] = customized_data;
    // timestamp
    std::time_t time_epoch_ntp = std::time(nullptr);
    uint64_t tv_ntp            = time_epoch_ntp + SECONDS_SINCE_FIRST_EPOCH;
    event_notif["timeStamp"]   = std::to_string(tv_ntp);
    event_notifs.push_back(event_notif);
    json_data["eventNotifs"] = event_notifs;
    std::string body         = json_data.dump();

    std::string response_data;
    // Generate a promise and associate this promise to the curl handle
    uint32_t promise_id = generate_promise_id();
    Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
    uint32_t* pid_ptr = &promise_id;
    boost::shared_ptr<boost::promise<uint32_t>> p =
        boost::make_shared<boost::promise<uint32_t>>();
    boost::shared_future<uint32_t> f;
    f = p->get_future();
    add_promise(promise_id, p);

    std::string url = i.get_notif_uri();

    // Create a new curl easy handle and add to the multi handle
    if (!curl_create_handle(url, body, response_data, pid_ptr, "POST")) {
      Logger::smf_sbi().warn("Could not create a new handle to send message");
      remove_promise(promise_id);
      return;
    }

    // Wait for the response back
    uint32_t response_code = get_available_response(f);

    Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
    Logger::smf_sbi().debug("Response code %u", response_code);
    Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  }
  return;
}

//-----------------------------------------------------------------------------------------------------
void smf_sbi::register_nf_instance(
    std::shared_ptr<itti_n11_register_nf_instance_request> msg) {
  Logger::smf_sbi().debug(
      "Send NF Instance Registration to NRF (HTTP version %d)",
      msg->http_version);
  nlohmann::json json_data = {};
  msg->profile.to_json(json_data);

  std::string url =
      std::string(inet_ntoa(*((struct in_addr*) &smf_cfg.nrf_addr.ipv4_addr))) +
      ":" + std::to_string(smf_cfg.nrf_addr.port) + NNRF_NFM_BASE +
      smf_cfg.nrf_addr.api_version + NNRF_NF_REGISTER_URL +
      msg->profile.get_nf_instance_id();

  Logger::smf_sbi().debug(
      "Send NF Instance Registration to NRF, NRF URL %s", url.c_str());

  std::string body = json_data.dump();
  Logger::smf_sbi().debug(
      "Send NF Instance Registration to NRF, msg body: \n %s", body.c_str());

  std::string response_data = {};
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          url, body, response_data, pid_ptr, "PUT", msg->http_version)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  Logger::smf_sbi().debug(
      "NF Instance Registration, response from NRF, HTTP Code: %u", httpCode);

  if (static_cast<http_response_codes_e>(httpCode) ==
      http_response_codes_e::HTTP_RESPONSE_CODE_CREATED) {
    json response_json = {};
    try {
      response_json = json::parse(response_data);
    } catch (json::exception& e) {
      Logger::smf_sbi().warn(
          "NF Instance Registration, could not parse json from the NRF "
          "response");
    }
    Logger::smf_sbi().debug(
        "NF Instance Registration, response from NRF, json data: \n %s",
        response_json.dump().c_str());

    // Send response to APP to process
    std::shared_ptr<itti_n11_register_nf_instance_response> itti_msg =
        std::make_shared<itti_n11_register_nf_instance_response>(
            TASK_SMF_SBI, TASK_SMF_APP);
    itti_msg->http_response_code = httpCode;
    itti_msg->http_version       = msg->http_version;
    Logger::smf_app().debug("Registered SMF profile (from NRF)");
    itti_msg->profile.from_json(response_json);

    int ret = itti_inst->send_msg(itti_msg);
    if (RETURNok != ret) {
      Logger::smf_sbi().error(
          "Could not send ITTI message %s to task TASK_SMF_APP",
          itti_msg->get_msg_name());
    }
  } else {
    Logger::smf_sbi().warn(
        "NF Instance Registration, could not get response from NRF");
  }
}

//-----------------------------------------------------------------------------------------------------
void smf_sbi::update_nf_instance(
    std::shared_ptr<itti_n11_update_nf_instance_request> msg) {
  Logger::smf_sbi().debug(
      "Send NF Update to NRF (HTTP version %d)", msg->http_version);

  nlohmann::json json_data = nlohmann::json::array();
  for (auto i : msg->patch_items) {
    nlohmann::json item = {};
    to_json(item, i);
    json_data.push_back(item);
  }
  std::string body = json_data.dump();
  Logger::smf_sbi().debug("Send NF Update to NRF, Msg body %s", body.c_str());

  std::string url =
      std::string(inet_ntoa(*((struct in_addr*) &smf_cfg.nrf_addr.ipv4_addr))) +
      ":" + std::to_string(smf_cfg.nrf_addr.port) + NNRF_NFM_BASE +
      smf_cfg.nrf_addr.api_version + NNRF_NF_REGISTER_URL +
      msg->smf_instance_id;

  Logger::smf_sbi().debug("Send NF Update to NRF, NRF URL %s", url.c_str());

  std::string response_data = {};
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          url, body, response_data, pid_ptr, "PATCH", msg->http_version)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  Logger::smf_sbi().debug(
      "NF Instance Registration, response from NRF, HTTP Code: %u", httpCode);

  if ((static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_OK) or
      (static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_NO_CONTENT)) {
    Logger::smf_sbi().debug("NF Update, got successful response from NRF");

    // TODO: In case of response containing NF profile
    // Send response to APP to process
    std::shared_ptr<itti_n11_update_nf_instance_response> itti_msg =
        std::make_shared<itti_n11_update_nf_instance_response>(
            TASK_SMF_SBI, TASK_SMF_APP);
    itti_msg->http_response_code = httpCode;
    itti_msg->http_version       = msg->http_version;
    itti_msg->smf_instance_id    = msg->smf_instance_id;

    int ret = itti_inst->send_msg(itti_msg);
    if (RETURNok != ret) {
      Logger::smf_sbi().error(
          "Could not send ITTI message %s to task TASK_SMF_APP",
          itti_msg->get_msg_name());
    }
  } else {
    Logger::smf_sbi().warn("NF Update, could not get response from NRF");
  }
}

//-----------------------------------------------------------------------------------------------------
void smf_sbi::deregister_nf_instance(
    std::shared_ptr<itti_n11_deregister_nf_instance> msg) {
  Logger::smf_sbi().debug(
      "Send NF De-register to NRF (HTTP version %d)", msg->http_version);

  std::string url =
      std::string(inet_ntoa(*((struct in_addr*) &smf_cfg.nrf_addr.ipv4_addr))) +
      ":" + std::to_string(smf_cfg.nrf_addr.port) + NNRF_NFM_BASE +
      smf_cfg.nrf_addr.api_version + NNRF_NF_REGISTER_URL +
      msg->smf_instance_id;

  Logger::smf_sbi().debug(
      "Send NF De-register to NRF (NRF URL %s)", url.c_str());

  std::string response_data = {};
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          url, response_data, pid_ptr, "DELETE", msg->http_version)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  Logger::smf_sbi().debug(
      "NF Instance Registration, response from NRF, HTTP Code: %u", httpCode);

  if ((static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_OK) or
      (static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_NO_CONTENT)) {
    Logger::smf_sbi().debug("NF De-register, got successful response from NRF");

  } else {
    Logger::smf_sbi().warn("NF De-register, could not get response from NRF");
  }
}

//-----------------------------------------------------------------------------------------------------
void smf_sbi::subscribe_upf_status_notify(
    std::shared_ptr<itti_n11_subscribe_upf_status_notify> msg) {
  Logger::smf_sbi().debug(
      "Send NFSubscribeNotify to NRF to be notified when a new UPF becomes "
      "available (HTTP version %d)",
      msg->http_version);

  Logger::smf_sbi().debug(
      "Send NFStatusNotify to NRF, NRF URL %s", msg->url.c_str());

  std::string body = msg->json_data.dump();
  Logger::smf_sbi().debug(
      "Send NFStatusNotify to NRF, msg body: %s", body.c_str());

  std::string response_data = {};
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(
          msg->url, body, response_data, pid_ptr, "POST", msg->http_version)) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  Logger::smf_sbi().debug(
      "NF Instance Registration, response from NRF, HTTP Code: %u", httpCode);

  if ((static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_CREATED) or
      (static_cast<http_response_codes_e>(httpCode) ==
       http_response_codes_e::HTTP_RESPONSE_CODE_NO_CONTENT)) {
    Logger::smf_sbi().debug(
        "NFSubscribeNotify, got successful response from NRF");

  } else {
    Logger::smf_sbi().warn(
        "NFSubscribeNotify, could not get response from NRF");
  }
}

//------------------------------------------------------------------------------
bool smf_sbi::get_sm_data(
    const supi64_t& supi, const std::string& dnn, const snssai_t& snssai,
    std::shared_ptr<session_management_subscription>& subscription,
    plmn_t plmn) {
  nlohmann::json jsonData = {};
  std::string query_str   = {};
  std::string mcc         = {};
  std::string mnc         = {};
  conv::plmnToMccMnc(plmn, mcc, mnc);

  query_str = "?single-nssai={\"sst\":" + std::to_string(snssai.sST) +
              ",\"sd\":\"" + snssai.sD + "\"}&dnn=" + dnn +
              "&plmn-id={\"mcc\":\"" + mcc + "\",\"mnc\":\"" + mnc + "\"}";
  std::string url =
      std::string(inet_ntoa(*((struct in_addr*) &smf_cfg.udm_addr.ipv4_addr))) +
      ":" + std::to_string(smf_cfg.udm_addr.port) + NUDM_SDM_BASE +
      smf_cfg.udm_addr.api_version +
      fmt::format(NUDM_SDM_GET_SM_DATA_URL, std::to_string(supi)) + query_str;

  Logger::smf_sbi().debug("UDM's URL: %s ", url.c_str());

  std::string response_data = {};
  // Generate a promise and associate this promise to the curl handle
  uint32_t promise_id = generate_promise_id();
  Logger::smf_sbi().debug("Promise ID generated %d", promise_id);
  uint32_t* pid_ptr = &promise_id;
  boost::shared_ptr<boost::promise<uint32_t>> p =
      boost::make_shared<boost::promise<uint32_t>>();
  boost::shared_future<uint32_t> f;
  f = p->get_future();
  add_promise(promise_id, p);

  // Create a new curl easy handle and add to the multi handle
  if (!curl_create_handle(url, response_data, pid_ptr, "GET")) {
    Logger::smf_sbi().warn("Could not create a new handle to send message");
    remove_promise(promise_id);
    return false;
  }

  // Wait for the response back
  uint32_t httpCode = get_available_response(f);

  Logger::smf_sbi().debug("Got result for promise ID %d", promise_id);
  Logger::smf_sbi().debug("Response data %s", response_data.c_str());
  Logger::smf_sbi().debug(
      "Session Management Subscription Data Retrieval, response from UDM, HTTP "
      "Code: %u",
      httpCode);

  if (static_cast<http_response_codes_e>(httpCode) ==
      http_response_codes_e::HTTP_RESPONSE_CODE_OK) {
    Logger::smf_sbi().debug(
        "Got successful response from UDM, URL: %s ", url.c_str());
    try {
      jsonData = nlohmann::json::parse(response_data);
    } catch (json::exception& e) {
      Logger::smf_sbi().warn("Could not parse json data from UDM");
    }
  } else {
    Logger::smf_sbi().warn(
        "Could not get response from UDM, URL %s, retry ...", url.c_str());
    // retry
    // TODO
  }

  // Process the response
  if (!jsonData.empty()) {
    Logger::smf_sbi().debug("Response from UDM %s", jsonData.dump().c_str());
    // Verify SNSSAI
    if (jsonData.find("singleNssai") == jsonData.end()) return false;
    if (jsonData["singleNssai"].find("sst") != jsonData["singleNssai"].end()) {
      uint8_t sst = jsonData["singleNssai"]["sst"].get<uint8_t>();
      if (sst != snssai.sST) {
        return false;
      }
    }
    if (jsonData["singleNssai"].find("sd") != jsonData["singleNssai"].end()) {
      std::string sd = jsonData["singleNssai"]["sd"];
      if (sd.compare(snssai.sD) != 0) {
        return false;
      }
    }

    // Retrieve SessionManagementSubscription and store in the context
    for (nlohmann::json::iterator it = jsonData["dnnConfigurations"].begin();
         it != jsonData["dnnConfigurations"].end(); ++it) {
      Logger::smf_sbi().debug("DNN %s", it.key().c_str());
      if (it.key().compare(dnn) != 0) break;

      try {
        std::shared_ptr<dnn_configuration_t> dnn_configuration =
            std::make_shared<dnn_configuration_t>();
        // PDU Session Type (Mandatory)
        std::string default_session_type =
            it.value()["pduSessionTypes"]["defaultSessionType"];
        Logger::smf_sbi().debug(
            "Default session type %s", default_session_type.c_str());
        pdu_session_type_t pdu_session_type(default_session_type);
        dnn_configuration->pdu_session_types.default_session_type =
            pdu_session_type;

        // SSC_Mode (Mandatory)
        std::string default_ssc_mode = it.value()["sscModes"]["defaultSscMode"];
        Logger::smf_sbi().debug(
            "Default SSC Mode %s", default_ssc_mode.c_str());
        ssc_mode_t ssc_mode(default_ssc_mode);
        dnn_configuration->ssc_modes.default_ssc_mode = ssc_mode;

        // 5gQosProfile (Optional)
        if (it.value().find("5gQosProfile") != it.value().end()) {
          dnn_configuration->_5g_qos_profile._5qi =
              it.value()["5gQosProfile"]["5qi"];
          dnn_configuration->_5g_qos_profile.arp.priority_level =
              it.value()["5gQosProfile"]["arp"]["priorityLevel"];
          dnn_configuration->_5g_qos_profile.arp.preempt_cap =
              it.value()["5gQosProfile"]["arp"]["preemptCap"];
          dnn_configuration->_5g_qos_profile.arp.preempt_vuln =
              it.value()["5gQosProfile"]["arp"]["preemptVuln"];
          // Optinal
          if (it.value()["5gQosProfile"].find("") !=
              it.value()["5gQosProfile"].end()) {
            dnn_configuration->_5g_qos_profile.priority_level =
                it.value()["5gQosProfile"]["5QiPriorityLevel"];
          }
        }

        // session_ambr (Optional)
        if (it.value().find("sessionAmbr") != it.value().end()) {
          dnn_configuration->session_ambr.uplink =
              it.value()["sessionAmbr"]["uplink"];
          dnn_configuration->session_ambr.downlink =
              it.value()["sessionAmbr"]["downlink"];
          Logger::smf_sbi().debug(
              "Session AMBR Uplink %s, Downlink %s",
              dnn_configuration->session_ambr.uplink.c_str(),
              dnn_configuration->session_ambr.downlink.c_str());
        }

        // Static IP Addresses (Optional)
        if (it.value().find("staticIpAddress") != it.value().end()) {
          for (const auto& ip_addr : it.value()["staticIpAddress"]) {
            if (ip_addr.find("ipv4Addr") != ip_addr.end()) {
              struct in_addr ue_ipv4_addr = {};
              std::string ue_ip_str = ip_addr["ipv4Addr"].get<std::string>();
              // ip_addr.at("ipv4Addr").get_to(ue_ip_str);
              IPV4_STR_ADDR_TO_INADDR(
                  util::trim(ue_ip_str).c_str(), ue_ipv4_addr,
                  "BAD IPv4 ADDRESS FORMAT FOR UE IP ADDR !");
              ip_address_t ue_ip = {};
              ue_ip              = ue_ipv4_addr;
              dnn_configuration->static_ip_addresses.push_back(ue_ip);
            } else if (ip_addr.find("ipv6Addr") != ip_addr.end()) {
              unsigned char buf_in6_addr[sizeof(struct in6_addr)];
              struct in6_addr ue_ipv6_addr;
              std::string ue_ip_str = ip_addr["ipv6Addr"].get<std::string>();

              if (inet_pton(
                      AF_INET6, util::trim(ue_ip_str).c_str(), buf_in6_addr) ==
                  1) {
                memcpy(&ue_ipv6_addr, buf_in6_addr, sizeof(struct in6_addr));
              } else {
                Logger::smf_app().error(
                    "Bad UE IPv6 Addr %s", ue_ip_str.c_str());
                throw("Bad UE IPv6 Addr %s", ue_ip_str.c_str());
              }

              ip_address_t ue_ip = {};
              ue_ip              = ue_ipv6_addr;
              dnn_configuration->static_ip_addresses.push_back(ue_ip);
            } else if (ip_addr.find("ipv6Prefix") != ip_addr.end()) {
              unsigned char buf_in6_addr[sizeof(struct in6_addr)];
              struct in6_addr ipv6_prefix;
              std::string prefix_str = ip_addr["ipv6Prefix"].get<std::string>();
              std::vector<std::string> words = {};
              boost::split(
                  words, prefix_str, boost::is_any_of("/"),
                  boost::token_compress_on);
              if (words.size() != 2) {
                Logger::smf_app().error(
                    "Bad value for UE IPv6 Prefix %s", prefix_str.c_str());
                return RETURNerror;
              }

              if (inet_pton(
                      AF_INET6, util::trim(words.at(0)).c_str(),
                      buf_in6_addr) == 1) {
                memcpy(&ipv6_prefix, buf_in6_addr, sizeof(struct in6_addr));
              } else {
                Logger::smf_app().error(
                    "Bad UE IPv6 Addr %s", words.at(0).c_str());
                throw("Bad UE IPv6 Addr %s", words.at(0).c_str());
              }

              ip_address_t ue_ip           = {};
              ipv6_prefix_t ue_ipv6_prefix = {};
              ue_ipv6_prefix.prefix_len    = std::stoi(util::trim(words.at(1)));
              ue_ipv6_prefix.prefix        = ipv6_prefix;
              ue_ip                        = ue_ipv6_prefix;
              dnn_configuration->static_ip_addresses.push_back(ue_ip);
            }
          }
        }

        subscription->insert_dnn_configuration(it.key(), dnn_configuration);
        return true;
      } catch (nlohmann::json::exception& e) {
        Logger::smf_sbi().warn(
            "Exception message %s, exception id %d ", e.what(), e.id);
        return false;
      } catch (std::exception& e) {
        Logger::smf_sbi().warn("Exception message %s", e.what());
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
void smf_sbi::subscribe_sm_data() {
  // TODO:
}

//------------------------------------------------------------------------------
bool smf_sbi::curl_create_handle(
    const std::string& uri, const char* data, uint32_t data_len,
    std::string& response_data, uint32_t* promise_id, const std::string& method,
    bool is_multipart, uint8_t http_version) {
  // Create handle for a curl request
  CURL* curl = curl_easy_init();

  if (is_multipart) {
    std::string content_type = "content-type: multipart/related; boundary=" +
                               std::string(CURL_MIME_BOUNDARY);
    headers = curl_slist_append(headers, content_type.c_str());
  }

  if ((curl == nullptr) or (headers == nullptr)) {
    Logger::smf_sbi().error("Cannot initialize a new Curl Handle");
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_PRIVATE, promise_id);
  if (method.compare("POST") == 0)
    curl_easy_setopt(curl, CURLOPT_POST, 1);
  else if (method.compare("PATCH") == 0)
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  else if (method.compare("PUT") == 0)
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  else
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, NF_CURL_TIMEOUT_MS);
  curl_easy_setopt(curl, CURLOPT_INTERFACE, smf_cfg.sbi.if_name.c_str());

  if (http_version == 2) {
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // We use a self-signed test server, skip verification during debugging
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(
        curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
  }

  // Hook up data handling function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

  // Add to the multi handle
  curl_multi_add_handle(curl_multi, curl);
  handles.push_back(curl);

  // The curl cmd will actually be performed in perform_curl_multi
  perform_curl_multi(
      0);  // TODO: current time as parameter if curl is performed per event

  return true;
}

//------------------------------------------------------------------------------
bool smf_sbi::curl_create_handle(
    const std::string& uri, const std::string& data, std::string& response_data,
    uint32_t* promise_id, const std::string& method, uint8_t http_version) {
  // Create handle for a curl request
  CURL* curl = curl_easy_init();

  if ((curl == nullptr) or (headers == nullptr)) {
    Logger::smf_sbi().error("Cannot initialize a new Curl Handle");
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_PRIVATE, promise_id);

  if (method.compare("POST") == 0)
    curl_easy_setopt(curl, CURLOPT_POST, 1);
  else if (method.compare("PATCH") == 0)
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  else if (method.compare("PUT") == 0)
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  else
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, NF_CURL_TIMEOUT_MS);
  curl_easy_setopt(curl, CURLOPT_INTERFACE, smf_cfg.sbi.if_name.c_str());

  if (http_version == 2) {
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // We use a self-signed test server, skip verification during debugging
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(
        curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
  }

  // Hook up data handling function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  if (method.compare("DELETE") != 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  }
  // Add to the multi handle
  curl_multi_add_handle(curl_multi, curl);
  handles.push_back(curl);

  // Curl cmd will actually be performed in perform_curl_multi
  perform_curl_multi(
      0);  // TODO: current time as parameter if curl is performed per event
  return true;
}

//------------------------------------------------------------------------------
bool smf_sbi::curl_create_handle(
    const std::string& uri, std::string& response_data, uint32_t* promise_id,
    const std::string& method, uint8_t http_version) {
  // Create handle for a curl request
  CURL* curl = curl_easy_init();

  if ((curl == nullptr) or (headers == nullptr)) {
    Logger::smf_sbi().error("Cannot initialize a new Curl Handle");
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_PRIVATE, promise_id);

  if (method.compare("DELETE") == 0)
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  else
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, NF_CURL_TIMEOUT_MS);
  curl_easy_setopt(curl, CURLOPT_INTERFACE, smf_cfg.sbi.if_name.c_str());

  if (http_version == 2) {
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // We use a self-signed test server, skip verification during debugging
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(
        curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
  }

  // Hook up data handling function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  // Add to the multi handle
  curl_multi_add_handle(curl_multi, curl);
  handles.push_back(curl);

  // Curl cmd will actually be performed in perform_curl_multi
  perform_curl_multi(
      0);  // TODO: current time as parameter if curl is performed per event

  return true;
}

//------------------------------------------------------------------------------
void smf_sbi::perform_curl_multi(uint64_t ms) {
  //_unused(ms);
  int still_running = 0;
  int numfds        = 0;

  CURLMcode code = curl_multi_perform(curl_multi, &still_running);

  do {
    code = curl_multi_wait(curl_multi, NULL, 0, 200000, &numfds);
    if (code != CURLM_OK) {
      Logger::smf_app().debug("curl_multi_wait() returned %d!", code);
    }
    curl_multi_perform(curl_multi, &still_running);
  } while (still_running);

  curl_release_handles();
}

//------------------------------------------------------------------------------
void smf_sbi::curl_release_handles() {
  CURLMsg* curl_msg = nullptr;
  CURL* curl        = nullptr;
  CURLcode code     = {};
  int http_code     = 0;
  int msgs_left     = 0;

  while ((curl_msg = curl_multi_info_read(curl_multi, &msgs_left))) {
    if (curl_msg && curl_msg->msg == CURLMSG_DONE) {
      curl = curl_msg->easy_handle;
      code = curl_msg->data.result;

      if (code != CURLE_OK) {
        Logger::smf_app().debug("CURL error code  %d!", curl_msg->data.result);
        continue;
      }
      // Get HTTP code
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      Logger::smf_app().debug("Got response with HTTP code  %d!", http_code);
      uint32_t* promise_id = nullptr;
      curl_easy_getinfo(curl, CURLINFO_PRIVATE, &promise_id);
      if (promise_id) {
        Logger::smf_app().debug(
            "Prepare to make promise id %d ready!", *promise_id);
        trigger_process_response(*promise_id, http_code);
      }

      curl_multi_remove_handle(curl_multi, curl);
      curl_easy_cleanup(curl);

      std::vector<CURL*>::iterator it;
      it = find(handles.begin(), handles.end(), curl);
      if (it != handles.end()) {
        handles.erase(it);
      }

    } else if (curl_msg) {
      curl = curl_msg->easy_handle;
      Logger::smf_app().debug("Error after curl_multi_info_read()");
      curl_multi_remove_handle(curl_multi, curl);
      curl_easy_cleanup(curl);

      std::vector<CURL*>::iterator it;
      it = find(handles.begin(), handles.end(), curl);
      if (it != handles.end()) {
        handles.erase(it);
      }
    } else {
      Logger::smf_app().debug("curl_msg null");
    }
  }
}

//---------------------------------------------------------------------------------------------
uint32_t smf_sbi::get_available_response(boost::shared_future<uint32_t>& f) {
  f.wait();  // Wait for it to finish
  assert(f.is_ready());
  assert(f.has_value());
  assert(!f.has_exception());

  uint32_t response_code = f.get();
  return response_code;
}

//---------------------------------------------------------------------------------------------
void smf_sbi::add_promise(
    uint32_t id, boost::shared_ptr<boost::promise<uint32_t>>& p) {
  std::unique_lock lock(m_curl_handle_promises);
  curl_handle_promises.emplace(id, p);
}

//---------------------------------------------------------------------------------------------
void smf_sbi::remove_promise(uint32_t id) {
  std::unique_lock lock(m_curl_handle_promises);
  curl_handle_promises.erase(id);
}

//------------------------------------------------------------------------------
void smf_sbi::trigger_process_response(uint32_t pid, uint32_t http_code) {
  Logger::smf_app().debug(
      "Trigger process response: Set promise with ID %u "
      "to ready",
      pid);
  std::unique_lock lock(m_curl_handle_promises);
  if (curl_handle_promises.count(pid) > 0) {
    curl_handle_promises[pid]->set_value(http_code);
    // Remove this promise from list
    curl_handle_promises.erase(pid);
  }
}
