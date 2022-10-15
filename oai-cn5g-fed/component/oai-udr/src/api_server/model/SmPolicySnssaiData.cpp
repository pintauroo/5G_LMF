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
/**
 * Nudr_DataRepository API OpenAPI file
 * Unified Data Repository Service. © 2020, 3GPP Organizational Partners (ARIB,
 * ATIS, CCSA, ETSI, TSDSI, TTA, TTC). All rights reserved.
 *
 * The version of the OpenAPI document: 2.1.2
 *
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * (https://openapi-generator.tech). https://openapi-generator.tech Do not edit
 * the class manually.
 */

#include "SmPolicySnssaiData.h"

namespace oai::udr::model {

SmPolicySnssaiData::SmPolicySnssaiData() { m_SmPolicyDnnDataIsSet = false; }

SmPolicySnssaiData::~SmPolicySnssaiData() {}

void SmPolicySnssaiData::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const SmPolicySnssaiData &o) {
  j = nlohmann::json();
  j["snssai"] = o.m_Snssai;
  if (o.smPolicyDnnDataIsSet() || !o.m_SmPolicyDnnData.empty())
    j["smPolicyDnnData"] = o.m_SmPolicyDnnData;
}

void from_json(const nlohmann::json &j, SmPolicySnssaiData &o) {
  j.at("snssai").get_to(o.m_Snssai);
  if (j.find("smPolicyDnnData") != j.end()) {
    j.at("smPolicyDnnData").get_to(o.m_SmPolicyDnnData);
    o.m_SmPolicyDnnDataIsSet = true;
  }
}

Snssai SmPolicySnssaiData::getSnssai() const { return m_Snssai; }
void SmPolicySnssaiData::setSnssai(Snssai const &value) { m_Snssai = value; }
std::map<std::string, SmPolicyDnnData> &
SmPolicySnssaiData::getSmPolicyDnnData() {
  return m_SmPolicyDnnData;
}
void SmPolicySnssaiData::setSmPolicyDnnData(
    std::map<std::string, SmPolicyDnnData> const &value) {
  m_SmPolicyDnnData = value;
  m_SmPolicyDnnDataIsSet = true;
}
bool SmPolicySnssaiData::smPolicyDnnDataIsSet() const {
  return m_SmPolicyDnnDataIsSet;
}
void SmPolicySnssaiData::unsetSmPolicyDnnData() {
  m_SmPolicyDnnDataIsSet = false;
}

} // namespace oai::udr::model