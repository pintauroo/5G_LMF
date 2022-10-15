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
 * LocationFilter_anyOf.h
 *
 *
 */

#ifndef LocationFilter_anyOf_H_
#define LocationFilter_anyOf_H_

#include <nlohmann/json.hpp>

namespace oai::amf::model {

/// <summary>
///
/// </summary>
class LocationFilter_anyOf {
 public:
  LocationFilter_anyOf();
  virtual ~LocationFilter_anyOf() = default;

  enum class eLocationFilter_anyOf {
    // To have a valid default value.
    // Avoiding nameclashes with user defined
    // enum values
    INVALID_VALUE_OPENAPI_GENERATED = 0,
    TAI,
    CELL_ID,
    N3IWF,
    UE_IP,
    UDP_PORT
  };

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

  bool operator==(const LocationFilter_anyOf& rhs) const;
  bool operator!=(const LocationFilter_anyOf& rhs) const;

  /////////////////////////////////////////////
  /// LocationFilter_anyOf members

  LocationFilter_anyOf::eLocationFilter_anyOf getValue() const;
  void setValue(LocationFilter_anyOf::eLocationFilter_anyOf value);

  friend void to_json(nlohmann::json& j, const LocationFilter_anyOf& o);
  friend void from_json(const nlohmann::json& j, LocationFilter_anyOf& o);

 protected:
  LocationFilter_anyOf::eLocationFilter_anyOf m_value = LocationFilter_anyOf::
      eLocationFilter_anyOf::INVALID_VALUE_OPENAPI_GENERATED;

  // Helper overload for validate. Used when one model stores another model and
  // calls it's validate.
  bool validate(std::stringstream& msg, const std::string& pathPrefix) const;
};

}  // namespace oai::amf::model

#endif /* LocationFilter_anyOf_H_ */