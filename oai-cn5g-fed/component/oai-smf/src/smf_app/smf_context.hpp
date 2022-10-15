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

/*! \file smf_context.hpp
 \brief
 \author  Lionel GAUTHIER, Tien-Thinh NGUYEN
 \company Eurecom
 \date 2019
 \email: lionel.gauthier@eurecom.fr, tien-thinh.nguyen@eurecom.fr
 */

#ifndef FILE_SMF_CONTEXT_HPP_SEEN
#define FILE_SMF_CONTEXT_HPP_SEEN

#include <map>
#include <memory>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "3gpp_24.008.h"
#include "3gpp_29.244.h"
#include "3gpp_29.502.h"
#include "3gpp_29.503.h"
#include "common_root_types.h"
#include "itti.hpp"
#include "msg_pfcp.hpp"
#include "smf_event.hpp"
#include "smf_procedure.hpp"
#include "uint_generator.hpp"

extern "C" {
#include "Ngap_PDUSessionAggregateMaximumBitRate.h"
#include "PDUSessionEstablishmentAccept.h"
#include "QOSFlowDescriptions.h"
#include "QOSRules.h"
#include "nas_message.h"
}

namespace smf {

class smf_qos_flow {
 public:
  smf_qos_flow() { clear(); }

  void clear() {
    ul_fteid    = {};
    dl_fteid    = {};
    pdr_id_ul   = {};
    pdr_id_dl   = {};
    precedence  = {};
    far_id_ul   = {};
    far_id_dl   = {};
    released    = false;
    qos_profile = {};
    cause_value = 0;
  }

  /*
   * Release resources associated with this flow
   * @param void
   * @return void
   */
  void deallocate_ressources();

  /*
   * Mark this flow as released
   * @param void
   * @return void
   */
  void mark_as_released();

  /*
   * Represent flow as string to be printed
   * @param void
   * @return void
   */
  std::string toString() const;

  pfcp::qfi_t qfi;           // QoS Flow Identifier
  pfcp::fteid_t ul_fteid;    // fteid of UPF
  pfcp::fteid_t dl_fteid;    // fteid of AN
  pfcp::pdr_id_t pdr_id_ul;  // Packet Detection Rule ID, UL
  pfcp::pdr_id_t pdr_id_dl;  // Packet Detection Rule ID, DL
  pfcp::precedence_t precedence;
  std::pair<bool, pfcp::far_id_t> far_id_ul;  // FAR ID, UL
  std::pair<bool, pfcp::far_id_t> far_id_dl;  // FAR ID, DL
  bool released;  // finally seems necessary, TODO try to find heuristic ?
  pdu_session_id_t pdu_session_id;
  qos_profile_t qos_profile;  // QoS profile
  uint8_t cause_value;        // cause
};

class smf_pdu_session : public std::enable_shared_from_this<smf_pdu_session> {
 public:
  smf_pdu_session() : m_pdu_session_mutex() { clear(); }

  smf_pdu_session(pdu_session_id_t psi)
      : pdu_session_id(psi), m_pdu_session_mutex() {
    ipv4                = false;
    ipv6                = false;
    ipv4_address.s_addr = INADDR_ANY;
    ipv6_address        = in6addr_any;
    released            = false;
    dnn                 = {};
    snssai              = {};
    pdu_session_type    = {};
    seid                = 0;
    up_fseid            = {};
    qos_flows.clear();
    default_qfi.qfi    = NO_QOS_FLOW_IDENTIFIER_ASSIGNED;
    pdu_session_status = pdu_session_status_e::PDU_SESSION_INACTIVE;
    upCnx_state        = upCnx_state_e::UPCNX_STATE_DEACTIVATED;
    ho_state           = ho_state_e::HO_STATE_NONE;
    timer_T3590        = ITTI_INVALID_TIMER_ID;
    timer_T3591        = ITTI_INVALID_TIMER_ID;
    timer_T3592        = ITTI_INVALID_TIMER_ID;
    number_of_supported_packet_filters         = 0;
    maximum_number_of_supported_packet_filters = 0;
    number_retransmission_T3591                = 0;
    number_retransmission_T3592                = 0;
  }

  void clear() {
    ipv4                = false;
    ipv6                = false;
    ipv4_address.s_addr = INADDR_ANY;
    ipv6_address        = in6addr_any;
    released            = false;
    pdu_session_id      = 0;
    dnn                 = {};
    snssai              = {};
    pdu_session_type    = {};
    seid                = 0;
    up_fseid            = {};
    qos_flows.clear();
    default_qfi.qfi             = NO_QOS_FLOW_IDENTIFIER_ASSIGNED;
    pdu_session_status          = pdu_session_status_e::PDU_SESSION_INACTIVE;
    upCnx_state                 = upCnx_state_e::UPCNX_STATE_DEACTIVATED;
    ho_state                    = ho_state_e::HO_STATE_NONE;
    timer_T3590                 = ITTI_INVALID_TIMER_ID;
    timer_T3591                 = ITTI_INVALID_TIMER_ID;
    timer_T3592                 = ITTI_INVALID_TIMER_ID;
    number_retransmission_T3591 = 0;
    number_retransmission_T3592 = 0;
  }

