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

/*! \file udm_http2-server.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: tien-thinh.nguyen@eurecom.fr
 */

#include "udm-http2-server.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <regex>
#include <nlohmann/json.hpp>
#include <string>
#include "string.hpp"

#include "logger.hpp"
#include "udm_config.hpp"
#include "3gpp_29.500.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

using namespace oai::udm::config;
using namespace oai::udm::model;

extern udm_config udm_cfg;

//------------------------------------------------------------------------------
void udm_http2_server::start() {
  boost::system::error_code ec;

  Logger::udm_server().info("HTTP2 server started");
  // Generate Auth Data
  server.handle(
      NUDM_UE_AU_BASE + udm_cfg.sbi.api_version + "/",
      [&](const request& request, const response& response) {
        request.on_data([&](const uint8_t* data, std::size_t len) {
          std::string msg((char*) data, len);
          try {
            std::vector<std::string> split_q;
            boost::split(split_q, request.uri().path, boost::is_any_of("/"));
            if (split_q[split_q.size() - 1].compare(NUDM_UE_AU_GEN_AU_DATA) ==
                0) {
              if (request.method().compare("POST") == 0 && len > 0) {
                AuthenticationInfoRequest authenticationInfoRequest;
                std::string supiOrSuci = split_q[split_q.size() - 3].c_str();
                nlohmann::json::parse(msg.c_str())
                    .get_to(authenticationInfoRequest);

                this->generate_auth_data_request_handler(
                    supiOrSuci, authenticationInfoRequest, response);
              }
            }
          } catch (std::exception& e) {
            Logger::udm_server().warn("Invalid request (error: %s)!", e.what());
            response.write_head(
                http_status_code_e::HTTP_STATUS_CODE_400_BAD_REQUEST);
            response.end();
            return;
          }
        });
      });

  // Confirm/Delete Auth
  server.handle(
      NUDM_UE_AU_BASE + udm_cfg.sbi.api_version,
      [&](const request& request, const response& response) {
        request.on_data([&](const uint8_t* data, std::size_t len) {
          std::string msg((char*) data, len);
          try {
            std::vector<std::string> split_q;
            boost::split(split_q, request.uri().path, boost::is_any_of("/"));
            if (split_q[split_q.size() - 2].compare(NUDM_UE_AU_EVENTS) == 0) {
              if (request.method().compare("PUT") == 0 && len > 0) {
                std::string supi        = split_q[split_q.size() - 3].c_str();
                std::string authEventId = split_q[split_q.size() - 1].c_str();
                AuthEvent authEvent;
                // Parse Body
                nlohmann::json::parse(msg.c_str()).get_to(authEvent);

                this->delete_auth_handler(
                    supi, authEventId, authEvent, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDM_UE_AU_EVENTS) == 0) {
              if (request.method().compare("POST") == 0 && len > 0) {
                std::string supi = split_q[split_q.size() - 2].c_str();
                AuthEvent authEvent;
                // Parse Body
                nlohmann::json::parse(msg.c_str()).get_to(authEvent);

                this->confirm_auth_handler(supi, authEvent, response);
              }
            }
          } catch (std::exception& e) {
            Logger::udm_server().warn("Invalid request (error: %s)!", e.what());
            response.write_head(
                http_status_code_e::HTTP_STATUS_CODE_400_BAD_REQUEST);
            response.end();
            return;
          }
        });
      });

  server.handle(
      NUDM_SDM_BASE + udm_cfg.sbi.api_version,
      [&](const request& request, const response& response) {
        request.on_data([&](const uint8_t* data, std::size_t len) {
          std::string msg((char*) data, len);
          try {
            std::vector<std::string> split_q;
            boost::split(split_q, request.uri().path, boost::is_any_of("/"));
            // Access and Mobility Subscription Data Retrieval
            if (split_q[split_q.size() - 1].compare(NUDM_AM_DATA) == 0) {
              if (request.method().compare("GET") == 0 && len < 0) {
                std::string supi = split_q[split_q.size() - 2].c_str();
                PlmnId plmnId;
                // Parse URI
                std::string qs = request.uri().raw_query;
                Logger::udm_server().debug("QueryString: %s", qs.c_str());
                std::string plmn_id = util::get_query_param(qs, "plmn-id");
                nlohmann::json::parse(plmn_id.c_str()).get_to(plmnId);

                this->access_mobility_subscription_data_retrieval_handler(
                    supi, response, plmnId);
              }
            }
            // AMF registration for 3GPP access
            if (split_q[split_q.size() - 1].compare(NUDM_UECM_XGPP_ACCESS) ==
                0) {
              if (request.method().compare("PUT") == 0 && len > 0) {
                std::string ue_id = split_q[split_q.size() - 3].c_str();
                Amf3GppAccessRegistration amf_3gpp_access_registration;
                // Parse Body
                nlohmann::json::parse(msg.c_str())
                    .get_to(amf_3gpp_access_registration);

                this->amf_registration_for_3gpp_access_handler(
                    ue_id, amf_3gpp_access_registration, response);
              }
            }
            // Session Management Subscription Data Retrieval
            if (split_q[split_q.size() - 1].compare(NUDM_SM_DATA) == 0) {
              if (request.method().compare("GET") == 0 && len < 0) {
                std::string supi = split_q[split_q.size() - 2].c_str();
                PlmnId plmnId    = {};
                Snssai snssai    = {};
                // Parse URI
                std::string qs = request.uri().raw_query;
                Logger::udm_server().debug("QueryString: %s", qs.c_str());
                std::string supported_features =
                    util::get_query_param(qs, "supported-features");
                std::string plmn_id = util::get_query_param(qs, "plmn-id");
                nlohmann::json::parse(plmn_id.c_str()).get_to(plmnId);
                std::string single_nssai =
                    util::get_query_param(qs, "single-nssai");
                nlohmann::json::parse(single_nssai.c_str()).get_to(snssai);
                std::string dnn = util::get_query_param(qs, "dnn");

                this->session_management_subscription_data_retrieval_handler(
                    supi, response, snssai, dnn, plmnId);
              }
            }
            // Slice Selection Subscription Data Retrieval
            if (split_q[split_q.size() - 1].compare(NUDM_NSSAI) == 0) {
              if (request.method().compare("GET") == 0 && len < 0) {
                std::string supi = split_q[split_q.size() - 2].c_str();
                PlmnId plmnId;
                // Parse URI
                std::string qs = request.uri().raw_query;
                Logger::udm_server().debug("QueryString: %s", qs.c_str());
                std::string supported_features =
                    util::get_query_param(qs, "supported-features");
                std::string plmn_id = util::get_query_param(qs, "plmn-id");
                nlohmann::json::parse(plmn_id.c_str()).get_to(plmnId);

                this->slice_selection_subscription_data_retrieval_handler(
                    supi, response, supported_features, plmnId);
              }
            }
            // SMF Selection Subscription Data Retrieval
            if (split_q[split_q.size() - 1].compare(NUDM_SMF_SELECT) == 0) {
              if (request.method().compare("GET") == 0 && len < 0) {
                std::string supi = split_q[split_q.size() - 2].c_str();
                PlmnId plmnId;
                // Parse URI
                std::string qs = request.uri().raw_query;
                Logger::udm_server().debug("QueryString: %s", qs.c_str());
                std::string supported_features =
                    util::get_query_param(qs, "supported-features");
                std::string plmn_id = util::get_query_param(qs, "plmn-id");
                nlohmann::json::parse(plmn_id.c_str()).get_to(plmnId);

                this->smf_selection_subscription_data_retrieval_handler(
                    supi, response, supported_features, plmnId);
              }
            }
            // Subscription Creation
            if (split_q[split_q.size() - 1].compare(NUDM_SDM_SUB) == 0) {
              if (request.method().compare("POST") == 0 && len > 0) {
                SdmSubscription sdmSubscription;
                std::string supi = split_q[split_q.size() - 2].c_str();
                nlohmann::json::parse(msg.c_str()).get_to(sdmSubscription);

                this->subscription_creation_handler(
                    supi, sdmSubscription, response);
              }
            }
          } catch (std::exception& e) {
            Logger::udm_server().warn("Invalid request (error: %s)!", e.what());
            response.write_head(
                http_status_code_e::HTTP_STATUS_CODE_400_BAD_REQUEST);
            response.end();
            return;
          }
        });
      });

  if (server.listen_and_serve(ec, m_address, std::to_string(m_port))) {
    std::cerr << "HTTP Server error: " << ec.message() << std::endl;
  }
}
//------------------------------------------------------------------------------

void udm_http2_server::generate_auth_data_request_handler(
    const std::string& supiOrSuci,
    const oai::udm::model::AuthenticationInfoRequest& authenticationInfoRequest,
    const response& response) {
  Logger::udm_ueau().info("Handle generate_auth_data()");
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_generate_auth_data_request(
      supiOrSuci, authenticationInfoRequest, response_data, http_code);

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/json"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  Logger::udm_ueau().info("Send response to AUSF");
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());

  Logger::udm_ueau().info("Update sqn in Database");
}
//------------------------------------------------------------------------------

