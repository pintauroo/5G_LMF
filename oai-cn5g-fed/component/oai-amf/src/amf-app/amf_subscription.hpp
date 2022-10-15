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

/*! \file amf_subscription.hpp
 \brief
 \author  Shivam Gandhi
 \company KCL
 \date 2021
 \email: shivam.gandhi@kcl.ac.uk
 */

#include "3gpp_29.518.h"
#include "amf.hpp"

namespace amf_application {

/*
 * Manage the Subscription Info
 */

class amf_subscription {
 public:
  amf_subscription()
      : sub_id(),
        ev_type(),
        supi(),
        notify_correlation_id(),
        notify_uri(),
        nf_id() {
    supi_is_set = false;
  }
  void display();

  evsub_id_t sub_id;
  amf_event_type_t ev_type;
  bool supi_is_set;
  std::string supi;
  std::string notify_correlation_id;
  std::string notify_uri;  // subsChangeNotifyUri ?
  std::string nf_id;
};

}  // namespace amf_application
