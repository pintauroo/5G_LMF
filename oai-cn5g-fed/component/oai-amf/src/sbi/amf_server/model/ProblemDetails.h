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
 * ProblemDetails.h
 *
 *
 */

#ifndef ProblemDetails_H_
#define ProblemDetails_H_

#include <string>
#include "InvalidParam.h"
#include "AccessTokenErr.h"
#include "AccessTokenReq.h"
#include <vector>
#include <nlohmann/json.hpp>

namespace oai::amf::model {

/// <summary>
///
/// </summary>
class ProblemDetails {
 public:
  ProblemDetails();
  virtual ~ProblemDetails() = default;

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

  bool operator==(const ProblemDetails& rhs) const;
  bool operator!=(const ProblemDetails& rhs) const;

  /////////////////////////////////////////////
  /// ProblemDetails members

  /// <summary>
  ///
  /// </summary>
  std::string getType() const;
  void setType(std::string const& value);
  bool typeIsSet() const;
  void unsetType();
  /// <summary>
  ///
  /// </summary>
  std::string getTitle() const;
  void setTitle(std::string const& value);
  bool titleIsSet() const;
  void unsetTitle();
  /// <summary>
  ///
  /// </summary>
  int32_t getStatus() const;
  void setStatus(int32_t const value);
  bool statusIsSet() const;
  void unsetStatus();
  /// <summary>
  ///
  /// </summary>
  std::string getDetail() const;
  void setDetail(std::string const& value);
  bool detailIsSet() const;
  void unsetDetail();
  /// <summary>
  ///
  /// </summary>
  std::string getInstance() const;
  void setInstance(std::string const& value);
  bool instanceIsSet() const;
  void unsetInstance();
  /// <summary>
  ///
  /// </summary>
  std::string getCause() const;
  void setCause(std::string const& value);
  bool causeIsSet() const;
  void unsetCause();
  /// <summary>
  ///
  /// </summary>
  std::vector<InvalidParam> getInvalidParams() const;
  void setInvalidParams(std::vector<InvalidParam> const& value);
  bool invalidParamsIsSet() const;
  void unsetInvalidParams();
  /// <summary>
  ///
  /// </summary>
  std::string getSupportedFeatures() const;
  void setSupportedFeatures(std::string const& value);
  bool supportedFeaturesIsSet() const;
  void unsetSupportedFeatures();
  /// <summary>
  ///
  /// </summary>
  std::string getTargetScp() const;
  void setTargetScp(std::string const& value);
  bool targetScpIsSet() const;
  void unsetTargetScp();
  /// <summary>
  ///
  /// </summary>
  AccessTokenErr getAccessTokenError() const;
  void setAccessTokenError(AccessTokenErr const& value);
  bool accessTokenErrorIsSet() const;
  void unsetAccessTokenError();
  /// <summary>
  ///
  /// </summary>
  AccessTokenReq getAccessTokenRequest() const;
  void setAccessTokenRequest(AccessTokenReq const& value);
  bool accessTokenRequestIsSet() const;
  void unsetAccessTokenRequest();
  /// <summary>
  ///
  /// </summary>
  std::string getNrfId() const;
  void setNrfId(std::string const& value);
  bool nrfIdIsSet() const;
  void unsetNrfId();

  friend void to_json(nlohmann::json& j, const ProblemDetails& o);
  friend void from_json(const nlohmann::json& j, ProblemDetails& o);

 protected:
  std::string m_Type;
  bool m_TypeIsSet;
  std::string m_Title;
  bool m_TitleIsSet;
  int32_t m_Status;
  bool m_StatusIsSet;
  std::string m_Detail;
  bool m_DetailIsSet;
  std::string m_Instance;
  bool m_InstanceIsSet;
  std::string m_Cause;
  bool m_CauseIsSet;
  std::vector<InvalidParam> m_InvalidParams;
  bool m_InvalidParamsIsSet;
  std::string m_SupportedFeatures;
  bool m_SupportedFeaturesIsSet;
  std::string m_TargetScp;
  bool m_TargetScpIsSet;
  AccessTokenErr m_AccessTokenError;
  bool m_AccessTokenErrorIsSet;
  AccessTokenReq m_AccessTokenRequest;
  bool m_AccessTokenRequestIsSet;
  std::string m_NrfId;
  bool m_NrfIdIsSet;

  // Helper overload for validate. Used when one model stores another model and
  // calls it's validate.
  bool validate(std::stringstream& msg, const std::string& pathPrefix) const;
};

}  // namespace oai::amf::model

#endif /* ProblemDetails_H_ */
