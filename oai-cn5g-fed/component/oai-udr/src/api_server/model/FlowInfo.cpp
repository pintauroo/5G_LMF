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

#include "FlowInfo.h"

namespace oai::udr::model {

FlowInfo::FlowInfo() {
  m_FlowId = 0;
  m_FlowDescriptionsIsSet = false;
}

FlowInfo::~FlowInfo() {}

void FlowInfo::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const FlowInfo &o) {
  j = nlohmann::json();
  j["flowId"] = o.m_FlowId;
  if (o.flowDescriptionsIsSet() || !o.m_FlowDescriptions.empty())
    j["flowDescriptions"] = o.m_FlowDescriptions;
}

void from_json(const nlohmann::json &j, FlowInfo &o) {
  j.at("flowId").get_to(o.m_FlowId);
  if (j.find("flowDescriptions") != j.end()) {
    j.at("flowDescriptions").get_to(o.m_FlowDescriptions);
    o.m_FlowDescriptionsIsSet = true;
  }
}

int32_t FlowInfo::getFlowId() const { return m_FlowId; }
void FlowInfo::setFlowId(int32_t const value) { m_FlowId = value; }
std::vector<std::string> &FlowInfo::getFlowDescriptions() {
  return m_FlowDescriptions;
}
void FlowInfo::setFlowDescriptions(std::vector<std::string> const &value) {
  m_FlowDescriptions = value;
  m_FlowDescriptionsIsSet = true;
}
bool FlowInfo::flowDescriptionsIsSet() const { return m_FlowDescriptionsIsSet; }
void FlowInfo::unsetFlowDescriptions() { m_FlowDescriptionsIsSet = false; }

} // namespace oai::udr::model
