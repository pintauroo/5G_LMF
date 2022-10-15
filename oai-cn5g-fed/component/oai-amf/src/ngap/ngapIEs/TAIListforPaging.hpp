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

#ifndef _TAILISTFORPAGING_H_
#define _TAILISTFORPAGING_H_

#include "TAI.hpp"

extern "C" {
#include "Ngap_TAIListForPaging.h"
}

namespace ngap {

class TAIListForPaging {
 public:
  TAIListForPaging();
  virtual ~TAIListForPaging();

  void setTAIListForPaging(TAI* m_tai, int numOfItem);
  void getTAIListForPaging(TAI*& m_tai, int& numOfItem);
  bool encode2TAIListForPaging(Ngap_TAIListForPaging_t* pdu);
  bool decodefromTAIListForPaging(Ngap_TAIListForPaging_t* pdu);

 private:
  TAI* tai;
  int numOftai;
};

}  // namespace ngap

#endif
