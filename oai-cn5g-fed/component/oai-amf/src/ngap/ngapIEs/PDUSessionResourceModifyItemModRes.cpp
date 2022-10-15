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

#include "PDUSessionResourceModifyItemModRes.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PDUSessionResourceModifyItemModRes::PDUSessionResourceModifyItemModRes() {}

//------------------------------------------------------------------------------
PDUSessionResourceModifyItemModRes::~PDUSessionResourceModifyItemModRes() {}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyItemModRes::setPDUSessionResourceModifyItemModRes(
    const PDUSessionID& m_pDUSessionID,
    const OCTET_STRING_t m_pDUSessionResourceModifyResponseTransfer) {
  pDUSessionID = m_pDUSessionID;
  pDUSessionResourceModifyResponseTransfer =
      m_pDUSessionResourceModifyResponseTransfer;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyItemModRes::
    encode2PDUSessionResourceModifyItemModRes(
        Ngap_PDUSessionResourceModifyItemModRes_t&
            pduSessionResourceModifyItemModReq) {
  if (!pDUSessionID.encode2PDUSessionID(
          pduSessionResourceModifyItemModReq.pDUSessionID))
    return false;

  pduSessionResourceModifyItemModReq.pDUSessionResourceModifyResponseTransfer =
      pDUSessionResourceModifyResponseTransfer;

  return true;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyItemModRes::
    decodefromPDUSessionResourceModifyItemModRes(
        Ngap_PDUSessionResourceModifyItemModRes_t&
            pduSessionResourceModifyItemModReq) {
  if (!pDUSessionID.decodefromPDUSessionID(
          pduSessionResourceModifyItemModReq.pDUSessionID))
    return false;

  pDUSessionResourceModifyResponseTransfer =
      pduSessionResourceModifyItemModReq
          .pDUSessionResourceModifyResponseTransfer;

  return true;
}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyItemModRes::getPDUSessionResourceModifyItemModRes(
    PDUSessionID& m_pDUSessionID,
    OCTET_STRING_t& m_pDUSessionResourceModifyResponseTransfer) {
  m_pDUSessionID = pDUSessionID;
  m_pDUSessionResourceModifyResponseTransfer =
      pDUSessionResourceModifyResponseTransfer;
}

}  // namespace ngap
