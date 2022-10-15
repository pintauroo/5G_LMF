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

/*! \file amf_app.hpp
 \brief
 \author  Keliang DU, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */

#ifndef _AMF_APP_H_
#define _AMF_APP_H_

#include <map>
#include <shared_mutex>
#include <string>

#include "amf_config.hpp"
#include "amf_module_from_config.hpp"
#include "amf_profile.hpp"
#include "itti.hpp"
#include "itti_msg_amf_app.hpp"
#include "ue_context.hpp"
#include "amf_subscription.hpp"
#include "itti_msg_sbi.hpp"
#include "amf_msg.hpp"
#include "ProblemDetails.h"

#include "uint_generator.hpp"
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

using namespace config;

static uint32_t amf_app_ue_ngap_id_generator = 1;

namespace amf_application {

#define TASK_AMF_APP_PERIODIC_STATISTICS (0)
#define TASK_AMF_MOBILE_REACHABLE_TIMER_EXPIRE (1)
#define TASK_AMF_IMPLICIT_DEREGISTRATION_TIMER_EXPIRE (2)

class amf_app {
 private:
  amf_profile nf_instance_profile;  // AMF profile
  std::string amf_instance_id;      // AMF instance id
  timer_id_t timer_nrf_heartbeat;

  util::uint_generator<uint32_t> evsub_id_generator;
  std::map<
      std::pair<evsub_id_t, amf_event_type_t>,
      std::shared_ptr<amf_subscription>>
      amf_event_subscriptions;

  mutable std::shared_mutex m_amf_event_subscriptions;

  util::uint_generator<uint32_t> tmsi_generator;

 public:
  explicit amf_app(const amf_config& amf_cfg);
  amf_app(amf_app const&) = delete;
  void operator=(amf_app const&) = delete;
  void allRegistredModulesInit(const amf_modules& modules);
  long generate_amf_ue_ngap_id();
  // itti handlers
  void handle_itti_message(itti_nas_signalling_establishment_request& itti_msg);
  void handle_itti_message(itti_n1n2_message_transfer_request& itti_msg);

  bool is_amf_ue_id_2_ue_context(const long& amf_ue_ngap_id) const;
  std::shared_ptr<ue_context> amf_ue_id_2_ue_context(
      const long& amf_ue_ngap_id) const;
  void set_amf_ue_ngap_id_2_ue_context(
      const long& amf_ue_ngap_id, std::shared_ptr<ue_context> uc);

  bool is_ran_amf_id_2_ue_context(const std::string& ue_context_key) const;
  std::shared_ptr<ue_context> ran_amf_id_2_ue_context(
      const std::string& ue_context_key) const;

  bool ran_amf_id_2_ue_context(
      const std::string& ue_context_key, std::shared_ptr<ue_context>& uc) const;

  void set_ran_amf_id_2_ue_context(
      const std::string& ue_context_key, std::shared_ptr<ue_context> uc);

  bool is_supi_2_ue_context(const string& supi) const;
  std::shared_ptr<ue_context> supi_2_ue_context(const string& supi) const;
  void set_supi_2_ue_context(
      const string& ue_context_key, std::shared_ptr<ue_context>& uc);

  bool find_pdu_session_context(
      const string& supi, const std::uint8_t pdu_session_id,
      std::shared_ptr<pdu_session_context>& psc);

  bool get_pdu_sessions_context(
      const string& supi,
      std::vector<std::shared_ptr<pdu_session_context>>& sessions_ctx);
  // SMF Client response handlers
  void handle_post_sm_context_response_error_400();
  // others
  uint32_t generate_tmsi();
  bool generate_5g_guti(
      uint32_t ranid, long amfid, std::string& mcc, std::string& mnc,
      uint32_t& tmsi);

  /*
   * Generate an Event Exposure Subscription ID
   * @param [void]
   * @return the generated reference
   */
  evsub_id_t generate_ev_subscription_id();

  /*
   * Trigger NF instance registration to NRF
   * @param [void]
   * @return void
   */
  void register_to_nrf();

  /*
   * Handle Event Exposure Msg from NF
   * @param [std::shared_ptr<itti_sbi_event_exposure_request>&] Request message
   * @return [evsub_id_t] ID of the created subscription
   */
  evsub_id_t handle_event_exposure_subscription(
      std::shared_ptr<itti_sbi_event_exposure_request> msg);