  smf_pdu_session(smf_pdu_session& b) = delete;

  void get_pdu_session_id(uint32_t& psi) const;
  uint32_t get_pdu_session_id() const;

  /*
   * Set UE Address for this session
   * @param [paa_t &] paa: PAA
   * @return void
   */
  void set(const paa_t& paa);

  /*
   * Get UE Address of this session
   * @param [paa_t &] paa: PAA
   * @return void
   */
  void get_paa(paa_t& paa);

  /*
   * Find a QoS flow by a PDR ID
   * @param [const pfcp::pdr_id_t &] pdr_id: PDR ID
   * @param [const smf_qos_flow &] flow: flow to be assigned
   * @return bool: Return True if found, otherwise return false
   */
  bool get_qos_flow(const pfcp::pdr_id_t& pdr_id, smf_qos_flow& flow);

  /*
   * Find a QoS flow by a FAR
   * @param [const pfcp::far_id_t &] far_id: FAR ID
   * @param [const smf_qos_flow &] flow: flow to be assigned if found
   * @return bool: Return True if found, otherwise return false
   */
  bool get_qos_flow(const pfcp::far_id_t& far_id, smf_qos_flow& flow);

  /*
   * Find a QoS flow by its QFI
   * @param [const pfcp::qfi_t &] QFI
   * @param [const smf_qos_flow &] flow: flow to be assigned
   * @return bool: Return True if found, otherwise return false
   */
  bool get_qos_flow(const pfcp::qfi_t& qfi, smf_qos_flow& flow);

  /*
   * Add a QoS flow to a PDU Session
   * @param [const smf_qos_flow &] flow: Flow to be added
   * @return void
   */
  void add_qos_flow(const smf_qos_flow& flow);

  /*
   * Get all QoS Flows associated with this PDU Session
   * @param [std::vector<smf_qos_flow> &] flows: list of Flows associated with
   * this session
   * @return void
   */
  void get_qos_flows(std::vector<smf_qos_flow>& flows);

  /*
   * Set a default flow for this PDU Session
   * @param [const pfcp::qfi_t &] qfi: Default QFI
   * @return void
   */
  void set_default_qos_flow(const pfcp::qfi_t& qfi);

  /*
   * Get the default QoS flow of this PDU Session
   * @param [smf_qos_flow &] flow: Default QoS flow
   * @return bool: Return true if the default QoS flow exist
   */
  bool get_default_qos_flow(smf_qos_flow& flow);

  /*
   * Find a QoS flow by its PDR ID
   * @param [const pfcp::pdr_id_t &] pdr_id: PDR ID
   * @param [smf_qos_flow &] flow: Flow to be returned if found
   * @return bool: return true if a flow is found, otherwise false
   */
  bool find_qos_flow(const pfcp::pdr_id_t& pdr_id, smf_qos_flow& flow);

  /*
   * Remove a QoS flow identified by its QFI
   * @param [const pfcp::qfi_t &] qfi: QFI of the flow to be removed
   * @return void
   */
  void remove_qos_flow(const pfcp::qfi_t& qfi);

  /*
   * Remove a QoS flow
   * @param [smf_qos_flow &] flow: flow to be removed
   * @return void
   */
  void remove_qos_flow(smf_qos_flow& flow);

  /*
   * Remove all QoS flow associated with this PDU Session
   * @return void
   */
  void remove_qos_flows();

  /*
   * Set current status of PDU Session
   * @param [const pdu_session_status_e &] status: status to be set
   * @return void
   */
  void set_pdu_session_status(const pdu_session_status_e& status);

  /*
   * Get current status of PDU Session
   * @param void
   * @return pdu_session_status_e: status of PDU session
   */
  pdu_session_status_e get_pdu_session_status() const;

  /*
   * Set upCnxState for a N3 Tunnel
   * @param [upCnx_state_e&] state: new state of the N3 tunnel
   * @return void
   */
  void set_upCnx_state(const upCnx_state_e& state);

  /*
   * Get upCnxState of a N3 Tunnel
   * @param void
   * @return upCnx_state_e: current state of this N3 tunnel
   */
  upCnx_state_e get_upCnx_state() const;

  /*
   * Set HOState of a PDU Session
   * @param [ho_state_e&] state: new HO State for the PDU Session
   * @return void
   */
  void set_ho_state(const ho_state_e& state);

  /*
   * Get HOState of a PDU Session
   * @param void
   * @return ho_state_e: current state of this PDU Session
   */
  ho_state_e get_ho_state() const;

  // deallocate_ressources is for releasing related-resources prior to the
  // deletion of objects since shared_ptr is actually heavy used for handling
  // objects, deletion of object instances cannot be always guaranteed when
  // removing them from a collection, so that is why actually the deallocation
  // of resources is not done in the destructor of objects.
  void deallocate_ressources(const std::string& dnn);

