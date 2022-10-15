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

/*! \file ausf_http2-server.h
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email: tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_AUSF_HTTP2_SERVER_SEEN
#define FILE_AUSF_HTTP2_SERVER_SEEN

#include "conversions.hpp"

#include "ausf_app.hpp"
#include <nghttp2/asio_http2_server.h>
#include "ConfirmationData.h"
#include "AuthenticationInfo.h"
#include "DeregistrationInfo.h"
#include "EapSession.h"
#include "RgAuthenticationInfo.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
// using namespace oai::ausf_server::model;
using namespace oai::ausf::app;

class ausf_http2_server {
 public:
  ausf_http2_server(std::string addr, uint32_t port, ausf_app* ausf_app_inst)
      : m_address(addr), m_port(port), server(), m_ausf_app(ausf_app_inst) {}
  void start();
  void init(size_t thr) {}

  void eap_auth_method_handler(
      const std::string& authCtxId, const EapSession& eapSession,
      const response& response);
  void rg_authentications_post_handler(
      const RgAuthenticationInfo& rgAuthenticationInfo,
      const response& response);
  void ue_authentications_auth_ctx_id5g_aka_confirmation_put_handler(
      const std::string& authCtxId, const ConfirmationData& confirmationData,
      const response& response);
  void ue_authentications_deregister_post_handler(
      const DeregistrationInfo& deregistrationInfo, const response& response);
  void ue_authentications_post_handler(
      const AuthenticationInfo& authenticationInfo, const response& response);

  void stop();

 private:
  std::string m_address;
  uint32_t m_port;
  http2 server;
  ausf_app* m_ausf_app;
};

#endif
