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

/*! \file amf_http2-server.h
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email: tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_AMF_HTTP2_SERVER_SEEN
#define FILE_AMF_HTTP2_SERVER_SEEN

#include "amf_app.hpp"
#include <nghttp2/asio_http2_server.h>
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include "uint_generator.hpp"
#include "amf.hpp"

#include "N1N2MessageTransferError.h"
#include "N1N2MessageTransferReqData.h"
#include "N1N2MessageTransferRspData.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using namespace oai::amf::model;

class amf_http2_server {
 public:
  amf_http2_server(
      std::string addr, uint32_t port, amf_application::amf_app* amf_app_inst)
      : m_address(addr), m_port(port), server(), m_amf_app(amf_app_inst) {}
  virtual ~amf_http2_server(){};
  void start();
  void init(size_t thr) {}

  void n1_n2_message_transfer_handler(
      const std::string& ueContextId,
      const N1N2MessageTransferReqData& n1N2MessageTransferReqData,
      const std::string& n1sm_str, const response& res,
      const std::string& n2sm_str = "");

  void stop();

 private:
  util::uint_generator<uint32_t> m_promise_id_generator;
  std::string m_address;
  uint32_t m_port;
  http2 server;
  amf_application::amf_app* m_amf_app;

 protected:
  static uint64_t generate_promise_id() {
    return util::uint_uid_generator<uint64_t>::get_instance().get_uid();
  }
};

#endif
