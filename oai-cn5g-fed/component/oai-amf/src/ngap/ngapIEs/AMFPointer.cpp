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

#include "AMFPointer.hpp"

#include <iostream>

#include "String2Value.hpp"
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
AMFPointer::AMFPointer() {
  pointer = 0;
}

//------------------------------------------------------------------------------
AMFPointer::~AMFPointer() {}

//------------------------------------------------------------------------------
void AMFPointer::setAMFPointer(const std::string charPointer) {
  pointer = fromString<int>(charPointer);
}

//------------------------------------------------------------------------------
void AMFPointer::getAMFPointer(std::string& charPointer) {
  charPointer = to_string(pointer);
}

//------------------------------------------------------------------------------
bool AMFPointer::encode2bitstring(Ngap_AMFPointer_t& amfpointer) {
  amfpointer.size = 1;
  uint8_t* buffer = (uint8_t*) calloc(1, sizeof(uint8_t));
  if (!buffer) return false;
  *buffer                = ((pointer & 0x3f) << 2);
  amfpointer.buf         = buffer;
  amfpointer.bits_unused = 2;

  return true;
}

//------------------------------------------------------------------------------
bool AMFPointer::decodefrombitstring(Ngap_AMFPointer_t& amfpointer) {
  if (!amfpointer.buf) return false;
  pointer = (amfpointer.buf[0] & 0xfc) >> 2;  // 1111 1100

  return true;
}
}  // namespace ngap