  /*
   * Represent PDU Session as string to be printed
   * @param void
   * @return void
   */
  std::string toString() const;

  /*
   * Generate a SEID to be used to communicate with UPF
   * @param void
   * @return void
   */
  void generate_seid();

  /*
   * Set a value to SEID
   * @param [const uint64_t &] seid: value to be set
   * @return void
   */
  void set_seid(const uint64_t& seid);

  /*
   * Generate a PDR ID
   * @param [pfcp::pdr_id_t &]: pdr_id: PDR ID generated
   * @return void
   */
  void generate_pdr_id(pfcp::pdr_id_t& pdr_id);

  /*
   * Release a PDR ID
   * @param [const pfcp::pdr_id_t &]: pdr_id: PDR ID to be released
   * @return void
   */
  void release_pdr_id(const pfcp::pdr_id_t& pdr_id);
  /*
   * Generate a URR ID
   * @param [pfcp::urr_id_t &]: far_id: URR ID generated
   * @return void
   */
  void generate_urr_id(pfcp::urr_id_t& urr_id);

  /*
   * Release a URR ID
   * @param [const pfcp::urr_id_t &]: far_id: URR ID to be released
   * @return void
   */
  void release_urr_id(const pfcp::urr_id_t& urr_id);
  /*
   * Generate a FAR ID
   * @param [pfcp::far_id_t &]: far_id: FAR ID generated
   * @return void
   */
  void generate_far_id(pfcp::far_id_t& far_id);

  /*
   * Release a FAR ID
   * @param [const pfcp::far_id_t &]: far_id: FAR ID to be released
   * @return void
   */
  void release_far_id(const pfcp::far_id_t& far_id);

  /*
   * Generate a QoS Rule ID
   * @param [uint8_t &]: rule_id: QoS Rule ID generated
   * @return void
   */
  void generate_qos_rule_id(uint8_t& rule_id);

  /*
   * Release a QoS Rule ID
   * @param [uint8_t &]: rule_id: QoS Rule ID to be released
   * @return void
   */
  void release_qos_rule_id(const uint8_t& rule_id);

  /*
   * Mark a QoS Rule to be synchronised with UE
   * @param [const uint8_t ]: rule_id: QoS Rule ID to be synchronised with UE
   * @return void
   */
  void mark_qos_rule_to_be_synchronised(const uint8_t rule_id);

  /*
   * Get all QoS Rules to be synchronised with UE
   * @param [std::vector<QOSRulesIE> &]: qos_rules: List of QoS Rules to be
   * synchronised with UE
   * @return void
   */
  void get_qos_rules_to_be_synchronised(
      std::vector<QOSRulesIE>& qos_rules) const;

  /*
   * Get list of QoS rules associated with a given QFI
   * @param [pfcp::qfi_t &] qfi
   * @param [std::vector<QOSRulesIE> &] rules
   * @void
   */
  void get_qos_rules(
      const pfcp::qfi_t& qfi, std::vector<QOSRulesIE>& rules) const;

  /*
   * Get default QoS Rule associated with this PDU Session
   * @param [QOSRulesIE &] qos_rule
   * @void
   */
  bool get_default_qos_rule(QOSRulesIE& qos_rule) const;

  /*
   * Get QoS Rule with Rule ID
   * @param [uint8_t] rule_id: Rule ID
   * @param [QOSRulesIE &] qos_rule
   * @return bool: Return true if Rule exist, otherwise return false
   */
  bool get_qos_rule(const uint8_t rule_id, QOSRulesIE& qos_rule) const;

  /*
   * Update QoS Rule
   * @param [QOSRulesIE &] qos_rule
   * @void
   */
  void update_qos_rule(const QOSRulesIE& qos_rule);

  /*
   * Add a QoS Rule
   * @param [QOSRulesIE &] qos_rule
   * @void
   */
  void add_qos_rule(const QOSRulesIE& qos_rule);

  /*
   * Get PDN Type of this PDU session
   * @param void
   * @return pdu_session_type_t: PDN Type
   */
  pdu_session_type_t get_pdu_session_type() const;

  /*
   * Get NWI associated with this PDU Session
   * @param void
   * @return std::string: NWI
   */
  std::string get_nwi_access() const;

  /*
   * Set DNN associated with this PDU Session
   * @param [const std::string&] nwi: NWI
   * @return void
   */
  void set_nwi_access(const std::string& nwiAccess);

  /*
   * Get NWI associated with this PDU Session
   * @param void
   * @return std::string: NWI
   */
  std::string get_nwi_core() const;

  /*
   * Set DNN associated with this PDU Session
   * @param [const std::string&] nwi: NWI
   * @return void
   */
  void set_nwi_core(const std::string& nwiCore);

  /*
   * Set UPF Node ID of this PDU Session
   * @param [const pfcp::node_id_t&] node_id: UPF Node Id
   * @return void
   */
  void set_upf_node_id(const pfcp::node_id_t& node_id);

