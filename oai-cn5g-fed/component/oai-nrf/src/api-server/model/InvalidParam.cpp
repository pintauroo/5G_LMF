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

#include "InvalidParam.h"

namespace oai {
namespace nrf {
namespace model {

InvalidParam::InvalidParam() {
  m_Param       = "";
  m_Reason      = "";
  m_ReasonIsSet = false;
}

InvalidParam::~InvalidParam() {}

void InvalidParam::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json& j, const InvalidParam& o) {
  j          = nlohmann::json();
  j["param"] = o.m_Param;
  if (o.reasonIsSet()) j["reason"] = o.m_Reason;
}

void from_json(const nlohmann::json& j, InvalidParam& o) {
  j.at("param").get_to(o.m_Param);
  if (j.find("reason") != j.end()) {
    j.at("reason").get_to(o.m_Reason);
    o.m_ReasonIsSet = true;
  }
}

std::string InvalidParam::getParam() const {
  return m_Param;
}
void InvalidParam::setParam(std::string const& value) {
  m_Param = value;
}
std::string InvalidParam::getReason() const {
  return m_Reason;
}
void InvalidParam::setReason(std::string const& value) {
  m_Reason      = value;
  m_ReasonIsSet = true;
}
bool InvalidParam::reasonIsSet() const {
  return m_ReasonIsSet;
}
void InvalidParam::unsetReason() {
  m_ReasonIsSet = false;
}

}  // namespace model
}  // namespace nrf
}  // namespace oai
