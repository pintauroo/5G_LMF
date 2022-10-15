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
 * PgwInfo.h
 *
 *
 */

#ifndef PgwInfo_H_
#define PgwInfo_H_

#include <nlohmann/json.hpp>
#include <string>

#include "PlmnId.h"

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class PgwInfo {
public:
  PgwInfo();
  virtual ~PgwInfo();

  void validate();

  /////////////////////////////////////////////
  /// PgwInfo members

  /// <summary>
  ///
  /// </summary>
  std::string getDnn() const;
  void setDnn(std::string const &value);
  /// <summary>
  ///
  /// </summary>
  std::string getPgwFqdn() const;
  void setPgwFqdn(std::string const &value);
  /// <summary>
  ///
  /// </summary>
  PlmnId getPlmnId() const;
  void setPlmnId(PlmnId const &value);
  bool plmnIdIsSet() const;
  void unsetPlmnId();
  /// <summary>
  ///
  /// </summary>
  bool isEpdgInd() const;
  void setEpdgInd(bool const value);
  bool epdgIndIsSet() const;
  void unsetEpdgInd();

  friend void to_json(nlohmann::json &j, const PgwInfo &o);
  friend void from_json(const nlohmann::json &j, PgwInfo &o);

protected:
  std::string m_Dnn;

  std::string m_PgwFqdn;

  PlmnId m_PlmnId;
  bool m_PlmnIdIsSet;
  bool m_EpdgInd;
  bool m_EpdgIndIsSet;
};

} // namespace oai::udr::model

#endif /* PgwInfo_H_ */
