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
 * ExposureDataChangeNotification.h
 *
 *
 */

#ifndef ExposureDataChangeNotification_H_
#define ExposureDataChangeNotification_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "AccessAndMobilityData.h"
#include "PduSessionManagementData.h"

namespace oai::udr::model {

/// <summary>
///
/// </summary>
class ExposureDataChangeNotification {
public:
  ExposureDataChangeNotification();
  virtual ~ExposureDataChangeNotification();

  void validate();

  /////////////////////////////////////////////
  /// ExposureDataChangeNotification members

  /// <summary>
  ///
  /// </summary>
  std::string getUeId() const;
  void setUeId(std::string const &value);
  bool ueIdIsSet() const;
  void unsetUeId();
  /// <summary>
  ///
  /// </summary>
  AccessAndMobilityData getAccessAndMobilityData() const;
  void setAccessAndMobilityData(AccessAndMobilityData const &value);
  bool accessAndMobilityDataIsSet() const;
  void unsetAccessAndMobilityData();
  /// <summary>
  ///
  /// </summary>
  std::vector<PduSessionManagementData> &getPduSessionManagementData();
  void setPduSessionManagementData(
      std::vector<PduSessionManagementData> const &value);
  bool pduSessionManagementDataIsSet() const;
  void unsetPduSessionManagementData();
  /// <summary>
  ///
  /// </summary>
  std::vector<std::string> &getDelResources();
  void setDelResources(std::vector<std::string> const &value);
  bool delResourcesIsSet() const;
  void unsetDelResources();

  friend void to_json(nlohmann::json &j,
                      const ExposureDataChangeNotification &o);
  friend void from_json(const nlohmann::json &j,
                        ExposureDataChangeNotification &o);

protected:
  std::string m_UeId;
  bool m_UeIdIsSet;
  AccessAndMobilityData m_AccessAndMobilityData;
  bool m_AccessAndMobilityDataIsSet;
  std::vector<PduSessionManagementData> m_PduSessionManagementData;
  bool m_PduSessionManagementDataIsSet;
  std::vector<std::string> m_DelResources;
  bool m_DelResourcesIsSet;
};

} // namespace oai::udr::model

#endif /* ExposureDataChangeNotification_H_ */
