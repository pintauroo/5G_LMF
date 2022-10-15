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

#include "HandoverRequiredMsg.hpp"
#include "logger.hpp"
#include "TAI.hpp"

extern "C" {
#include "Ngap_NGAP-PDU.h"
#include "asn_codecs.h"
#include "constr_TYPE.h"
#include "constraints.h"
#include "per_decoder.h"
#include "per_encoder.h"
}
#include <iostream>
#include <vector>

using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
HandoverRequiredMsg::HandoverRequiredMsg() {
  amfUeNgapId                         = nullptr;
  ranUeNgapId                         = nullptr;
  handovertype                        = nullptr;
  cause                               = nullptr;
  targetid                            = nullptr;
  directforwardingPathAvailability    = nullptr;
  PDUSessionResourceList              = nullptr;
  SourceToTarget_TransparentContainer = nullptr;
  handoverRequiredPdu                 = nullptr;
  handoverRequiredIEs                 = nullptr;
}

//------------------------------------------------------------------------------
HandoverRequiredMsg::~HandoverRequiredMsg() {}

//------------------------------------------------------------------------------
unsigned long HandoverRequiredMsg::getAmfUeNgapId() {
  if (amfUeNgapId)
    return amfUeNgapId->getAMF_UE_NGAP_ID();
  else
    return 0;
}

//------------------------------------------------------------------------------
uint32_t HandoverRequiredMsg::getRanUeNgapId() {
  if (ranUeNgapId)
    return ranUeNgapId->getRanUeNgapId();
  else
    return 0;
}

//------------------------------------------------------------------------------
Ngap_HandoverType_t HandoverRequiredMsg::getHandoverType() {
  if (handovertype)
    return *handovertype;
  else
    return Ngap_HandoverType_t();
}

//------------------------------------------------------------------------------
Ngap_Cause_PR HandoverRequiredMsg::getChoiceOfCause() {
  if (cause)
    return cause->getChoiceOfCause();
  else
    return Ngap_Cause_PR();
}

//------------------------------------------------------------------------------
long HandoverRequiredMsg::getCauseValue() {
  if (cause)
    return cause->getValue();
  else
    return 0;
}

//------------------------------------------------------------------------------
void HandoverRequiredMsg::getGlobalRanNodeId(GlobalgNBId*& ptr) {
  if (ptr && targetid) {
    if (targetid->present == Ngap_TargetID_PR_targetRANNodeID) {
      if (targetid->choice.targetRANNodeID->globalRANNodeID.present ==
          Ngap_GlobalRANNodeID_PR_globalGNB_ID) {
        ptr->decodefromGlobalgNBId(targetid->choice.targetRANNodeID
                                       ->globalRANNodeID.choice.globalGNB_ID);
        return;
      }
    }
  }
  return;
}

//------------------------------------------------------------------------------
void HandoverRequiredMsg::getTAI(TAI*& ptr) {
  if (ptr) {
    if (targetid->present == Ngap_TargetID_PR_targetRANNodeID)
      ptr->decodefromTAI(&(targetid->choice.targetRANNodeID->selectedTAI));
  }
}

//------------------------------------------------------------------------------
OCTET_STRING_t HandoverRequiredMsg::getSourceToTarget_TransparentContainer() {
  if (SourceToTarget_TransparentContainer)
    return *SourceToTarget_TransparentContainer;
  else
    return OCTET_STRING_t();
}

//------------------------------------------------------------------------------
bool HandoverRequiredMsg::getPDUSessionResourceList(
    std::vector<PDUSessionResourceItem_t>& list) {
  if (!PDUSessionResourceList) return false;

  PDUSessionResourceItemHORqd* m_pduSessionResourceItemHORqd = nullptr;
  int num                                                    = 0;
  PDUSessionResourceList->getPDUSessionResourceListHORqd(
      m_pduSessionResourceItemHORqd, num);
  if (!m_pduSessionResourceItemHORqd) return false;

  for (int i = 0; i < num; i++) {
    PDUSessionResourceItem_t response = {};

    PDUSessionID* m_pDUSessionID = nullptr;
    m_pduSessionResourceItemHORqd[i].getPDUSessionResourceItemHORqd(
        m_pDUSessionID, response.HandoverRequiredTransfer);
    m_pDUSessionID->getPDUSessionID(response.pduSessionId);

    list.push_back(response);
  }

  return true;
}

