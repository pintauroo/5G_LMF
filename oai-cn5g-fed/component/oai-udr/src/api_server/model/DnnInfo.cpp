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

#include "DnnInfo.h"

namespace oai::udr::model {

DnnInfo::DnnInfo() {
  m_DefaultDnnIndicator = false;
  m_DefaultDnnIndicatorIsSet = false;
  m_LboRoamingAllowed = false;
  m_LboRoamingAllowedIsSet = false;
  m_IwkEpsInd = false;
  m_IwkEpsIndIsSet = false;
  m_DnnBarred = false;
  m_DnnBarredIsSet = false;
  m_InvokeNefInd = false;
  m_InvokeNefIndIsSet = false;
  m_SmfListIsSet = false;
  m_SameSmfInd = false;
  m_SameSmfIndIsSet = false;
}

DnnInfo::~DnnInfo() {}

void DnnInfo::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const DnnInfo &o) {
  j = nlohmann::json();
  //    j["dnn"] = o.m_Dnn;
  if (o.defaultDnnIndicatorIsSet())
    j["defaultDnnIndicator"] = o.m_DefaultDnnIndicator;
  if (o.lboRoamingAllowedIsSet())
    j["lboRoamingAllowed"] = o.m_LboRoamingAllowed;
  if (o.iwkEpsIndIsSet())
    j["iwkEpsInd"] = o.m_IwkEpsInd;
  if (o.dnnBarredIsSet())
    j["dnnBarred"] = o.m_DnnBarred;
  if (o.invokeNefIndIsSet())
    j["invokeNefInd"] = o.m_InvokeNefInd;
  if (o.smfListIsSet() || !o.m_SmfList.empty())
    j["smfList"] = o.m_SmfList;
  if (o.sameSmfIndIsSet())
    j["sameSmfInd"] = o.m_SameSmfInd;
}

void from_json(const nlohmann::json &j, DnnInfo &o) {
  //   j.at("dnn").get_to(o.m_Dnn);
  if (j.find("defaultDnnIndicator") != j.end()) {
    j.at("defaultDnnIndicator").get_to(o.m_DefaultDnnIndicator);
    o.m_DefaultDnnIndicatorIsSet = true;
  }
  if (j.find("lboRoamingAllowed") != j.end()) {
    j.at("lboRoamingAllowed").get_to(o.m_LboRoamingAllowed);
    o.m_LboRoamingAllowedIsSet = true;
  }
  if (j.find("iwkEpsInd") != j.end()) {
    j.at("iwkEpsInd").get_to(o.m_IwkEpsInd);
    o.m_IwkEpsIndIsSet = true;
  }
  if (j.find("dnnBarred") != j.end()) {
    j.at("dnnBarred").get_to(o.m_DnnBarred);
    o.m_DnnBarredIsSet = true;
  }
  if (j.find("invokeNefInd") != j.end()) {
    j.at("invokeNefInd").get_to(o.m_InvokeNefInd);
    o.m_InvokeNefIndIsSet = true;
  }
  if (j.find("smfList") != j.end()) {
    j.at("smfList").get_to(o.m_SmfList);
    o.m_SmfListIsSet = true;
  }
  if (j.find("sameSmfInd") != j.end()) {
    j.at("sameSmfInd").get_to(o.m_SameSmfInd);
    o.m_SameSmfIndIsSet = true;
  }
}

// AnyOfstringstring DnnInfo::getDnn() const
//{
//    return m_Dnn;
//}
// void DnnInfo::setDnn(AnyOfstringstring const& value)
//{
//    m_Dnn = value;
//}
bool DnnInfo::isDefaultDnnIndicator() const { return m_DefaultDnnIndicator; }
void DnnInfo::setDefaultDnnIndicator(bool const value) {
  m_DefaultDnnIndicator = value;
  m_DefaultDnnIndicatorIsSet = true;
}
bool DnnInfo::defaultDnnIndicatorIsSet() const {
  return m_DefaultDnnIndicatorIsSet;
}
void DnnInfo::unsetDefaultDnnIndicator() { m_DefaultDnnIndicatorIsSet = false; }
bool DnnInfo::isLboRoamingAllowed() const { return m_LboRoamingAllowed; }
void DnnInfo::setLboRoamingAllowed(bool const value) {
  m_LboRoamingAllowed = value;
  m_LboRoamingAllowedIsSet = true;
}
bool DnnInfo::lboRoamingAllowedIsSet() const {
  return m_LboRoamingAllowedIsSet;
}
void DnnInfo::unsetLboRoamingAllowed() { m_LboRoamingAllowedIsSet = false; }
bool DnnInfo::isIwkEpsInd() const { return m_IwkEpsInd; }
void DnnInfo::setIwkEpsInd(bool const value) {
  m_IwkEpsInd = value;
  m_IwkEpsIndIsSet = true;
}
bool DnnInfo::iwkEpsIndIsSet() const { return m_IwkEpsIndIsSet; }
void DnnInfo::unsetIwkEpsInd() { m_IwkEpsIndIsSet = false; }
bool DnnInfo::isDnnBarred() const { return m_DnnBarred; }
void DnnInfo::setDnnBarred(bool const value) {
  m_DnnBarred = value;
  m_DnnBarredIsSet = true;
}
bool DnnInfo::dnnBarredIsSet() const { return m_DnnBarredIsSet; }
void DnnInfo::unsetDnnBarred() { m_DnnBarredIsSet = false; }
bool DnnInfo::isInvokeNefInd() const { return m_InvokeNefInd; }
void DnnInfo::setInvokeNefInd(bool const value) {
  m_InvokeNefInd = value;
  m_InvokeNefIndIsSet = true;
}
bool DnnInfo::invokeNefIndIsSet() const { return m_InvokeNefIndIsSet; }
void DnnInfo::unsetInvokeNefInd() { m_InvokeNefIndIsSet = false; }
std::vector<std::string> &DnnInfo::getSmfList() { return m_SmfList; }
void DnnInfo::setSmfList(std::vector<std::string> const &value) {
  m_SmfList = value;
  m_SmfListIsSet = true;
}
bool DnnInfo::smfListIsSet() const { return m_SmfListIsSet; }
void DnnInfo::unsetSmfList() { m_SmfListIsSet = false; }
bool DnnInfo::isSameSmfInd() const { return m_SameSmfInd; }
void DnnInfo::setSameSmfInd(bool const value) {
  m_SameSmfInd = value;
  m_SameSmfIndIsSet = true;
}
bool DnnInfo::sameSmfIndIsSet() const { return m_SameSmfIndIsSet; }
void DnnInfo::unsetSameSmfInd() { m_SameSmfIndIsSet = false; }

} // namespace oai::udr::model
