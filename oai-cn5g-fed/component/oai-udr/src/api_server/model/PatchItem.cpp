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

#include "PatchItem.h"

#include <iostream>
using namespace std;

namespace oai::udr::model {

PatchItem::PatchItem() {
  m_Path = "";
  m_From = "";
  m_Value = "";
  m_FromIsSet = false;
  m_ValueIsSet = false;
}

PatchItem::~PatchItem() {}

void PatchItem::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const PatchItem &o) {
  j = nlohmann::json();
  j["op"] = o.m_Op;
  j["path"] = o.m_Path;
  if (o.fromIsSet())
    j["from"] = o.m_From;
  if (o.valueIsSet())
    j["value"] = o.m_Value;
}

void from_json(const nlohmann::json &j, PatchItem &o) {
  j.at("op").get_to(o.m_Op);
  j.at("path").get_to(o.m_Path);
  if (j.find("from") != j.end()) {
    j.at("from").get_to(o.m_From);
    o.m_FromIsSet = true;
  }
  if (j.find("value") != j.end()) {
    j.at("value").get_to(o.m_Value);
    o.m_ValueIsSet = true;
  }
}

std::string PatchItem::getOp() const { return m_Op; }
void PatchItem::setOp(std::string const &value) { m_Op = value; }
std::string PatchItem::getPath() const { return m_Path; }
void PatchItem::setPath(std::string const &value) { m_Path = value; }
std::string PatchItem::getFrom() const { return m_From; }
void PatchItem::setFrom(std::string const &value) {
  m_From = value;
  m_FromIsSet = true;
}
bool PatchItem::fromIsSet() const { return m_FromIsSet; }
void PatchItem::unsetFrom() { m_FromIsSet = false; }
std::string PatchItem::getValue() const { return m_Value; }
void PatchItem::setValue(std::string const &value) {
  m_Value = value;
  m_ValueIsSet = true;
}
bool PatchItem::valueIsSet() const { return m_ValueIsSet; }
void PatchItem::unsetValue() { m_ValueIsSet = false; }

} // namespace oai::udr::model
