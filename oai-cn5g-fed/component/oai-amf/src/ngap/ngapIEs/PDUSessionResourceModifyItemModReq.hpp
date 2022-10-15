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

#ifndef _PDU_SESSION_RESOURCE_MODIFY_ITEM_MOD_REQ_H_
#define _PDU_SESSION_RESOURCE_MODIFY_ITEM_MOD_REQ_H_

#include "NAS-PDU.hpp"
#include "PDUSessionID.hpp"
#include "S-NSSAI.hpp"

extern "C" {
#include "Ngap_PDUSessionResourceModifyItemModReq.h"
}

namespace ngap {

class PDUSessionResourceModifyItemModReq {
 public:
  PDUSessionResourceModifyItemModReq();
  virtual ~PDUSessionResourceModifyItemModReq();

  void setPDUSessionResourceModifyItemModReq(
      const PDUSessionID& m_pDUSessionID, const NAS_PDU& m_nAS_PDU,
      const OCTET_STRING_t m_pDUSessionResourceModifyRequestTransfer,
      const S_NSSAI& m_s_NSSAI);
  void getPDUSessionResourceModifyItemModReq(
      PDUSessionID& m_pDUSessionID, NAS_PDU& m_nAS_PDU,
      OCTET_STRING_t& m_pDUSessionResourceModifyRequestTransfer,
      S_NSSAI& m_s_NSSAI);

  bool encode2PDUSessionResourceModifyItemModReq(
      Ngap_PDUSessionResourceModifyItemModReq_t&
          pduSessionResourceModifyItemModReq);
  bool decodefromPDUSessionResourceModifyItemModReq(
      Ngap_PDUSessionResourceModifyItemModReq_t&
          pduSessionResourceModifyItemModReq);

 private:
  PDUSessionID pDUSessionID;
  NAS_PDU* nAS_PDU;  // Optional
  OCTET_STRING_t pDUSessionResourceModifyRequestTransfer;
  S_NSSAI* s_NSSAI;  // Optional
};

}  // namespace ngap

#endif
