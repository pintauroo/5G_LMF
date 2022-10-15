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

#ifndef FILE_3GPP_24_501_SEEN
#define FILE_3GPP_24_501_SEEN

// Table 10.3.1 @3GPP TS 24.501 V16.1.0 (2019-06)
#define T3512_TIMER_VALUE_SEC 3240  // 54 minutes
#define T3512_TIMER_VALUE_MIN 54    // 54 minutes
#define MOBILE_REACHABLE_TIMER_NO_EMERGENCY_SERVICES_MIN                       \
  (T3512_TIMER_VALUE_MIN + 4)  // T3512 + 4, not for emergency services
#define IMPLICIT_DEREGISTRATION_TIMER_MIN (T3512_TIMER_VALUE_MIN + 4)
#endif
