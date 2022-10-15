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

#ifndef FILE_3GPP_29_571_SEEN
#define FILE_3GPP_29_571_SEEN

#include "3gpp_23.003.h"
#include "3gpp_29.510.h"

#include <vector>

enum access_type_e { ACESS_3GPP = 1, ACESS_NON_3GPP = 2 };

static const std::vector<std::string> access_type_e2str = {"3GPP_ACCESS",
                                                           "NON_3GPP_ACCESS"};

typedef struct sd_range_s {
  std::string start;
  std::string end;
} sd_range_t;

typedef struct snssai_extension_s {
  std::vector<sd_range_t> sd_ranges;
  bool wildcard_sd;
} snssai_extension_t;

typedef struct ext_snssai_s {
  snssai_t snssai;
  snssai_extension_t snssai_extension;
} ext_snssai_t;
#endif
