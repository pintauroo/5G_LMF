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

/*
 * SubscriptionIDDocumentApiImpl.h
 *
 *
 */

#ifndef SUBSCRIPTION_ID_DOCUMENT_API_IMPL_H_
#define SUBSCRIPTION_ID_DOCUMENT_API_IMPL_H_

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <memory>

#include <SubscriptionIDDocumentApi.h>

#include <pistache/optional.h>

#include "PatchItem.h"
#include "ProblemDetails.h"
#include "SubscriptionData.h"
#include "nrf_app.hpp"
#include <string>
#include <vector>

namespace oai {
namespace nrf {
namespace api {

using namespace oai::nrf::model;
using namespace oai::nrf::app;

class SubscriptionIDDocumentApiImpl
    : public oai::nrf::api::SubscriptionIDDocumentApi {
 public:
  SubscriptionIDDocumentApiImpl(
      std::shared_ptr<Pistache::Rest::Router>, nrf_app* nrf_app_inst,
      std::string address);
  ~SubscriptionIDDocumentApiImpl() {}

  void remove_subscription(
      const std::string& subscriptionID,
      Pistache::Http::ResponseWriter& response);
  void update_subscription(
      const std::string& subscriptionID,
      const std::vector<PatchItem>& patchItem,
      Pistache::Http::ResponseWriter& response);

 private:
  nrf_app* m_nrf_app;
  std::string m_address;
};

}  // namespace api
}  // namespace nrf
}  // namespace oai

#endif
