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

#include "HandoverCommandMsg.hpp"
#include "logger.hpp"

extern "C" {
#include "Ngap_NGAP-PDU.h"
#include "Ngap_PDUSessionResourceHandoverItem.h"
#include "asn_codecs.h"
#include "constr_TYPE.h"
#include "constraints.h"
#include "dynamic_memory_check.h"
#include "per_decoder.h"
#include "per_encoder.h"
}

#include <iostream>
#include <vector>

using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
HandoverCommandMsg::HandoverCommandMsg() {
  amfUeNgapId                          = nullptr;
  ranUeNgapId                          = nullptr;
  ngap_handovertype                    = nullptr;
  NASSecurityParametersFromNGRAN       = nullptr;
  PDUSessionResourceHandoverList       = nullptr;
  PDUSessionResourceToReleaseListHOCmd = nullptr;
  TargetToSource_TransparentContainer  = nullptr;
  CriticalityDiagnostics               = nullptr;
  handoverCommandPdu                   = nullptr;
  handoverCommandIEs                   = nullptr;
}

//------------------------------------------------------------------------------
HandoverCommandMsg::~HandoverCommandMsg() {}

//------------------------------------------------------------------------------
unsigned long HandoverCommandMsg::getAmfUeNgapId() {
  if (amfUeNgapId)
    return amfUeNgapId->getAMF_UE_NGAP_ID();
  else
    return 0;
}

//------------------------------------------------------------------------------
uint32_t HandoverCommandMsg::getRanUeNgapId() {
  if (ranUeNgapId)
    return ranUeNgapId->getRanUeNgapId();
  else
    return 0;
}

