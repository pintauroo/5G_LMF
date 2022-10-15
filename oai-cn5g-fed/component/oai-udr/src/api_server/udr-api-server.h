/**
 * RNI API
 * The ETSI MEC ISG MEC012 Radio Network Information API described using OpenAPI
 *
 * OpenAPI spec version: 1.1.1
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

#ifndef FILE_UDR_API_SERVER_SEEN
#define FILE_UDR_API_SERVER_SEEN

#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"
#ifdef __linux__
#include <signal.h>
#include <unistd.h>

#include <vector>
#endif

#include "AMF3GPPAccessRegistrationDocumentApiImpl.h"
#include "AccessAndMobilitySubscriptionDataDocumentApiImpl.h"
#include "AuthenticationStatusDocumentApiImpl.h"
#include "AuthenticationSubscriptionDocumentApiImpl.h"
#include "SDMSubscriptionDocumentApiImpl.h"
#include "SDMSubscriptionsCollectionApiImpl.h"
#include "SMFRegistrationDocumentApiImpl.h"
#include "SMFRegistrationsCollectionApiImpl.h"
#include "SMFSelectionSubscriptionDataDocumentApiImpl.h"
#include "SessionManagementSubscriptionDataApiImpl.h"

#include "udr_app.hpp"

using namespace oai::udr::app;
using namespace oai::udr::api;
using namespace oai::udr::model;
using namespace oai::udr::config;

class UDRApiServer {
public:
  UDRApiServer(Pistache::Address address, udr_app *udr_app_inst)
      : m_httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(address)) {
    m_router = std::make_shared<Pistache::Rest::Router>();
    m_address = address.host() + ":" + (address.port()).toString();

    m_authenticationSubscriptionDocumentApiserver =
        std::make_shared<AuthenticationSubscriptionDocumentApiImpl>(
            m_router, udr_app_inst, m_address);
    m_authenticationStatusDocumentApiserver =
        std::make_shared<AuthenticationStatusDocumentApiImpl>(
            m_router, udr_app_inst, m_address);
    m_accessAndMobilitySubscriptionDataDocumentApiserver =
        std::make_shared<AccessAndMobilitySubscriptionDataDocumentApiImpl>(
            m_router, udr_app_inst, m_address);
    m_sMFSelectionSubscriptionDataDocumentApiserver =
        std::make_shared<SMFSelectionSubscriptionDataDocumentApiImpl>(
            m_router, udr_app_inst, m_address);
    m_sessionManagementSubscriptionDataApiserver =
        std::make_shared<SessionManagementSubscriptionDataApiImpl>(
            m_router, udr_app_inst, m_address);
    m_aMF3GPPAccessRegistrationDocumentApiserver =
        std::make_shared<AMF3GPPAccessRegistrationDocumentApiImpl>(
            m_router, udr_app_inst, m_address);
    m_sMFRegistrationDocumentApiserver =
        std::make_shared<SMFRegistrationDocumentApiImpl>(m_router, udr_app_inst,
                                                         m_address);
    m_sMFRegistrationsCollectionApiserver =
        std::make_shared<SMFRegistrationsCollectionApiImpl>(
            m_router, udr_app_inst, m_address);
    m_sDMSubscriptionDocumentApiserver =
        std::make_shared<SDMSubscriptionDocumentApiImpl>(m_router, udr_app_inst,
                                                         m_address);
    m_sDMSubscriptionsCollectionApiserver =
        std::make_shared<SDMSubscriptionsCollectionApiImpl>(
            m_router, udr_app_inst, m_address);
  }
  void init(size_t thr = 1);
  void start();
  void shutdown();

private:
  std::shared_ptr<Pistache::Http::Endpoint> m_httpEndpoint;
  std::shared_ptr<Pistache::Rest::Router> m_router;

  std::shared_ptr<AuthenticationSubscriptionDocumentApiImpl>
      m_authenticationSubscriptionDocumentApiserver;
  std::shared_ptr<AuthenticationStatusDocumentApiImpl>
      m_authenticationStatusDocumentApiserver;
  std::shared_ptr<AccessAndMobilitySubscriptionDataDocumentApiImpl>
      m_accessAndMobilitySubscriptionDataDocumentApiserver;
  std::shared_ptr<SMFSelectionSubscriptionDataDocumentApiImpl>
      m_sMFSelectionSubscriptionDataDocumentApiserver;
  std::shared_ptr<SessionManagementSubscriptionDataApiImpl>
      m_sessionManagementSubscriptionDataApiserver;
  std::shared_ptr<AMF3GPPAccessRegistrationDocumentApiImpl>
      m_aMF3GPPAccessRegistrationDocumentApiserver;
  std::shared_ptr<SMFRegistrationDocumentApiImpl>
      m_sMFRegistrationDocumentApiserver;
  std::shared_ptr<SMFRegistrationsCollectionApiImpl>
      m_sMFRegistrationsCollectionApiserver;
  std::shared_ptr<SDMSubscriptionDocumentApiImpl>
      m_sDMSubscriptionDocumentApiserver;
  std::shared_ptr<SDMSubscriptionsCollectionApiImpl>
      m_sDMSubscriptionsCollectionApiserver;

  std::string m_address;
};

#endif
