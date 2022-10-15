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

#include "Paging.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PagingMsg::PagingMsg() {
  pagingPdu        = nullptr;
  pagingIEs        = nullptr;
  uePagingIdentity = nullptr;
  taIListForPaging = nullptr;
}

//------------------------------------------------------------------------------
PagingMsg::~PagingMsg() {
  if (!uePagingIdentity) delete uePagingIdentity;
  if (!taIListForPaging) delete taIListForPaging;
}

//------------------------------------------------------------------------------
void PagingMsg::setMessageType() {
  if (!pagingPdu)
    pagingPdu = (Ngap_NGAP_PDU_t*) calloc(1, sizeof(Ngap_NGAP_PDU_t));

  MessageType PagingTypeIE;
  PagingTypeIE.setProcedureCode(Ngap_ProcedureCode_id_Paging);
  PagingTypeIE.setTypeOfMessage(Ngap_NGAP_PDU_PR_initiatingMessage);
  PagingTypeIE.setCriticality(Ngap_Criticality_ignore);
  PagingTypeIE.setValuePresent(Ngap_InitiatingMessage__value_PR_Paging);

  if (PagingTypeIE.getProcedureCode() == Ngap_ProcedureCode_id_Paging &&
      PagingTypeIE.getTypeOfMessage() == Ngap_NGAP_PDU_PR_initiatingMessage &&
      PagingTypeIE.getCriticality() == Ngap_Criticality_ignore) {
    PagingTypeIE.encode2pdu(pagingPdu);
    pagingIEs = &(pagingPdu->choice.initiatingMessage->value.choice.Paging);
  } else {
    cout << "[warning] This information doesn't refer to Paging "
            "Message!!!"
         << endl;
  }
}

//------------------------------------------------------------------------------
int PagingMsg::encode2buffer(uint8_t* buf, int buf_size) {
  asn_fprint(stderr, &asn_DEF_Ngap_NGAP_PDU, pagingPdu);
  asn_enc_rval_t er = aper_encode_to_buffer(
      &asn_DEF_Ngap_NGAP_PDU, NULL, pagingPdu, buf, buf_size);
  cout << "er.encoded(" << er.encoded << ")" << endl;
  return er.encoded;
}