  /*
   * Get UPF Node ID of this PDU Session
   * @param [pfcp::node_id_t&] node_id: UPF Node Id
   * @return void
   */
  void get_upf_node_id(pfcp::node_id_t& node_id) const;

  /*
   * Get UPF Node ID of this PDU Session
   * @param void
   * @return UPF Node Id
   */
  pfcp::node_id_t get_upf_node_id() const;

  /*
   * Get DNN associated with this PDU Session
   * @param void
   * @return std::string: DNN
   */
  std::string get_dnn() const;

  /*
   * Set DNN associated with this PDU Session
   * @param [const std::string&] d: DNN
   * @return void
   */
  void set_dnn(const std::string& d);

  /*
   * Get SNSSAI associated with this PDU Session
   * @param void
   * @return snssai_t: SNSSAI
   */
  snssai_t get_snssai() const;

  /*
   * Set SNSSAI associated with this PDU Session
   * @param [const snssai_t&] s: SNSSAI
   * @return void
   */
  void set_snssai(const snssai_t s);

  void set_pending_n11_msg(const std::shared_ptr<itti_n11_msg>& msg);
  void get_pending_n11_msg(std::shared_ptr<itti_n11_msg>& msg) const;
  void set_number_retransmission_T3591(const uint8_t& n);
  void get_number_retransmission_T3591(uint8_t& n) const;
  uint8_t get_number_retransmission_T3591() const;

  void set_number_retransmission_T3592(const uint8_t& n);
  void get_number_retransmission_T3592(uint8_t& n) const;
  uint8_t get_number_retransmission_T3592() const;

 public:
  bool ipv4;                            // IPv4 Addr
  struct in_addr ipv4_address;          // IPv4 Addr
  bool ipv6;                            // IPv6 prefix
  struct in6_addr ipv6_address;         // IPv6 prefix
  pdu_session_type_t pdu_session_type;  // IPv4, IPv6, IPv4v6 or Non-IP

  bool released;  // release session request

  uint32_t pdu_session_id;
  std::string dnn;  // associated DNN
  snssai_t snssai;  // associated SNSSAI

  std::string nwi_access;  // associated nwi_access
  std::string nwi_core;    // associated nwi_core

  pfcp::node_id_t upf_node_id;

  pdu_session_status_e pdu_session_status;
  upCnx_state_e
      upCnx_state;  // N3 tunnel status (ACTIVATED, DEACTIVATED, ACTIVATING)
  ho_state_e ho_state;
  timer_id_t timer_T3590;
  timer_id_t timer_T3591;
  timer_id_t timer_T3592;

  pfcp::qfi_t default_qfi;                    // Default QFI for this session
  std::map<uint8_t, smf_qos_flow> qos_flows;  // QFI <-> QoS Flow
  std::map<uint8_t, QOSRulesIE> qos_rules;    // QRI <-> QoS Rules
  std::vector<uint8_t> qos_rules_to_be_synchronised;
  std::vector<uint8_t> qos_rules_to_be_removed;
  // 5GSM parameters and capabilities
  uint8_t maximum_number_of_supported_packet_filters;
  // TODO: 5GSM Capability (section 9.11.4.1@3GPP TS 24.501 V16.1.0)
  // TODO: Integrity protection maximum data rate (section 9.11.4.7@@3GPP
  // TS 24.501 V16.1.0)
  uint8_t
      number_of_supported_packet_filters;  // number_of_supported_packet_filters
  util::uint_generator<uint32_t> qos_rule_id_generator;

  // PFCP related members
  // PFCP Session
  uint64_t seid;
  pfcp::fseid_t up_fseid;
  //
  util::uint_generator<uint16_t> pdr_id_generator;
  util::uint_generator<uint32_t> far_id_generator;
  util::uint_generator<uint32_t> urr_id_generator;

  // Shared lock
  mutable std::shared_mutex m_pdu_session_mutex;

 private:
  std::shared_ptr<itti_n11_msg> pending_n11_msg;
  uint8_t number_retransmission_T3591;
  uint8_t number_retransmission_T3592;
};

class session_management_subscription {
 public:
  session_management_subscription(snssai_t snssai)
      : single_nssai(snssai), dnn_configurations(), m_mutex() {}

  /*
   * Insert a DNN configuration into the subscription
   * @param [std::string] dnn
   * @param [std::shared_ptr<dnn_configuration_t> &] dnn_configuration
   * @return void
   */
  void insert_dnn_configuration(
      const std::string& dnn,
      std::shared_ptr<dnn_configuration_t>& dnn_configuration);

  /*
   * Find a DNN configuration
   * @param [std::string] dnn
   * @param [std::shared_ptr<dnn_configuration_t> &] dnn_configuration
   * @return void
   */
  void find_dnn_configuration(
      const std::string& dnn,
      std::shared_ptr<dnn_configuration_t>& dnn_configuration) const;

  /*
   * Verify whether DNN configuration with a given DNN exist
   * @param [std::string &] dnn
   * @return bool: return true if the configuration exist, otherwise return
   * false
   */
  bool dnn_configuration(const std::string& dnn) const;

