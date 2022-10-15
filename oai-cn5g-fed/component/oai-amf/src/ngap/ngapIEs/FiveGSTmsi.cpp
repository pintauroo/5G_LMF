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

#include "FiveGSTmsi.hpp"

using namespace ngap;

//------------------------------------------------------------------------------
FiveGSTmsi::FiveGSTmsi() {}

//------------------------------------------------------------------------------
FiveGSTmsi::~FiveGSTmsi() {}

//------------------------------------------------------------------------------
bool FiveGSTmsi::decodefrompdu(Ngap_FiveG_S_TMSI_t pdu) {
  amfSetid.decodefrombitstring(pdu.aMFSetID);
  amfPointer.decodefrombitstring(pdu.aMFPointer);
  uint32_t tmsi = ntohl(*(uint32_t*) pdu.fiveG_TMSI.buf);
  int size      = pdu.fiveG_TMSI.size;
  std::string setId, pointer;
  amfSetid.getAMFSetID(setId);
  amfPointer.getAMFPointer(pointer);
  _5g_s_tmsi = setId + pointer + std::to_string(tmsi);
  tmsi_value = std::to_string(tmsi);
  return true;
}

//------------------------------------------------------------------------------
void FiveGSTmsi::getValue(std::string& tmsi) {
  tmsi = _5g_s_tmsi;
}

//------------------------------------------------------------------------------
void FiveGSTmsi::getValue(
    std::string& setid, std::string& pointer, std::string& tmsi) {
  amfSetid.getAMFSetID(setid);
  amfPointer.getAMFPointer(pointer);
  tmsi = tmsi_value;
}

//------------------------------------------------------------------------------
void FiveGSTmsi::setValue(
    std::string& setid, std::string& pointer, std::string& tmsi) {
  amfSetid.setAMFSetID(setid);
  amfPointer.setAMFPointer(pointer);
  _5g_s_tmsi = tmsi;
}

//------------------------------------------------------------------------------
bool FiveGSTmsi::encode2pdu(Ngap_FiveG_S_TMSI_t* pdu) {
  amfSetid.encode2bitstring(pdu->aMFSetID);
  amfPointer.encode2bitstring(pdu->aMFPointer);

  uint32_t tmsi        = (uint32_t) std::stol(_5g_s_tmsi);
  uint8_t* buf         = (uint8_t*) malloc(sizeof(uint32_t));
  *(uint32_t*) buf     = htonl(tmsi);
  pdu->fiveG_TMSI.buf  = buf;
  pdu->fiveG_TMSI.size = sizeof(uint32_t);

  return true;
}
