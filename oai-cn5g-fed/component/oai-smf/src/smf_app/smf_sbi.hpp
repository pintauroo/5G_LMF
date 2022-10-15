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

/*! \file smf_sbi.hpp
 \author  Lionel GAUTHIER, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2019
 \email: lionel.gauthier@eurecom.fr, tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_SMF_SBI_HPP_SEEN
#define FILE_SMF_SBI_HPP_SEEN

#include <map>
#include <thread>

#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <curl/curl.h>
#include "3gpp_29.503.h"
#include "smf.h"
#include "smf_context.hpp"

namespace smf {

#define TASK_SMF_SBI_TIMEOUT_NRF_HEARTBEAT_REQUEST 1

class smf_sbi {
 private:
  CURLM* curl_multi;
  std::vector<CURL*> handles;
  struct curl_slist* headers;

  mutable std::shared_mutex m_curl_handle_promises;

  std::map<uint32_t, boost::shared_ptr<boost::promise<uint32_t>>>
      curl_handle_promises;

  std::thread::id thread_id;
  std::thread thread;

 public:
  smf_sbi();
  virtual ~smf_sbi();
  smf_sbi(smf_sbi const&) = delete;
  void operator=(smf_sbi const&) = delete;

  /*
   * Send N1N2 Message Transfer Request to AMF
   * @param [std::shared_ptr<itti_n11_create_sm_context_response>]
   * sm_context_res: Content of message to be sent
   * @return void
   */
  void send_n1n2_message_transfer_request(
      std::shared_ptr<itti_n11_create_sm_context_response> sm_context_res);

  /*
   * Send N1N2 Message Transfer Request to AMF
   * @param [std::shared_ptr<itti_nx_trigger_pdu_session_modification>]
   * sm_session_modification: Content of message to be sent
   * @return void
   */
  void send_n1n2_message_transfer_request(
      std::shared_ptr<itti_nx_trigger_pdu_session_modification>
          sm_session_modification);

  /*
   * Send N1N2 Message Transfer Request to AMF
   * @param [std::shared_ptr<itti_n11_session_report_request>] n11_msg: Content
   * of message to be sent
   * @return void
   */
  void send_n1n2_message_transfer_request(
      std::shared_ptr<itti_n11_session_report_request> report_msg);

  /*
   * Send SM Context Status Notification to AMF
   * @param [std::shared_ptr<itti_n11_notify_sm_context_status>]
   * sm_context_status: Content of message to be sent
   * @return void
   */
  void send_sm_context_status_notification(
      std::shared_ptr<itti_n11_notify_sm_context_status> sm_context_status);

  /*
   * Send Notification for the associated event to the subscribers
   * @param [std::shared_ptr<itti_n11_notify_subscribed_event>] msg: Content of
   * message to be sent
   * @return void
   */
  void notify_subscribed_event(
      std::shared_ptr<itti_n11_notify_subscribed_event> msg);

  /*
   * Send NF instance registration to NRF
   * @param [std::shared_ptr<itti_n11_register_nf_instance_request>] msg:
   * Content of message to be sent
   * @return void
   */
  void register_nf_instance(
      std::shared_ptr<itti_n11_register_nf_instance_request> msg);

  /*
   * Send NF instance update to NRF
   * @param [std::shared_ptr<itti_n11_update_nf_instance_request>] msg: Content
   * of message to be sent
   * @return void
   */
  void update_nf_instance(
      std::shared_ptr<itti_n11_update_nf_instance_request> msg);

  /*
   * Send NF deregister to NRF
   * @param [std::shared_ptr<itti_n11_deregister_nf_instance>] msg: Content
   * of message to be sent
   * @return void
   */
  void deregister_nf_instance(
      std::shared_ptr<itti_n11_deregister_nf_instance> msg);

  /*
   * Send a NFStatusSubscribe to NRF (to be notified when a new UPF becomes
   * available)
   * @param [std::shared_ptr<itti_n11_subscribe_upf_status_notify>] msg: Content
   * of message to be sent
   * @return void
   */
  void subscribe_upf_status_notify(
      std::shared_ptr<itti_n11_subscribe_upf_status_notify> msg);

  /*
   * Get SM subscription data from UDM
   * @param [const supi64_t &] supi
   * @param [const std::string &] dnn
   * @param [const snssai_t &] snssai
   * @param [std::shared_ptr<session_management_subscription>] subscription
   * @return bool: True if successful, otherwise false
   *
   */
  bool get_sm_data(
      const supi64_t& supi, const std::string& dnn, const snssai_t& snssai,
      std::shared_ptr<session_management_subscription>& subscription,
      plmn_t plmn = {});

  /*
   * Subscribe to be notify from UDM
   * @param []
   * @return void
   *
   */
  void subscribe_sm_data();

  /*
   * Create Curl handle for multi curl
   * @param [const std::string &] uri: URI of the subscribed NF
   * @param [const char* ] data: pointer to the data to be sent
   * @param [uint32_t] data_len: len of data to be sent
   * @param [std::string &] response_data: response data
   * @param [uint32_t* ] promise_id: pointer to the promise id
   * @param [const std::string&] method: HTTP method
   * @param [bool] is_multipart: use multipart or json format
   * @return true if a handle was created successfully, otherwise return false
   */
  bool curl_create_handle(
      const std::string& uri, const char* data, uint32_t data_len,
      std::string& response_data, uint32_t* promise_id,
      const std::string& method, bool is_multipart, uint8_t http_version = 1);

  /*
   * Create Curl handle for multi curl
   * @param [const std::string &] uri: URI of the subscribed NF
   * @param [const std::string& ] data: data to be sent
   * @param [std::string &] response_data: response data
   * @param [uint32_t* ] promise_id: pointer to the promise id
   * @param [const std::string&] method: HTTP method
   * @param [bool] is_multipart: use multipart or json format
   * @return true if a handle was created successfully, otherwise return false
   */
  bool curl_create_handle(
      const std::string& uri, const std::string& data,
      std::string& response_data, uint32_t* promise_id,
      const std::string& method, uint8_t http_version = 1);

  /*
   * Create Curl handle for multi curl
   * @param [const std::string &] uri: URI of the subscribed NF
   * @param [std::string &] response_data: response data
   * @param [uint32_t* ] promise_id: pointer to the promise id
   * @param [const std::string&] method: HTTP method
   * @return true if a handle was created successfully, otherwise return false
   */
  bool curl_create_handle(
      const std::string& uri, std::string& response_data, uint32_t* promise_id,
      const std::string& method, uint8_t http_version = 1);

  /*
   * Perform curl multi to actually process the available data
   * @param [uint64_t ms] ms: current time
   * @return void
   */
  void perform_curl_multi(uint64_t ms);

  /*
   * Release all the handles
   * @param void
   * @return void
   */
  void curl_release_handles();

  /*
   * Wait for the promise ready
   * @param [boost::shared_future<uint32_t>&] f: future
   * @return future value
   */
  uint32_t get_available_response(boost::shared_future<uint32_t>& f);

  /*
   * Store the promise
   * @param [uint32_t] pid: promise id
   * @param [boost::shared_ptr<boost::promise<uint32_t>>&] p: promise
   * @return void
   */
  void add_promise(
      uint32_t pid, boost::shared_ptr<boost::promise<uint32_t>>& p);

  /*
   * Remove the promise
   * @param [uint32_t] pid: promise id
   * @return void
   */
  void remove_promise(uint32_t id);

  /*
   * Set the value of the promise to make it ready
   * @param [uint32_t] pid: promise id
   * @param [uint32_t ] http_code: http response code
   * @return void
   */
  void trigger_process_response(uint32_t pid, uint32_t http_code);

  /*
   * Generate an unique value for promise id
   * @param void
   * @return generated promise id
   */
  static uint64_t generate_promise_id() {
    return util::uint_uid_generator<uint64_t>::get_instance().get_uid();
  }
};
}  // namespace smf
#endif /* FILE_SMF_SBI_HPP_SEEN */
