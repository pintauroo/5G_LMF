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

#ifndef _PDU_SESSION_RESOURCE_MODIFY_LIST_MOD_RES_H_
#define _PDU_SESSION_RESOURCE_MODIFY_LIST_MOD_RES_H_

#include "PDUSessionResourceModifyItemModRes.hpp"
#include <vector>

extern "C" {
#include "Ngap_PDUSessionResourceModifyListModRes.h"
}

namespace ngap {

class PDUSessionResourceModifyListModRes {
 public:
  PDUSessionResourceModifyListModRes();
  virtual ~PDUSessionResourceModifyListModRes();

  void setPDUSessionResourceModifyListModRes(
      const std::vector<PDUSessionResourceModifyItemModRes>&
          m_pduSessionResourceModifyListModRes);
  void getPDUSessionResourceModifyListModRes(
      std::vector<PDUSessionResourceModifyItemModRes>&
          m_pduSessionResourceModifyListModRes);
  bool encode2PDUSessionResourceModifyListModRes(
      Ngap_PDUSessionResourceModifyListModRes_t&
          m_pduSessionResourceModifyListModRes);
  bool decodefromPDUSessionResourceModifyListModRes(
      Ngap_PDUSessionResourceModifyListModRes_t&
          m_pduSessionResourceModifyListModRes);

 private:
  std::vector<PDUSessionResourceModifyItemModRes>
      pduSessionResourceModifyListModRes;
};

}  // namespace ngap

#endif
