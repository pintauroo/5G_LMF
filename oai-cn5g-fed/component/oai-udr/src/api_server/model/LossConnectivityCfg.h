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
 * LossConnectivityCfg.h
 *
 *
 */

#ifndef LossConnectivityCfg_H_
#define LossConnectivityCfg_H_

#include <nlohmann/json.hpp>

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class LossConnectivityCfg {
public:
  LossConnectivityCfg();
  virtual ~LossConnectivityCfg();

  void validate();

  /////////////////////////////////////////////
  /// LossConnectivityCfg members

  /// <summary>
  ///
  /// </summary>
  int32_t getMaxDetectionTime() const;
  void setMaxDetectionTime(int32_t const value);
  bool maxDetectionTimeIsSet() const;
  void unsetMaxDetectionTime();

  friend void to_json(nlohmann::json &j, const LossConnectivityCfg &o);
  friend void from_json(const nlohmann::json &j, LossConnectivityCfg &o);

protected:
  int32_t m_MaxDetectionTime;
  bool m_MaxDetectionTimeIsSet;
};

} // namespace oai::udr::model

#endif /* LossConnectivityCfg_H_ */
