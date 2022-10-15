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
 \date
 \email: contact@openairinterface.org
 */

#include "PDUSessionResourceModifyListModReq.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PDUSessionResourceModifyListModReq::PDUSessionResourceModifyListModReq() {}

//------------------------------------------------------------------------------
PDUSessionResourceModifyListModReq::~PDUSessionResourceModifyListModReq() {}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyListModReq::setPDUSessionResourceModifyListModReq(
    const std::vector<PDUSessionResourceModifyItemModReq>&
        m_pduSessionResourceModifyListModReq) {
  pduSessionResourceModifyListModReq = m_pduSessionResourceModifyListModReq;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyListModReq::
    encode2PDUSessionResourceModifyListModReq(
        Ngap_PDUSessionResourceModifyListModReq_t&
            m_pduSessionResourceModifyListModReq) {
  for (auto pdu : pduSessionResourceModifyListModReq) {
    Ngap_PDUSessionResourceModifyItemModReq_t* request =
        (Ngap_PDUSessionResourceModifyItemModReq_t*) calloc(
            1, sizeof(Ngap_PDUSessionResourceModifyItemModReq_t));

    if (!request) return false;
    if (!pdu.encode2PDUSessionResourceModifyItemModReq(*request)) return false;
    if (ASN_SEQUENCE_ADD(&m_pduSessionResourceModifyListModReq.list, request) !=
        0)
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyListModReq::
    decodefromPDUSessionResourceModifyListModReq(
        Ngap_PDUSessionResourceModifyListModReq_t&
            pduSessionResourceSetupListSUReq) {
  uint32_t numberofPDUSessions = pduSessionResourceSetupListSUReq.list.count;

  for (int i = 0; i < numberofPDUSessions; i++) {
    PDUSessionResourceModifyItemModReq pduSessionResourceModifyItemModReq = {};

    if (!pduSessionResourceModifyItemModReq
             .decodefromPDUSessionResourceModifyItemModReq(
                 *pduSessionResourceSetupListSUReq.list.array[i]))
      return false;
    pduSessionResourceModifyListModReq.push_back(
        pduSessionResourceModifyItemModReq);
  }

  return true;
}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyListModReq::getPDUSessionResourceModifyListModReq(
    std::vector<PDUSessionResourceModifyItemModReq>&
        m_pduSessionResourceModifyListModReq) {
  m_pduSessionResourceModifyListModReq = pduSessionResourceModifyListModReq;
}

}  // namespace ngap
