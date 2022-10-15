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
#include "PLMNList.h"

int encode_plmn_list(
    PLMNList plmnlist, uint8_t iei, uint8_t* buffer, uint32_t len) {
  uint8_t* lenPtr;
  uint32_t encoded      = 0;
  uint8_t plmn_index    = 0;
  uint8_t mcc_mnc_value = 0x0;

  CHECK_PDU_POINTER_AND_LENGTH_ENCODER(buffer, PLMN_LIST_MINIMUM_LENGTH, len);

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  lenPtr = (buffer + encoded);
  encoded++;

  for (; plmn_index < 15; plmn_index++) {
    mcc_mnc_value = 0;
    mcc_mnc_value |= (uint8_t)(plmnlist[plmn_index].mcc & 0x00ff);
    ENCODE_U8(buffer + encoded, mcc_mnc_value, encoded);
    mcc_mnc_value = 0x0;
    mcc_mnc_value = (uint8_t)((plmnlist[plmn_index].mcc & 0x0f00) >> 8) |
                    (uint8_t)((plmnlist[plmn_index].mnc & 0x0f00) >> 4);
    ENCODE_U8(buffer + encoded, mcc_mnc_value, encoded);
    mcc_mnc_value = 0x0;
    mcc_mnc_value |= (uint8_t)(plmnlist[plmn_index].mnc & 0x00ff);
    ENCODE_U8(buffer + encoded, mcc_mnc_value, encoded);
  }

  *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
  return encoded;
}

int decode_plmn_list(
    PLMNList plmnlist, uint8_t iei, uint8_t* buffer, uint32_t len) {
  int decoded           = 0;
  uint8_t ielen         = 0;
  uint8_t mcc_mnc_value = 0x0;
  uint8_t octet_index   = 0;

  if (iei > 0) {
    CHECK_IEI_DECODER(iei, *buffer);
    decoded++;
  }

  ielen = *(buffer + decoded);
  decoded++;
  CHECK_LENGTH_DECODER(len - decoded, ielen);

  for (; octet_index < 15; octet_index++) {
    DECODE_U8(buffer + decoded, mcc_mnc_value, decoded);
    plmnlist[octet_index].mcc = 0x0000 | mcc_mnc_value;

    DECODE_U8(buffer + decoded, mcc_mnc_value, decoded);
    plmnlist[octet_index].mcc |= ((mcc_mnc_value & 0x0f) << 8);
    plmnlist[octet_index].mnc = 0x0000 | ((mcc_mnc_value & 0xf0) << 4);

    DECODE_U8(buffer + decoded, mcc_mnc_value, decoded);
    plmnlist[octet_index].mnc |= mcc_mnc_value;
  }

  return decoded;
}