//------------------------------------------------------------------------------
bool PagingMsg::decodefrompdu(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  pagingPdu = ngap_msg_pdu;

  if (pagingPdu->present == Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (pagingPdu->choice.initiatingMessage &&
        pagingPdu->choice.initiatingMessage->procedureCode ==
            Ngap_ProcedureCode_id_Paging &&
        pagingPdu->choice.initiatingMessage->criticality ==
            Ngap_Criticality_ignore &&
        pagingPdu->choice.initiatingMessage->value.present ==
            Ngap_InitiatingMessage__value_PR_Paging) {
      pagingIEs = &pagingPdu->choice.initiatingMessage->value.choice.Paging;
    } else {
      cout << "Check Paging message error!!!" << endl;
      return false;
    }
  } else {
    cout << "MessageType error!!!" << endl;
    return false;
  }
  for (int i = 0; i < pagingIEs->protocolIEs.list.count; i++) {
    switch (pagingIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_UEPagingIdentity: {
        if (pagingIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            pagingIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_PagingIEs__value_PR_UEPagingIdentity) {
          uePagingIdentity = new UEPagingIdentity();
          if (!uePagingIdentity->decodefrompdu(
                  pagingIEs->protocolIEs.list.array[i]
                      ->value.choice.UEPagingIdentity)) {
            cout << "Decoded NGAP UEPagingIdentity IE error" << endl;
            return false;
          }
          cout << "[Paging] Received UEPagingIdentity " << endl;
        } else {
          cout << "Decoded NGAP UEPagingIdentity IE error" << endl;
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_TAIListForPaging: {
        if (pagingIEs->protocolIEs.list.array[i]->criticality ==
                Ngap_Criticality_ignore &&
            pagingIEs->protocolIEs.list.array[i]->value.present ==
                Ngap_PagingIEs__value_PR_TAIListForPaging) {
          taIListForPaging = new TAIListForPaging();
          if (!taIListForPaging->decodefromTAIListForPaging(
                  &pagingIEs->protocolIEs.list.array[i]
                       ->value.choice.TAIListForPaging)) {
            cout << "Decoded NGAP TAIListForPaging IE error" << endl;
            return false;
          }
          cout << "[Paging] Received TAIListForPaging " << endl;
        } else {
          cout << "Decoded NGAP TAIListForPaging IE error" << endl;
          return false;
        }
      } break;
      default: {
        cout << "not decoded IE:" << pagingIEs->protocolIEs.list.array[i]->id
             << endl;
        return true;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void PagingMsg::setUEPagingIdentity(
    std::string SetId, std::string Pointer, std::string tmsi) {
  if (!uePagingIdentity) uePagingIdentity = new UEPagingIdentity();
  uePagingIdentity->setUEPagingIdentity(SetId, Pointer, tmsi);

  Ngap_PagingIEs_t* ie =
      (Ngap_PagingIEs_t*) calloc(1, sizeof(Ngap_PagingIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_UEPagingIdentity;
  ie->criticality   = Ngap_Criticality_ignore;
  ie->value.present = Ngap_PagingIEs__value_PR_UEPagingIdentity;

  int ret = uePagingIdentity->encode2pdu(&ie->value.choice.UEPagingIdentity);
  if (!ret) {
    cout << "encode UEPagingIdentity IE error" << endl;
    return;
  }

  ret = ASN_SEQUENCE_ADD(&pagingIEs->protocolIEs.list, ie);
  if (ret != 0) cout << "encode UEPagingIdentity IE error" << endl;
}

//------------------------------------------------------------------------------
void PagingMsg::getUEPagingIdentity(std::string& _5g_s_tmsi) {
  if (uePagingIdentity) uePagingIdentity->getUEPagingIdentity(_5g_s_tmsi);
}

//------------------------------------------------------------------------------
void PagingMsg::getUEPagingIdentity(
    std::string& setid, std::string& pointer, std::string& tmsi) {
  if (uePagingIdentity)
    uePagingIdentity->getUEPagingIdentity(setid, pointer, tmsi);
}

//------------------------------------------------------------------------------
void PagingMsg::setTAIListForPaging(const std::vector<struct Tai_s> list) {
  if (list.size() == 0) {
    cout << "[Warning] Setup failed, vector is empty!!!" << endl;
    return;
  }
  if (!taIListForPaging) taIListForPaging = new TAIListForPaging();

  TAI tai[list.size()];
  PlmnId plmnid[list.size()];
  TAC tac[list.size()];
  for (int i = 0; i < list.size(); i++) {
    plmnid[i].setMccMnc(list[i].mcc, list[i].mnc);
    tac[i].setTac(list[i].tac);
    tai[i].setTAI(&plmnid[i], &tac[i]);
  }
  taIListForPaging->setTAIListForPaging(tai, list.size());

  Ngap_PagingIEs_t* ie =
      (Ngap_PagingIEs_t*) calloc(1, sizeof(Ngap_PagingIEs_t));
  ie->id            = Ngap_ProtocolIE_ID_id_TAIListForPaging;
  ie->criticality   = Ngap_Criticality_ignore;
  ie->value.present = Ngap_PagingIEs__value_PR_TAIListForPaging;

  int ret = taIListForPaging->encode2TAIListForPaging(
      &ie->value.choice.TAIListForPaging);
  if (!ret) {
    cout << "encode TAIListForPaging IE error" << endl;
    return;
  }

  ret = ASN_SEQUENCE_ADD(&pagingIEs->protocolIEs.list, ie);
  if (ret != 0) cout << "encode TAIListForPaging IE error" << endl;
}

//------------------------------------------------------------------------------
void PagingMsg::getTAIListForPaging(std::vector<struct Tai_s>& list) {
  if (!taIListForPaging) return;
  TAI* tailist      = nullptr;
  int sizeoftailist = 0;
  taIListForPaging->getTAIListForPaging(tailist, sizeoftailist);

  for (int i = 0; i < sizeoftailist; i++) {
    Tai_t tai      = {};
    PlmnId* plmnid = nullptr;
    TAC* tac       = nullptr;
    tailist[i].getTAI(plmnid, tac);
    plmnid->getMcc(tai.mcc);
    plmnid->getMnc(tai.mnc);
    tai.tac = tac->getTac();

    list.push_back(tai);
  }
}

}  // namespace ngap
