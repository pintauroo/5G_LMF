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

/*! \file udr_http2-server.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: tien-thinh.nguyen@eurecom.fr
 */

#include "udr-http2-server.h"
#include "string.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

#include "3gpp_29.500.h"
#include "logger.hpp"
#include "udr.h"
#include "udr_config.hpp"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using namespace oai::udr::model;
using namespace oai::udr::config;

extern udr_config udr_cfg;

//------------------------------------------------------------------------------
void udr_http2_server::start() {
  boost::system::error_code ec;

  Logger::udr_server().info("HTTP2 server started ");

  server.handle(
      NUDR_DR_BASE + udr_cfg.nudr.api_version + "/",
      [&](const request &request, const response &response) {
        request.on_data([&](const uint8_t *data, std::size_t len) {
          std::string msg((char *)data, len);
          try {
            std::vector<std::string> split_q;
            boost::split(split_q, request.uri().path, boost::is_any_of("/"));
            if (split_q[split_q.size() - 1].compare(NUDR_DR_AUTH_SUBS) == 0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              if (request.method().compare("GET") == 0 && len == 0) {
                this->read_authentication_subscription_handler(ueId, response);
              }
              if (request.method().compare("PATCH") == 0 && len > 0) {
                std::vector<PatchItem> patchItem;
                // Parse Body
                nlohmann::json::parse(msg.c_str()).get_to(patchItem);
                this->modify_authentication_subscription_handler(
                    ueId, patchItem, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_AUTH_STATUS) == 0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_authentication_status_handler(ueId, response);
              }
              if (request.method().compare("PUT") == 0 && len > 0) {
                AuthEvent authEvent;
                nlohmann::json::parse(msg.c_str()).get_to(authEvent);
                this->create_authentication_status_handler(ueId, authEvent,
                                                           response);
              }
              if (request.method().compare("DELETE") == 0 && len < 0) {
                this->delete_authentication_status_handler(ueId, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_AMF_XGPP_ACCESS) ==
                0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              if (request.method().compare("PUT") == 0 && len > 0) {
                Amf3GppAccessRegistration amf3GppAccessRegistration;
                nlohmann::json::parse(msg.c_str())
                    .get_to(amf3GppAccessRegistration);
                this->create_amf_context_3gpp_handler(
                    ueId, amf3GppAccessRegistration, response);
              }
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_amf_context_3gpp_handler(ueId, response);
              }
              if (request.method().compare("PATCH") == 0 && len > 0) {
                // ToDo
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_AM_DATA) == 0) {
              if (request.method().compare("GET") == 0 && len == 0) {
                std::string ueId = split_q[split_q.size() - 3].c_str();
                std::string qs = request.uri().raw_query;
                Logger::udr_server().debug("QueryString: %s", qs.c_str());
                std::string servingPlmnId =
                    util::get_query_param(qs, "servingPlmnId");
                this->query_am_data_handler(ueId, servingPlmnId, response);
              }
            }
            if (split_q[split_q.size() - 2].compare(NUDR_DR_SDM_SUBS) == 0) {
              std::string ueId = split_q[split_q.size() - 4].c_str();
              std::string subsId = split_q[split_q.size() - 1].c_str();
              SdmSubscription sdmSubscription;
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_sdm_subscription_handler(ueId, subsId, response);
              }
              if (request.method().compare("PATCH") == 0 && len > 0) {
                nlohmann::json::parse(msg.c_str()).get_to(sdmSubscription);
                this->modify_sdm_subscription_handler(
                    ueId, subsId, sdmSubscription, response);
              }
              if (request.method().compare("DELETE") == 0 && len < 0) {
                this->remove_sdm_subscription_handler(ueId, subsId, response);
              }
              if (request.method().compare("PUT") == 0 && len > 0) {
                nlohmann::json::parse(msg.c_str()).get_to(sdmSubscription);
                this->update_sdm_subscription_handler(
                    ueId, subsId, sdmSubscription, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_SDM_SUBS) == 0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              if (request.method().compare("POST") == 0 && len > 0) {
                SdmSubscription sdmSubscription;
                nlohmann::json::parse(msg.c_str()).get_to(sdmSubscription);
                this->create_sdm_subscriptions_handler(ueId, sdmSubscription,
                                                       response);
              }
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_sdm_subscriptions_handler(ueId, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_SM_DATA) == 0) {
              if (request.method().compare("GET") == 0 && len == 0) {
                Snssai singleNssai;
                std::string ueId = split_q[split_q.size() - 4].c_str();
                std::string qs = request.uri().raw_query;
                Logger::udr_server().debug("QueryString: %s", qs.c_str());

                std::string servingPlmnId =
                    util::get_query_param(qs, "servingPlmnId");
                std::string dnn = util::get_query_param(qs, "dnn");
                std::string snssai = util::get_query_param(qs, "single-nssai");
                nlohmann::json::parse(snssai.c_str()).get_to(singleNssai);

                this->query_sm_data_handler(ueId, servingPlmnId, response,
                                            singleNssai, dnn);
              }
            }
            if (split_q[split_q.size() - 2].compare(NUDR_DR_SMF_REG) == 0) {
              std::string ueId = split_q[split_q.size() - 4].c_str();
              int32_t pduSessId = atoi(split_q[split_q.size() - 1].c_str());
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_smf_registration_handler(ueId, pduSessId, response);
              }
              if (request.method().compare("PUT") == 0 && len > 0) {
                SmfRegistration smfRegistration;
                nlohmann::json::parse(msg.c_str()).get_to(smfRegistration);
                this->create_smf_context_non_3gpp_handler(
                    ueId, pduSessId, smfRegistration, response);
              }
              if (request.method().compare("DELETE") == 0 && len < 0) {
                this->delete_smf_context_handler(ueId, pduSessId, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_SMF_REG) == 0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              int32_t pduSessId = atoi(split_q[split_q.size() - 1].c_str());
              if (request.method().compare("GET") == 0 && len == 0) {
                this->query_smf_reg_list_handler(ueId, response);
              }
            }
            if (split_q[split_q.size() - 1].compare(NUDR_DR_SMF_SELECT) == 0) {
              std::string ueId = split_q[split_q.size() - 3].c_str();
              int32_t pduSessId = atoi(split_q[split_q.size() - 1].c_str());
              if (request.method().compare("GET") == 0 && len == 0) {
                std::string qs = request.uri().raw_query;
                Logger::udr_server().debug("QueryString: %s", qs.c_str());

                std::string servingPlmnId =
                    util::get_query_param(qs, "servingPlmnId");
                this->query_smf_select_data_handler(ueId, servingPlmnId,
                                                    response);
              }
            }
          } catch (std::exception &e) {
            Logger::udr_server().warn("Invalid request (error: %s)!", e.what());
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
void udr_http2_server::query_am_data_handler(const std::string &ue_id,
                                             const std::string &serving_plmn_id,
                                             const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_query_am_data(ue_id, serving_plmn_id, response_data,
                                  http_code);
  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::create_amf_context_3gpp_handler(
    const std::string &ue_id,
    Amf3GppAccessRegistration &amf3GppAccessRegistration,
    const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_create_amf_context_3gpp(ue_id, amf3GppAccessRegistration,
                                            response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_amf_context_3gpp_handler(
    const std::string &ue_id, const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_query_amf_context_3gpp(ue_id, response_data, http_code);
  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::create_authentication_status_handler(
    const std::string &ue_id, const AuthEvent &authEvent,
    const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_create_authentication_status(ue_id, authEvent,
                                                 response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::delete_authentication_status_handler(
    const std::string &ue_id, const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_delete_authentication_status(ue_id, response_data,
                                                 http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_authentication_status_handler(
    const std::string &ue_id, const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_query_authentication_status(ue_id, response_data,
                                                http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::modify_authentication_subscription_handler(
    const std::string &ue_id, const std::vector<PatchItem> &patchItem,
    const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_modify_authentication_subscription(
      ue_id, patchItem, response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::read_authentication_subscription_handler(
    const std::string &ue_id, const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_read_authentication_subscription(ue_id, response_data,
                                                     http_code);

  Logger::udr_server().info("HTTP Response code %d (HTTP Version 2).\n",
                            http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_sdm_subscription_handler(
    const std::string &ue_id, const std::string &subs_id,
    const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_query_sdm_subscription(ue_id, subs_id, response_data,
                                           http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------
void udr_http2_server::remove_sdm_subscription_handler(
    const std::string &ue_id, const std::string &subs_id,
    const response &response) {
  nlohmann::json response_data = {};
  long http_code = 0;
  header_map h;

  m_udr_app->handle_remove_sdm_subscription(ue_id, subs_id, response_data,
                                            http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::modify_sdm_subscription_handler(
    const std::string &ue_id, const std::string &subs_id,
    SdmSubscription &sdmSubscription, const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  response.write_head(HTTP_STATUS_CODE_400_BAD_REQUEST, h);
  response.end("This API has not been implemented yet! (HTTP Version 2)\n");
}

//------------------------------------------------------------------------------
void udr_http2_server::update_sdm_subscription_handler(
    const std::string &ue_id, const std::string &subs_id,
    SdmSubscription &sdmSubscription, const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_update_sdm_subscription(ue_id, subs_id, sdmSubscription,
                                            response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::create_sdm_subscriptions_handler(
    const std::string &ue_id, SdmSubscription &sdmSubscription,
    const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_create_sdm_subscriptions(ue_id, sdmSubscription,
                                             response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_sdm_subscriptions_handler(
    const std::string &ue_id, const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_query_sdm_subscriptions(ue_id, response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_sm_data_handler(const std::string &ue_id,
                                             const std::string &serving_plmn_id,
                                             const response &response,
                                             oai::udr::model::Snssai snssai,
                                             std::string dnn) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_query_sm_data(ue_id, serving_plmn_id, response_data,
                                  http_code, snssai, dnn);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}
//------------------------------------------------------------------------------
void udr_http2_server::create_smf_context_non_3gpp_handler(
    const std::string &ue_id, const int32_t &pdu_session_id,
    const SmfRegistration &smfRegistration, const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_create_smf_context_non_3gpp(
      ue_id, pdu_session_id, smfRegistration, response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::delete_smf_context_handler(const std::string &ue_id,
                                                  const int32_t &pdu_session_id,
                                                  const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_delete_smf_context(ue_id, pdu_session_id, response_data,
                                       http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_smf_registration_handler(
    const std::string &ue_id, const int32_t &pdu_session_id,
    const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_query_smf_registration(ue_id, pdu_session_id, response_data,
                                           http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_smf_reg_list_handler(const std::string &ue_id,
                                                  const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_query_smf_reg_list(ue_id, response_data, http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_http2_server::query_smf_select_data_handler(
    const std::string &ue_id, const std::string &serving_plmn_id,
    const response &response) {
  nlohmann::json response_data = {};
  header_map h;
  long http_code = 0;

  m_udr_app->handle_query_smf_select_data(ue_id, serving_plmn_id, response_data,
                                          http_code);

  Logger::udr_server().debug("HTTP Response code %d (HTTP Version 2).\n",
                             http_code);
  response.write_head(http_code, h);
  response.end(response_data.dump().c_str());
}

//------------------------------------------------------------------------------