void udm_http2_server::confirm_auth_handler(
    const std::string& supi, const oai::udm::model::AuthEvent& authEvent,
    const response& response) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  std::string location;
  header_map h;

  m_udm_app->handle_confirm_auth(
      supi, authEvent, response_data, location, http_code);

  if (http_code == HTTP_RESPONSE_CODE_CREATED)
    h.emplace("location", header_value{location});

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/json"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  Logger::udm_ueau().info("Send response to AUSF");
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------

void udm_http2_server::delete_auth_handler(
    const std::string& supi, const std::string& authEventId,
    const oai::udm::model::AuthEvent& authEvent, const response& response) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_delete_auth(
      supi, authEventId, authEvent, response_data, http_code);

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------

void udm_http2_server::access_mobility_subscription_data_retrieval_handler(
    const std::string& supi, const response& response,
    oai::udm::model::PlmnId PlmnId) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_access_mobility_subscription_data_retrieval(
      supi, response_data, http_code, PlmnId);

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------

void udm_http2_server::amf_registration_for_3gpp_access_handler(
    const std::string& ue_id,
    const oai::udm::model::Amf3GppAccessRegistration&
        amf_3gpp_access_registration,
    const response& response) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_amf_registration_for_3gpp_access(
      ue_id, amf_3gpp_access_registration, response_data, http_code);

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------

