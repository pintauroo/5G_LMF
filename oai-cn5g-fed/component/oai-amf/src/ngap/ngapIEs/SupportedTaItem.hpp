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

#ifndef _SupportedTaItem_H
#define _SupportedTaItem_H

#include "BroadcastPLMNItem.hpp"
#include "Tac.hpp"

extern "C" {
#include "Ngap_SupportedTAItem.h"
}

namespace ngap {

class SupportedTaItem {
 public:
  SupportedTaItem();
  virtual ~SupportedTaItem();

  void setTac(TAC* m_tac);
  void getTac(TAC*& m_tac);
  void setBroadcastPlmnList(
      BroadcastPLMNItem* m_broadcastPLMNItem, int numOfItem);
  void getBroadcastPlmnList(
      BroadcastPLMNItem*& m_broadcastPLMNItem, int& numOfItem);
  bool encode2SupportedTaItem(Ngap_SupportedTAItem_t* ta);
  bool decodefromSupportedTaItem(Ngap_SupportedTAItem_t* ta);

 private:
  TAC* tac;
  BroadcastPLMNItem* broadcastPLMNItem;
  int numberOfBroadcastItem;
};

}  // namespace ngap

#endif