 private:
  snssai_t single_nssai;
  std::map<std::string, std::shared_ptr<dnn_configuration_t>>
      dnn_configurations;  // dnn <->dnn_configuration

  // Shared lock
  mutable std::shared_mutex m_mutex;
};

class smf_context;

class smf_context : public std::enable_shared_from_this<smf_context> {
 public:
  smf_context()
      : m_context(),
        m_pdu_sessions_mutex(),
        pdu_sessions(),
        pending_procedures(),
        dnn_subscriptions(),
        event_sub(),
        plmn() {
    amf_id         = {};
    amf_addr       = {};
    amf_status_uri = {};
    target_amf     = {};
    supi_prefix    = {};
    // Subscribe to SM Context Status Change
    sm_context_status_connection =
        event_sub.subscribe_sm_context_status(boost::bind(
            &smf_context::handle_sm_context_status_change, this, _1, _2, _3));
    // Subscribe to PDU Session Release (event exposure)
    ee_pdu_session_release_connection =
        event_sub.subscribe_ee_pdu_session_release(boost::bind(
            &smf_context::handle_ee_pdu_session_release, this, _1, _2, _3));

    // Subscribe to UE IP Change Event
    ee_ue_ip_change_connection = event_sub.subscribe_ee_ue_ip_change(
        boost::bind(&smf_context::handle_ue_ip_change, this, _1, _2));

    // Subscribe to PLMN Change Event
    ee_plmn_change_connection = event_sub.subscribe_ee_plmn_change(
        boost::bind(&smf_context::handle_plmn_change, this, _1, _2));

    // Subscribe to DDDS event
    ee_ddds_connection = event_sub.subscribe_ee_ddds(
        boost::bind(&smf_context::handle_ddds, this, _1, _2));

    // Subscribe to FlexCN event
    ee_flexcn = event_sub.subscribe_ee_flexcn_event(
        boost::bind(&smf_context::handle_flexcn_event, this, _1, _2));
  }

  smf_context(smf_context& b) = delete;

  virtual ~smf_context() {
    Logger::smf_app().debug("Delete SMF Context instance...");
    // Disconnect the boost connection
    if (sm_context_status_connection.connected())
      sm_context_status_connection.disconnect();
    if (ee_pdu_session_release_connection.connected())
      ee_pdu_session_release_connection.disconnect();
    if (ee_ue_ip_change_connection.connected())
      ee_ue_ip_change_connection.disconnect();
    if (ee_plmn_change_connection.connected())
      ee_plmn_change_connection.disconnect();
    if (ee_ddds_connection.connected()) ee_ddds_connection.disconnect();
    if (ee_flexcn.connected()) ee_flexcn.disconnect();
  }

  /*
   * Insert a procedure to be processed
   * @param [std::shared_ptr<smf_procedure> &] sproc: procedure to be processed
   * @return void
   */
  void insert_procedure(std::shared_ptr<smf_procedure>& sproc);

  /*
   * Find a with its transaction ID
   * @param [const uint64_t &] trxn_id: Transaction ID
   * @param [std::shared_ptr<smf_procedure> &] proc: Stored procedure if found
   * @return void
   */
  bool find_procedure(
      const uint64_t& trxn_id, std::shared_ptr<smf_procedure>& proc);

  /*
   * Remove a procedure from the list
   * @param [smf_procedure *] sproc: procedure to be removed
   * @return void
   */
  void remove_procedure(smf_procedure* proc);

  /*
   * Handle N4 message (session establishment response) from UPF
   * @param [itti_n4_session_establishment_responset&]
   * @return void
   */
  void handle_itti_msg(itti_n4_session_establishment_response&);

  /*
   * Handle N4 message (session modification response) from UPF
   * @param [itti_n4_session_modification_response&]
   * @return void
   */
  void handle_itti_msg(itti_n4_session_modification_response&);

  /*
   * Handle N4 message (session deletion response) from UPF
   * @param [itti_n4_session_deletion_response&]
   * @return void
   */
  void handle_itti_msg(itti_n4_session_deletion_response&);

  /*
   * Handle N4 message (session report) from UPF
   * @param [itti_n4_session_report_request&]
   * @return void
   */
  void handle_itti_msg(std::shared_ptr<itti_n4_session_report_request>&);

  /*
   * Handle messages from AMF (e.g., PDU_SESSION_CreateSMContextRequest)
   * @param [std::shared_ptr<itti_n11_create_sm_context_request] smreq Request
   * message
   * @return void
   */
  void handle_pdu_session_create_sm_context_request(
      std::shared_ptr<itti_n11_create_sm_context_request> smreq);
  /*
   * Handle messages from AMF (e.g., PDU_SESSION_UpdateSMContextRequest)
   * @param [std::shared_ptr<itti_n11_update_sm_context_request] smreq Request
   * message
   * @return void
   */
  bool handle_pdu_session_update_sm_context_request(
      std::shared_ptr<itti_n11_update_sm_context_request> smreq);

