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

#include "PDUSessionResourceModifyItemModReq.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
PDUSessionResourceModifyItemModReq::PDUSessionResourceModifyItemModReq() {
  nAS_PDU = nullptr;
}

//------------------------------------------------------------------------------
PDUSessionResourceModifyItemModReq::~PDUSessionResourceModifyItemModReq() {}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyItemModReq::setPDUSessionResourceModifyItemModReq(
    const PDUSessionID& m_pDUSessionID, const NAS_PDU& m_nAS_PDU,
    const OCTET_STRING_t m_pDUSessionResourceModifyRequestTransfer,
    const S_NSSAI& m_s_NSSAI) {
  pDUSessionID     = m_pDUSessionID;
  uint8_t* nas_buf = nullptr;
  size_t nas_len   = 0;
  m_nAS_PDU.getNasPdu(nas_buf, nas_len);
  if (!nAS_PDU) nAS_PDU = new NAS_PDU();
  nAS_PDU->setNasPdu(nas_buf, nas_len);
  pDUSessionResourceModifyRequestTransfer =
      m_pDUSessionResourceModifyRequestTransfer;
  s_NSSAI->setSd(m_s_NSSAI.getSd());
  s_NSSAI->setSst(m_s_NSSAI.getSst());
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyItemModReq::
    encode2PDUSessionResourceModifyItemModReq(
        Ngap_PDUSessionResourceModifyItemModReq_t&
            pduSessionResourceModifyItemModReq) {
  if (!pDUSessionID.encode2PDUSessionID(
          pduSessionResourceModifyItemModReq.pDUSessionID))
    return false;
  if (nAS_PDU) {
    pduSessionResourceModifyItemModReq.nAS_PDU =
        (Ngap_NAS_PDU_t*) calloc(1, sizeof(Ngap_NAS_PDU_t));
    if (!pduSessionResourceModifyItemModReq.nAS_PDU) return false;
    if (!nAS_PDU->encode2octetstring(
            *pduSessionResourceModifyItemModReq.nAS_PDU)) {
      if (pduSessionResourceModifyItemModReq.nAS_PDU != nullptr)
        free(pduSessionResourceModifyItemModReq.nAS_PDU);
      return false;
    }
  }

  pduSessionResourceModifyItemModReq.pDUSessionResourceModifyRequestTransfer =
      pDUSessionResourceModifyRequestTransfer;

  return true;
}

//------------------------------------------------------------------------------
bool PDUSessionResourceModifyItemModReq::
    decodefromPDUSessionResourceModifyItemModReq(
        Ngap_PDUSessionResourceModifyItemModReq_t&
            pduSessionResourceModifyItemModReq) {
  if (!pDUSessionID.decodefromPDUSessionID(
          pduSessionResourceModifyItemModReq.pDUSessionID))
    return false;

  if (pduSessionResourceModifyItemModReq.nAS_PDU) {
    nAS_PDU = new NAS_PDU();
    if (!nAS_PDU->decodefromoctetstring(
            *pduSessionResourceModifyItemModReq.nAS_PDU))
      return false;
  }

  pDUSessionResourceModifyRequestTransfer =
      pduSessionResourceModifyItemModReq
          .pDUSessionResourceModifyRequestTransfer;

  return true;
}

//------------------------------------------------------------------------------
void PDUSessionResourceModifyItemModReq::getPDUSessionResourceModifyItemModReq(
    PDUSessionID& m_pDUSessionID, NAS_PDU& m_nAS_PDU,
    OCTET_STRING_t& m_pDUSessionResourceModifyRequestTransfer,
    S_NSSAI& m_s_NSSAI) {
  m_pDUSessionID = pDUSessionID;
  m_nAS_PDU      = *nAS_PDU;
  m_pDUSessionResourceModifyRequestTransfer =
      pDUSessionResourceModifyRequestTransfer;
  m_s_NSSAI = *s_NSSAI;
}

}  // namespace ngap
