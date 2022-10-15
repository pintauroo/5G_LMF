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

#include "PDUSessionResourceModifyListModRes.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PDUSessionResourceModifyListModRes::PDUSessionResourceModifyListModRes() {}

//------------------------------------------------------------------------------
PDUSessionResourceModifyListModRes::~PDUSessionResourceModifyListModRes() {}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyListModRes::setPDUSessionResourceModifyListModRes(
    const std::vector<PDUSessionResourceModifyItemModRes>&
        m_pduSessionResourceModifyListModRes) {
  pduSessionResourceModifyListModRes = m_pduSessionResourceModifyListModRes;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyListModRes::
    encode2PDUSessionResourceModifyListModRes(
        Ngap_PDUSessionResourceModifyListModRes_t&
            m_pduSessionResourceModifyListModRes) {
  for (auto pdu : pduSessionResourceModifyListModRes) {
    Ngap_PDUSessionResourceModifyItemModRes_t* request =
        (Ngap_PDUSessionResourceModifyItemModRes_t*) calloc(
            1, sizeof(Ngap_PDUSessionResourceModifyItemModRes_t));

    if (!request) return false;
    if (!pdu.encode2PDUSessionResourceModifyItemModRes(*request)) return false;
    if (ASN_SEQUENCE_ADD(&m_pduSessionResourceModifyListModRes.list, request) !=
        0)
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyListModRes::
    decodefromPDUSessionResourceModifyListModRes(
        Ngap_PDUSessionResourceModifyListModRes_t&
            pduSessionResourceSetupListSURes) {
  uint32_t numberofPDUSessions = pduSessionResourceSetupListSURes.list.count;

  for (int i = 0; i < numberofPDUSessions; i++) {
    PDUSessionResourceModifyItemModRes pduSessionResourceModifyItemModRes = {};

    if (!pduSessionResourceModifyItemModRes
             .decodefromPDUSessionResourceModifyItemModRes(
                 *pduSessionResourceSetupListSURes.list.array[i]))
      return false;
    pduSessionResourceModifyListModRes.push_back(
        pduSessionResourceModifyItemModRes);
  }

  return true;
}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyListModRes::getPDUSessionResourceModifyListModRes(
    std::vector<PDUSessionResourceModifyItemModRes>&
        m_pduSessionResourceModifyListModRes) {
  m_pduSessionResourceModifyListModRes = pduSessionResourceModifyListModRes;
}

}  // namespace ngap
