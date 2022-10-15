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

#ifndef FILE_3GPP_29_510_nssf_SEEN
#define FILE_3GPP_29_510_nssf_SEEN

#include <vector>
#include <nlohmann/json.hpp>
#include "3gpp_23.003.h"

// Section 28.4, TS23.003
typedef struct s_nssai {
  uint8_t sST;
  std::string sD;
  s_nssai(const uint8_t& sst, const std::string sd) : sST(sst), sD(sd) {}
  s_nssai() : sST(), sD() {}
  s_nssai(const s_nssai& p) : sST(p.sST), sD(p.sD) {}
  bool operator==(const struct s_nssai& s) const {
    if ((s.sST == this->sST) && (s.sD.compare(this->sD) == 0)) {
      return true;
    } else {
      return false;
    }
  }
  s_nssai& operator=(const s_nssai& s) {
    sST = s.sST;
    sD  = s.sD;
    return *this;
  }

} snssai_t;

typedef struct dnai_s {
} dnai_t;

typedef struct patch_item_s {
  std::string op;
  std::string path;
  // std::string from;
  std::string value;

  nlohmann::json to_json() const {
    nlohmann::json json_data = {};
    json_data["op"]          = op;
    json_data["path"]        = path;
    json_data["value"]       = value;
    return json_data;
  }
} patch_item_t;

#define NSSF_CURL_TIMEOUT_MS 100L
#define NNRF_NFM_BASE "/nnrf-nfm/"
#define NSSF_NF_REGISTER_URL "/nf-instances/"

// #### new data types
typedef struct tac_range_s {
  std::string start;
  std::string end;
  std::string pattern;
} tac_range_t;

typedef struct tai_range_s {
  plmn_t plmnid;
  std::vector<tac_range_t> tac_range_list;
  // std::string Nid;
} tai_range_t;

#endif
