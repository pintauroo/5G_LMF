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

#ifndef _PAGING_H_
#define _PAGING_H_

#include "NgapIEsStruct.hpp"

#include "MessageType.hpp"
#include "UEPagingIdentity.hpp"
#include "TAIListforPaging.hpp"

extern "C" {
#include "Ngap_NGAP-PDU.h"
#include "Ngap_ProtocolIE-Field.h"
#include "Ngap_Paging.h"
}

namespace ngap {

class PagingMsg {
 public:
  PagingMsg();
  virtual ~PagingMsg();

  void setMessageType();
  int encode2buffer(uint8_t* buf, int buf_size);
  bool decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu);

  void setUEPagingIdentity(
      std::string SetId, std::string Pointer, std::string tmsi);
  void getUEPagingIdentity(std::string& _5g_s_tmsi);
  void getUEPagingIdentity(
      std::string& setid, std::string& pointer, std::string& tmsi);

  void setTAIListForPaging(const std::vector<struct Tai_s> list);
  void getTAIListForPaging(std::vector<struct Tai_s>& list);

 private:
  Ngap_NGAP_PDU_t* pagingPdu;
  Ngap_Paging_t* pagingIEs;

  UEPagingIdentity* uePagingIdentity;
  TAIListForPaging* taIListForPaging;
  // Paging DRX
  // Paging Priority
  // UE Radio Capability for Paging
  // Paging Origin
  // Assistance Data for Paging
};

}  // namespace ngap

#endif
