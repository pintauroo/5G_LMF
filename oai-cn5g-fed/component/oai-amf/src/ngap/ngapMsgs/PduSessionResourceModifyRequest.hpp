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
 \author
 \date 2021
 \email: contact@openairinterface.org
 */

#ifndef PDU_SESSION_RESOURCE_MODIFY_REQUEST_H_
#define PDU_SESSION_RESOURCE_MODIFY_REQUEST_H_

#include "AMF-UE-NGAP-ID.hpp"
#include "MessageType.hpp"
#include "NgapIEsStruct.hpp"
#include "PDUSessionResourceModifyListModReq.hpp"
#include "RAN-UE-NGAP-ID.hpp"
#include "RANPagingPriority.hpp"

extern "C" {
#include "Ngap_InitialContextSetupRequest.h"
#include "Ngap_PDUSessionResourceModifyRequest.h"
#include "Ngap_NGAP-PDU.h"
#include "Ngap_ProtocolIE-Field.h"
}

namespace ngap {

class PduSessionResourceModifyRequestMsg {
 public:
  PduSessionResourceModifyRequestMsg();
  virtual ~PduSessionResourceModifyRequestMsg();

  void setMessageType();
  void setAmfUeNgapId(unsigned long id);  // 40 bits
  void setRanUeNgapId(uint32_t id);       // 32 bits
  void setRanPagingPriority(uint8_t priority);
  void setNasPdu(uint8_t* nas, size_t sizeofnas);

  void setPduSessionResourceModifyRequestList(
      std::vector<PDUSessionResourceModifyRequestItem_t> list);

  int encode2buffer(uint8_t* buf, int buf_size);
  void encode2buffer_new(char* buf, int& encoded_size);

  bool decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu);
  unsigned long getAmfUeNgapId();
  uint32_t getRanUeNgapId();
  int getRanPagingPriority();
  bool getNasPdu(uint8_t*& nas, size_t& sizeofnas);
  bool getPduSessionResourceModifyRequestList(
      std::vector<PDUSessionResourceModifyRequestItem_t>& list);

 private:
  Ngap_NGAP_PDU_t* pduSessionResourceModifyRequestPdu;
  Ngap_PDUSessionResourceModifyRequest_t* pduSessionResourceModifyRequestIEs;

  AMF_UE_NGAP_ID amfUeNgapId;            // Mandatory
  RAN_UE_NGAP_ID ranUeNgapId;            // Mandatory
  RANPagingPriority* ranPagingPriority;  // Optional
  PDUSessionResourceModifyListModReq* pduSessionResourceModifyList;
};

}  // namespace ngap
#endif
