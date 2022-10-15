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

#include "InitialContextSetupRequest.hpp"
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
InitialContextSetupRequestMsg::InitialContextSetupRequestMsg() {
  initialContextSetupRequestPdu      = nullptr;
  initialContextSetupRequestIEs      = nullptr;
  amfUeNgapId                        = nullptr;
  ranUeNgapId                        = nullptr;
  oldAmfName                         = nullptr;
  uEAggregateMaxBitRate              = nullptr;
  coreNetworkAssistanceInfo          = nullptr;
  guami                              = nullptr;
  pduSessionResourceSetupRequestList = nullptr;
  allowedNssai                       = nullptr;
  uESecurityCapabilities             = nullptr;
  securityKey                        = nullptr;
  nasPdu                             = nullptr;
  ueRadioCapability                  = nullptr;
}

//------------------------------------------------------------------------------
InitialContextSetupRequestMsg::~InitialContextSetupRequestMsg() {}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setMessageType() {
  if (!initialContextSetupRequestPdu)
    initialContextSetupRequestPdu =
        (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType initialContextSetupRequestPduTypeIE;
  initialContextSetupRequestPduTypeIE.setProcedureCode(
      Ngap_ProcedureCode_id_InitialContextSetup);
  initialContextSetupRequestPduTypeIE.setTypeOfMessage(
      Ngap_NGAP_PDU_PR_initiatingMessage);
  initialContextSetupRequestPduTypeIE.setCriticality(Ngap_Criticality_reject);
  initialContextSetupRequestPduTypeIE.setValuePresent(
      Ngap_InitiatingMessage__value_PR_InitialContextSetupRequest);

  if (initialContextSetupRequestPduTypeIE.getProcedureCode() ==
          Ngap_ProcedureCode_id_InitialContextSetup &&
      initialContextSetupRequestPduTypeIE.getTypeOfMessage() ==
          Ngap_NGAP_PDU_PR_initiatingMessage &&
      initialContextSetupRequestPduTypeIE.getCriticality() ==
          Ngap_Criticality_reject) {
    initialContextSetupRequestPduTypeIE.encode2pdu(
        initialContextSetupRequestPdu);
    initialContextSetupRequestIEs =
        &(initialContextSetupRequestPdu->choice.initiatingMessage->value.choice
              .InitialContextSetupRequest);
  } else {
    Logger::ngap().warn(
        " This information doesn't refer to InitialContextSetupRequest "
        "Message!");
  }
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setAmfUeNgapId(unsigned long id) {
  if (!amfUeNgapId) amfUeNgapId = new AMF_UE_NGAP_ID();
  amfUeNgapId->setAMF_UE_NGAP_ID(id);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_AMF_UE_NGAP_ID;

  int ret = amfUeNgapId->encode2AMF_UE_NGAP_ID(ie->value.choice.AMF_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error!");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode AMF_UE_NGAP_ID IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setRanUeNgapId(uint32_t ran_ue_ngap_id) {
  if (!ranUeNgapId) ranUeNgapId = new RAN_UE_NGAP_ID();
  ranUeNgapId->setRanUeNgapId(ran_ue_ngap_id);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_RAN_UE_NGAP_ID;

  int ret = ranUeNgapId->encode2RAN_UE_NGAP_ID(ie->value.choice.RAN_UE_NGAP_ID);
  if (!ret) {
    Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode RAN_UE_NGAP_ID IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setOldAmfName(const std::string name) {
  if (!oldAmfName) oldAmfName = new AmfName();
  oldAmfName->setValue(name);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_OldAMF;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_InitialContextSetupRequestIEs__value_PR_AMFName;

  int ret = oldAmfName->encode2AmfName(&ie->value.choice.AMFName);
  if (!ret) {
    Logger::ngap().error("Encode oldAmfName IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode oldAmfName IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setUEAggregateMaxBitRate(
    long bit_rate_downlink, long bit_rate_uplink) {
  if (!uEAggregateMaxBitRate)
    uEAggregateMaxBitRate = new UEAggregateMaxBitRate();

  uEAggregateMaxBitRate->setUEAggregateMaxBitRate(
      bit_rate_downlink, bit_rate_uplink);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_UEAggregateMaximumBitRate;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_UEAggregateMaximumBitRate;

  int ret = uEAggregateMaxBitRate->encode2UEAggregateMaxBitRate(
      ie->value.choice.UEAggregateMaximumBitRate);
  if (!ret) {
    Logger::ngap().error("Encode UEAggregateMaxBitRate IE error!");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode UEAggregateMaxBitRate IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setCoreNetworkAssistanceInfo(
    uint16_t ueIdentityIndexValue /*10bits*/, e_Ngap_PagingDRX ueSpecificDrx,
    uint8_t periodicRegUpdateTimer, bool micoModeInd,
    std::vector<Tai_t> taiListForRRcInactive) {
  if (!coreNetworkAssistanceInfo)
    coreNetworkAssistanceInfo = new CoreNetworkAssistanceInfo();

  UEIdentityIndexValue* m_ueIdentityIndexValue = new UEIdentityIndexValue();
  m_ueIdentityIndexValue->setUEIdentityIndexValue(ueIdentityIndexValue);
  DefaultPagingDRX* m_pagingDRX = new DefaultPagingDRX();
  m_pagingDRX->setValue(ueSpecificDrx);
  PeriodicRegistrationUpdateTimer* m_periodicRegUpdateTimer =
      new PeriodicRegistrationUpdateTimer();
  m_periodicRegUpdateTimer->setPeriodicRegistrationUpdateTimer(
      periodicRegUpdateTimer);
  TAI* m_tai = new TAI[taiListForRRcInactive.size()]();
  for (int i = 0; i < taiListForRRcInactive.size(); i++) {
    PlmnId* m_plmnId = new PlmnId();
    m_plmnId->setMccMnc(
        taiListForRRcInactive[i].mcc, taiListForRRcInactive[i].mnc);
    TAC* m_tac = new TAC();
    m_tac->setTac(taiListForRRcInactive[i].tac);
    m_tai[i].setTAI(m_plmnId, m_tac);
  }
  coreNetworkAssistanceInfo->setCoreNetworkAssistanceInfo(
      m_ueIdentityIndexValue, m_pagingDRX, m_periodicRegUpdateTimer,
      micoModeInd, m_tai, taiListForRRcInactive.size());

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_CoreNetworkAssistanceInformation;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_CoreNetworkAssistanceInformation;

  int ret = coreNetworkAssistanceInfo->encode2CoreNetworkAssistanceInfo(
      &ie->value.choice.CoreNetworkAssistanceInformation);
  if (!ret) {
    Logger::ngap().error("Encode CoreNetworkAssistanceInformation IE error!");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error("Encode CoreNetworkAssistanceInformation IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setCoreNetworkAssistanceInfo(
    uint16_t ueIdentityIndexValue /*10bits*/, uint8_t periodicRegUpdateTimer,
    bool micoModeInd, std::vector<Tai_t> taiListForRRcInactive) {
  if (!coreNetworkAssistanceInfo)
    coreNetworkAssistanceInfo = new CoreNetworkAssistanceInfo();

  UEIdentityIndexValue* m_ueIdentityIndexValue = new UEIdentityIndexValue();
  m_ueIdentityIndexValue->setUEIdentityIndexValue(ueIdentityIndexValue);
  PeriodicRegistrationUpdateTimer* m_periodicRegUpdateTimer =
      new PeriodicRegistrationUpdateTimer();
  m_periodicRegUpdateTimer->setPeriodicRegistrationUpdateTimer(
      periodicRegUpdateTimer);
  TAI* m_tai = new TAI[taiListForRRcInactive.size()]();
  for (int i = 0; i < taiListForRRcInactive.size(); i++) {
    PlmnId* m_plmnId = new PlmnId();
    m_plmnId->setMccMnc(
        taiListForRRcInactive[i].mcc, taiListForRRcInactive[i].mnc);
    TAC* m_tac = new TAC();
    m_tac->setTac(taiListForRRcInactive[i].tac);
    m_tai[i].setTAI(m_plmnId, m_tac);
  }
  coreNetworkAssistanceInfo->setCoreNetworkAssistanceInfo(
      m_ueIdentityIndexValue, m_periodicRegUpdateTimer, micoModeInd, m_tai,
      taiListForRRcInactive.size());

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_CoreNetworkAssistanceInformation;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_CoreNetworkAssistanceInformation;

  int ret = coreNetworkAssistanceInfo->encode2CoreNetworkAssistanceInfo(
      &ie->value.choice.CoreNetworkAssistanceInformation);
  if (!ret) {
    Logger::ngap().error("Encode CoreNetworkAssistanceInformation IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error("Encode CoreNetworkAssistanceInformation IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setGuami(Guami_t value) {
  if (!guami) guami = new GUAMI();

  PlmnId* m_plmnId           = new PlmnId();
  AMFRegionID* m_aMFRegionID = new AMFRegionID();
  AMFSetID* m_aMFSetID       = new AMFSetID();
  AMFPointer* m_aMFPointer   = new AMFPointer();

  m_plmnId->setMccMnc(value.mcc, value.mnc);
  m_aMFRegionID->setAMFRegionID(value.regionID);
  m_aMFSetID->setAMFSetID(value.AmfSetID);
  m_aMFPointer->setAMFPointer(value.AmfPointer);

  guami->setGUAMI(m_plmnId, m_aMFRegionID, m_aMFSetID, m_aMFPointer);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_GUAMI;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_InitialContextSetupRequestIEs__value_PR_GUAMI;

  int ret = guami->encode2GUAMI(&ie->value.choice.GUAMI);
  if (!ret) {
    Logger::ngap().error("Encode GUAMI IE error!");
    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode GUAMI IE error!");

  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setPduSessionResourceSetupRequestList(
    std::vector<PDUSessionResourceSetupRequestItem_t> list) {
  if (!pduSessionResourceSetupRequestList)
    pduSessionResourceSetupRequestList =
        new PDUSessionResourceSetupListCxtReq();
  PDUSessionResourceSetupItemCxtReq* m_pduSessionResourceSetupItemCxtReq =
      new PDUSessionResourceSetupItemCxtReq[list.size()]();

  for (int i = 0; i < list.size(); i++) {
    PDUSessionID* m_pDUSessionID = new PDUSessionID();
    m_pDUSessionID->setPDUSessionID(list[i].pduSessionId);
    NAS_PDU* m_nAS_PDU = nullptr;
    if (list[i].pduSessionNAS_PDU) {
      m_nAS_PDU = new NAS_PDU();
      m_nAS_PDU->setNasPdu(
          list[i].pduSessionNAS_PDU, list[i].sizeofpduSessionNAS_PDU);
    }
    S_NSSAI* m_s_NSSAI = new S_NSSAI();
    m_s_NSSAI->setSst(list[i].s_nssai.sst);
    if (list[i].s_nssai.sd.size()) m_s_NSSAI->setSd(list[i].s_nssai.sd);
    m_pduSessionResourceSetupItemCxtReq[i].setPDUSessionResourceSetupItemCxtReq(
        m_pDUSessionID, m_nAS_PDU, m_s_NSSAI,
        list[i].pduSessionResourceSetupRequestTransfer);
  }
  pduSessionResourceSetupRequestList->setPDUSessionResourceSetupListCxtReq(
      m_pduSessionResourceSetupItemCxtReq, list.size());

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtReq;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_PDUSessionResourceSetupListCxtReq;

  int ret = pduSessionResourceSetupRequestList
                ->encode2PDUSessionResourceSetupListCxtReq(
                    &ie->value.choice.PDUSessionResourceSetupListCxtReq);
  if (!ret) {
    Logger::ngap().error("Encode PDUSessionResourceSetupListCxtReq IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0)
    Logger::ngap().error("Encode PDUSessionResourceSetupListCxtReq IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setAllowedNssai(std::vector<S_Nssai> list) {
  if (!allowedNssai) allowedNssai = new AllowedNSSAI();
  S_NSSAI* m_snssai = new S_NSSAI[list.size()]();
  for (int i = 0; i < list.size(); i++) {
    m_snssai[i].setSst(list[i].sst);
    if (list[i].sd.size() &&
        (list[i].sd.compare("None") && list[i].sd.compare("none")))
      m_snssai[i].setSd(list[i].sd);
  }
  allowedNssai->setAllowedNSSAI(m_snssai, list.size());

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_AllowedNSSAI;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_InitialContextSetupRequestIEs__value_PR_AllowedNSSAI;

  int ret = allowedNssai->encode2AllowedNSSAI(&ie->value.choice.AllowedNSSAI);
  if (!ret) {
    Logger::ngap().error("Encode AllowedNSSAI IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode AllowedNSSAI IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setUESecurityCapability(
    uint16_t NR_EncryptionAlgs, uint16_t NR_IntegrityProtectionAlgs,
    uint16_t E_UTRA_EncryptionAlgs, uint16_t E_UTRA_IntegrityProtectionAlgs) {
  if (!uESecurityCapabilities)
    uESecurityCapabilities = new UESecurityCapabilities();

  uESecurityCapabilities->setUESecurityCapabilities(
      NR_EncryptionAlgs, NR_IntegrityProtectionAlgs, E_UTRA_EncryptionAlgs,
      E_UTRA_IntegrityProtectionAlgs);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_UESecurityCapabilities;
  ie->criticality = Ngap_Criticality_reject;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_UESecurityCapabilities;

  int ret = uESecurityCapabilities->encode2UESecurityCapabilities(
      ie->value.choice.UESecurityCapabilities);
  if (!ret) {
    Logger::ngap().error("Encode UESecurityCapabilities IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode UESecurityCapabilities IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setSecurityKey(uint8_t* key) {
  if (!securityKey) securityKey = new SecurityKey();

  securityKey->setSecurityKey(key);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_SecurityKey;
  ie->criticality   = Ngap_Criticality_reject;
  ie->value.present = Ngap_InitialContextSetupRequestIEs__value_PR_SecurityKey;

  int ret = securityKey->encode2bitstring(ie->value.choice.SecurityKey);
  if (!ret) {
    Logger::ngap().error("Encode SecurityKey IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode SecurityKey IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setNasPdu(uint8_t* nas, size_t sizeofnas) {
  if (!nasPdu) nasPdu = new NAS_PDU();

  nasPdu->setNasPdu(nas, sizeofnas);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_InitialContextSetupRequestIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_NAS_PDU;
  ie->criticality   = Ngap_Criticality_ignore;
  ie->value.present = Ngap_InitialContextSetupRequestIEs__value_PR_NAS_PDU;

  int ret = nasPdu->encode2octetstring(ie->value.choice.NAS_PDU);
  if (!ret) {
    Logger::ngap().error("Encode NAS PDU error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode NAS PDU error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
void InitialContextSetupRequestMsg::setUERadioCapability(
    uint8_t* buffer, size_t size) {
  if (!ueRadioCapability) ueRadioCapability = new UERadioCapability();

  ueRadioCapability->setUERadioCapability(buffer, size);

  Ngap_InitialContextSetupRequestIEs_t* ie =
      (Ngap_InitialContextSetupRequestIEs_t*) calloc(
          1, sizeof(Ngap_PDUSessionResourceSetupRequestIEs_t));
  ie->id          = Ngap_ProtocolIE_ID_id_UERadioCapability;
  ie->criticality = Ngap_Criticality_ignore;
  ie->value.present =
      Ngap_InitialContextSetupRequestIEs__value_PR_UERadioCapability;

  int ret = ueRadioCapability->encode2UERadioCapability(
      ie->value.choice.UERadioCapability);
  if (!ret) {
    Logger::ngap().error("Encode UERadioCapability IE error!");

    free_wrapper((void**) &ie);
    return;
  }

  ret = ASN_SEQUENCE_ADD(&initialContextSetupRequestIEs->protocolIEs.list, ie);
  if (ret != 0) Logger::ngap().error("Encode UERadioCapability IE error!");
  // free_wrapper((void**) &ie);
}

//------------------------------------------------------------------------------
int InitialContextSetupRequestMsg::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, initialContextSetupRequestPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, initialContextSetupRequestPdu, buf,
      buf_size);
  Logger::ngap().debug("er.encoded( %d )", er.encoded);
  return er.encoded;
}

//------------------------------------------------------------------------------
// Decapsulation
bool InitialContextSetupRequestMsg::decodefrompdu(
    Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  initialContextSetupRequestPdu = ngap_msg_pdu;

  if (initialContextSetupRequestPdu->present ==
      Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (initialContextSetupRequestPdu->choice.initiatingMessage &&
        initialContextSetupRequestPdu->choice.initiatingMessage
                ->procedureCode == Ngap_ProcedureCode_id_InitialContextSetup &&
        initialContextSetupRequestPdu->choice.initiatingMessage->criticality ==
            Ngap_Criticality_reject &&
        initialContextSetupRequestPdu->choice.initiatingMessage->value
                .present ==
            Ngap_InitiatingMessage__value_PR_InitialContextSetupRequest) {
      initialContextSetupRequestIEs =
          &initialContextSetupRequestPdu->choice.initiatingMessage->value.choice
               .InitialContextSetupRequest;
    } else {
      cout << "Check InitialContextSetupRequest message error!!!" << endl;
      return false;
    }
  } else {
    Logger::ngap().error("MessageType error!");
    return false;
  }
  for (int i = 0; i < initialContextSetupRequestIEs->protocolIEs.list.count;
       i++) {
    switch (initialContextSetupRequestIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
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
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_OldAMF: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_AMFName) {
          oldAmfName = new AmfName();
          if (!oldAmfName->decodefromAmfName(
                  &initialContextSetupRequestIEs->protocolIEs.list.array[i]
                       ->value.choice.AMFName)) {
            Logger::ngap().error("Decoded NGAP OldAMFName IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP OldAMFName IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_UEAggregateMaximumBitRate: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_UEAggregateMaximumBitRate) {
          uEAggregateMaxBitRate = new UEAggregateMaxBitRate();
          if (!uEAggregateMaxBitRate->decodefromUEAggregateMaxBitRate(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.UEAggregateMaximumBitRate)) {
            Logger::ngap().error(
                "Decoded NGAP UEAggregateMaximumBitRate IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP UEAggregateMaximumBitRate IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_CoreNetworkAssistanceInformation: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_CoreNetworkAssistanceInformation) {
          coreNetworkAssistanceInfo = new CoreNetworkAssistanceInfo();
          if (!coreNetworkAssistanceInfo->decodefromCoreNetworkAssistanceInfo(
                  &initialContextSetupRequestIEs->protocolIEs.list.array[i]
                       ->value.choice.CoreNetworkAssistanceInformation)) {
            Logger::ngap().error(
                "Decoded NGAP CoreNetworkAssistanceInformation IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP CoreNetworkAssistanceInformation IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_GUAMI: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_GUAMI) {
          guami = new GUAMI();
          if (!guami->decodefromGUAMI(
                  &initialContextSetupRequestIEs->protocolIEs.list.array[i]
                       ->value.choice.GUAMI)) {
            Logger::ngap().error("Decoded NGAP GUAMI IE error");

            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP GUAMI IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtReq: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_PDUSessionResourceSetupListCxtReq) {
          pduSessionResourceSetupRequestList =
              new PDUSessionResourceSetupListCxtReq();
          if (!pduSessionResourceSetupRequestList
                   ->decodefromPDUSessionResourceSetupListCxtReq(
                       &initialContextSetupRequestIEs->protocolIEs.list
                            .array[i]
                            ->value.choice.PDUSessionResourceSetupListCxtReq)) {
            Logger::ngap().error(
                "Decoded NGAP PDUSessionResourceSetupListCxtReq IE error");
            return false;
          }
        } else {
          Logger::ngap().error(
              "Decoded NGAP PDUSessionResourceSetupListCxtReq IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_AllowedNSSAI: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_AllowedNSSAI) {
          allowedNssai = new AllowedNSSAI();
          if (!allowedNssai->decodefromAllowedNSSAI(
                  &initialContextSetupRequestIEs->protocolIEs.list.array[i]
                       ->value.choice.AllowedNSSAI)) {
            Logger::ngap().error("Decoded NGAP AllowedNSSAI IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP AllowedNSSAI IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_UESecurityCapabilities: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_UESecurityCapabilities) {
          uESecurityCapabilities = new UESecurityCapabilities();
          if (!uESecurityCapabilities->decodefromUESecurityCapabilities(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.UESecurityCapabilities)) {
            Logger::ngap().error(
                "Decoded NGAP UESecurityCapabilities IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP UESecurityCapabilities IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_SecurityKey: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_SecurityKey) {
          securityKey = new SecurityKey();
          if (!securityKey->decodefrombitstring(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.SecurityKey)) {
            Logger::ngap().error("Decoded NGAP SecurityKey IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP SecurityKey IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_NAS_PDU: {
        if (initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_ignore &&
            initialContextSetupRequestIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_InitialContextSetupRequestIEs__value_PR_NAS_PDU) {
          nasPdu = new NAS_PDU();
          if (!nasPdu->decodefromoctetstring(
                  initialContextSetupRequestIEs->protocolIEs.list.array[i]
                      ->value.choice.NAS_PDU)) {
            Logger::ngap().error("Decoded NGAP NAS_PDU IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP NAS_PDU IE error");
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
unsigned long InitialContextSetupRequestMsg::getAmfUeNgapId() {
  if (!amfUeNgapId) return -1;
  return amfUeNgapId->getAMF_UE_NGAP_ID();
}

//------------------------------------------------------------------------------
uint32_t InitialContextSetupRequestMsg::getRanUeNgapId() {
  if (!ranUeNgapId) return 0;
  return ranUeNgapId->getRanUeNgapId();
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getOldAmfName(std::string& name) {
  if (!oldAmfName) return false;
  oldAmfName->getValue(name);
  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getUEAggregateMaxBitRate(
    long& bit_rate_downlink, long& bit_rate_uplink) {
  if (!uEAggregateMaxBitRate) return false;
  return uEAggregateMaxBitRate->getUEAggregateMaxBitRate(
      bit_rate_downlink, bit_rate_uplink);
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getCoreNetworkAssistanceInfo(
    uint16_t& ueIdentityIndexValue /*10bits*/, int& ueSpecificDrx /*-1*/,
    uint8_t& periodicRegUpdateTimer, bool& micoModeInd,
    std::vector<Tai_t>& taiListForRRcInactive) {
  if (!coreNetworkAssistanceInfo) return false;
  UEIdentityIndexValue* m_ueIdentityIndexValue;
  DefaultPagingDRX* m_pagingDRX;
  PeriodicRegistrationUpdateTimer* m_periodicRegUpdateTimer;
  TAI* m_tai;
  int m_numoftai = 0;
  coreNetworkAssistanceInfo->getCoreNetworkAssistanceInfo(
      m_ueIdentityIndexValue, m_pagingDRX, m_periodicRegUpdateTimer,
      micoModeInd, m_tai, m_numoftai);
  m_ueIdentityIndexValue->getUEIdentityIndexValue(ueIdentityIndexValue);
  if (m_pagingDRX)
    ueSpecificDrx = m_pagingDRX->getValue();
  else
    ueSpecificDrx = -1;
  m_periodicRegUpdateTimer->getPeriodicRegistrationUpdateTimer(
      periodicRegUpdateTimer);

  for (int i = 0; i < m_numoftai; i++) {
    Tai_t tai_str;
    PlmnId* m_plmnId;
    TAC* m_tac;
    m_tai[i].getTAI(m_plmnId, m_tac);
    m_plmnId->getMcc(tai_str.mcc);
    m_plmnId->getMnc(tai_str.mnc);
    tai_str.tac = m_tac->getTac();

    taiListForRRcInactive.push_back(tai_str);
  }
  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getGuami(Guami_t& value) {
  if (!guami) return false;

  PlmnId* m_plmnId;
  AMFRegionID* m_aMFRegionID;
  AMFSetID* m_aMFSetID;
  AMFPointer* m_aMFPointer;

  guami->getGUAMI(m_plmnId, m_aMFRegionID, m_aMFSetID, m_aMFPointer);

  m_plmnId->getMcc(value.mcc);
  m_plmnId->getMnc(value.mnc);
  m_aMFRegionID->getAMFRegionID(value.regionID);
  m_aMFSetID->getAMFSetID(value.AmfSetID);
  m_aMFPointer->getAMFPointer(value.AmfPointer);

  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getPduSessionResourceSetupRequestList(
    std::vector<PDUSessionResourceSetupRequestItem_t>& list) {
  if (!pduSessionResourceSetupRequestList) return false;
  PDUSessionResourceSetupItemCxtReq* m_pduSessionResourceSetupItemCxtReq;
  int num = 0;
  pduSessionResourceSetupRequestList->getPDUSessionResourceSetupListCxtReq(
      m_pduSessionResourceSetupItemCxtReq, num);

  for (int i = 0; i < num; i++) {
    PDUSessionResourceSetupRequestItem_t request;

    PDUSessionID* m_pDUSessionID;
    NAS_PDU* m_nAS_PDU;
    S_NSSAI* m_s_NSSAI;
    m_pduSessionResourceSetupItemCxtReq[i].getPDUSessionResourceSetupItemCxtReq(
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

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getAllowedNssai(
    std::vector<S_Nssai>& list) {
  if (!allowedNssai) return false;
  S_NSSAI* m_snssai;
  int m_numofsnssai;
  allowedNssai->getAllowedNSSAI(m_snssai, m_numofsnssai);
  for (int i = 0; i < m_numofsnssai; i++) {
    S_Nssai s_nssai;
    m_snssai[i].getSst(s_nssai.sst);
    m_snssai[i].getSd(s_nssai.sd);
    list.push_back(s_nssai);
  }

  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getUESecurityCapability(
    uint16_t& NR_EncryptionAlgs, uint16_t& NR_IntegrityProtectionAlgs,
    uint16_t& E_UTRA_EncryptionAlgs, uint16_t& E_UTRA_IntegrityProtectionAlgs) {
  if (!uESecurityCapabilities) return false;
  if (!uESecurityCapabilities->getUESecurityCapabilities(
          NR_EncryptionAlgs, NR_IntegrityProtectionAlgs, E_UTRA_EncryptionAlgs,
          E_UTRA_IntegrityProtectionAlgs))
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getSecurityKey(uint8_t*& key) {
  if (!securityKey) return false;
  if (!securityKey->getSecurityKey(key)) return false;

  return true;
}

//------------------------------------------------------------------------------
bool InitialContextSetupRequestMsg::getNasPdu(
    uint8_t*& nas, size_t& sizeofnas) {
  if (!nasPdu) return false;
  if (!nasPdu->getNasPdu(nas, sizeofnas)) return false;

  return true;
}

}  // namespace ngap