//------------------------------------------------------------------------------
long HandoverRequiredMsg::getDirectForwardingPathAvailability() {
  if (directforwardingPathAvailability)
    return *directforwardingPathAvailability;
  else
    return 0;
}

//------------------------------------------------------------------------------
bool HandoverRequiredMsg::decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  handoverRequiredPdu = ngap_msg_pdu;

  if (handoverRequiredPdu->present == Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (handoverRequiredPdu->choice.initiatingMessage &&
        handoverRequiredPdu->choice.initiatingMessage->procedureCode ==
            Ngap_ProcedureCode_id_HandoverPreparation &&
        handoverRequiredPdu->choice.initiatingMessage->criticality ==
            Ngap_Criticality_reject &&
        handoverRequiredPdu->choice.initiatingMessage->value.present ==
            Ngap_InitiatingMessage__value_PR_HandoverRequired) {
      handoverRequiredIEs = &handoverRequiredPdu->choice.initiatingMessage
                                 ->value.choice.HandoverRequired;
    } else {
      Logger::ngap().error("Check HandoverRequired message error!");
      return false;
    }
  } else {
    Logger::ngap().error("HandoverRequired MessageType error!");
    return false;
  }
  for (int i = 0; i < handoverRequiredIEs->protocolIEs.list.count; i++) {
    switch (handoverRequiredIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  handoverRequiredIEs->protocolIEs.list.array[i]
                      ->value.choice.AMF_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded ngap AMF_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded ngap AMF_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  handoverRequiredIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded ngap RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded ngap RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_HandoverType: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_HandoverType) {
          handovertype  = new Ngap_HandoverType_t();
          *handovertype = handoverRequiredIEs->protocolIEs.list.array[i]
                              ->value.choice.HandoverType;
        } else {
          Logger::ngap().error("Decoded ngap Handover Type error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_Cause: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_Cause) {
          cause = new Cause();
          if (!cause->decodefromCause(
                  &handoverRequiredIEs->protocolIEs.list.array[i]
                       ->value.choice.Cause)) {
            Logger::ngap().error("Decoded ngap Cause IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded ngap Cause IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_TargetID: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_TargetID) {
          targetid  = new Ngap_TargetID_t();
          *targetid = handoverRequiredIEs->protocolIEs.list.array[i]
                          ->value.choice.TargetID;
        } else {
          Logger::ngap().error("Decoded ngap TargetID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_DirectForwardingPathAvailability: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_DirectForwardingPathAvailability) {
          directforwardingPathAvailability =
              new Ngap_DirectForwardingPathAvailability_t();
          *directforwardingPathAvailability =
              handoverRequiredIEs->protocolIEs.list.array[i]
                  ->value.choice.DirectForwardingPathAvailability;
        } else {
          Logger::ngap().error(
              "Decoded ngap DirectForwardingPathAvailability IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceListHORqd: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_PDUSessionResourceListHORqd) {
          PDUSessionResourceList = new PDUSessionResourceListHORqd();
          if (!PDUSessionResourceList->decodefromPDUSessionResourceListHORqd(
                  &handoverRequiredIEs->protocolIEs.list.array[i]
                       ->value.choice.PDUSessionResourceListHORqd)) {
            Logger::ngap().error(
                "Decoded ngap PDUSessionResourceSetupListCxtRes IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded ngap PDUSessionResourceSetupListCxtRes IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_SourceToTarget_TransparentContainer: {
        if (handoverRequiredIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverRequiredIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverRequiredIEs__value_PR_SourceToTarget_TransparentContainer) {
          SourceToTarget_TransparentContainer =
              new Ngap_SourceToTarget_TransparentContainer_t();
          *SourceToTarget_TransparentContainer =
              handoverRequiredIEs->protocolIEs.list.array[i]
                  ->value.choice.SourceToTarget_TransparentContainer;
        } else {
          Logger::ngap().error(
              "Decoded ngap SourceToTarget_TransparentContainer IE error");
          return false;
        }
      } break;
      default: {
        Logger::ngap().error("Decoded ngap message PDU error");
        return false;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
int HandoverRequiredMsg::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, handoverRequiredPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, handoverRequiredPdu, buf, buf_size);
  Logger::ngap().debug(
      "Encode Handover Required to buffer, er.encoded( %d)", er.encoded);
  return er.encoded;
}

}  // namespace ngap
