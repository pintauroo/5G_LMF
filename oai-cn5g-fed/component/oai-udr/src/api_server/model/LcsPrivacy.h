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
 * LcsPrivacy.h
 *
 *
 */

#ifndef LcsPrivacy_H_
#define LcsPrivacy_H_

#include <nlohmann/json.hpp>
#include <string>

#include "Lpi.h"

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class LcsPrivacy {
public:
  LcsPrivacy();
  virtual ~LcsPrivacy();

  void validate();

  /////////////////////////////////////////////
  /// LcsPrivacy members

  /// <summary>
  ///
  /// </summary>
  std::string getAfInstanceId() const;
  void setAfInstanceId(std::string const &value);
  bool afInstanceIdIsSet() const;
  void unsetAfInstanceId();
  /// <summary>
  ///
  /// </summary>
  int32_t getReferenceId() const;
  void setReferenceId(int32_t const value);
  bool referenceIdIsSet() const;
  void unsetReferenceId();
  /// <summary>
  ///
  /// </summary>
  Lpi getLpi() const;
  void setLpi(Lpi const &value);
  bool lpiIsSet() const;
  void unsetLpi();
  /// <summary>
  ///
  /// </summary>
  std::string getMtcProviderInformation() const;
  void setMtcProviderInformation(std::string const &value);
  bool mtcProviderInformationIsSet() const;
  void unsetMtcProviderInformation();

  friend void to_json(nlohmann::json &j, const LcsPrivacy &o);
  friend void from_json(const nlohmann::json &j, LcsPrivacy &o);

protected:
  std::string m_AfInstanceId;
  bool m_AfInstanceIdIsSet;
  int32_t m_ReferenceId;
  bool m_ReferenceIdIsSet;
  Lpi m_Lpi;
  bool m_LpiIsSet;
  std::string m_MtcProviderInformation;
  bool m_MtcProviderInformationIsSet;
};

} // namespace oai::udr::model

#endif /* LcsPrivacy_H_ */
