/**
 * NRF NFManagement Service
 * NRF NFManagement Service. © 2019, 3GPP Organizational Partners (ARIB, ATIS,
 * CCSA, ETSI, TSDSI, TTA, TTC). All rights reserved.
 *
 * The version of the OpenAPI document: 1.1.0.alpha-1
 *
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * (https://openapi-generator.tech). https://openapi-generator.tech Do not edit
 * the class manually.
 */
#include "Helpers.h"

namespace oai {
namespace nrf {
namespace helpers {

std::string toStringValue(const std::string& value) {
  return std::string(value);
}

std::string toStringValue(const int32_t& value) {
  return std::to_string(value);
}

std::string toStringValue(const int64_t& value) {
  return std::to_string(value);
}

std::string toStringValue(const bool& value) {
  return value ? std::string("true") : std::string("false");
}

std::string toStringValue(const float& value) {
  return std::to_string(value);
}

std::string toStringValue(const double& value) {
  return std::to_string(value);
}

bool fromStringValue(const std::string& inStr, std::string& value) {
  value = std::string(inStr);
  return true;
}

bool fromStringValue(const std::string& inStr, int32_t& value) {
  try {
    value = std::stoi(inStr);
  } catch (const std::invalid_argument&) {
    return false;
  }
  return true;
}

bool fromStringValue(const std::string& inStr, int64_t& value) {
  try {
    value = std::stol(inStr);
  } catch (const std::invalid_argument&) {
    return false;
  }
  return true;
}

bool fromStringValue(const std::string& inStr, bool& value) {
  bool result                                = true;
  inStr == "true" ? value                    = true :
                    inStr == "false" ? value = false : result = false;
  return result;
}

bool fromStringValue(const std::string& inStr, float& value) {
  try {
    value = std::stof(inStr);
  } catch (const std::invalid_argument&) {
    return false;
  }
  return true;
}

bool fromStringValue(const std::string& inStr, double& value) {
  try {
    value = std::stod(inStr);
  } catch (const std::invalid_argument&) {
    return false;
  }
  return true;
}

bool fromStringValue(
    const std::string& inStr, oai::nrf::model::ServiceName& value) {
  // TODO
  return true;
}

bool fromStringValue(const std::string& inStr, oai::nrf::model::PlmnId& value) {
  // TODO
  return true;
}
bool fromStringValue(const std::string& inStr, oai::nrf::model::Snssai& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::PlmnSnssai& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::PduSessionType& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::EventId& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::NwdafEvent& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::AccessType& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::ComplexQuery& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::AtsssCapability& value) {
  // TODO
  return true;
}

bool fromStringValue(const std::string& inStr, oai::nrf::model::Tai& value) {
  // TODO
  return true;
}

bool fromStringValue(const std::string& inStr, oai::nrf::model::Guami& value) {
  // TODO
  return true;
}

bool fromStringValue(
    const std::string& inStr, oai::nrf::model::Ipv6Prefix& value) {
  // TODO
  return true;
}
bool fromStringValue(
    const std::string& inStr, oai::nrf::model::DataSetId& value) {
  // TODO
  return true;
}
}  // namespace helpers
}  // namespace nrf
}  // namespace oai
