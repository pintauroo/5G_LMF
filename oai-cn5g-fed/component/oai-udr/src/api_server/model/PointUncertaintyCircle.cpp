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

#include "PointUncertaintyCircle.h"

namespace oai::udr::model {

PointUncertaintyCircle::PointUncertaintyCircle() { m_Uncertainty = 0.0f; }

PointUncertaintyCircle::~PointUncertaintyCircle() {}

void PointUncertaintyCircle::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json &j, const PointUncertaintyCircle &o) {
  j = nlohmann::json();
  j["shape"] = o.m_Shape;
  j["point"] = o.m_Point;
  j["uncertainty"] = o.m_Uncertainty;
}

void from_json(const nlohmann::json &j, PointUncertaintyCircle &o) {
  j.at("shape").get_to(o.m_Shape);
  j.at("point").get_to(o.m_Point);
  j.at("uncertainty").get_to(o.m_Uncertainty);
}

SupportedGADShapes PointUncertaintyCircle::getShape() const { return m_Shape; }
void PointUncertaintyCircle::setShape(SupportedGADShapes const &value) {
  m_Shape = value;
}
GeographicalCoordinates PointUncertaintyCircle::getPoint() const {
  return m_Point;
}
void PointUncertaintyCircle::setPoint(GeographicalCoordinates const &value) {
  m_Point = value;
}
float PointUncertaintyCircle::getUncertainty() const { return m_Uncertainty; }
void PointUncertaintyCircle::setUncertainty(float const value) {
  m_Uncertainty = value;
}

} // namespace oai::udr::model
