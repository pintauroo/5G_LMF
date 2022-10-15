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

#include "PduSessionResourceModifyResponse.hpp"
#include "amf.hpp"
#include "logger.hpp"

extern "C" {
#include "asn_codecs.h"
#include "constr_TYPE.h"
#include "constraints.h"
#include "dynamic_memory_check.h"
#include "per_decoder.h"
#include "per_encoder.h"
}

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PduSessionResourceModifyResponseMsg::PduSessionResourceModifyResponseMsg() {
  // Set Message Type
  pduSessionResourceModifyResponsePdu =
      (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType pdu = {};
  pdu.setProcedureCode(Ngap_ProcedureCode_id_PDUSessionResourceModify);

  pdu.setTypeOfMessage(Ngap_NGAP_PDU_PR_successfulOutcome);
  pdu.setCriticality(Ngap_Criticality_reject);
  pdu.setValuePresent(
      Ngap_SuccessfulOutcome__value_PR_PDUSessionResourceModifyResponse);
  pdu.encode2pdu(pduSessionResourceModifyResponsePdu);
  pduSessionResourceModifyResponseIEs =
      &(pduSessionResourceModifyResponsePdu->choice.successfulOutcome->value
            .choice.PDUSessionResourceModifyResponse);
}

//------------------------------------------------------------------------------
PduSessionResourceModifyResponseMsg::~PduSessionResourceModifyResponseMsg() {
  // TODO:
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyResponseMsg::setMessageType() {
  if (!pduSessionResourceModifyResponsePdu)
    pduSessionResourceModifyResponsePdu =
        (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType pdu = {};
  pdu.setProcedureCode(Ngap_ProcedureCode_id_PDUSessionResourceModify);

  pdu.setTypeOfMessage(Ngap_NGAP_PDU_PR_successfulOutcome);
  pdu.setCriticality(Ngap_Criticality_reject);
  pdu.setValuePresent(
      Ngap_SuccessfulOutcome__value_PR_PDUSessionResourceModifyResponse);
  pdu.encode2pdu(pduSessionResourceModifyResponsePdu);
  pduSessionResourceModifyResponseIEs =
      &(pduSessionResourceModifyResponsePdu->choice.successfulOutcome->value
            .choice.PDUSessionResourceModifyResponse);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyResponseMsg::setAmfUeNgapId(unsigned long id) {
  amfUeNgapId.setAMF_UE_NGAP_ID(id);

  Ngap_PDUSessionResourceModifyResponseIEs_t* ie =
      (Ngap_PDUSessionResourceModifyResponseIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyResponseIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_PDUSessionResourceModifyResponseIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId.encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyResponseIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyResponseMsg::setRanUeNgapId(
    uint32_t ran_ue_ngap_id) {
  ranUeNgapId.setRanUeNgapId(ran_ue_ngap_id);

  Ngap_PDUSessionResourceModifyResponseIEs_t* ie =
      (Ngap_PDUSessionResourceModifyResponseIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyResponseIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_PDUSessionResourceModifyResponseIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId.encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyResponseIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyResponseMsg::
    setPduSessionResourceModifyResponseList(
        std::vector<PDUSessionResourceModifyResponseItem_t> list) {
  std::vector<PDUSessionResourceModifyItemModRes>
      m_pduSessionResourceModifyItemModRes;

  for (int i = 0; i < list.size(); i++) {
    PDUSessionID m_pDUSessionID = {};
    m_pDUSessionID.setPDUSessionID(list[i].pduSessionId);

    PDUSessionResourceModifyItemModRes item = {};

    item.setPDUSessionResourceModifyItemModRes(
        m_pDUSessionID, list[i].pduSessionResourceModifyResponseTransfer);
    m_pduSessionResourceModifyItemModRes.push_back(item);
  }

  pduSessionResourceModifyList.setPDUSessionResourceModifyListModRes(
      m_pduSessionResourceModifyItemModRes);

  Ngap_PDUSessionResourceModifyResponseIEs_t* ie =
      (Ngap_PDUSessionResourceModifyResponseIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyResponseIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionResourceModifyListModRes;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceModifyResponseIEs__value_PR_PDUSessionResourceModifyListModRes;

  int ret =
      pduSessionResourceModifyList.encode2PDUSessionResourceModifyListModRes(
          ie->value.choice.PDUSessionResourceModifyListModRes);
  if (!ret) {
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceModifyListModRes IE error");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyResponseIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceSetupListSUReq IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
int PduSessionResourceModifyResponseMsg::encode2buffer(
    uint8_t* buf, int buf_size) {
  asn_fprint(
      stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceModifyResponsePdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceModifyResponsePdu, buf,
      buf_size);
  Logger::ngap().debug("er.encoded (%d)", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyResponseMsg::encode2buffer_new(
    char* buf, int& encoded_size) {
  char* buffer = (char*) calloc(1, BUFFER_SIZE_1024);
  asn_fprint(
      stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceModifyResponsePdu);
  encoded_size = aper_encode_to_new_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceModifyResponsePdu,
      (void**) &buffer);

  Logger::ngap().debug("er.encoded (%d)", encoded_size);
  memcpy((void*) buf, (void*) buffer, encoded_size);
  free(buffer);
}

//------------------------------------------------------------------------------
bool PduSessionResourceModifyResponseMsg::decodefrompdu(
    Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  pduSessionResourceModifyResponsePdu = ngap_msg_pdu;

  if (pduSessionResourceModifyResponsePdu->present ==
      Ngap_NGAP_PDU_PR_successfulOutcome) {
    if (pduSessionResourceModifyResponsePdu->choice.successfulOutcome &&
        pduSessionResourceModifyResponsePdu->choice.successfulOutcome
                ->procedureCode ==
            Ngap_ProcedureCode_id_PDUSessionResourceModify &&
        pduSessionResourceModifyResponsePdu->choice.successfulOutcome
                ->criticality == Ngap_Criticality_reject &&
        pduSessionResourceModifyResponsePdu->choice.successfulOutcome->value
                .present ==
            Ngap_SuccessfulOutcome__value_PR_PDUSessionResourceModifyResponse) {
      pduSessionResourceModifyResponseIEs =
          &pduSessionResourceModifyResponsePdu->choice.successfulOutcome->value
               .choice.PDUSessionResourceModifyResponse;
    } else {
      Logger::ngap().error(
          "Check PDUSessionResourceModifyResponse message error!");

      return false;
    }
  } else {
    Logger::ngap().error("MessageType error!");
    return false;
  }

  for (int i = 0;
       i < pduSessionResourceModifyResponseIEs->protocolIEs.list.count; i++) {
    switch (
        pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyResponseIEs__value_PR_AMF_UE_NGAP_ID) {
          if (!amfUeNgapId.decodefromAMF_UE_NGAP_ID(
                  pduSessionResourceModifyResponseIEs->protocolIEs.list
                      .array[i]
                      ->value.choice.AMF_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP AMF_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP AMF_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID: {
        if (pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyResponseIEs__value_PR_RAN_UE_NGAP_ID) {
          if (!ranUeNgapId.decodefromRAN_UE_NGAP_ID(
                  pduSessionResourceModifyResponseIEs->protocolIEs.list
                      .array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceModifyListModRes: {
        if (pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            pduSessionResourceModifyResponseIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyResponseIEs__value_PR_PDUSessionResourceModifyListModRes) {
          if (!pduSessionResourceModifyList
                   .decodefromPDUSessionResourceModifyListModRes(
                       pduSessionResourceModifyResponseIEs->protocolIEs.list
                           .array[i]
                           ->value.choice.PDUSessionResourceModifyListModRes)) {
            Logger::ngap().error(
                "Decoded NGAP PDUSessionResourceModifyListModRes IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceModifyListModRes IE error");

          return false;
        }
      } break;
      default: {
        Logger::ngap().error("Decoded NGAP Message PDU error");

        return false;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
unsigned long PduSessionResourceModifyResponseMsg::getAmfUeNgapId() {
  return amfUeNgapId.getAMF_UE_NGAP_ID();
}

//------------------------------------------------------------------------------
uint32_t PduSessionResourceModifyResponseMsg::getRanUeNgapId() {
  return ranUeNgapId.getRanUeNgapId();
}

//------------------------------------------------------------------------------
bool PduSessionResourceModifyResponseMsg::
    getPduSessionResourceModifyResponseList(
        std::vector<PDUSessionResourceModifyResponseItem_t>& list) {
  std::vector<PDUSessionResourceModifyItemModRes>
      m_pduSessionResourceModifyItemModRes;
  int num = 0;
  pduSessionResourceModifyList.getPDUSessionResourceModifyListModRes(
      m_pduSessionResourceModifyItemModRes);

  for (int i = 0; i < m_pduSessionResourceModifyItemModRes.size(); i++) {
    PDUSessionResourceModifyResponseItem_t response;

    PDUSessionID m_pDUSessionID = {};

    m_pduSessionResourceModifyItemModRes[i]
        .getPDUSessionResourceModifyItemModRes(
            m_pDUSessionID, response.pduSessionResourceModifyResponseTransfer);

    m_pDUSessionID.getPDUSessionID(response.pduSessionId);

    list.push_back(response);
  }

  return true;
}

}  // namespace ngap
