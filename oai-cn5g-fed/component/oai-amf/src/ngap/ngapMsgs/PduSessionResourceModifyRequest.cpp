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

#include "PduSessionResourceModifyRequest.hpp"
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
PduSessionResourceModifyRequestMsg::PduSessionResourceModifyRequestMsg() {
  // Set Message Type
  pduSessionResourceModifyRequestPdu =
      (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType pdu = {};
  pdu.setProcedureCode(Ngap_ProcedureCode_id_PDUSessionResourceModify);

  pdu.setTypeOfMessage(Ngap_NGAP_PDU_PR_initiatingMessage);
  pdu.setCriticality(Ngap_Criticality_reject);
  pdu.setValuePresent(
      Ngap_InitiatingMessage__value_PR_PDUSessionResourceModifyRequest);
  pdu.encode2pdu(pduSessionResourceModifyRequestPdu);
  pduSessionResourceModifyRequestIEs =
      &(pduSessionResourceModifyRequestPdu->choice.initiatingMessage->value
            .choice.PDUSessionResourceModifyRequest);

  ranPagingPriority            = nullptr;
  pduSessionResourceModifyList = nullptr;
}

//------------------------------------------------------------------------------
PduSessionResourceModifyRequestMsg::~PduSessionResourceModifyRequestMsg() {
  if (pduSessionResourceModifyRequestPdu)
    free(pduSessionResourceModifyRequestPdu);
  // TODO:
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::setMessageType() {
  if (!pduSessionResourceModifyRequestPdu)
    pduSessionResourceModifyRequestPdu =
        (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType pdu = {};
  pdu.setProcedureCode(Ngap_ProcedureCode_id_PDUSessionResourceModify);

  pdu.setTypeOfMessage(Ngap_NGAP_PDU_PR_initiatingMessage);
  pdu.setCriticality(Ngap_Criticality_reject);
  pdu.setValuePresent(
      Ngap_InitiatingMessage__value_PR_PDUSessionResourceModifyRequest);
  pdu.encode2pdu(pduSessionResourceModifyRequestPdu);
  pduSessionResourceModifyRequestIEs =
      &(pduSessionResourceModifyRequestPdu->choice.initiatingMessage->value
            .choice.PDUSessionResourceModifyRequest);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::setAmfUeNgapId(unsigned long id) {
  amfUeNgapId.setAMF_UE_NGAP_ID(id);

  Ngap_PDUSessionResourceModifyRequestIEs_t* ie =
      (Ngap_PDUSessionResourceModifyRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceModifyRequestIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId.encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::setRanUeNgapId(
    uint32_t ran_ue_ngap_id) {
  ranUeNgapId.setRanUeNgapId(ran_ue_ngap_id);

  Ngap_PDUSessionResourceModifyRequestIEs_t* ie =
      (Ngap_PDUSessionResourceModifyRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceModifyRequestIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId.encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::setRanPagingPriority(
    uint8_t priority) {
  if (!ranPagingPriority) ranPagingPriority = new RANPagingPriority();

  ranPagingPriority->setRANPagingPriority(priority);

  Ngap_PDUSessionResourceModifyRequestIEs_t* ie =
      (Ngap_PDUSessionResourceModifyRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RANPagingPriority;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_PDUSessionResourceModifyRequestIEs__value_PR_RANPagingPriority;

  int ret = ranPagingPriority->encode2RANPagingPriority(
      ie->value.choice.RANPagingPriority);
  if (!ret) {
    Logger::ngap().error("Encode NGAP RANPagingPriority IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP RANPagingPriority IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::setPduSessionResourceModifyRequestList(
    std::vector<PDUSessionResourceModifyRequestItem_t> list) {
  if (!pduSessionResourceModifyList)
    pduSessionResourceModifyList = new PDUSessionResourceModifyListModReq();

  std::vector<PDUSessionResourceModifyItemModReq>
      m_pduSessionResourceModifyItemModReq;

  for (int i = 0; i < list.size(); i++) {
    PDUSessionID m_pDUSessionID = {};
    m_pDUSessionID.setPDUSessionID(list[i].pduSessionId);
    NAS_PDU m_nAS_PDU = {};
    if (list[i].pduSessionNAS_PDU) {
      m_nAS_PDU.setNasPdu(
          list[i].pduSessionNAS_PDU, list[i].sizeofpduSessionNAS_PDU);
    }
    S_NSSAI s_NSSAI = {};
    s_NSSAI.setSd(list[i].s_nssai.sd);
    s_NSSAI.setSst(list[i].s_nssai.sst);

    PDUSessionResourceModifyItemModReq item = {};

    item.setPDUSessionResourceModifyItemModReq(
        m_pDUSessionID, m_nAS_PDU,
        list[i].pduSessionResourceModifyRequestTransfer, s_NSSAI);
    m_pduSessionResourceModifyItemModReq.push_back(item);
  }

  pduSessionResourceModifyList->setPDUSessionResourceModifyListModReq(
      m_pduSessionResourceModifyItemModReq);

  // pduSessionResourceSetupRequestList->setPDUSessionResourceSetupListSUReq(
  //    m_pduSessionResourceSetupItemSUReq, list.size());

  Ngap_PDUSessionResourceModifyRequestIEs_t* ie =
      (Ngap_PDUSessionResourceModifyRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceModifyRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionResourceModifyListModReq;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceModifyRequestIEs__value_PR_PDUSessionResourceModifyListModReq;

  int ret =
      pduSessionResourceModifyList->encode2PDUSessionResourceModifyListModReq(
          ie->value.choice.PDUSessionResourceModifyListModReq);
  if (!ret) {
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceModifyListModReq IE error");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceModifyRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceSetupListSUReq IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
int PduSessionResourceModifyRequestMsg::encode2buffer(
    uint8_t* buf, int buf_size) {
  asn_fprint(
      stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceModifyRequestPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceModifyRequestPdu, buf,
      buf_size);
  Logger::ngap().debug("er.encoded (%d)", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
void PduSessionResourceModifyRequestMsg::encode2buffer_new(
    char* buf, int& encoded_size) {
  char* buffer = (char*) calloc(1, BUFFER_SIZE_1024);
  asn_fprint(
      stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceModifyRequestPdu);
  encoded_size = aper_encode_to_new_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceModifyRequestPdu,
      (void**) &buffer);

  Logger::ngap().debug("er.encoded (%d)", encoded_size);
  memcpy((void*) buf, (void*) buffer, encoded_size);
  free(buffer);
}

//------------------------------------------------------------------------------
bool PduSessionResourceModifyRequestMsg::decodefrompdu(
    Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  pduSessionResourceModifyRequestPdu = ngap_msg_pdu;

  if (pduSessionResourceModifyRequestPdu->present ==
      Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (pduSessionResourceModifyRequestPdu->choice.initiatingMessage &&
        pduSessionResourceModifyRequestPdu->choice.initiatingMessage
                ->procedureCode ==
            Ngap_ProcedureCode_id_PDUSessionResourceModify &&
        pduSessionResourceModifyRequestPdu->choice.initiatingMessage
                ->criticality == Ngap_Criticality_reject &&
        pduSessionResourceModifyRequestPdu->choice.initiatingMessage->value
                .present ==
            Ngap_InitiatingMessage__value_PR_PDUSessionResourceModifyRequest) {
      pduSessionResourceModifyRequestIEs =
          &pduSessionResourceModifyRequestPdu->choice.initiatingMessage->value
               .choice.PDUSessionResourceModifyRequest;
    } else {
      Logger::ngap().error(
          "Check PDUSessionResourceModifyRequest message error!");

      return false;
    }
  } else {
    Logger::ngap().error("MessageType error!");
    return false;
  }
  for (int i = 0;
       i < pduSessionResourceModifyRequestIEs->protocolIEs.list.count; i++) {
    switch (pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyRequestIEs__value_PR_AMF_UE_NGAP_ID) {
          if (!amfUeNgapId.decodefromAMF_UE_NGAP_ID(
                  pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
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
        if (pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyRequestIEs__value_PR_RAN_UE_NGAP_ID) {
          if (!ranUeNgapId.decodefromRAN_UE_NGAP_ID(
                  pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_RANPagingPriority: {
        if (pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyRequestIEs__value_PR_RANPagingPriority) {
          ranPagingPriority = new RANPagingPriority();
          if (!ranPagingPriority->decodefromRANPagingPriority(
                  pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.RANPagingPriority)) {
            Logger::ngap().error("Decoded NGAP RANPagingPriority IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RANPagingPriority IE error");
          return false;
        }
      } break;

      case Ngap_ProtocolIE_ID_id_PDUSessionResourceModifyListModReq: {
        if (pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceModifyRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceModifyRequestIEs__value_PR_PDUSessionResourceModifyListModReq) {
          pduSessionResourceModifyList =
              new PDUSessionResourceModifyListModReq();
          if (!pduSessionResourceModifyList
                   ->decodefromPDUSessionResourceModifyListModReq(
                       pduSessionResourceModifyRequestIEs->protocolIEs.list
                           .array[i]
                           ->value.choice.PDUSessionResourceModifyListModReq)) {
            Logger::ngap().error(
                "Decoded NGAP PDUSessionResourceModifyListModReq IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceModifyListModReq IE error");

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
unsigned long PduSessionResourceModifyRequestMsg::getAmfUeNgapId() {
  return amfUeNgapId.getAMF_UE_NGAP_ID();
}

//------------------------------------------------------------------------------
uint32_t PduSessionResourceModifyRequestMsg::getRanUeNgapId() {
  return ranUeNgapId.getRanUeNgapId();
}

//------------------------------------------------------------------------------
int PduSessionResourceModifyRequestMsg::getRanPagingPriority() {
  if (!ranPagingPriority) return -1;
  return ranPagingPriority->getRANPagingPriority();
}

//------------------------------------------------------------------------------
bool PduSessionResourceModifyRequestMsg::getPduSessionResourceModifyRequestList(
    std::vector<PDUSessionResourceModifyRequestItem_t>& list) {
  if (!pduSessionResourceModifyList) return false;
  std::vector<PDUSessionResourceModifyItemModReq>
      m_pduSessionResourceModifyItemModReq;
  int num = 0;
  pduSessionResourceModifyList->getPDUSessionResourceModifyListModReq(
      m_pduSessionResourceModifyItemModReq);

  for (int i = 0; i < m_pduSessionResourceModifyItemModReq.size(); i++) {
    PDUSessionResourceModifyRequestItem_t request;

    PDUSessionID m_pDUSessionID = {};
    NAS_PDU m_nAS_PDU           = {};
    S_NSSAI s_NSSAI             = {};

    m_pduSessionResourceModifyItemModReq[i]
        .getPDUSessionResourceModifyItemModReq(
            m_pDUSessionID, m_nAS_PDU,
            request.pduSessionResourceModifyRequestTransfer, s_NSSAI);

    m_pDUSessionID.getPDUSessionID(request.pduSessionId);

    m_nAS_PDU.getNasPdu(
        request.pduSessionNAS_PDU, request.sizeofpduSessionNAS_PDU);
    s_NSSAI.getSd(request.s_nssai.sd);
    s_NSSAI.getSst(request.s_nssai.sst);
    list.push_back(request);
  }

  return true;
}

}  // namespace ngap