  /*
   * Handle messages from AMF (e.g., PDU_SESSION_ReleaseSMContextRequest)
   * @param [std::shared_ptr<itti_n11_release_sm_context_request] smreq Request
   * message
   * @return void
   */
  void handle_pdu_session_release_sm_context_request(
      std::shared_ptr<itti_n11_release_sm_context_request> smreq);

  /*
   * Handle network-requested session modification (SMF, AN, AMF -requested)
   * @param [std::shared_ptr<itti_nx_trigger_pdu_session_modification] msg:
   * Request message
   * @return void
   */
  void handle_pdu_session_modification_network_requested(
      std::shared_ptr<itti_nx_trigger_pdu_session_modification> msg);

  /*
   * Handle PDU Session Modification Request
   * @param [nas_message_t&] nas_msg: NAS message received from AMF
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_modification_request(
      nas_message_t& nas_msg,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle PDU Session Modification Complete
   * @param [nas_message_t&] nas_msg: NAS message received from AMF
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_modification_complete(
      nas_message_t& nas_msg,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle PDU Session Modification Command Reject
   * @param [nas_message_t&] nas_msg: NAS message received from AMF
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_modification_command_reject(
      nas_message_t& nas_msg,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle PDU Session Release Request
   * @param [nas_message_t&] nas_msg: NAS message received from AMF
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_release_request(
      nas_message_t& nas_msg,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle PDU Session Release Complete
   * @param [nas_message_t&] nas_msg: NAS message received from AMF
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_release_complete(
      nas_message_t& nas_msg,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle Service Request
   * @param [std::string&] n2_sm_information: NGAP message in form of string
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_service_request(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle PDU Session Resource Setup Response Transfer
   * @param [std::string&] n2_sm_information: NGAP message in form of string
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_resource_setup_response_transfer(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request);

  /*
   * Handle PDU Session Resource Setup Unsuccessful Transfer
   * @param [std::string&] n2_sm_information: NGAP message in form of string
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_resource_setup_unsuccessful_transfer(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request);

  /*
   * Handle PDU Session Resource Modify Response Transfer
   * @param [std::string&] n2_sm_information: NGAP message in form of string
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_resource_modify_response_transfer(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request);

  /*
   * Handle PDU Session Resource Release Response Transfer
   * @param [std::string&] n2_sm_information: NGAP message in form of string
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @return True if handle successful, otherwise return false
   */
  bool handle_pdu_session_resource_release_response_transfer(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request);

