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
 * NrLocation.h
 *
 *
 */

#ifndef NrLocation_H_
#define NrLocation_H_

#include <nlohmann/json.hpp>
#include <string>

#include "GlobalRanNodeId.h"
#include "Ncgi.h"
#include "Tai.h"

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class NrLocation {
public:
  NrLocation();
  virtual ~NrLocation();

  void validate();

  /////////////////////////////////////////////
  /// NrLocation members

  /// <summary>
  ///
  /// </summary>
  Tai getTai() const;
  void setTai(Tai const &value);
  /// <summary>
  ///
  /// </summary>
  Ncgi getNcgi() const;
  void setNcgi(Ncgi const &value);
  /// <summary>
  ///
  /// </summary>
  int32_t getAgeOfLocationInformation() const;
  void setAgeOfLocationInformation(int32_t const value);
  bool ageOfLocationInformationIsSet() const;
  void unsetAgeOfLocationInformation();
  /// <summary>
  ///
  /// </summary>
  std::string getUeLocationTimestamp() const;
  void setUeLocationTimestamp(std::string const &value);
  bool ueLocationTimestampIsSet() const;
  void unsetUeLocationTimestamp();
  /// <summary>
  ///
  /// </summary>
  std::string getGeographicalInformation() const;
  void setGeographicalInformation(std::string const &value);
  bool geographicalInformationIsSet() const;
  void unsetGeographicalInformation();
  /// <summary>
  ///
  /// </summary>
  std::string getGeodeticInformation() const;
  void setGeodeticInformation(std::string const &value);
  bool geodeticInformationIsSet() const;
  void unsetGeodeticInformation();
  /// <summary>
  ///
  /// </summary>
  GlobalRanNodeId getGlobalGnbId() const;
  void setGlobalGnbId(GlobalRanNodeId const &value);
  bool globalGnbIdIsSet() const;
  void unsetGlobalGnbId();

  friend void to_json(nlohmann::json &j, const NrLocation &o);
  friend void from_json(const nlohmann::json &j, NrLocation &o);

protected:
  Tai m_Tai;

  Ncgi m_Ncgi;

  int32_t m_AgeOfLocationInformation;
  bool m_AgeOfLocationInformationIsSet;
  std::string m_UeLocationTimestamp;
  bool m_UeLocationTimestampIsSet;
  std::string m_GeographicalInformation;
  bool m_GeographicalInformationIsSet;
  std::string m_GeodeticInformation;
  bool m_GeodeticInformationIsSet;
  GlobalRanNodeId m_GlobalGnbId;
  bool m_GlobalGnbIdIsSet;
};

} // namespace oai::udr::model

#endif /* NrLocation_H_ */
