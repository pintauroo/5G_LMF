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

#include "TAIListforPaging.hpp"

extern "C" {
#include "Ngap_TAIListForPagingItem.h"
}

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
TAIListForPaging::TAIListForPaging() {
  tai      = nullptr;
  numOftai = 0;
}

//------------------------------------------------------------------------------
TAIListForPaging::~TAIListForPaging() {
  if (!tai) delete[] tai;
}

//------------------------------------------------------------------------------
bool TAIListForPaging::encode2TAIListForPaging(Ngap_TAIListForPaging_t* pdu) {
  for (int i = 0; i < numOftai; i++) {
    Ngap_TAIListForPagingItem_t* ta = (Ngap_TAIListForPagingItem_t*) calloc(
        1, sizeof(Ngap_TAIListForPagingItem_t));
    if (!tai[i].encode2TAI(&ta->tAI)) return false;
    if (ASN_SEQUENCE_ADD(&pdu->list, ta) != 0) return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool TAIListForPaging::decodefromTAIListForPaging(
    Ngap_TAIListForPaging_t* pdu) {
  numOftai = pdu->list.count;
  if (numOftai < 0) return false;
  tai = new TAI[numOftai];
  for (int i = 0; i < numOftai; i++) {
    if (!tai[i].decodefromTAI(&pdu->list.array[i]->tAI)) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
void TAIListForPaging::setTAIListForPaging(TAI* m_tai, int numOfItem) {
  tai      = m_tai;
  numOftai = numOfItem;
}

//------------------------------------------------------------------------------
void TAIListForPaging::getTAIListForPaging(TAI*& m_tai, int& numOfItem) {
  m_tai     = tai;
  numOfItem = numOftai;
}

}  // namespace ngap
