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

/*! \file HandoverNotifyMsg.cpp
 \brief
 \author  niuxiansheng-niu, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */
#include "HandoverNotifyMsg.hpp"
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
#include <vector>

using namespace std;
namespace ngap {

//------------------------------------------------------------------------------
HandoverNotifyMsg::HandoverNotifyMsg() {
  amfUeNgapId             = nullptr;
  ranUeNgapId             = nullptr;
  userLocationInformation = nullptr;
  handoverNotifyPdu       = nullptr;
  handoverNotifyIEs       = nullptr;
}

//------------------------------------------------------------------------------
HandoverNotifyMsg::~HandoverNotifyMsg() {
  if (amfUeNgapId) delete (amfUeNgapId);
  if (ranUeNgapId) delete (ranUeNgapId);
  if (userLocationInformation) delete (userLocationInformation);
  if (handoverNotifyPdu) free(handoverNotifyPdu);
  if (handoverNotifyIEs) free(handoverNotifyIEs);
};

//------------------------------------------------------------------------------
unsigned long HandoverNotifyMsg::getAmfUeNgapId() {
  if (amfUeNgapId)
    return amfUeNgapId->getAMF_UE_NGAP_ID();
  else
    return 0;
}

//------------------------------------------------------------------------------
int HandoverNotifyMsg::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, handoverNotifyPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, handoverNotifyPdu, buf, buf_size);
  Logger::ngap().debug(
      "Encode Handover Notify to buffer, er.encoded( %d )", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
bool HandoverNotifyMsg::decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  if (!ngap_msg_pdu) return false;
  handoverNotifyPdu = ngap_msg_pdu;

  if (handoverNotifyPdu->present == Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (handoverNotifyPdu->choice.initiatingMessage &&
        handoverNotifyPdu->choice.initiatingMessage->procedureCode ==
            Ngap_ProcedureCode_id_HandoverNotification &&
        handoverNotifyPdu->choice.initiatingMessage->criticality ==
            Ngap_Criticality_ignore &&
        handoverNotifyPdu->choice.initiatingMessage->value.present ==
            Ngap_InitiatingMessage__value_PR_HandoverNotify) {
      handoverNotifyIEs = &handoverNotifyPdu->choice.initiatingMessage->value
                               .choice.HandoverNotify;
    } else {
      Logger::ngap().error("Check HandoverNotify message error!");
      return false;
    }
  } else {
    Logger::ngap().error("HandoverNotify MessageType error!");
    return false;
  }
  for (int i = 0; i < handoverNotifyIEs->protocolIEs.list.count; i++) {
    switch (handoverNotifyIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (handoverNotifyIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverNotifyIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverNotifyIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  handoverNotifyIEs->protocolIEs.list.array[i]
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
        if (handoverNotifyIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_reject &&
            handoverNotifyIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_HandoverNotifyIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  handoverNotifyIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_UserLocationInformation: {
        // TODO: Temporarily disable Criticality check to be tested with dsTest
        /*if (handoverNotifyIEs->protocolIEs.list.array[i]->criticality ==
              Ngap_Criticality_ignore &&
          handoverNotifyIEs->protocolIEs.list.array[i]->value.present ==
              Ngap_HandoverNotifyIEs__value_PR_UserLocationInformation) {
              */
        if (handoverNotifyIEs->protocolIEs.list.array[i]->value.present ==
            Ngap_HandoverNotifyIEs__value_PR_UserLocationInformation) {
          userLocationInformation = new UserLocationInformation();
          if (!userLocationInformation->decodefromUserLocationInformation(
                  &handoverNotifyIEs->protocolIEs.list.array[i]
                       ->value.choice.UserLocationInformation)) {
            Logger::ngap().error(
                "Decoded NGAP UserLocationInformation IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP UserLocationInformation IE error");
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
void HandoverNotifyMsg::setUserLocationInfoNR(
    struct NrCgi_s cig, struct Tai_s tai) {
  if (!userLocationInformation)
    userLocationInformation = new UserLocationInformation();

  UserLocationInformationNR* informationNR = new UserLocationInformationNR();
  NR_CGI* nR_CGI                           = new NR_CGI();
  PlmnId* plmnId_cgi                       = new PlmnId();
  NRCellIdentity* nRCellIdentity           = new NRCellIdentity();
  plmnId_cgi->setMccMnc(cig.mcc, cig.mnc);
  nRCellIdentity->setNRCellIdentity(cig.nrCellID);
  nR_CGI->setNR_CGI(plmnId_cgi, nRCellIdentity);

  TAI* tai_nr        = new TAI();
  PlmnId* plmnId_tai = new PlmnId();
  plmnId_tai->setMccMnc(tai.mcc, tai.mnc);
  TAC* tac = new TAC();
  tac->setTac(tai.tac);
  tai_nr->setTAI(plmnId_tai, tac);
  informationNR->setInformationNR(nR_CGI, tai_nr);
  userLocationInformation->setInformation(informationNR);

  Ngap_HandoverNotifyIEs_t* ie =
      (Ngap_HandoverNotifyIEs_t*) calloc(1, sizeof(Ngap_HandoverNotifyIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_UserLocationInformation;
  ie->criticality   = Ngap_Criticality_ignore;
  ie->value.present = Ngap_HandoverNotifyIEs__value_PR_UserLocationInformation;

  int ret = userLocationInformation->encodefromUserLocationInformation(
      &ie->value.choice.UserLocationInformation);
  if (!ret) {
    Logger::ngap().error("Encode UserLocationInformation IE error");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&handoverNotifyIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode UserLocationInformation IE error");

  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
uint32_t HandoverNotifyMsg::getRanUeNgapId() {
  if (ranUeNgapId)
    return ranUeNgapId->getRanUeNgapId();
  else
    return 0;
}

//------------------------------------------------------------------------------
bool HandoverNotifyMsg::getUserLocationInfoNR(
    struct NrCgi_s& cig, struct Tai_s& tai) {
  if (!userLocationInformation) return false;

  UserLocationInformationNR* informationNR = nullptr;
  userLocationInformation->getInformation(informationNR);
  if (!informationNR) return false;

  if (userLocationInformation->getChoiceOfUserLocationInformation() !=
      Ngap_UserLocationInformation_PR_userLocationInformationNR)
    return false;

  NR_CGI* nR_CGI = nullptr;
  TAI* nR_TAI    = nullptr;
  informationNR->getInformationNR(nR_CGI, nR_TAI);
  if (!nR_CGI or !nR_TAI) return false;

  PlmnId* cgi_plmnId             = nullptr;
  NRCellIdentity* nRCellIdentity = nullptr;
  nR_CGI->getNR_CGI(cgi_plmnId, nRCellIdentity);
  if (!cgi_plmnId or !nRCellIdentity) return false;

  cgi_plmnId->getMcc(cig.mcc);
  cgi_plmnId->getMnc(cig.mnc);
  cig.nrCellID = nRCellIdentity->getNRCellIdentity();

  PlmnId* tai_plmnId = nullptr;
  TAC* tac           = nullptr;
  nR_TAI->getTAI(tai_plmnId, tac);
  if (!tai_plmnId or !tac) return false;
  tai_plmnId->getMcc(tai.mcc);
  tai_plmnId->getMnc(tai.mnc);
  tai.tac = tac->getTac();

  return true;
}
}  // namespace ngap
