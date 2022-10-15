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

#ifndef _BroadcastPLMNItem_H
#define _BroadcastPLMNItem_H

#include "PlmnId.hpp"
#include "S-NSSAI.hpp"

extern "C" {
#include "Ngap_BroadcastPLMNItem.h"
}

namespace ngap {

class BroadcastPLMNItem {
 public:
  BroadcastPLMNItem();
  virtual ~BroadcastPLMNItem();

  void setPlmnSliceSupportList(PlmnId* m_plmn, S_NSSAI* snssai, int num);
  void getPlmnSliceSupportList(
      PlmnId*& m_plmn, S_NSSAI*& m_snssai, int& snssainum);
  bool encode2BroadcastPLMNItem(Ngap_BroadcastPLMNItem_t*);
  bool decodefromBroadcastPLMNItem(Ngap_BroadcastPLMNItem_t* pdu);

 private:
  PlmnId* plmn;
  S_NSSAI* snssai;
  int numOfSnssai;
};
}  // namespace ngap
#endif
