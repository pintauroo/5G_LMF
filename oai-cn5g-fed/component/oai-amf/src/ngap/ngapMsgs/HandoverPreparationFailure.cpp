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

#include "HandoverPreparationFailure.hpp"
#include "logger.hpp"

extern "C" {
#include "Ngap_NGAP-PDU.h"
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
HandoverPreparationFailure::HandoverPreparationFailure() {
  amfUeNgapId             = nullptr;
  ranUeNgapId             = nullptr;
  cause                   = nullptr;
  hoPreparationFailureIEs = nullptr;
  hoPreparationFailurePdu = nullptr;
  CriticalityDiagnostics  = nullptr;
}

//------------------------------------------------------------------------------
HandoverPreparationFailure::~HandoverPreparationFailure() {}

//------------------------------------------------------------------------------
unsigned long HandoverPreparationFailure::getAmfUeNgapId() const {
  if (amfUeNgapId)
    return amfUeNgapId->getAMF_UE_NGAP_ID();
  else
    return 0;
}

//------------------------------------------------------------------------------
uint32_t HandoverPreparationFailure::getRanUeNgapId() const {
  if (ranUeNgapId)
    return ranUeNgapId->getRanUeNgapId();
  else
    return 0;
}

//------------------------------------------------------------------------------
bool HandoverPreparationFailure::decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  if (!ngap_msg_pdu) return false;
  hoPreparationFailurePdu = ngap_msg_pdu;

  if (hoPreparationFailurePdu->present ==
      Ngap_NGAP_PDU_PR_unsuccessfulOutcome) {
    if (hoPreparationFailurePdu->choice.unsuccessfulOutcome &&
        hoPreparationFailurePdu->choice.unsuccessfulOutcome->procedureCode ==
            Ngap_ProcedureCode_id_HandoverPreparation &&
        hoPreparationFailurePdu->choice.unsuccessfulOutcome->criticality ==
            Ngap_Criticality_reject &&
        hoPreparationFailurePdu->choice.unsuccessfulOutcome->value.present ==
            Ngap_UnsuccessfulOutcome__value_PR_HandoverPreparationFailure) {
      hoPreparationFailureIEs =
          &hoPreparationFailurePdu->choice.unsuccessfulOutcome->value.choice
               .HandoverPreparationFailure;
    } else {
      Logger::ngap().error("Check HandoverPreparationFailure message error");
      return false;
    }
  } else {
    Logger::ngap().error("HandoverPreparationFailure MessageType error");
    return false;
  }

  for (int i = 0; i < hoPreparationFailureIEs->protocolIEs.list.count; i++) {
    switch (hoPreparationFailureIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (hoPreparationFailureIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            hoPreparationFailureIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverPreparationFailureIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  hoPreparationFailureIEs->protocolIEs.list.array[i]
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
        if (hoPreparationFailureIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            hoPreparationFailureIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverPreparationFailureIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  hoPreparationFailureIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_Cause: {
        if (hoPreparationFailureIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            hoPreparationFailureIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverPreparationFailureIEs__value_PR_Cause) {
          cause = new Cause();
          if (!cause->decodefromCause(
                  &hoPreparationFailureIEs->protocolIEs.list.array[i]
                       ->value.choice.Cause)) {
            Logger::ngap().error("Decoded NGAP Cause IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP Cause IE error");
          return false;
        }

      } break;
      case Ngap_ProtocolIE_ID_id_CriticalityDiagnostics: {
        if (hoPreparationFailureIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            hoPreparationFailureIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverPreparationFailureIEs__value_PR_CriticalityDiagnostics) {
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
int HandoverPreparationFailure::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, hoPreparationFailurePdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, hoPreparationFailurePdu, buf, buf_size);
  Logger::ngap().debug(
      "Encode Handover Preparation Failure to buffer, er.encoded( %d )",
      er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
void HandoverPreparationFailure::setMessageType() {
  if (!hoPreparationFailurePdu)
    hoPreparationFailurePdu =
        (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType hoPreparationFailureMessageTypeIE;
  hoPreparationFailureMessageTypeIE.setProcedureCode(
      Ngap_ProcedureCode_id_HandoverPreparation);
  hoPreparationFailureMessageTypeIE.setTypeOfMessage(
      Ngap_NGAP_PDU_PR_unsuccessfulOutcome);
  hoPreparationFailureMessageTypeIE.setCriticality(Ngap_Criticality_reject);
  hoPreparationFailureMessageTypeIE.setValuePresent(
      Ngap_UnsuccessfulOutcome__value_PR_HandoverPreparationFailure);

  if (hoPreparationFailureMessageTypeIE.getProcedureCode() ==
          Ngap_ProcedureCode_id_HandoverPreparation &&
      hoPreparationFailureMessageTypeIE.getTypeOfMessage() ==
          Ngap_NGAP_PDU_PR_unsuccessfulOutcome) {
    hoPreparationFailureMessageTypeIE.encode2pdu(hoPreparationFailurePdu);
    hoPreparationFailureIEs =
        &(hoPreparationFailurePdu->choice.unsuccessfulOutcome->value.choice
              .HandoverPreparationFailure);
  } else {
    Logger::ngap().warn(
        "This information doesn't refer to HandoverPreparationFailure message");
  }
}

//------------------------------------------------------------------------------
void HandoverPreparationFailure::setAmfUeNgapId(unsigned long id) {
  if (!amfUeNgapId) amfUeNgapId = new AMF_UE_NGAP_ID();
  amfUeNgapId->setAMF_UE_NGAP_ID(id);

  Ngap_HandoverPreparationFailureIEs_t* ie =
      (Ngap_HandoverPreparationFailureIEs_t*) calloc(
          1, sizeof(Ngap_HandoverPreparationFailureIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_HandoverPreparationFailureIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId->encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&hoPreparationFailureIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverPreparationFailure::setRanUeNgapId(uint32_t ran_ue_ngap_id) {
  if (!ranUeNgapId) ranUeNgapId = new RAN_UE_NGAP_ID();
  ranUeNgapId->setRanUeNgapId(ran_ue_ngap_id);

  Ngap_HandoverPreparationFailureIEs_t* ie =
      (Ngap_HandoverPreparationFailureIEs_t*) calloc(
          1, sizeof(Ngap_HandoverPreparationFailureIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_HandoverPreparationFailureIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId->encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&hoPreparationFailureIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error");

  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void HandoverPreparationFailure::setCause(
    Ngap_Cause_PR m_causePresent, long value)  //
{
  if (!cause) cause = new Cause();
  Ngap_HandoverPreparationFailureIEs_t* ie =
      (Ngap_HandoverPreparationFailureIEs_t*) calloc(
          1, sizeof(Ngap_HandoverPreparationFailureIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_Cause;
  ie->criticality   = Ngap_Criticality_ignore;
  ie->value.present = Ngap_HandoverPreparationFailureIEs__value_PR_Cause;

  cause->setChoiceOfCause(m_causePresent);
  if (m_causePresent != Ngap_Cause_PR_NOTHING) cause->setValue(value);
  cause->encode2Cause(&(ie->value.choice.Cause));
  int ret = ASN_SEQUENCE_ADD(&hoPreparationFailureIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode Cause IE error");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
Ngap_Cause_PR HandoverPreparationFailure::getChoiceOfCause() const {
  if (cause)
    return cause->getChoiceOfCause();
  else
    return Ngap_Cause_PR();
}

}  // namespace ngap
