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

/*! \file udm_http2-server.h
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email: tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_UDM_HTTP2_SERVER_SEEN
#define FILE_UDM_HTTP2_SERVER_SEEN

#include "conversions.hpp"

#include "udm_app.hpp"
#include "udm.h"
#include <nghttp2/asio_http2_server.h>
#include "logger.hpp"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

using namespace oai::udm::app;
using namespace oai::udm::config;
using namespace oai::udm::model;

class udm_http2_server {
 public:
  udm_http2_server(std::string addr, uint32_t port, udm_app* udm_app_inst)
      : m_address(addr), m_port(port), server(), m_udm_app(udm_app_inst) {}
  void start();
  void init(size_t thr) {}

  void generate_auth_data_request_handler(
      const std::string& supiOrSuci,
      const oai::udm::model::AuthenticationInfoRequest&
          authenticationInfoRequest,
      const response& response);

  void confirm_auth_handler(
      const std::string& supi, const oai::udm::model::AuthEvent& authEvent,
      const response& response);

  void delete_auth_handler(
      const std::string& supi, const std::string& authEventId,
      const oai::udm::model::AuthEvent& authEvent, const response& response);

  void access_mobility_subscription_data_retrieval_handler(
      const std::string& supi, const response& response,
      oai::udm::model::PlmnId PlmnId = {});

  void amf_registration_for_3gpp_access_handler(
      const std::string& ue_id,
      const oai::udm::model::Amf3GppAccessRegistration&
          amf_3gpp_access_registration,
      const response& response);

  void session_management_subscription_data_retrieval_handler(
      const std::string& supi, const response& response,
      oai::udm::model::Snssai snssai = {}, std::string dnn = {},
      oai::udm::model::PlmnId PlmnId = {});

  void slice_selection_subscription_data_retrieval_handler(
      const std::string& supi, const response& response,
      std::string supported_features = {}, oai::udm::model::PlmnId PlmnId = {});

  void smf_selection_subscription_data_retrieval_handler(
      const std::string& supi, const response& response,
      std::string supported_features = {}, oai::udm::model::PlmnId PlmnId = {});

  void subscription_creation_handler(
      const std::string& supi,
      const oai::udm::model::SdmSubscription& sdmSubscription,
      const response& response);

  void stop();

 private:
  std::string m_address;
  uint32_t m_port;
  http2 server;
  udm_app* m_udm_app;
};

#endif