void udm_http2_server::session_management_subscription_data_retrieval_handler(
    const std::string& supi, const response& response,
    oai::udm::model::Snssai snssai, std::string dnn,
    oai::udm::model::PlmnId plmnid) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_session_management_subscription_data_retrieval(
      supi, response_data, http_code, snssai, dnn, plmnid);
  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------

void udm_http2_server::slice_selection_subscription_data_retrieval_handler(
    const std::string& supi, const response& response,
    std::string supportedfeatures, oai::udm::model::PlmnId plmnid) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_slice_selection_subscription_data_retrieval(
      supi, response_data, http_code, supportedfeatures, plmnid);
  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udm_http2_server::smf_selection_subscription_data_retrieval_handler(
    const std::string& supi, const response& response,
    std::string supportedfeatures, oai::udm::model::PlmnId plmnid) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_smf_selection_subscription_data_retrieval(
      supi, response_data, http_code, supportedfeatures, plmnid);
  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udm_http2_server::subscription_creation_handler(
    const std::string& supi,
    const oai::udm::model::SdmSubscription& sdmSubscription,
    const response& response) {
  nlohmann::json response_data = {};
  long http_code               = 0;
  header_map h;

  m_udm_app->handle_subscription_creation(
      supi, sdmSubscription, response_data, http_code);

  // Set content type
  if ((http_code == HTTP_RESPONSE_CODE_CREATED) or
      (http_code == HTTP_RESPONSE_CODE_ACCEPTED) or
      (http_code == HTTP_RESPONSE_CODE_OK) or
      (http_code == HTTP_RESPONSE_CODE_NO_CONTENT)) {
    h.emplace("content-type", header_value{"application/problem"});
  } else {
    h.emplace("content-type", header_value{"application/problem+json"});
  }
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------