  /*
   * Handle Xn Handover Patch Switch Request
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_path_switch_req(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle N2 Handover Preparation (Phrase 1- Preparing)
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_preparation_request(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle N2 Handover Preparation (Phrase 2- Prepared)
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_preparation_request_ack(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle N2 Handover Preparation failure
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_preparation_request_fail(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle N2 Handover Execution
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_execution(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Handle N2 Handover Cancellation
   * @param [std::shared_ptr<itti_n11_update_sm_context_request>&]
   * sm_context_request: Request message
   * @param [std::shared_ptr<itti_n11_update_sm_context_response>&]
   * sm_context_resp: Response message
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @return True if handle successful, otherwise return false
   */
  bool handle_ho_cancellation(
      std::string& n2_sm_information,
      std::shared_ptr<itti_n11_update_sm_context_request>& sm_context_request,
      std::shared_ptr<itti_n11_update_sm_context_response>& sm_context_resp,
      std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Check the validity of the request according to user subscription and local
   * policies
   * @param [std::shared_ptr<itti_n11_create_sm_context_request>] smreq
   * @return true if the request is valid, otherwise return false
   *
   */
  bool verify_sm_context_request(
      std::shared_ptr<itti_n11_create_sm_context_request> smreq);

  /*
   * Insert a session management subscription into the SMF context
   * @param [const snssai_t&] snssai
   * @param [std::shared_ptr<session_management_subscription>&] ss: pointer to
   * the subscription
   * @return void
   */
  void insert_dnn_subscription(
      const snssai_t& snssai,
      std::shared_ptr<session_management_subscription>& ss);

  /*
   * Insert a session management subscription into the SMF context
   * @param [const snssai_t&] snssai
   * @param [const dnn&] dnn
   * @param [std::shared_ptr<session_management_subscription>&] ss: pointer to
   * the subscription
   * @return void
   */
  void insert_dnn_subscription(
      const snssai_t& snssai, const std::string& dnn,
      std::shared_ptr<session_management_subscription>& ss);

  /*
   * Verify whether a subscription data exist with a given dnn and snssai
   * @param [const std::string &] dnn: DNN
   * @param [const snssai_t&] snssai: single NSSAI
   *@return bool: Return true if a subscription data corresponding with dnn and
   *snssai exist, otherwise return false
   */
  bool is_dnn_snssai_subscription_data(
      const std::string& dnn, const snssai_t& snssai);

  /*
   * Find a session management subscription from a SMF context
   * @param [const snssai_t&] snssai
   * @param [std::shared_ptr<session_management_subscription>&] ss: pointer to
   * the subscription
   * @return void
   */
  bool find_dnn_subscription(
      const snssai_t& snssai,
      std::shared_ptr<session_management_subscription>& ss);

  /*
   * Convert all members of this class to string for logging
   * @return std::string
   */
  std::string toString() const;

  /*
   * Get the default QoS from the subscription
   * @param [const snssai_t&] snssai
   * @param [const std::string&] dnn
   * @param [subscribed_default_qos_t] default_qos
   * @return void
   */
  void get_default_qos(
      const snssai_t& snssai, const std::string& dnn,
      subscribed_default_qos_t& default_qos);

  /*
   * Set the value for Supi
   * @param [const supi_t&] s
   * @return void
   */
  void set_supi(const supi_t& s);

  /*
   * Get Supi member
   * @param
   * @return supi_t
   */
  supi_t get_supi() const;

  /*
   * Get Supi prefix
   * @param [const std::string &]  prefix: Supi prefix (e.g., imsi)
   * @return void
   */
  void get_supi_prefix(std::string& prefix) const;

  /*
   * Get Supi prefix
   * @param [const std::string &]  prefix: Supi prefix (e.g., imsi)
   * @return void
   */
  void set_supi_prefix(std::string const& value);

  /*
   * Get the default QoS Rule for all QFIs
   * @param [QOSRulesIE] qos_rule
   * @param [const uint8_t] pdu_session_type: PDU session type (e.g., Ipv4,
   * Ipv6)
   * @return void
   */
  void get_default_qos_rule(QOSRulesIE& qos_rule, uint8_t pdu_session_type);

  /*
   * Get the default QoS Flow Description, according to PDU session type and QFI
   * @param [QOSFlowDescriptionsContents &] qos_flow_description
   * @param [uint8_t] pdu_session_type: PDU Session Type
   * @param [const pfcp::qfi_t &] qfi
   * @return void
   */
  void get_default_qos_flow_description(
      QOSFlowDescriptionsContents& qos_flow_description,
      uint8_t pdu_session_type, const pfcp::qfi_t& qfi);

  /*
   * Get the default value of Session-AMBR
   * @param [SessionAMBR &] session_ambr
   * @param [const snssai_t &] snssai
   * @param [const std::string &] dnn
   * @return void
   */
  void get_session_ambr(
      SessionAMBR& session_ambr, const snssai_t& snssai,
      const std::string& dnn);

  /*
   * Get the default value of Session-AMBR and stored as
   * Ngap_PDUSessionAggregateMaximumBitRate
   * @param [Ngap_PDUSessionAggregateMaximumBitRate_t &] session_ambr
   * @param [const snssai_t &] snssai
   * @param [const std::string &] dnn
   * @return void
   */
  void get_session_ambr(
      Ngap_PDUSessionAggregateMaximumBitRate_t& session_ambr,
      const snssai_t& snssai, const std::string& dnn);

  /*
   * Find the PDU Session with its ID
   * @param [const pdu_session_id_t &] psi: PDU Session ID
   * @param [std::shared_ptr<smf_pdu_session> &] sp: pointer to the PDU session
   * @return bool: return true if found, otherwise return false
   */
  bool find_pdu_session(
      const pdu_session_id_t& psi, std::shared_ptr<smf_pdu_session>& sp) const;

  bool find_pdu_session(
      const pfcp::pdr_id_t& pdr_id, pfcp::qfi_t& qfi,
      std::shared_ptr<smf_pdu_session>& sp);
  /*
   * Add a PDU Session to the PDU Session List
   * @param [const pdu_session_id_t &] psi: PDU Session ID
   * @param [const std::shared_ptr<smf_pdu_session> &] sp: pointer to the PDU
   * session
   * @return bool: return true if found, otherwise return false
   */
  bool add_pdu_session(
      const pdu_session_id_t& psi, const std::shared_ptr<smf_pdu_session>& sp);

  /*
   * Remove a PDU Session from the PDU Session List
   * @param [const pdu_session_id_t &] psi: PDU Session ID
   * @return bool: return true if success, otherwise return false
   */
  bool remove_pdu_session(const pdu_session_id_t& psi);

  /*
   * Get number of pdu sessions associated with this context
   * @param void
   * @return size_t: number of PDU sessions
   */
  size_t get_number_pdu_sessions() const;

  /*
   * Get all the PDU Sessions
   * @param [std::map<pdu_session_id_t, std::shared_ptr<smf_pdu_session>>&]
   * sessions: all PDU sessions
   * @return void
   */
  void get_pdu_sessions(
      std::map<pdu_session_id_t, std::shared_ptr<smf_pdu_session>>& sessions);

  /*
   * Handle SM Context Status Change (Send notification AMF)
   * @param [scid_t] scid: SMF Context ID
   * @param [uint32_t] status: Updated status
   * @param [uint8_t] http_version: HTTP version
   * @return void
   */
  void handle_sm_context_status_change(
      scid_t scid, const std::string& status, uint8_t http_version);

  /*
   * Handle SM Context Status Change (Send notification AMF)
   * @param [scid_t] scid: SMF Context ID
   * @param [uint32_t] status: Updated status
   * @param [uint8_t] http_version: HTTP version
   * @return void
   */
  void handle_ee_pdu_session_release(
      supi64_t supi, pdu_session_id_t pdu_session_id, uint8_t http_version);

  void trigger_ue_ip_change(scid_t scid, uint8_t http_version);
  void handle_ue_ip_change(scid_t scid, uint8_t http_version);

  void trigger_plmn_change(scid_t scid, uint8_t http_version);
  void handle_plmn_change(scid_t scid, uint8_t http_version);

  void trigger_ddds(scid_t scid, uint8_t http_version);
  void handle_ddds(scid_t scid, uint8_t http_version);

  void trigger_flexcn_event(scid_t scid, uint8_t http_version);
  void handle_flexcn_event(scid_t scid, uint8_t http_version);
  /*
   * Update QoS information in the Response message according to the content of
   * decoded NAS msg
   * @param [std::shared_ptr<smf_pdu_session>&] sp: PDU session
   * @param [std::shared_ptr<pdu_session_update_sm_context_response>&] res:
   * Response message
   * @param [const nas_message_t] nas_msg: NAS message
   * @return void
   */
  void update_qos_info(
      std::shared_ptr<smf_pdu_session>& sp,
      smf::pdu_session_update_sm_context_response& res,
      const nas_message_t& nas_msg);

  /*
   * Set AMF Addr of the serving AMF
   * @param [const std::string&] addr: AMF Addr in string representation
   * @return void
   */
  void set_amf_addr(const std::string& addr);

  /*
   * Get AMF Addr of the serving AMF (in string representation)
   * @param [std::string&] addr: store AMF IP Addr
   * @return void
   */
  void get_amf_addr(std::string& addr) const;

  /*
   * Get AMF Addr of the serving AMF (in string representation)
   * @param void
   * @return string: AMF IP Addr
   */
  std::string get_amf_addr() const;

  /*
   * Set the URI of AMF for receiving context status update
   * @param [const std::string&] status_uri: AMF's URI
   * @return void
   */
  void set_amf_status_uri(const std::string& status_uri);

  /*
   * Get the URI of AMF for receiving context status update
   * @param [std::string&] status_uri: AMF's URI
   * @return void
   */
  void get_amf_status_uri(std::string& status_uri) const;

  /*
   * Get the URI of AMF for receiving context status update
   * @param void
   * @return string
   */
  std::string get_amf_status_uri() const;

  /*
   * Set target AMF in case of HO
   * @param [const std::string&] amf: Target AMF
   * @return void
   */
  void set_target_amf(const std::string& amf);

  /*
   * Get target AMF in case of HO
   * @param [std::string&] amf: Target AMF
   * @return void
   */
  void get_target_amf(std::string& amf) const;

  /*
   * Get target AMF in case of HO
   * @param void
   * @return std::string
   */
  std::string get_target_amf() const;

  /*
   * Set PLMN
   * @param [const plmn_t&] plmn: PLMN
   * @return void
   */
  void set_plmn(const plmn_t& plmn);

  /*
   * get PLMN
   * @param [plmn_t&] plmn: store PLMN
   * @return void
   */
  void get_plmn(plmn_t& plmn) const;

  /*
   * Check if N2 HO for this PDU session can be accepted
   * @param [const ng_ran_target_id_t&] ran_target_id: Target ID
   * @param [const pdu_session_id_t&] pdu_session_id: PDU Session ID
   * @return true if the N2 HO can be accepted, otherwise return false
   *
   */
  bool check_handover_possibility(
      const ng_ran_target_id_t& ran_target_id,
      const pdu_session_id_t& pdu_session_id) const;

 private:
  std::vector<std::shared_ptr<smf_procedure>> pending_procedures;
  // snssai-sst <-> session management subscription
  std::map<uint8_t, std::shared_ptr<session_management_subscription>>
      dnn_subscriptions;
  std::map<pdu_session_id_t, std::shared_ptr<smf_pdu_session>>
      pdu_sessions;  // Store all PDU Sessions associated with this UE
  mutable std::shared_mutex m_pdu_sessions_mutex;

  supi_t supi;
  std::string supi_prefix;
  plmn_t plmn;

  std::string amf_id;
  std::string amf_addr;
  std::string amf_status_uri;
  std::string target_amf;  // targetServingNfId

  // Big recursive lock
  mutable std::recursive_mutex m_context;

  // for Event Handling
  smf_event event_sub;
  bs2::connection sm_context_status_connection;
  bs2::connection ee_pdu_session_release_connection;
  bs2::connection ee_ue_ip_change_connection;
  bs2::connection ee_plmn_change_connection;
  bs2::connection ee_ddds_connection;
  bs2::connection ee_flexcn;
};
}  // namespace smf

#endif
