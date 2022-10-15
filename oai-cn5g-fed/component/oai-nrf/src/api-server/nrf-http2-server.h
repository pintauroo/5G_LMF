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

/*! \file smf_http2-server.h
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_NRF_HTTP2_SERVER_SEEN
#define FILE_NRF_HTTP2_SERVER_SEEN

#include "conversions.hpp"

//#include "nrf.h"
#include "nrf_app.hpp"
#include "uint_generator.hpp"
#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using namespace oai::nrf::model;
using namespace oai::nrf::app;

class nrf_http2_server {
 public:
  nrf_http2_server(std::string addr, uint32_t port, nrf_app* nrf_app_inst)
      : m_address(addr), m_port(port), server(), m_nrf_app(nrf_app_inst) {}
  void start();
  void init(size_t thr) {}
  void register_nf_instance_handler(
      const NFProfile& NFProfiledata, const response& response);
  void deregister_nf_instance_handler(
      const std::string& nfInstanceID, const response& response);
  void get_nf_instance_handler(
      const std::string& nfInstanceID, const response& response);
  void get_nf_instances_handler(
      const std::string& nf_type, const std::string& limit_nfs,
      const response& response);
  void update_instance_handler(
      const std::string& nfInstanceID, const std::vector<PatchItem>& patchItem,
      const response& response);
  void create_subscription_handler(
      const SubscriptionData& subscriptionData, const response& response);
  void update_subscription_handler(
      const std::string& subscriptionID,
      const std::vector<PatchItem>& patchItem, const response& response);
  void remove_subscription_handler(
      const std::string& subscriptionID, const response& response);
  void search_nf_instances_handler(
      const std::string& target_nf_type, const std::string& requester_nf_type,
      const std::string& requester_nf_instance_id, const std::string& limit_nfs,
      const response& response);

  void access_token_request_handler(
      const SubscriptionData& subscriptionData, const response& response);
  void stop();

 private:
  util::uint_generator<uint32_t> m_promise_id_generator;
  std::string m_address;
  uint32_t m_port;
  http2 server;
  nrf_app* m_nrf_app;

 protected:
  static uint64_t generate_promise_id() {
    return util::uint_uid_generator<uint64_t>::get_instance().get_uid();
  }
};

#endif
