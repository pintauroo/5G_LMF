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
/*
 * AfExternal.h
 *
 *
 */

#ifndef AfExternal_H_
#define AfExternal_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "GeographicArea.h"
#include "PrivacyCheckRelatedAction.h"
#include "ValidTimePeriod.h"

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class AfExternal {
public:
  AfExternal();
  virtual ~AfExternal();

  void validate();

  /////////////////////////////////////////////
  /// AfExternal members

  /// <summary>
  ///
  /// </summary>
  std::string getAfId() const;
  void setAfId(std::string const &value);
  bool afIdIsSet() const;
  void unsetAfId();
  /// <summary>
  ///
  /// </summary>
  std::vector<GeographicArea> &getAllowedGeographicArea();
  void setAllowedGeographicArea(std::vector<GeographicArea> const &value);
  bool allowedGeographicAreaIsSet() const;
  void unsetAllowedGeographicArea();
  /// <summary>
  ///
  /// </summary>
  PrivacyCheckRelatedAction getPrivacyCheckRelatedAction() const;
  void setPrivacyCheckRelatedAction(PrivacyCheckRelatedAction const &value);
  bool privacyCheckRelatedActionIsSet() const;
  void unsetPrivacyCheckRelatedAction();
  /// <summary>
  ///
  /// </summary>
  ValidTimePeriod getValidTimePeriod() const;
  void setValidTimePeriod(ValidTimePeriod const &value);
  bool validTimePeriodIsSet() const;
  void unsetValidTimePeriod();

  friend void to_json(nlohmann::json &j, const AfExternal &o);
  friend void from_json(const nlohmann::json &j, AfExternal &o);

protected:
  std::string m_AfId;
  bool m_AfIdIsSet;
  std::vector<GeographicArea> m_AllowedGeographicArea;
  bool m_AllowedGeographicAreaIsSet;
  PrivacyCheckRelatedAction m_PrivacyCheckRelatedAction;
  bool m_PrivacyCheckRelatedActionIsSet;
  ValidTimePeriod m_ValidTimePeriod;
  bool m_ValidTimePeriodIsSet;
};

} // namespace oai::udr::model

#endif /* AfExternal_H_ */
