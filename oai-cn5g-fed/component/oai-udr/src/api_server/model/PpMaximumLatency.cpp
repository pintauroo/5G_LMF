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

#include "PpMaximumLatency.h"

namespace oai::udr::model {

PpMaximumLatency::PpMaximumLatency() {
  m_MaximumLatency = 0;
  m_AfInstanceId = "";
  m_ReferenceId = 0;
  m_ValidityTime = "";
  m_ValidityTimeIsSet = false;
  m_MtcProviderInformation = "";
  m_MtcProviderInformationIsSet = false;
}

PpMaximumLatency::~PpMaximumLatency() {}

void PpMaximumLatency::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const PpMaximumLatency &o) {
  j = nlohmann::json();
  j["maximumLatency"] = o.m_MaximumLatency;
  j["afInstanceId"] = o.m_AfInstanceId;
  j["referenceId"] = o.m_ReferenceId;
  if (o.validityTimeIsSet())
    j["validityTime"] = o.m_ValidityTime;
  if (o.mtcProviderInformationIsSet())
    j["mtcProviderInformation"] = o.m_MtcProviderInformation;
}

void from_json(const nlohmann::json &j, PpMaximumLatency &o) {
  j.at("maximumLatency").get_to(o.m_MaximumLatency);
  j.at("afInstanceId").get_to(o.m_AfInstanceId);
  j.at("referenceId").get_to(o.m_ReferenceId);
  if (j.find("validityTime") != j.end()) {
    j.at("validityTime").get_to(o.m_ValidityTime);
    o.m_ValidityTimeIsSet = true;
  }
  if (j.find("mtcProviderInformation") != j.end()) {
    j.at("mtcProviderInformation").get_to(o.m_MtcProviderInformation);
    o.m_MtcProviderInformationIsSet = true;
  }
}

int32_t PpMaximumLatency::getMaximumLatency() const { return m_MaximumLatency; }
void PpMaximumLatency::setMaximumLatency(int32_t const value) {
  m_MaximumLatency = value;
}
std::string PpMaximumLatency::getAfInstanceId() const { return m_AfInstanceId; }
void PpMaximumLatency::setAfInstanceId(std::string const &value) {
  m_AfInstanceId = value;
}
int32_t PpMaximumLatency::getReferenceId() const { return m_ReferenceId; }
void PpMaximumLatency::setReferenceId(int32_t const value) {
  m_ReferenceId = value;
}
std::string PpMaximumLatency::getValidityTime() const { return m_ValidityTime; }
void PpMaximumLatency::setValidityTime(std::string const &value) {
  m_ValidityTime = value;
  m_ValidityTimeIsSet = true;
}
bool PpMaximumLatency::validityTimeIsSet() const { return m_ValidityTimeIsSet; }
void PpMaximumLatency::unsetValidityTime() { m_ValidityTimeIsSet = false; }
std::string PpMaximumLatency::getMtcProviderInformation() const {
  return m_MtcProviderInformation;
}
void PpMaximumLatency::setMtcProviderInformation(std::string const &value) {
  m_MtcProviderInformation = value;
  m_MtcProviderInformationIsSet = true;
}
bool PpMaximumLatency::mtcProviderInformationIsSet() const {
  return m_MtcProviderInformationIsSet;
}
void PpMaximumLatency::unsetMtcProviderInformation() {
  m_MtcProviderInformationIsSet = false;
}

} // namespace oai::udr::model