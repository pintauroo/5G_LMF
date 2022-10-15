/**
 * AUSF API
 * AUSF UE Authentication Service. © 2020, 3GPP Organizational Partners (ARIB,
 * ATIS, CCSA, ETSI, TSDSI, TTA, TTC). All rights reserved.
 *
 * The version of the OpenAPI document: 1.1.1
 *
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * (https://openapi-generator.tech). https://openapi-generator.tech Do not edit
 * the class manually.
 */
/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this
 *file except in compliance with the License. You may obtain a copy of the
 *License at
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

/*
 * DefaultApi.h
 *
 *
 */

#ifndef DefaultApi_H_
#define DefaultApi_H_

#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <pistache/optional.h>
#include <pistache/router.h>

#include "AuthenticationInfo.h"
#include "ConfirmationData.h"
#include "ConfirmationDataResponse.h"
#include "DeregistrationInfo.h"
#include "EapSession.h"
#include "ProblemDetails.h"
#include "RgAuthCtx.h"
#include "RgAuthenticationInfo.h"
#include "UEAuthenticationCtx.h"
#include <string>

namespace oai {
namespace ausf_server {
namespace api {

using namespace oai::ausf_server::model;

class DefaultApi {
 public:
  DefaultApi(std::shared_ptr<Pistache::Rest::Router>);
  virtual ~DefaultApi() {}
  void init();

  const std::string base = "/nausf-auth/v1";

 private:
  void setupRoutes();

  void eap_auth_method_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
  void rg_authentications_post_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
  void ue_authentications_auth_ctx_id5g_aka_confirmation_put_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
  void ue_authentications_deregister_post_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
  void ue_authentications_post_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
  void default_api_default_handler(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);

  std::shared_ptr<Pistache::Rest::Router> router;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  ///
  /// </remarks>
  /// <param name="authCtxId"></param>
  /// <param name="eapSession"> (optional)</param>
  virtual void eap_auth_method(
      const std::string& authCtxId, const EapSession& eapSession,
      Pistache::Http::ResponseWriter& response) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  ///
  /// </remarks>
  /// <param name="rgAuthenticationInfo"></param>
  virtual void rg_authentications_post(
      const RgAuthenticationInfo& rgAuthenticationInfo,
      Pistache::Http::ResponseWriter& response) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  ///
  /// </remarks>
  /// <param name="authCtxId"></param>
  /// <param name="confirmationData"> (optional)</param>
  virtual void ue_authentications_auth_ctx_id5g_aka_confirmation_put(
      const std::string& authCtxId, const ConfirmationData& confirmationData,
      Pistache::Http::ResponseWriter& response) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  ///
  /// </remarks>
  /// <param name="deregistrationInfo"></param>
  virtual void ue_authentications_deregister_post(
      const DeregistrationInfo& deregistrationInfo,
      Pistache::Http::ResponseWriter& response) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  ///
  /// </remarks>
  /// <param name="authenticationInfo"></param>
  virtual void ue_authentications_post(
      const AuthenticationInfo& authenticationInfo,
      Pistache::Http::ResponseWriter& response) = 0;
};

}  // namespace api
}  // namespace ausf_server
}  // namespace oai

#endif /* DefaultApi_H_ */
