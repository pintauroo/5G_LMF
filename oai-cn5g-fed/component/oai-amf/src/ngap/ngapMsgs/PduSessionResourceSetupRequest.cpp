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

#include "PduSessionResourceSetupRequest.hpp"
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
PduSessionResourceSetupRequestMsg::PduSessionResourceSetupRequestMsg() {
  pduSessionResourceSetupRequestPdu  = nullptr;
  pduSessionResourceSetupRequestIEs  = nullptr;
  amfUeNgapId                        = nullptr;
  ranUeNgapId                        = nullptr;
  ranPagingPriority                  = nullptr;
  nasPdu                             = nullptr;
  pduSessionResourceSetupRequestList = nullptr;
  uEAggregateMaxBitRate              = nullptr;
  pduSessionAggregateMaximumBitRate  = nullptr;
}

//------------------------------------------------------------------------------
PduSessionResourceSetupRequestMsg::~PduSessionResourceSetupRequestMsg() {}
//-----------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setUEAggregateMaxBitRate(
    long bit_rate_downlink, long bit_rate_uplink) {
  if (!uEAggregateMaxBitRate)
    uEAggregateMaxBitRate = new UEAggregateMaxBitRate();

  uEAggregateMaxBitRate->setUEAggregateMaxBitRate(
      bit_rate_downlink, bit_rate_uplink);

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_UEAggregateMaximumBitRate;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_PDUSessionResourceSetupRequestIEs__value_PR_UEAggregateMaximumBitRate;

  int ret = uEAggregateMaxBitRate->encode2UEAggregateMaxBitRate(
      ie->value.choice.UEAggregateMaximumBitRate);
  if (!ret) {
    Logger::ngap().error("Encode NGAP UEAggregateMaxBitRate IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error("Encode NGAP UEAggregateMaxBitRate IE error");
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setMessageType() {
  if (!pduSessionResourceSetupRequestPdu)
    pduSessionResourceSetupRequestPdu =
        (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType initialContextSetupRequestPduTypeIE;
  initialContextSetupRequestPduTypeIE.setProcedureCode(
      Ngap_ProcedureCode_id_PDUSessionResourceSetup);
  initialContextSetupRequestPduTypeIE.setTypeOfMessage(
      Ngap_NGAP_PDU_PR_initiatingMessage);
  initialContextSetupRequestPduTypeIE.setCriticality(Ngap_Criticality_reject);
  initialContextSetupRequestPduTypeIE.setValuePresent(
      Ngap_InitiatingMessage__value_PR_PDUSessionResourceSetupRequest);

  if (initialContextSetupRequestPduTypeIE.getProcedureCode() ==
          Ngap_ProcedureCode_id_PDUSessionResourceSetup &&
      initialContextSetupRequestPduTypeIE.getTypeOfMessage() ==
          Ngap_NGAP_PDU_PR_initiatingMessage &&
      initialContextSetupRequestPduTypeIE.getCriticality() ==
          Ngap_Criticality_reject) {
    initialContextSetupRequestPduTypeIE.encode2pdu(
        pduSessionResourceSetupRequestPdu);
    pduSessionResourceSetupRequestIEs =
        &(pduSessionResourceSetupRequestPdu->choice.initiatingMessage->value
              .choice.PDUSessionResourceSetupRequest);
  } else {
    Logger::ngap().warn(
        "This information doesn't refer to PDUSessionResourceSetupRequest "
        "message!");
  }
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setAmfUeNgapId(unsigned long id) {
  if (!amfUeNgapId) amfUeNgapId = new AMF_UE_NGAP_ID();
  amfUeNgapId->setAMF_UE_NGAP_ID(id);

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceSetupRequestIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId->encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP AMF_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setRanUeNgapId(
    uint32_t ran_ue_ngap_id) {
  if (!ranUeNgapId) ranUeNgapId = new RAN_UE_NGAP_ID();
  ranUeNgapId->setRanUeNgapId(ran_ue_ngap_id);

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceSetupRequestIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId->encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP RAN_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setRanPagingPriority(uint8_t priority) {
  if (!ranPagingPriority) ranPagingPriority = new RANPagingPriority();

  ranPagingPriority->setRANPagingPriority(priority);

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RANPagingPriority;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_PDUSessionResourceSetupRequestIEs__value_PR_RANPagingPriority;

  int ret = ranPagingPriority->encode2RANPagingPriority(
      ie->value.choice.RANPagingPriority);
  if (!ret) {
    Logger::ngap().error("Encode NGAP RANPagingPriority IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP RANPagingPriority IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setNasPdu(
    uint8_t* nas, size_t sizeofnas) {
  if (!nasPdu) nasPdu = new NAS_PDU();

  nasPdu->setNasPdu(nas, sizeofnas);

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_NAS_PDU;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_PDUSessionResourceSetupRequestIEs__value_PR_NAS_PDU;

  int ret = nasPdu->encode2octetstring(ie->value.choice.NAS_PDU);
  if (!ret) {
    Logger::ngap().error("Encode NGAP NAS_PDU IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NGAP NAS_PDU IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setPduSessionResourceSetupRequestList(
    std::vector<PDUSessionResourceSetupRequestItem_t> list) {
  if (!pduSessionResourceSetupRequestList)
    pduSessionResourceSetupRequestList = new PDUSessionResourceSetupListSUReq();
  PDUSessionResourceSetupItemSUReq* m_pduSessionResourceSetupItemSUReq =
      new PDUSessionResourceSetupItemSUReq[list.size()]();

  for (int i = 0; i < list.size(); i++) {
    PDUSessionID* m_pDUSessionID = new PDUSessionID();
    m_pDUSessionID->setPDUSessionID(list[i].pduSessionId);
    NAS_PDU* m_nAS_PDU = NULL;
    if (list[i].pduSessionNAS_PDU) {
      m_nAS_PDU = new NAS_PDU();
      m_nAS_PDU->setNasPdu(
          list[i].pduSessionNAS_PDU, list[i].sizeofpduSessionNAS_PDU);
    }
    S_NSSAI* m_s_NSSAI = new S_NSSAI();
    m_s_NSSAI->setSst(list[i].s_nssai.sst);
    if (list[i].s_nssai.sd.size()) m_s_NSSAI->setSd(list[i].s_nssai.sd);
    m_pduSessionResourceSetupItemSUReq[i].setPDUSessionResourceSetupItemSUReq(
        m_pDUSessionID, m_nAS_PDU, m_s_NSSAI,
        list[i].pduSessionResourceSetupRequestTransfer);
  }

  pduSessionResourceSetupRequestList->setPDUSessionResourceSetupListSUReq(
      m_pduSessionResourceSetupItemSUReq, list.size());

  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionResourceSetupListSUReq;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_PDUSessionResourceSetupRequestIEs__value_PR_PDUSessionResourceSetupListSUReq;

  int ret = pduSessionResourceSetupRequestList
                ->encode2PDUSessionResourceSetupListSUReq(
                    &ie->value.choice.PDUSessionResourceSetupListSUReq);
  if (!ret) {
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceSetupListSUReq IE error");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(
      &pduSessionResourceSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error(
        "Encode NGAP PDUSessionResourceSetupListSUReq IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::setPduSessionAggregateMaximumBitRate(
    long bit_rate_downlink, long bit_rate_uplink) {
  if (!pduSessionAggregateMaximumBitRate)
    pduSessionAggregateMaximumBitRate = new PduSessionAggregateMaximumBitRate();
  pduSessionAggregateMaximumBitRate->setPduSessionAggregateMaximumBitRate(
      bit_rate_downlink, bit_rate_uplink);
  Ngap_PDUSessionResourceSetupRequestIEs_t* ie =
      (Ngap_PDUSessionResourceSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionAggregateMaximumBitRate;
  ie->criticality = Ngap_Criticality_ignore;
  free_wrapper((void**) &ie);
  // TODO
}

//------------------------------------------------------------------------------
int PduSessionResourceSetupRequestMsg::encode2buffer(
    uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceSetupRequestPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceSetupRequestPdu, buf,
      buf_size);
  Logger::ngap().debug("er.encoded (%d)", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
void PduSessionResourceSetupRequestMsg::encode2buffer_new(
    char* buf, int& encoded_size) {
  char* buffer = (char*) calloc(1, 1024);  // TODO: remove hardcoded value
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, pduSessionResourceSetupRequestPdu);
  encoded_size = aper_encode_to_new_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pduSessionResourceSetupRequestPdu,
      (void**) &buffer);

  Logger::ngap().debug("er.encoded (%d)", encoded_size);
  memcpy((void*) buf, (void*) buffer, encoded_size);
  free(buffer);
}

//------------------------------------------------------------------------------
// Decapsulation
bool PduSessionResourceSetupRequestMsg::decodefrompdu(
    Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  pduSessionResourceSetupRequestPdu = ngap_msg_pdu;

  if (pduSessionResourceSetupRequestPdu->present ==
      Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (pduSessionResourceSetupRequestPdu->choice.initiatingMessage &&
        pduSessionResourceSetupRequestPdu->choice.initiatingMessage
                ->procedureCode ==
            Ngap_ProcedureCode_id_PDUSessionResourceSetup &&
        pduSessionResourceSetupRequestPdu->choice.initiatingMessage
                ->criticality == Ngap_Criticality_reject &&
        pduSessionResourceSetupRequestPdu->choice.initiatingMessage->value
                .present ==
            Ngap_InitiatingMessage__value_PR_PDUSessionResourceSetupRequest) {
      pduSessionResourceSetupRequestIEs =
          &pduSessionResourceSetupRequestPdu->choice.initiatingMessage->value
               .choice.PDUSessionResourceSetupRequest;
    } else {
      Logger::ngap().error(
          "Check PDUSessionResourceSetupRequest message error!");

      return false;
    }
  } else {
    Logger::ngap().error("MessageType error!");
    return false;
  }
  for (int i = 0; i < pduSessionResourceSetupRequestIEs->protocolIEs.list.count;
       i++) {
    switch (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceSetupRequestIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
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
        if (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceSetupRequestIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
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
        if (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceSetupRequestIEs__value_PR_RANPagingPriority) {
          ranPagingPriority = new RANPagingPriority();
          if (!ranPagingPriority->decodefromRANPagingPriority(
                  pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.RANPagingPriority)) {
            Logger::ngap().error("Decoded NGAP RANPagingPriority IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RANPagingPriority IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_NAS_PDU: {
        if (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceSetupRequestIEs__value_PR_NAS_PDU) {
          nasPdu = new NAS_PDU();
          if (!nasPdu->decodefromoctetstring(
                  pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.NAS_PDU)) {
            Logger::ngap().error("Decoded NGAP NAS_PDU IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP NAS_PDU IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceSetupListSUReq: {
        if (pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            pduSessionResourceSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_PDUSessionResourceSetupRequestIEs__value_PR_PDUSessionResourceSetupListSUReq) {
          pduSessionResourceSetupRequestList =
              new PDUSessionResourceSetupListSUReq();
          if (!pduSessionResourceSetupRequestList
                   ->decodefromPDUSessionResourceSetupListSUReq(
                       &pduSessionResourceSetupRequestIEs->protocolIEs.list
                            .array[i]
                            ->value.choice.PDUSessionResourceSetupListSUReq)) {
            Logger::ngap().error(
                "Decoded NGAP PDUSessionResourceSetupListSUReq IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceSetupListSUReq IE error");

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
unsigned long PduSessionResourceSetupRequestMsg::getAmfUeNgapId() {
  if (!amfUeNgapId) return -1;
  return amfUeNgapId->getAMF_UE_NGAP_ID();
}

//------------------------------------------------------------------------------
uint32_t PduSessionResourceSetupRequestMsg::getRanUeNgapId() {
  if (!ranUeNgapId) return 0;
  return ranUeNgapId->getRanUeNgapId();
}

//------------------------------------------------------------------------------
int PduSessionResourceSetupRequestMsg::getRanPagingPriority() {
  if (!ranPagingPriority) return -1;
  return ranPagingPriority->getRANPagingPriority();
}

//------------------------------------------------------------------------------
bool PduSessionResourceSetupRequestMsg::getNasPdu(
    uint8_t*& nas, size_t& sizeofnas) {
  if (!nasPdu) return false;
  if (!nasPdu->getNasPdu(nas, sizeofnas)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool PduSessionResourceSetupRequestMsg::getPduSessionResourceSetupRequestList(
    std::vector<PDUSessionResourceSetupRequestItem_t>& list) {
  if (!pduSessionResourceSetupRequestList) return false;
  PDUSessionResourceSetupItemSUReq* m_pduSessionResourceSetupItemSUReq;
  int num = 0;
  pduSessionResourceSetupRequestList->getPDUSessionResourceSetupListSUReq(
      m_pduSessionResourceSetupItemSUReq, num);

  for (int i = 0; i < num; i++) {
    PDUSessionResourceSetupRequestItem_t request;

    PDUSessionID* m_pDUSessionID;
    NAS_PDU* m_nAS_PDU;
    S_NSSAI* m_s_NSSAI;
    m_pduSessionResourceSetupItemSUReq[i].getPDUSessionResourceSetupItemSUReq(
        m_pDUSessionID, m_nAS_PDU, m_s_NSSAI,
        request.pduSessionResourceSetupRequestTransfer);
    m_pDUSessionID->getPDUSessionID(request.pduSessionId);
    m_s_NSSAI->getSst(request.s_nssai.sst);
    m_s_NSSAI->getSd(request.s_nssai.sd);
    if (m_nAS_PDU) {
      m_nAS_PDU->getNasPdu(
          request.pduSessionNAS_PDU, request.sizeofpduSessionNAS_PDU);
    } else {
      request.pduSessionNAS_PDU       = NULL;
      request.sizeofpduSessionNAS_PDU = 0;
    }

    list.push_back(request);
  }

  return true;
}

}  // namespace ngap
