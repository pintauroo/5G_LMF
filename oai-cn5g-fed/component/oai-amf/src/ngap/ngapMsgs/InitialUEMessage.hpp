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

#ifndef _INITIALUEMESSAGE_H_
#define _INITIALUEMESSAGE_H_

#include "FiveGSTmsi.hpp"
#include "MessageType.hpp"
#include "NAS-PDU.hpp"
#include "NgapIEsStruct.hpp"
#include "RAN-UE-NGAP-ID.hpp"
#include "RRCEstablishmentCause.hpp"
#include "UEContextRequest.hpp"
#include "UserLocationInformation.hpp"

extern "C" {
#include "Ngap_InitialUEMessage.h"
#include "Ngap_NGAP-PDU.h"
#include "Ngap_ProtocolIE-Field.h"
}

namespace ngap {

class InitialUEMessageMsg {
 public:
  InitialUEMessageMsg();
  virtual ~InitialUEMessageMsg();

  void setMessageType();
  void setRanUENgapID(uint32_t ran_ue_ngap_id);
  void setNasPdu(uint8_t* nas, size_t sizeofnas);
  void setUserLocationInfoNR(struct NrCgi_s cig, struct Tai_s tai);
  void setRRCEstablishmentCause(e_Ngap_RRCEstablishmentCause cause_value);
  // void set5GS_TMSI(string amfSetId, string amfPointer, string _5g_tmsi);
  void setUeContextRequest(e_Ngap_UEContextRequest ueCtxReq);
  int encode2buffer(uint8_t* buf, int buf_size);
  // Decapsulation
  bool decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu);
  bool getRanUENgapID(uint32_t& value);
  bool getNasPdu(uint8_t*& nas, size_t& sizeofnas);
  bool getUserLocationInfoNR(struct NrCgi_s& cig, struct Tai_s& tai);
  int getRRCEstablishmentCause();
  int getUeContextRequest();
  bool get5GS_TMSI(std::string& _5g_s_tmsi);
  bool get5GS_TMSI(
      std ::string& setid, std ::string& pointer, std ::string& tmsi);

 private:
  Ngap_NGAP_PDU_t* initialUEMessagePdu;
  Ngap_InitialUEMessage_t* initialUEMessageIEs;

  RAN_UE_NGAP_ID* ranUeNgapId;
  NAS_PDU* nasPdu;
  UserLocationInformation* userLocationInformation;
  RRCEstablishmentCause* rRCEstablishmentCause;
  UEContextRequest* uEContextRequest;
  FiveGSTmsi* fivegSTmsi;
};

}  // namespace ngap
#endif
