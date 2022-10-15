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

#ifndef __5GS_TRACKING_AREA_IDENTITY_H_
#define __5GS_TRACKING_AREA_IDENTITY_H_

#include <stdint.h>
#include "bstrlib.h"

#define _5GS_TRACKING_AREA_IDENTITY_MINIMUM_LENGTH 7
#define _5GS_TRACKING_AREA_IDENTITY_MAXIMUM_LENGTH 7

typedef struct {
  uint16_t mcc;
  uint16_t mnc;
  uint32_t tac;
} _5GSTrackingAreaIdentity;

int encode__5gs_tracking_area_identity(
    _5GSTrackingAreaIdentity _5gstrackingareaidentity, uint8_t iei,
    uint8_t* buffer, uint32_t len);
int decode__5gs_tracking_area_identity(
    _5GSTrackingAreaIdentity* _5gstrackingareaidentity, uint8_t iei,
    uint8_t* buffer, uint32_t len);

#endif
