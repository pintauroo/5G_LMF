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

/*! \file
 \brief
 \author  Keliang DU, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */

#ifndef _UE_CONTEXT_RELEASE_COMMAND_H_
#define _UE_CONTEXT_RELEASE_COMMAND_H_

#include "AMF-UE-NGAP-ID.hpp"
#include "Cause.hpp"
#include "MessageType.hpp"
#include "RAN-UE-NGAP-ID.hpp"

extern "C" {
#include "Ngap_NGAP-PDU.h"
#include "Ngap_ProtocolIE-Field.h"
}

namespace ngap {

class UEContextReleaseCommandMsg {
 public:
  UEContextReleaseCommandMsg();
  ~UEContextReleaseCommandMsg();

 public:
  void setMessageType();
  void setAmfUeNgapId(unsigned long id);
  void setUeNgapIdPair(unsigned long amfId, uint32_t ranId);
  void addCauseIE();
  void setCauseRadioNetwork(e_Ngap_CauseRadioNetwork cause_value);
  void setCauseNas(e_Ngap_CauseNas cause_value);
  int encode2buffer(uint8_t* buf, int buf_size);

 public:
  bool decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu);

 private:
  Ngap_NGAP_PDU_t* pdu;
  Ngap_UEContextReleaseCommand_t* ies;

  AMF_UE_NGAP_ID* amfUeNgapId;
  RAN_UE_NGAP_ID* ranUeNgapId;
  Cause* causeValue;
};

}  // namespace ngap

#endif
