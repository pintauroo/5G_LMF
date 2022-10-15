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

/*! \file amf_subscription.cpp
 \brief
 \author
 \company
 \date 2021
 \email:
 */

#include "amf_subscription.hpp"
#include "logger.hpp"
#include "3gpp_conversions.hpp"

using namespace amf_application;

void amf_subscription::display() {
  Logger::amf_app().debug("Subscription info");
  Logger::amf_app().debug("\tSubscription ID: %d", (uint32_t) sub_id);
  Logger::amf_app().debug(
      "\tEvent type: %s", xgpp_conv::amf_event_type_to_string(ev_type).c_str());
  if (supi_is_set) Logger::amf_app().debug("\tSUPI: %s", supi.c_str());
  Logger::amf_app().debug(
      "\tNotify Correlation ID: %s", notify_correlation_id.c_str());
  Logger::amf_app().debug("\tNotify URI: %s", notify_uri.c_str());
  Logger::amf_app().debug("\tNF ID: %s", nf_id.c_str());
};
