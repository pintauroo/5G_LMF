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

/*! \file 3gpp_conversions.cpp
 * \brief
 * \author
 * \company Eurecom
 * \email:
 */
#include "3gpp_conversions.hpp"

void xgpp_conv::amf_event_subscription_from_openapi(
    const oai::amf::model::AmfCreateEventSubscription& event_subscription,
    amf_application::event_exposure_msg& event_exposure) {
  event_exposure.set_notify_uri(
      event_subscription.getSubscription().getEventNotifyUri());
  event_exposure.set_notify_correlation_id(
      event_subscription.getSubscription().getNotifyCorrelationId());
  event_exposure.set_nf_id(event_subscription.getSubscription().getNfId());

  for (auto e : event_subscription.getSubscription().getEventList()) {
    amf_event_t ev      = {};
    std::string ev_type = e.getType().get_value();
    if (ev_type.compare("LOCATION_REPORT") == 0) {
      ev.type = amf_event_type_e::LOCATION_REPORT;
    } else if (ev_type.compare("PRESENCE_IN_AOI_REPORT") == 0) {
      ev.type = amf_event_type_e::PRESENCE_IN_AOI_REPORT;
    } else if (ev_type.compare("TIMEZONE_REPORT") == 0) {
      ev.type = amf_event_type_e::TIMEZONE_REPORT;
    } else if (ev_type.compare("ACCESS_TYPE_REPORT") == 0) {
      ev.type = amf_event_type_e::ACCESS_TYPE_REPORT;
    } else if (ev_type.compare("REGISTRATION_STATE_REPORT") == 0) {
      ev.type = amf_event_type_e::REGISTRATION_STATE_REPORT;
    } else if (ev_type.compare("CONNECTIVITY_STATE_REPORT") == 0) {
      ev.type = amf_event_type_e::CONNECTIVITY_STATE_REPORT;
    } else if (ev_type.compare("REACHABILITY_REPORT") == 0) {
      ev.type = amf_event_type_e::REACHABILITY_REPORT;
    } else if (ev_type.compare("COMMUNICATION_FAILURE_REPORT") == 0) {
      ev.type = amf_event_type_e::COMMUNICATION_FAILURE_REPORT;
    } else if (ev_type.compare("UES_IN_AREA_REPORT") == 0) {
      ev.type = amf_event_type_e::UES_IN_AREA_REPORT;
    } else if (ev_type.compare("SUBSCRIPTION_ID_CHANGE") == 0) {
      ev.type = amf_event_type_e::SUBSCRIPTION_ID_CHANGE;
    } else if (ev_type.compare("SUBSCRIPTION_ID_ADDITION") == 0) {
      ev.type = amf_event_type_e::SUBSCRIPTION_ID_ADDITION;
    } else if (ev_type.compare("LOSS_OF_CONNECTIVITY") == 0) {
      ev.type = amf_event_type_e::LOSS_OF_CONNECTIVITY;
    } else {
      ev.type = amf_event_type_e::AMF_EVENT_UNKNOWN;
    }
    event_exposure.add_event_sub(ev);
  }

  if (event_subscription.getSubscription().supiIsSet()) {
    // supi_t supi          = {};
    std::string supi = event_subscription.getSubscription().getSupi();
    // amf_string_to_supi(&supi, supi_str.c_str());
    event_exposure.set_supi(supi);
  }

  if (event_subscription.getSubscription().anyUEIsSet()) {
    event_exposure.set_any_ue(true);
  } else {
    event_exposure.set_any_ue(false);
  }

  // TODO:
}

std::string xgpp_conv::amf_event_type_to_string(amf_event_type_t type) {
  uint8_t t = (uint8_t) type;
  if ((t > 0) and (t <= 12)) {
    return amf_event_type_e2str.at(t);
  } else {
    return amf_event_type_e2str.at(0);
  }
}
