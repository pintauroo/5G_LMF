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

#include "ServedGUAMIItem.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
ServedGUAMIItem::ServedGUAMIItem() {
  guamiGroup         = NULL;
  backupAMFName      = NULL;
  backupAMFNameIsSet = false;
}

//------------------------------------------------------------------------------
ServedGUAMIItem::~ServedGUAMIItem() {}

//------------------------------------------------------------------------------
void ServedGUAMIItem::setGUAMI(GUAMI* m_guami) {
  guamiGroup = m_guami;
}

//------------------------------------------------------------------------------
void ServedGUAMIItem::setBackupAMFName(AmfName* m_backupAMFName) {
  backupAMFNameIsSet = true;
  backupAMFName      = m_backupAMFName;
}

//------------------------------------------------------------------------------
bool ServedGUAMIItem::encode2ServedGUAMIItem(
    Ngap_ServedGUAMIItem* servedGUAMIItem) {
  if (!guamiGroup->encode2GUAMI(&(servedGUAMIItem->gUAMI))) return false;
  if (backupAMFNameIsSet) {
    Ngap_AMFName_t* backupamfname =
        (Ngap_AMFName_t*) calloc(1, sizeof(Ngap_AMFName_t));
    if (!backupamfname) return false;
    if (!backupAMFName->encode2AmfName(backupamfname)) return false;
    servedGUAMIItem->backupAMFName = backupamfname;
  }
  return true;
}

//------------------------------------------------------------------------------
bool ServedGUAMIItem::decodefromServedGUAMIItem(Ngap_ServedGUAMIItem* pdu) {
  if (!guamiGroup) guamiGroup = new GUAMI();
  if (!guamiGroup->decodefromGUAMI(&pdu->gUAMI)) return false;
  if (pdu->backupAMFName) {
    backupAMFNameIsSet = true;
    backupAMFName      = new AmfName();
    if (!backupAMFName->decodefromAmfName(pdu->backupAMFName)) return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void ServedGUAMIItem::getGUAMI(GUAMI*& m_guami) {
  m_guami = guamiGroup;
}

//------------------------------------------------------------------------------
bool ServedGUAMIItem::getBackupAMFName(AmfName*& m_backupAMFName) {
  m_backupAMFName = backupAMFName;
  return backupAMFNameIsSet;
}
}  // namespace ngap
