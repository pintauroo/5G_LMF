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

/*! \file 3gpp_conversions.hpp
 \brief
 \author
 \company Eurecom
 \email:
 */

#ifndef FILE_3GPP_CONVERSIONS_HPP_SEEN
#define FILE_3GPP_CONVERSIONS_HPP_SEEN

#include "amf_msg.hpp"
#include "AmfCreateEventSubscription.h"
#include "3gpp_29.518.h"

using namespace amf_application;
using namespace oai::amf::model;
namespace xgpp_conv {

/*
 * Convert AmfCreatedEventSubscription from OpenAPI into Event Exposure Msg
 * @param [const oai::amf::model::AmfCreatedEventSubscription&]
 * event_subscription: AmfCreatedEventSubscription in OpenAPI
 * @param [amf_application::event_exposure_msg&] event_exposure: Event Exposure
 * Msg
 * @return void
 */
void amf_event_subscription_from_openapi(
    const oai::amf::model::AmfCreateEventSubscription& event_subscription,
    amf_application::event_exposure_msg& event_exposure);

std::string amf_event_type_to_string(amf_event_type_t type);
}  // namespace xgpp_conv

#endif /* FILE_3GPP_CONVERSIONS_HPP_SEEN */
