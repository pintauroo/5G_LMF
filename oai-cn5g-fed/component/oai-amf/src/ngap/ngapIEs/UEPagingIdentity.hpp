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

#ifndef _UEPAGINGIDENTITY_H_
#define _UEPAGINGIDENTITY_H_

#include "FiveGSTmsi.hpp"

extern "C" {
#include "Ngap_UEPagingIdentity.h"
}

namespace ngap {

class UEPagingIdentity {
 public:
  UEPagingIdentity();
  virtual ~UEPagingIdentity();

  void setUEPagingIdentity(
      std::string& setid, std::string& pointer, std::string& tmsi);
  void getUEPagingIdentity(std::string& _5g_s_tmsi);
  void getUEPagingIdentity(
      std::string& setid, std::string& pointer, std::string& tmsi);
  bool encode2pdu(Ngap_UEPagingIdentity_t* pdu);
  bool decodefrompdu(Ngap_UEPagingIdentity_t pdu);

 private:
  FiveGSTmsi fiveGSTmsi;
};

}  // namespace ngap

#endif
