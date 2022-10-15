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

#ifndef _PDU_SESSION_RESOURCE_MODIFY_ITEM_MOD_RES_H_
#define _PDU_SESSION_RESOURCE_MODIFY_ITEM_MOD_RES_H_

#include "NAS-PDU.hpp"
#include "PDUSessionID.hpp"
#include "S-NSSAI.hpp"

extern "C" {
#include "Ngap_PDUSessionResourceModifyItemModRes.h"
}

namespace ngap {

class PDUSessionResourceModifyItemModRes {
 public:
  PDUSessionResourceModifyItemModRes();
  virtual ~PDUSessionResourceModifyItemModRes();

  void setPDUSessionResourceModifyItemModRes(
      const PDUSessionID& m_pDUSessionID,
      const OCTET_STRING_t m_pDUSessionResourceModifyResponseTransfer);
  void getPDUSessionResourceModifyItemModRes(
      PDUSessionID& m_pDUSessionID,
      OCTET_STRING_t& m_pDUSessionResourceModifyResponseTransfer);

  bool encode2PDUSessionResourceModifyItemModRes(
      Ngap_PDUSessionResourceModifyItemModRes_t&
          pduSessionResourceModifyItemModRes);
  bool decodefromPDUSessionResourceModifyItemModRes(
      Ngap_PDUSessionResourceModifyItemModRes_t&
          pduSessionResourceModifyItemModRes);

 private:
  PDUSessionID pDUSessionID;
  OCTET_STRING_t pDUSessionResourceModifyResponseTransfer;  // Optional
};

}  // namespace ngap

#endif
