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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "EAPMessage.h"

int encode_eap_message(
    EAPMessage eapmessage, uint8_t iei, uint8_t* buffer, uint32_t len) {
  uint8_t* lenPtr   = NULL;
  uint32_t encoded  = 0;
  int encode_result = 0;
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER(
      buffer,
      ((iei > 0) ? EAP_MESSAGE_MINIMUM_LENGTH_TLVE :
                   EAP_MESSAGE_MINIMUM_LENGTH_LVE),
      len);

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  lenPtr = (buffer + encoded);
  encoded++;
  encoded++;

  if ((encode_result =
           encode_bstring(eapmessage, buffer + encoded, len - encoded)) < 0)
    return encode_result;
  else
    encoded += encode_result;

  uint32_t res = encoded - 2 - ((iei > 0) ? 1 : 0);
  *lenPtr      = res / (1 << 8);
  lenPtr++;
  *lenPtr = res % (1 << 8);

  return encoded;
}

int decode_eap_message(
    EAPMessage* eapmessage, uint8_t iei, uint8_t* buffer, uint32_t len) {
  int decoded       = 0;
  uint8_t ielen     = 0;
  int decode_result = 0;

  if (iei > 0) {
    CHECK_IEI_DECODER(iei, *buffer);
    decoded++;
  }

  ielen = *(buffer + decoded);
  decoded++;
  ielen = (ielen << 8) + *(buffer + decoded);
  decoded++;
  CHECK_LENGTH_DECODER(len - decoded, ielen);

  if ((decode_result = decode_bstring(
           eapmessage, ielen, buffer + decoded, len - decoded)) < 0)
    return decode_result;
  else
    decoded += decode_result;

  return decoded;
}
