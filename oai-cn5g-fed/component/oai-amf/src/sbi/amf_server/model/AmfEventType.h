/**
 * Namf_EventExposure
 * AMF Event Exposure Service © 2019, 3GPP Organizational Partners (ARIB, ATIS,
 * CCSA, ETSI, TSDSI, TTA, TTC). All rights reserved.
 *
 * The version of the OpenAPI document: 1.1.0.alpha-1
 *
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * (https://openapi-generator.tech). https://openapi-generator.tech Do not edit
 * the class manually.
 */
/*
 * AmfEventType.h
 *
 *
 */

#ifndef AmfEventType_H_
#define AmfEventType_H_

#include "AmfEventType_anyOf.h"
#include <nlohmann/json.hpp>

namespace oai::amf::model {

/// <summary>
///
/// </summary>
class AmfEventType {
 public:
  AmfEventType();
  virtual ~AmfEventType() = default;

  /// <summary>
  /// Validate the current data in the model. Throws a ValidationException on
  /// failure.
  /// </summary>
  void validate() const;

  /// <summary>
  /// Validate the current data in the model. Returns false on error and writes
  /// an error message into the given stringstream.
  /// </summary>
  bool validate(std::stringstream& msg) const;

  void set_value(std::string value);
  void get_value(std::string& value) const;
  std::string get_value() const;

  bool operator==(const AmfEventType& rhs) const;
  bool operator!=(const AmfEventType& rhs) const;

  /////////////////////////////////////////////
  /// AmfEventType members

  friend void to_json(nlohmann::json& j, const AmfEventType& o);
  friend void from_json(const nlohmann::json& j, AmfEventType& o);

 protected:
  std::string value;
  // Helper overload for validate. Used when one model stores another model and
  // calls it's validate.
  bool validate(std::stringstream& msg, const std::string& pathPrefix) const;
};

}  // namespace oai::amf::model

#endif /* AmfEventType_H_ */