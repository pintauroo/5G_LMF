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

#include "EventId.h"

namespace oai {
namespace nssf_server {
namespace model {

EventId::EventId() {}

EventId::~EventId() {}

void EventId::validate() {
  // TODO: implement validation
}

void to_json(nlohmann::json& j, const EventId& o) {
  j = nlohmann::json();
}

void from_json(const nlohmann::json& j, EventId& o) {}

}  // namespace model
}  // namespace nssf_server
}  // namespace oai
