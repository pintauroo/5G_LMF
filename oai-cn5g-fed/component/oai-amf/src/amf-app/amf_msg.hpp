/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this
 *file except in compliance with the License. You may obtain a copy of the
 *License at
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

/*! \file amf_msg.hpp
 \brief
 \author
 \company
 \date 2021
 \email:
 */
#ifndef FILE_AMF_MSG_HPP_SEEN
#define FILE_AMF_MSG_HPP_SEEN

#include "amf.hpp"
#include "3gpp_29.518.h"
#include "amf_profile.hpp"
#include "AmfEventReport.h"

namespace amf_application {

class event_exposure_msg {
 public:
  std::string get_supi() const;
  void set_supi(const std::string& value);
  bool is_supi_is_set() const;

  void set_sub_id(std::string const& value);
  std::string get_sub_id() const;
  bool is_sub_id_is_set() const;

  void set_notify_uri(std::string const& value);
  std::string get_notify_uri() const;

  void set_notify_correlation_id(std::string const& value);
  std::string get_notify_correlation_id() const;

  void set_nf_id(std::string const& value);
  std::string get_nf_id() const;

  std::vector<amf_event_t> get_event_subs() const;
  void set_event_subs(std::vector<amf_event_t> const& value);
  void add_event_sub(amf_event_t const& value);

  void set_any_ue(bool value);

 private:
  std::string m_sub_id;  // m_SubId;
  bool m_sub_id_is_set;

  std::vector<amf_event_t> m_event_list;  // eventList, Mandatory
  std::string m_notify_uri;               // eventNotifyUri, Mandatory
  std::string m_notify_correlation_id;    // notifyCorrelationId, Mandatory
  std::string m_nf_id;                    //  nfId, Mandatory

  // supi_t m_supi;  // Supi, Conditional
  std::string m_supi;
  bool m_supi_is_set;
  bool m_any_ue;  // anyUE, Conditional

  //  subsChangeNotifyUri:
  //  subsChangeNotifyCorrelationId:
  //  groupId:
  //  gpsi:
  //  pei:
  //  options: AmfEventMode
};

class event_notification {
 public:
  void set_notify_correlation_id(std::string const& value);
  std::string get_notify_correlation_id() const;

  void set_notify_uri(std::string const& value);
  std::string get_notify_uri() const;

  void set_subs_change_notify_correlation_id(std::string const& value);
  std::string get_subs_change_notify_correlation_id() const;
  void add_report(const oai::amf::model::AmfEventReport& report);
  void get_reports(std::vector<oai::amf::model::AmfEventReport>& reports) const;

 private:
  std::string m_notify_correlation_id;  // notifyCorrelationId
  bool m_notify_correlation_is_set;

  std::string m_notify_uri;  // notifyUri
  bool m_notify_uri_is_set;

  std::string
      m_subs_change_notify_correlation_id;  // SubsChangeNotifyCorrelationId;
  bool m_subs_change_notify_correlation_id_is_set;
  std::vector<oai::amf::model::AmfEventReport>
      m_event_report_list;  // Report List
  bool m_report_list_is_set;
};

class data_notification_msg {
 public:
  void set_notification_event_type(const std::string& type);
  void get_notification_event_type(std::string& type) const;
  void set_nf_instance_uri(const std::string& uri);
  void get_nf_instance_uri(std::string& uri) const;
  void set_profile(const std::shared_ptr<amf_application::nf_profile>& p);
  void get_profile(std::shared_ptr<amf_application::nf_profile>& p) const;

 private:
  std::string notification_event_type;
  std::string nf_instance_uri;
  // bool m_NfProfileIsSet;
  // std::vector<ChangeItem> m_ProfileChanges;
  // bool m_ProfileChangesIsSet;
};
}  // namespace amf_application

#endif