  /*
   * Handle Unsubscribe Request from an NF
   * @param [const std::string&] subscription_id: Subscription ID
   * @return true if the subscription is unsubscribed successfully, otherwise
   * return false
   */
  bool handle_event_exposure_delete(const std::string& subscription_id);

  /*
   * Handle NF status notification (e.g., when an UPF becomes available)
   * @param [std::shared_ptr<itti_sbi_notification_data>& ] msg: message
   * @param [oai::amf::model::ProblemDetails& ] problem_details
   * @param [uint8_t&] http_code
   * @return true if handle sucessfully, otherwise return false
   */
  bool handle_nf_status_notification(
      std::shared_ptr<itti_sbi_notification_data>& msg,
      oai::amf::model::ProblemDetails& problem_details, uint8_t& http_code);

  /*
   * Generate a random UUID for SMF instance
   * @param [void]
   * @return void
   */
  void generate_uuid();

  /*
   * Add an Event Subscription to the list
   * @param [const evsub_id_t&] sub_id: Subscription ID
   * @param [amf_event_t] ev: Event type
   * @param [std::shared_ptr<amf_subscription>] ss: a shared pointer stored
   * information of the subscription
   * @return void
   */
  void add_event_subscription(
      evsub_id_t sub_id, amf_event_type_t ev,
      std::shared_ptr<amf_subscription> ss);

  /*
   * Remove an Event Subscription from the list
   * @param [const evsub_id_t&] sub_id: Subscription ID
   * @return bool
   */
  bool remove_event_subscription(evsub_id_t sub_id);

  /*
   * Get a list of subscription associated with a particular event
   * @param [amf_event_t] ev: Event type
   * @param [std::vector<std::shared_ptr<amf_subscription>>&] subscriptions:
   * store the list of the subscription associated with this event type
   * @return void
   */
  void get_ee_subscriptions(
      amf_event_type_t ev,
      std::vector<std::shared_ptr<amf_subscription>>& subscriptions);

  /*
   * Get a list of subscription associated with a particular event
   * @param [evsub_id_t] sub_id: Subscription ID
   * @param [std::vector<std::shared_ptr<amf_subscription>>&] subscriptions:
   * store the list of the subscription associated with this event type
   * @return void
   */
  void get_ee_subscriptions(
      evsub_id_t sub_id,
      std::vector<std::shared_ptr<amf_subscription>>& subscriptions);

  /*
   * Get a list of subscription associated with a particular event
   * @param [amf_event_t] ev: Event type
   * @param [std::string&] supi: SUPI
   * @param [std::vector<std::shared_ptr<amf_subscription>>&] subscriptions:
   * store the list of the subscription associated with this event type
   * @return void
   */
  void get_ee_subscriptions(
      amf_event_type_t ev, std::string& supi,
      std::vector<std::shared_ptr<amf_subscription>>& subscriptions);

  /*
   * Generate a SMF profile for this instance
   * @param [void]
   * @return void
   */
  void generate_amf_profile();

  /*
   * Send request to N11 task to trigger NF instance registration to NRF
   * @param [void]
   * @return void
   */
  void trigger_nf_registration_request();

  /*
   * Send request to N11 task to trigger NF instance deregistration to NRF
   * @param [void]
   * @return void
   */
  void trigger_nf_deregistration();

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
   * Generate an unique value for promise id
   * @param void
   * @return generated promise id
   */
  static uint64_t generate_promise_id() {
    return util::uint_uid_generator<uint64_t>::get_instance().get_uid();
  }

  void trigger_process_response(uint32_t pid, uint32_t http_code);

  void add_promise(
      uint32_t pid, boost::shared_ptr<boost::promise<std::string>>& p);
  void trigger_process_response(uint32_t pid, std::string n2_sm);

 private:
  std::map<long, std::shared_ptr<ue_context>> amf_ue_ngap_id2ue_ctx;
  mutable std::shared_mutex m_amf_ue_ngap_id2ue_ctx;
  std::map<std::string, std::shared_ptr<ue_context>> ue_ctx_key;
  mutable std::shared_mutex m_ue_ctx_key;

  std::map<std::string, std::shared_ptr<ue_context>> supi2ue_ctx;
  mutable std::shared_mutex m_supi2ue_ctx;

  mutable std::shared_mutex m_curl_handle_responses_n2_sm;
  std::map<uint32_t, boost::shared_ptr<boost::promise<std::string>>>
      curl_handle_responses_n2_sm;
};

}  // namespace amf_application

#endif
