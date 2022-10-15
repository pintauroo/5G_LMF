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

/*! \file amf_msg.cpp
 \brief
 \author  Shivam Gandhi
 \company KCL
 \date 2021
 \email: shivam.gandhi@kcl.ac.uk
 */
#include "amf_msg.hpp"

using namespace amf_application;

/*
 * class: Event Exposure
 */

//-----------------------------------------------------------------------------
std::string event_exposure_msg::get_supi() const {
  return m_supi;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_supi(const std::string& value) {
  m_supi        = value;
  m_supi_is_set = true;
}

//-----------------------------------------------------------------------------
bool event_exposure_msg::is_supi_is_set() const {
  return m_supi_is_set;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_sub_id(std::string const& value) {
  m_sub_id        = value;
  m_sub_id_is_set = true;
}

//-----------------------------------------------------------------------------
std::string event_exposure_msg::get_sub_id() const {
  return m_sub_id;
}

//-----------------------------------------------------------------------------
bool event_exposure_msg::is_sub_id_is_set() const {
  return m_sub_id_is_set;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_notify_uri(std::string const& value) {
  m_notify_uri = value;
}

//-----------------------------------------------------------------------------
std::string event_exposure_msg::get_notify_uri() const {
  return m_notify_uri;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_notify_correlation_id(std::string const& value) {
  m_notify_correlation_id = value;
}

//-----------------------------------------------------------------------------
std::string event_exposure_msg::get_notify_correlation_id() const {
  return m_notify_correlation_id;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_nf_id(std::string const& value) {
  m_nf_id = value;
}

//-----------------------------------------------------------------------------
std::string event_exposure_msg::get_nf_id() const {
  return m_nf_id;
}

//-----------------------------------------------------------------------------
std::vector<amf_event_t> event_exposure_msg::get_event_subs() const {
  return m_event_list;
}

//-----------------------------------------------------------------------------
void event_exposure_msg::set_event_subs(std::vector<amf_event_t> const& value) {
  m_event_list.clear();
  for (auto it : value) {
    m_event_list.push_back(it);
  }
}

//-----------------------------------------------------------------------------
void event_exposure_msg::add_event_sub(amf_event_t const& value) {
  m_event_list.push_back(value);
}
//-----------------------------------------------------------------------------
void event_exposure_msg::set_any_ue(bool value) {
  m_any_ue = value;
}
/*
 * class: Event Notification
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void event_notification::set_notify_correlation_id(std::string const& value) {
  m_notify_correlation_id     = value;
  m_notify_correlation_is_set = true;
}

//-----------------------------------------------------------------------------
std::string event_notification::get_notify_correlation_id() const {
  return m_notify_correlation_id;
}

void event_notification::set_notify_uri(std::string const& value) {
  m_notify_uri        = value;
  m_notify_uri_is_set = true;
}

std::string event_notification::get_notify_uri() const {
  return m_notify_uri;
}

//-----------------------------------------------------------------------------
void event_notification::set_subs_change_notify_correlation_id(
    std::string const& value) {
  m_subs_change_notify_correlation_id = value;
}

//-----------------------------------------------------------------------------
std::string event_notification::get_subs_change_notify_correlation_id() const {
  return m_subs_change_notify_correlation_id;
}

//-----------------------------------------------------------------------------
void event_notification::add_report(
    const oai::amf::model::AmfEventReport& report) {
  m_event_report_list.push_back(report);
}

//-----------------------------------------------------------------------------
void event_notification::get_reports(
    std::vector<oai::amf::model::AmfEventReport>& reports) const {
  reports = m_event_report_list;
}

//-----------------------------------------------------------------------------
void data_notification_msg::set_notification_event_type(
    const std::string& type) {
  notification_event_type = type;
}

//-----------------------------------------------------------------------------
void data_notification_msg::get_notification_event_type(
    std::string& type) const {
  type = notification_event_type;
}

//-----------------------------------------------------------------------------
void data_notification_msg::set_nf_instance_uri(const std::string& uri) {
  nf_instance_uri = uri;
}

//-----------------------------------------------------------------------------
void data_notification_msg::get_nf_instance_uri(std::string& uri) const {
  uri = nf_instance_uri;
}

//-----------------------------------------------------------------------------
void data_notification_msg::set_profile(const std::shared_ptr<nf_profile>& p) {
  // profile = p;
}

//-----------------------------------------------------------------------------
void data_notification_msg::get_profile(std::shared_ptr<nf_profile>& p) const {
  // p = profile;
}