//------------------------------------------------------------------------------
bool HandoverCommandMsg::decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  if (!ngap_msg_pdu) return false;
  handoverCommandPdu = ngap_msg_pdu;

  if (handoverCommandPdu->present == Ngap_NGAP_PDU_PR_successfulOutcome) {
    if (handoverCommandPdu->choice.successfulOutcome &&
        handoverCommandPdu->choice.successfulOutcome->procedureCode ==
            Ngap_ProcedureCode_id_HandoverPreparation &&
        handoverCommandPdu->choice.successfulOutcome->criticality ==
            Ngap_Criticality_reject &&
        handoverCommandPdu->choice.successfulOutcome->value.present ==
            Ngap_SuccessfulOutcome__value_PR_HandoverCommand) {
      handoverCommandIEs = &handoverCommandPdu->choice.successfulOutcome->value
                                .choice.HandoverCommand;
    } else {
      Logger::ngap().error("Check Handover Command message error");
      return false;
    }
  } else {
    Logger::ngap().error("Handover Command MessageType error");
    return false;
  }
  for (int i = 0; i < handoverCommandIEs->protocolIEs.list.count; i++) {
    switch (handoverCommandIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  handoverCommandIEs->protocolIEs.list.array[i]
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
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  handoverCommandIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_HandoverType: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_HandoverType) {
          ngap_handovertype  = new Ngap_HandoverType_t();
          *ngap_handovertype = handoverCommandIEs->protocolIEs.list.array[i]
                                   ->value.choice.HandoverType;
        } else {
          Logger::ngap().error("Decoded NGAP Handover Type IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceHandoverList: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_PDUSessionResourceHandoverList) {
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceHandoverList IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceToReleaseListHOCmd: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_PDUSessionResourceToReleaseListHOCmd) {
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceToReleaseListHOCmd IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_TargetToSource_TransparentContainer: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_TargetToSource_TransparentContainer) {
        } else {
          Logger::ngap().error(
              "Decoded NGAP TargetToSource_TransparentContainer IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_CriticalityDiagnostics: {
        if (handoverCommandIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            handoverCommandIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverCommandIEs__value_PR_CriticalityDiagnostics) {
        } else {
          Logger::ngap().error("Decoded NGAP CriticalityDiagnostics IE error");
          return false;
        }
      } break;
      default: {
        Logger::ngap().error("Decoded NGAP message PDU error");
        return false;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
int HandoverCommandMsg::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, handoverCommandPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, handoverCommandPdu, buf, buf_size);
  Logger::ngap().debug(
      "Encode Handover Command to buffer, er.encoded( %d )", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setMessageType() {
  if (!handoverCommandPdu)
    handoverCommandPdu = (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType HandoverCommandMessageTypeIE;
  HandoverCommandMessageTypeIE.setProcedureCode(
      Ngap_ProcedureCode_id_HandoverPreparation);
  HandoverCommandMessageTypeIE.setTypeOfMessage(
      Ngap_NGAP_PDU_PR_successfulOutcome);
  HandoverCommandMessageTypeIE.setCriticality(Ngap_Criticality_reject);
  HandoverCommandMessageTypeIE.setValuePresent(
      Ngap_SuccessfulOutcome__value_PR_HandoverCommand);

  if (HandoverCommandMessageTypeIE.getProcedureCode() ==
          Ngap_ProcedureCode_id_HandoverPreparation &&
      HandoverCommandMessageTypeIE.getTypeOfMessage() ==
          Ngap_NGAP_PDU_PR_successfulOutcome) {
    HandoverCommandMessageTypeIE.encode2pdu(handoverCommandPdu);
    handoverCommandIEs = &(handoverCommandPdu->choice.successfulOutcome->value
                               .choice.HandoverCommand);
  } else {
    Logger::ngap().warn(
        "This information doesn't refer to Handover Command message");
  }
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setAmfUeNgapId(unsigned long id) {
  if (!amfUeNgapId) amfUeNgapId = new AMF_UE_NGAP_ID();
  amfUeNgapId->setAMF_UE_NGAP_ID(id);

  Ngap_HandoverCommandIEs_t* ie =
      (Ngap_HandoverCommandIEs_t*) calloc(1, sizeof(Ngap_HandoverCommandIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_HandoverCommandIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId->encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&handoverCommandIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setRanUeNgapId(uint32_t ran_ue_ngap_id) {
  if (!ranUeNgapId) ranUeNgapId = new RAN_UE_NGAP_ID();
  ranUeNgapId->setRanUeNgapId(ran_ue_ngap_id);

  Ngap_HandoverCommandIEs_t* ie =
      (Ngap_HandoverCommandIEs_t*) calloc(1, sizeof(Ngap_HandoverCommandIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_HandoverCommandIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId->encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&handoverCommandIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error");

  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setHandoverType(long type) {
  if (!ngap_handovertype) ngap_handovertype = new Ngap_HandoverType_t();
  Ngap_HandoverCommandIEs_t* ie =
      (Ngap_HandoverCommandIEs_t*) calloc(1, sizeof(Ngap_HandoverCommandIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_HandoverType;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_HandoverCommandIEs__value_PR_HandoverType;
  ie->value.choice.HandoverType = type;
  int ret = ASN_SEQUENCE_ADD(&handoverCommandIEs->protocolIEs.list, ie);

  if (ret != 0) Logger::ngap().error("Encode HandoverType IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setPduSessionResourceHandoverList(
    std::vector<PDUSessionResourceHandoverItem_t> list) {
  if (!PDUSessionResourceHandoverList)
    PDUSessionResourceHandoverList =
        new Ngap_PDUSessionResourceHandoverList_t();
  Ngap_HandoverCommandIEs_t* ie =
      (Ngap_HandoverCommandIEs_t*) calloc(1, sizeof(Ngap_HandoverCommandIEs_t));

  Ngap_PDUSessionResourceHandoverItem_t* item =
      (Ngap_PDUSessionResourceHandoverItem_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceHandoverItem_t));
  for (int i = 0; i < list.size(); i++) {
    item->pDUSessionID            = list[i].pduSessionId;
    item->handoverCommandTransfer = list[i].HandoverCommandTransfer;
    int ret = ASN_SEQUENCE_ADD(&PDUSessionResourceHandoverList->list, item);

    if (ret != 0)
      Logger::ngap().error(
          "Encode PDUSessionResourceHandoverListItem IE error");
  }

  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionResourceHandoverList;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_HandoverCommandIEs__value_PR_PDUSessionResourceHandoverList;
  ie->value.choice.PDUSessionResourceHandoverList =
      *PDUSessionResourceHandoverList;
  int ret = ASN_SEQUENCE_ADD(&handoverCommandIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error("Encode PDUSessionResourceHandoverList IE error");

  // free_wrapper((void**) &item);
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverCommandMsg::setTargetToSource_TransparentContainer(
    OCTET_STRING_t targetTosource) {
  if (!TargetToSource_TransparentContainer)
    TargetToSource_TransparentContainer =
        new Ngap_TargetToSource_TransparentContainer_t();

  Ngap_HandoverCommandIEs_t* ie =
      (Ngap_HandoverCommandIEs_t*) calloc(1, sizeof(Ngap_HandoverCommandIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_TargetToSource_TransparentContainer;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_HandoverCommandIEs__value_PR_TargetToSource_TransparentContainer;
  ie->value.choice.TargetToSource_TransparentContainer = targetTosource;
  int ret = ASN_SEQUENCE_ADD(&handoverCommandIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode HandoverType IE error");
  // free_wrapper((void**) &ie);
}

}  // namespace ngap
