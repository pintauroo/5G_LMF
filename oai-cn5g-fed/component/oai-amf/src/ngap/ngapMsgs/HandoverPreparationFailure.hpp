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

#ifndef _HANDOVER_PREPARATION_FAILURE_H_
#define _HANDOVER_PREPARATION_FAILURE_H_

#include "AMF-UE-NGAP-ID.hpp"
#include "Cause.hpp"
#include "MessageType.hpp"
#include "NgapIEsStruct.hpp"
#include "RAN-UE-NGAP-ID.hpp"
extern "C" {
#include "Ngap_NGAP-PDU.h"
#include "Ngap_ProtocolIE-Field.h"
}

namespace ngap {

class HandoverPreparationFailure {
 public:
  HandoverPreparationFailure();
  virtual ~HandoverPreparationFailure();

  void setMessageType();  // Initialize the PDU and populate the MessageType;

  void setAmfUeNgapId(unsigned long id);  // 40 bits
  unsigned long getAmfUeNgapId() const;

  void setRanUeNgapId(uint32_t id);  // 32 bits
  uint32_t getRanUeNgapId() const;

  int encode2buffer(uint8_t* buf, int buf_size);
  bool decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu);

  void getCause(Cause& cause) const;
  void setCause(Ngap_Cause_PR m_causePresent, long value = 0);
  Ngap_Cause_PR getChoiceOfCause() const;

 private:
  Ngap_NGAP_PDU_t* hoPreparationFailurePdu;
  Ngap_HandoverPreparationFailure_t* hoPreparationFailureIEs;
  AMF_UE_NGAP_ID* amfUeNgapId;
  RAN_UE_NGAP_ID* ranUeNgapId;
  Cause* cause;
  Ngap_CriticalityDiagnostics_t* CriticalityDiagnostics;
};

}  // namespace ngap

#endif
