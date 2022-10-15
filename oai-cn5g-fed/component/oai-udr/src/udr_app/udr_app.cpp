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

/*! \file udr_app.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: Tien-Thinh.Nguyen@eurecom.fr
 */

#include "udr_app.hpp"
#include <mysql/mysql.h>
#include <unistd.h>

#include "3gpp_29.500.h"
#include "AccessAndMobilitySubscriptionData.h"
#include "AuthenticationSubscription.h"
#include "ProblemDetails.h"
#include "SequenceNumber.h"
#include "logger.hpp"
#include "udr_config.hpp"

using namespace oai::udr::app;
using namespace oai::udr::model;
using namespace oai::udr::config;

extern udr_app *udr_app_inst;
extern udr_config udr_cfg;

//------------------------------------------------------------------------------
udr_app::udr_app(const std::string &config_file) {
  Logger::udr_app().startup("Starting...");

  if (!mysql_init(&mysql)) {
    Logger::udr_app().error("Cannot initialize MySQL");
    throw std::runtime_error("Cannot initialize MySQL");
  }

  if (!mysql_real_connect(&mysql, udr_cfg.mysql.mysql_server.c_str(),
                          udr_cfg.mysql.mysql_user.c_str(),
                          udr_cfg.mysql.mysql_pass.c_str(),
                          udr_cfg.mysql.mysql_db.c_str(), 0, 0, 0)) {
    Logger::udr_app().error(
        "An error occurred while connecting to MySQL DB: %s",
        mysql_error(&mysql));
    throw std::runtime_error("Cannot connect to MySQL DB");
  }

  // TODO: Register to NRF
  Logger::udr_app().startup("Started");
}

//------------------------------------------------------------------------------
udr_app::~udr_app() { Logger::udr_app().debug("Delete UDR APP instance..."); }

//------------------------------------------------------------------------------
void udr_app::handle_query_am_data(const std::string &ue_id,
                                   const std::string &serving_plmn_id,
                                   nlohmann::json &response_data, long &code) {
  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  response_data = {};

  oai::udr::model::AccessAndMobilitySubscriptionData subscription_data = {};

  // TODO: Define query template in a header file
  const std::string query =
      "select * from AccessAndMobilitySubscriptionData WHERE ueid='" + ue_id +
      "' and servingPlmnid='" + serving_plmn_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！");
    // code = Pistache::Http::Code::Internal_Server_Error;
    code = HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！");
    // code = Pistache::Http::Code::Internal_Server_Error;
    code = HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    return;
  }

  row = mysql_fetch_row(res);

  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      try {
        if (!strcmp("supportedFeatures", field->name) && row[i] != NULL) {
          subscription_data.setSupportedFeatures(row[i]);
        } else if (!strcmp("gpsis", field->name) && row[i] != NULL) {
          std::vector<std ::string> gpsis;
          nlohmann::json::parse(row[i]).get_to(gpsis);
          subscription_data.setGpsis(gpsis);
        } else if (!strcmp("internalGroupIds", field->name) && row[i] != NULL) {
          std::vector<std ::string> internalgroupids;
          nlohmann::json::parse(row[i]).get_to(internalgroupids);
          subscription_data.setInternalGroupIds(internalgroupids);
        } else if (!strcmp("sharedVnGroupDataIds", field->name) &&
                   row[i] != NULL) {
          std::map<std ::string, std::string> sharedvngroupdataids;
          nlohmann::json::parse(row[i]).get_to(sharedvngroupdataids);
          subscription_data.setSharedVnGroupDataIds(sharedvngroupdataids);
        } else if (!strcmp("subscribedUeAmbr", field->name) && row[i] != NULL) {
          AmbrRm subscribedueambr;
          nlohmann::json::parse(row[i]).get_to(subscribedueambr);
          subscription_data.setSubscribedUeAmbr(subscribedueambr);
        } else if (!strcmp("nssai", field->name) && row[i] != NULL) {
          Nssai nssai;
          nlohmann::json::parse(row[i]).get_to(nssai);
          subscription_data.setNssai(nssai);
        } else if (!strcmp("ratRestrictions", field->name) && row[i] != NULL) {
          std ::vector<RatType> ratrestrictions;
          nlohmann::json::parse(row[i]).get_to(ratrestrictions);
          subscription_data.setRatRestrictions(ratrestrictions);
        } else if (!strcmp("forbiddenAreas", field->name) && row[i] != NULL) {
          std ::vector<Area> forbiddenareas;
          nlohmann::json::parse(row[i]).get_to(forbiddenareas);
          subscription_data.setForbiddenAreas(forbiddenareas);
        } else if (!strcmp("serviceAreaRestriction", field->name) &&
                   row[i] != NULL) {
          ServiceAreaRestriction servicearearestriction;
          nlohmann::json::parse(row[i]).get_to(servicearearestriction);
          subscription_data.setServiceAreaRestriction(servicearearestriction);
        } else if (!strcmp("coreNetworkTypeRestrictions", field->name) &&
                   row[i] != NULL) {
          std ::vector<CoreNetworkType> corenetworktyperestrictions;
          nlohmann::json::parse(row[i]).get_to(corenetworktyperestrictions);
          subscription_data.setCoreNetworkTypeRestrictions(
              corenetworktyperestrictions);
        } else if (!strcmp("rfspIndex", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setRfspIndex(a);
        } else if (!strcmp("subsRegTimer", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setSubsRegTimer(a);
        } else if (!strcmp("ueUsageType", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setUeUsageType(a);
        } else if (!strcmp("mpsPriority", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setMpsPriority(true);
          else
            subscription_data.setMpsPriority(false);
        } else if (!strcmp("mcsPriority", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setMcsPriority(true);
          else
            subscription_data.setMcsPriority(false);
        } else if (!strcmp("activeTime", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setActiveTime(a);
        } else if (!strcmp("sorInfo", field->name) && row[i] != NULL) {
          SorInfo sorinfo;
          nlohmann::json::parse(row[i]).get_to(sorinfo);
          subscription_data.setSorInfo(sorinfo);
        } else if (!strcmp("sorInfoExpectInd", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setSorInfoExpectInd(true);
          else
            subscription_data.setSorInfoExpectInd(false);
        } else if (!strcmp("sorafRetrieval", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setSorafRetrieval(true);
          else
            subscription_data.setSorafRetrieval(false);
        } else if (!strcmp("sorUpdateIndicatorList", field->name) &&
                   row[i] != NULL) {
          std ::vector<SorUpdateIndicator> sorupdateindicatorlist;
          nlohmann::json::parse(row[i]).get_to(sorupdateindicatorlist);
          subscription_data.setSorUpdateIndicatorList(sorupdateindicatorlist);
        } else if (!strcmp("upuInfo", field->name) && row[i] != NULL) {
          UpuInfo upuinfo;
          nlohmann::json::parse(row[i]).get_to(upuinfo);
          subscription_data.setUpuInfo(upuinfo);
        } else if (!strcmp("micoAllowed", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setMicoAllowed(true);
          else
            subscription_data.setMicoAllowed(false);
        } else if (!strcmp("sharedAmDataIds", field->name) && row[i] != NULL) {
          std ::vector<std ::string> sharedamdataids;
          nlohmann::json::parse(row[i]).get_to(sharedamdataids);
          subscription_data.setSharedAmDataIds(sharedamdataids);
        } else if (!strcmp("odbPacketServices", field->name) &&
                   row[i] != NULL) {
          OdbPacketServices odbpacketservices;
          nlohmann::json::parse(row[i]).get_to(odbpacketservices);
          subscription_data.setOdbPacketServices(odbpacketservices);
        } else if (!strcmp("serviceGapTime", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setServiceGapTime(a);
        } else if (!strcmp("mdtUserConsent", field->name) && row[i] != NULL) {
          MdtUserConsent mdtuserconsent;
          nlohmann::json::parse(row[i]).get_to(mdtuserconsent);
          subscription_data.setMdtUserConsent(mdtuserconsent);
        } else if (!strcmp("mdtConfiguration", field->name) && row[i] != NULL) {
          MdtConfiguration mdtconfiguration;
          nlohmann::json::parse(row[i]).get_to(mdtconfiguration);
          subscription_data.setMdtConfiguration(mdtconfiguration);
        } else if (!strcmp("traceData", field->name) && row[i] != NULL) {
          TraceData tracedata;
          nlohmann::json::parse(row[i]).get_to(tracedata);
          subscription_data.setTraceData(tracedata);
        } else if (!strcmp("cagData", field->name) && row[i] != NULL) {
          CagData cagdata;
          nlohmann::json::parse(row[i]).get_to(cagdata);
          subscription_data.setCagData(cagdata);
        } else if (!strcmp("stnSr", field->name) && row[i] != NULL) {
          subscription_data.setStnSr(row[i]);
        } else if (!strcmp("cMsisdn", field->name) && row[i] != NULL) {
          subscription_data.setCMsisdn(row[i]);
        } else if (!strcmp("nbIoTUePriority", field->name) && row[i] != NULL) {
          int32_t a = std::stoi(row[i]);
          subscription_data.setNbIoTUePriority(a);
        } else if (!strcmp("nssaiInclusionAllowed", field->name) &&
                   row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setNssaiInclusionAllowed(true);
          else
            subscription_data.setNssaiInclusionAllowed(false);
        } else if (!strcmp("rgWirelineCharacteristics", field->name) &&
                   row[i] != NULL) {
          subscription_data.setRgWirelineCharacteristics(row[i]);
        } else if (!strcmp("ecRestrictionDataWb", field->name) &&
                   row[i] != NULL) {
          EcRestrictionDataWb ecrestrictiondatawb;
          nlohmann::json::parse(row[i]).get_to(ecrestrictiondatawb);
          subscription_data.setEcRestrictionDataWb(ecrestrictiondatawb);
        } else if (!strcmp("ecRestrictionDataNb", field->name) &&
                   row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setEcRestrictionDataNb(true);
          else
            subscription_data.setEcRestrictionDataNb(false);
        } else if (!strcmp("expectedUeBehaviourList", field->name) &&
                   row[i] != NULL) {
          ExpectedUeBehaviourData expecteduebehaviourlist;
          nlohmann::json::parse(row[i]).get_to(expecteduebehaviourlist);
          subscription_data.setExpectedUeBehaviourList(expecteduebehaviourlist);
        } else if (!strcmp("primaryRatRestrictions", field->name) &&
                   row[i] != NULL) {
          std ::vector<RatType> primaryratrestrictions;
          nlohmann::json::parse(row[i]).get_to(primaryratrestrictions);
          subscription_data.setPrimaryRatRestrictions(primaryratrestrictions);
        } else if (!strcmp("secondaryRatRestrictions", field->name) &&
                   row[i] != NULL) {
          std ::vector<RatType> secondaryratrestrictions;
          nlohmann::json::parse(row[i]).get_to(secondaryratrestrictions);
          subscription_data.setSecondaryRatRestrictions(
              secondaryratrestrictions);
        } else if (!strcmp("edrxParametersList", field->name) &&
                   row[i] != NULL) {
          std ::vector<EdrxParameters> edrxparameterslist;
          nlohmann::json::parse(row[i]).get_to(edrxparameterslist);
          subscription_data.setEdrxParametersList(edrxparameterslist);
        } else if (!strcmp("ptwParametersList", field->name) &&
                   row[i] != NULL) {
          std ::vector<PtwParameters> ptwparameterslist;
          nlohmann::json::parse(row[i]).get_to(ptwparameterslist);
          subscription_data.setPtwParametersList(ptwparameterslist);
        } else if (!strcmp("iabOperationAllowed", field->name) &&
                   row[i] != NULL) {
          if (strcmp(row[i], "0"))
            subscription_data.setIabOperationAllowed(true);
          else
            subscription_data.setIabOperationAllowed(false);
        } else if (!strcmp("wirelineForbiddenAreas", field->name) &&
                   row[i] != NULL) {
          std ::vector<WirelineArea> wirelineforbiddenareas;
          nlohmann::json::parse(row[i]).get_to(wirelineforbiddenareas);
          subscription_data.setWirelineForbiddenAreas(wirelineforbiddenareas);
        } else if (!strcmp("wirelineServiceAreaRestriction", field->name) &&
                   row[i] != NULL) {
          WirelineServiceAreaRestriction wirelineservicearearestriction;
          nlohmann::json::parse(row[i]).get_to(wirelineservicearearestriction);
          subscription_data.setWirelineServiceAreaRestriction(
              wirelineservicearearestriction);
        }
      } catch (std::exception e) {
        Logger::udr_server().error(
            " Cannot set values for Subscription Data: %s", e.what());
      }
    }

    to_json(j, subscription_data);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug(
        "AccessAndMobilitySubscriptionData GET - json:\n\"%s\"",
        j.dump().c_str());
  } else {
    Logger::udr_server().error("AccessAndMobilitySubscriptionData no data！");
    // code = Pistache::Http::Code::Internal_Server_Error;
    code = HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_create_amf_context_3gpp(
    const std::string &ue_id,
    Amf3GppAccessRegistration &amf3GppAccessRegistration,
    nlohmann::json &response_data, long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};

  const std::string select_AMF3GPPAccessRegistration =
      "select * from Amf3GppAccessRegistration WHERE ueid='" + ue_id + "'";
  std::string query = {};

  nlohmann::json j = {};

  if (mysql_real_query(
          &mysql, select_AMF3GPPAccessRegistration.c_str(),
          (unsigned long)select_AMF3GPPAccessRegistration.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               select_AMF3GPPAccessRegistration.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               select_AMF3GPPAccessRegistration.c_str());
    return;
  }
  if (mysql_num_rows(res)) {
    query =
        "update Amf3GppAccessRegistration set amfInstanceId='" +
        amf3GppAccessRegistration.getAmfInstanceId() + "'" +
        (amf3GppAccessRegistration.supportedFeaturesIsSet()
             ? ",supportedFeatures='" +
                   amf3GppAccessRegistration.getSupportedFeatures() + "'"
             : "") +
        (amf3GppAccessRegistration.purgeFlagIsSet()
             ? (amf3GppAccessRegistration.isPurgeFlag() ? ",purgeFlag=1"
                                                        : ",purgeFlag=0")
             : "") +
        (amf3GppAccessRegistration.peiIsSet()
             ? ",pei='" + amf3GppAccessRegistration.getPei() + "'"
             : "") +
        ",deregCallbackUri='" +
        amf3GppAccessRegistration.getDeregCallbackUri() + "'" +
        (amf3GppAccessRegistration.pcscfRestorationCallbackUriIsSet()
             ? ",pcscfRestorationCallbackUri='" +
                   amf3GppAccessRegistration.getPcscfRestorationCallbackUri() +
                   "'"
             : "") +
        (amf3GppAccessRegistration.initialRegistrationIndIsSet()
             ? (amf3GppAccessRegistration.isInitialRegistrationInd()
                    ? ",initialRegistrationInd=1"
                    : ",initialRegistrationInd=0")
             : "") +
        (amf3GppAccessRegistration.drFlagIsSet()
             ? (amf3GppAccessRegistration.isDrFlag() ? ",drFlag=1"
                                                     : ",drFlag=0")
             : "") +
        (amf3GppAccessRegistration.urrpIndicatorIsSet()
             ? (amf3GppAccessRegistration.isUrrpIndicator()
                    ? ",urrpIndicator=1"
                    : ",urrpIndicator=0")
             : "") +
        (amf3GppAccessRegistration.amfEeSubscriptionIdIsSet()
             ? ",amfEeSubscriptionId='" +
                   amf3GppAccessRegistration.getAmfEeSubscriptionId() + "'"
             : "") +
        (amf3GppAccessRegistration.ueSrvccCapabilityIsSet()
             ? (amf3GppAccessRegistration.isUeSrvccCapability()
                    ? ",ueSrvccCapability=1"
                    : ",ueSrvccCapability=0")
             : "") +
        (amf3GppAccessRegistration.registrationTimeIsSet()
             ? ",registrationTime='" +
                   amf3GppAccessRegistration.getRegistrationTime() + "'"
             : "") +
        (amf3GppAccessRegistration.noEeSubscriptionIndIsSet()
             ? (amf3GppAccessRegistration.isNoEeSubscriptionInd()
                    ? ",noEeSubscriptionInd=1"
                    : ",noEeSubscriptionInd=0")
             : "");

    if (amf3GppAccessRegistration.imsVoPsIsSet()) {
      to_json(j, amf3GppAccessRegistration.getImsVoPs());
      query += ",imsVoPs='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.amfServiceNameDeregIsSet()) {
      to_json(j, amf3GppAccessRegistration.getAmfServiceNameDereg());
      query += ",amfServiceNameDereg='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.amfServiceNamePcscfRestIsSet()) {
      to_json(j, amf3GppAccessRegistration.getAmfServiceNamePcscfRest());
      query += ",amfServiceNamePcscfRest='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.backupAmfInfoIsSet()) {
      nlohmann::json tmp;
      j.clear();
      std::vector<BackupAmfInfo> backupamfinfo =
          amf3GppAccessRegistration.getBackupAmfInfo();
      for (int i = 0; i < backupamfinfo.size(); i++) {
        to_json(tmp, backupamfinfo[i]);
        j += tmp;
      }
      query += ",backupAmfInfo='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.epsInterworkingInfoIsSet()) {
      to_json(j, amf3GppAccessRegistration.getEpsInterworkingInfo());
      query += ",epsInterworkingInfo='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.vgmlcAddressIsSet()) {
      to_json(j, amf3GppAccessRegistration.getVgmlcAddress());
      query += ",vgmlcAddress='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.contextInfoIsSet()) {
      to_json(j, amf3GppAccessRegistration.getContextInfo());
      query += ",contextInfo='" + j.dump() + "'";
    }

    to_json(j, amf3GppAccessRegistration.getGuami());
    query += ",guami='" + j.dump() + "'";
    to_json(j, amf3GppAccessRegistration.getRatType());
    query += ",ratType='" + j.dump() + "'";
    query += " where ueid='" + ue_id + "'";
  } else {
    query =
        "insert into Amf3GppAccessRegistration set ueid='" + ue_id + "'" +
        ",amfInstanceId='" + amf3GppAccessRegistration.getAmfInstanceId() +
        "'" +
        (amf3GppAccessRegistration.supportedFeaturesIsSet()
             ? ",supportedFeatures='" +
                   amf3GppAccessRegistration.getSupportedFeatures() + "'"
             : "") +
        (amf3GppAccessRegistration.purgeFlagIsSet()
             ? (amf3GppAccessRegistration.isPurgeFlag() ? ",purgeFlag=1"
                                                        : ",purgeFlag=0")
             : "") +
        (amf3GppAccessRegistration.peiIsSet()
             ? ",pei='" + amf3GppAccessRegistration.getPei() + "'"
             : "") +
        ",deregCallbackUri='" +
        amf3GppAccessRegistration.getDeregCallbackUri() + "'" +
        (amf3GppAccessRegistration.pcscfRestorationCallbackUriIsSet()
             ? ",pcscfRestorationCallbackUri='" +
                   amf3GppAccessRegistration.getPcscfRestorationCallbackUri() +
                   "'"
             : "") +
        (amf3GppAccessRegistration.initialRegistrationIndIsSet()
             ? (amf3GppAccessRegistration.isInitialRegistrationInd()
                    ? ",initialRegistrationInd=1"
                    : ",initialRegistrationInd=0")
             : "") +
        (amf3GppAccessRegistration.drFlagIsSet()
             ? (amf3GppAccessRegistration.isDrFlag() ? ",drFlag=1"
                                                     : ",drFlag=0")
             : "") +
        (amf3GppAccessRegistration.urrpIndicatorIsSet()
             ? (amf3GppAccessRegistration.isUrrpIndicator()
                    ? ",urrpIndicator=1"
                    : ",urrpIndicator=0")
             : "") +
        (amf3GppAccessRegistration.amfEeSubscriptionIdIsSet()
             ? ",amfEeSubscriptionId='" +
                   amf3GppAccessRegistration.getAmfEeSubscriptionId() + "'"
             : "") +
        (amf3GppAccessRegistration.ueSrvccCapabilityIsSet()
             ? (amf3GppAccessRegistration.isUeSrvccCapability()
                    ? ",ueSrvccCapability=1"
                    : ",ueSrvccCapability=0")
             : "") +
        (amf3GppAccessRegistration.registrationTimeIsSet()
             ? ",registrationTime='" +
                   amf3GppAccessRegistration.getRegistrationTime() + "'"
             : "") +
        (amf3GppAccessRegistration.noEeSubscriptionIndIsSet()
             ? (amf3GppAccessRegistration.isNoEeSubscriptionInd()
                    ? ",noEeSubscriptionInd=1"
                    : ",noEeSubscriptionInd=0")
             : "");

    if (amf3GppAccessRegistration.imsVoPsIsSet()) {
      to_json(j, amf3GppAccessRegistration.getImsVoPs());
      query += ",imsVoPs='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.amfServiceNameDeregIsSet()) {
      to_json(j, amf3GppAccessRegistration.getAmfServiceNameDereg());
      query += ",amfServiceNameDereg='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.amfServiceNamePcscfRestIsSet()) {
      to_json(j, amf3GppAccessRegistration.getAmfServiceNamePcscfRest());
      query += ",amfServiceNamePcscfRest='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.backupAmfInfoIsSet()) {
      nlohmann::json tmp;
      j.clear();
      std::vector<BackupAmfInfo> backupamfinfo =
          amf3GppAccessRegistration.getBackupAmfInfo();
      for (int i = 0; i < backupamfinfo.size(); i++) {
        to_json(tmp, backupamfinfo[i]);
        j += tmp;
      }
      query += ",backupAmfInfo='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.epsInterworkingInfoIsSet()) {
      to_json(j, amf3GppAccessRegistration.getEpsInterworkingInfo());
      query += ",epsInterworkingInfo='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.vgmlcAddressIsSet()) {
      to_json(j, amf3GppAccessRegistration.getVgmlcAddress());
      query += ",vgmlcAddress='" + j.dump() + "'";
    }
    if (amf3GppAccessRegistration.contextInfoIsSet()) {
      to_json(j, amf3GppAccessRegistration.getContextInfo());
      query += ",contextInfo='" + j.dump() + "'";
    }

    to_json(j, amf3GppAccessRegistration.getGuami());
    query += ",guami='" + j.dump() + "'";
    to_json(j, amf3GppAccessRegistration.getRatType());
    query += ",ratType='" + j.dump() + "'";
  }

  mysql_free_result(res);
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  to_json(j, amf3GppAccessRegistration);
  response_data = j;
  // code = Pistache::Http::Code::Created;
  code = HTTP_STATUS_CODE_201_CREATED;

  Logger::udr_server().debug("Amf3GppAccessRegistration PUT - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_query_amf_context_3gpp(const std::string &ue_id,
                                            nlohmann::json &response_data,
                                            long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;

  nlohmann::json j = {};

  Amf3GppAccessRegistration amf3gppaccessregistration;
  const std::string query =
      "select * from Amf3GppAccessRegistration WHERE ueid='" + ue_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);

  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("amfInstanceId", field->name)) {
        amf3gppaccessregistration.setAmfInstanceId(row[i]);
      } else if (!strcmp("supportedFeatures", field->name) && row[i] != NULL) {
        amf3gppaccessregistration.setSupportedFeatures(row[i]);
      } else if (!strcmp("purgeFlag", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setPurgeFlag(true);
        else
          amf3gppaccessregistration.setPurgeFlag(false);
      } else if (!strcmp("pei", field->name) && row[i] != NULL) {
        amf3gppaccessregistration.setPei(row[i]);
      } else if (!strcmp("imsVoPs", field->name) && row[i] != NULL) {
        ImsVoPs imsvops;
        nlohmann::json::parse(row[i]).get_to(imsvops);
        amf3gppaccessregistration.setImsVoPs(imsvops);
      } else if (!strcmp("deregCallbackUri", field->name)) {
        amf3gppaccessregistration.setDeregCallbackUri(row[i]);
      } else if (!strcmp("amfServiceNameDereg", field->name) &&
                 row[i] != NULL) {
        ServiceName amfservicenamedereg;
        nlohmann::json::parse(row[i]).get_to(amfservicenamedereg);
        amf3gppaccessregistration.setAmfServiceNameDereg(amfservicenamedereg);
      } else if (!strcmp("pcscfRestorationCallbackUri", field->name) &&
                 row[i] != NULL) {
        amf3gppaccessregistration.setPcscfRestorationCallbackUri(row[i]);
      } else if (!strcmp("amfServiceNamePcscfRest", field->name) &&
                 row[i] != NULL) {
        ServiceName amfservicenamepcscfrest;
        nlohmann::json::parse(row[i]).get_to(amfservicenamepcscfrest);
        amf3gppaccessregistration.setAmfServiceNamePcscfRest(
            amfservicenamepcscfrest);
      } else if (!strcmp("initialRegistrationInd", field->name) &&
                 row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setInitialRegistrationInd(true);
        else
          amf3gppaccessregistration.setInitialRegistrationInd(false);
      } else if (!strcmp("guami", field->name)) {
        Guami guami;
        nlohmann::json::parse(row[i]).get_to(guami);
        amf3gppaccessregistration.setGuami(guami);
      } else if (!strcmp("backupAmfInfo", field->name) && row[i] != NULL) {
        std ::vector<BackupAmfInfo> backupamfinfo;
        nlohmann::json::parse(row[i]).get_to(backupamfinfo);
        amf3gppaccessregistration.setBackupAmfInfo(backupamfinfo);
      } else if (!strcmp("drFlag", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setDrFlag(true);
        else
          amf3gppaccessregistration.setDrFlag(false);
      } else if (!strcmp("ratType", field->name)) {
        RatType rattype;
        nlohmann::json::parse(row[i]).get_to(rattype);
        amf3gppaccessregistration.setRatType(rattype);
      } else if (!strcmp("urrpIndicator", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setUrrpIndicator(true);
        else
          amf3gppaccessregistration.setUrrpIndicator(false);
      } else if (!strcmp("amfEeSubscriptionId", field->name) &&
                 row[i] != NULL) {
        amf3gppaccessregistration.setAmfEeSubscriptionId(row[i]);
      } else if (!strcmp("epsInterworkingInfo", field->name) &&
                 row[i] != NULL) {
        EpsInterworkingInfo epsinterworkinginfo;
        nlohmann::json::parse(row[i]).get_to(epsinterworkinginfo);
        amf3gppaccessregistration.setEpsInterworkingInfo(epsinterworkinginfo);
      } else if (!strcmp("ueSrvccCapability", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setUeSrvccCapability(true);
        else
          amf3gppaccessregistration.setUeSrvccCapability(false);
      } else if (!strcmp("registrationTime", field->name) && row[i] != NULL) {
        amf3gppaccessregistration.setRegistrationTime(row[i]);
      } else if (!strcmp("vgmlcAddress", field->name) && row[i] != NULL) {
        VgmlcAddress vgmlcaddress;
        nlohmann::json::parse(row[i]).get_to(vgmlcaddress);
        amf3gppaccessregistration.setVgmlcAddress(vgmlcaddress);
      } else if (!strcmp("contextInfo", field->name) && row[i] != NULL) {
        ContextInfo contextinfo;
        nlohmann::json::parse(row[i]).get_to(contextinfo);
        amf3gppaccessregistration.setContextInfo(contextinfo);
      } else if (!strcmp("noEeSubscriptionInd", field->name) &&
                 row[i] != NULL) {
        if (strcmp(row[i], "0"))
          amf3gppaccessregistration.setNoEeSubscriptionInd(true);
        else
          amf3gppaccessregistration.setNoEeSubscriptionInd(false);
      }
    }
    to_json(j, amf3gppaccessregistration);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug("Amf3GppAccessRegistration GET - json:\n\"%s\"",
                               j.dump().c_str());
  } else {
    Logger::udr_server().error("Amf3GppAccessRegistration no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_create_authentication_status(const std::string &ue_id,
                                                  const AuthEvent &authEvent,
                                                  nlohmann::json &response_data,
                                                  long &code) {
  Logger::udr_server().info("Handle Create Authentication Status");

  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};

  const std::string select_AuthenticationStatus =
      "select * from AuthenticationStatus WHERE ueid='" + ue_id + "'";
  std::string query = {};
  nlohmann::json j = {};

  Logger::udr_server().info("MySQL query: %s",
                            select_AuthenticationStatus.c_str());

  if (mysql_real_query(&mysql, select_AuthenticationStatus.c_str(),
                       (unsigned long)select_AuthenticationStatus.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               select_AuthenticationStatus.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               select_AuthenticationStatus.c_str());
    return;
  }
  if (mysql_num_rows(res)) {
    query = "update AuthenticationStatus set nfInstanceId='" +
            authEvent.getNfInstanceId() + "'" +
            ",success=" + (authEvent.isSuccess() ? "1" : "0") + ",timeStamp='" +
            authEvent.getTimeStamp() + "'" + ",authType='" +
            authEvent.getAuthType() + "'" + ",servingNetworkName='" +
            authEvent.getServingNetworkName() + "'" +
            (authEvent.authRemovalIndIsSet()
                 ? (authEvent.isAuthRemovalInd() ? ",authRemovalInd=1"
                                                 : ",authRemovalInd=0")
                 : "");
    //        to_json(j,authEvent.getAuthType());
    //        query += ",authType='"+j.dump()+"'";
    query += " where ueid='" + ue_id + "'";
  } else {
    query = "insert into AuthenticationStatus set ueid='" + ue_id + "'" +
            ",nfInstanceId='" + authEvent.getNfInstanceId() + "'" +
            ",success=" + (authEvent.isSuccess() ? "1" : "0") + ",timeStamp='" +
            authEvent.getTimeStamp() + "'" + ",authType='" +
            authEvent.getAuthType() + "'" + ",servingNetworkName='" +
            authEvent.getServingNetworkName() + "'" +
            (authEvent.authRemovalIndIsSet()
                 ? (authEvent.isAuthRemovalInd() ? ",authRemovalInd=1"
                                                 : ",authRemovalInd=0")
                 : "");
    //        to_json(j,authEvent.getAuthType());
    //        query += ",authType='"+j.dump()+"'";
  }

  Logger::udr_server().info("MySQL query: %s", query.c_str());

  mysql_free_result(res);
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql create failure！SQL(%s)", query.c_str());
    return;
  }

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;

  to_json(j, authEvent);
  Logger::udr_server().info("AuthenticationStatus PUT - json:\n\"%s\"",
                            j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_delete_authentication_status(const std::string &ue_id,
                                                  nlohmann::json &response_data,
                                                  long &code) {
  const std::string query =
      "DELETE from AuthenticationStatus WHERE ueid='" + ue_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;
  Logger::udr_server().debug("AuthenticationStatus DELETE - successful");
}

//------------------------------------------------------------------------------
void udr_app::handle_query_authentication_status(const std::string &ue_id,
                                                 nlohmann::json &response_data,
                                                 long &code) {
  Logger::udr_server().info("Handle Query Authentication Status");
  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  AuthEvent authenticationstatus = {};
  const std::string query =
      "select * from AuthenticationStatus WHERE ueid='" + ue_id + "'";

  Logger::udr_server().info("MySQL query: %s", query.c_str());
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！");
    return;
  }

  row = mysql_fetch_row(res);
  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("nfInstanceId", field->name)) {
        authenticationstatus.setNfInstanceId(row[i]);
      } else if (!strcmp("success", field->name)) {
        if (strcmp(row[i], "0"))
          authenticationstatus.setSuccess(true);
        else
          authenticationstatus.setSuccess(false);
      } else if (!strcmp("timeStamp", field->name)) {
        authenticationstatus.setTimeStamp(row[i]);
      } else if (!strcmp("authType", field->name)) {
        //                AuthType authtype;
        //                nlohmann::json::parse(row[i]).get_to(authtype);
        authenticationstatus.setAuthType(row[i]);
      } else if (!strcmp("servingNetworkName", field->name)) {
        authenticationstatus.setServingNetworkName(row[i]);
      } else if (!strcmp("authRemovalInd", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          authenticationstatus.setAuthRemovalInd(true);
        else
          authenticationstatus.setAuthRemovalInd(false);
      }
    }

    to_json(j, authenticationstatus);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().info("AuthenticationStatus GET - json:\n\"%s\"",
                              j.dump().c_str());
  } else {
    Logger::udr_server().error("AuthenticationStatus no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_modify_authentication_subscription(
    const std::string &ue_id, const std::vector<PatchItem> &patchItem,
    nlohmann::json &response_data, long &code) {
  Logger::udr_server().info("Handle Update Authentication Subscription");
  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};

  const std::string select_Authenticationsubscription =
      "select * from AuthenticationSubscription WHERE ueid='" + ue_id + "'";

  Logger::udr_server().info("MySQL Query (%s)",
                            select_Authenticationsubscription.c_str());

  std::string query = {};
  nlohmann::json j = {};
  nlohmann::json tmp_j = {};

  for (int i = 0; i < patchItem.size(); i++) {
    if ((!strcmp(patchItem[i].getOp().c_str(), PATCH_OPERATION_REPLACE)) &&
        patchItem[i].valueIsSet()) {
      patchItem[i].getValue();
      SequenceNumber sequencenumber;
      nlohmann::json::parse(patchItem[i].getValue().c_str())
          .get_to(sequencenumber);

      if (mysql_real_query(
              &mysql, select_Authenticationsubscription.c_str(),
              (unsigned long)select_Authenticationsubscription.size())) {
        Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                                   select_Authenticationsubscription.c_str());
        return;
      }

      res = mysql_store_result(&mysql);
      if (res == NULL) {
        Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                                   select_Authenticationsubscription.c_str());
        return;
      }
      if (mysql_num_rows(res)) {
        nlohmann::json sequencenumber_j;
        query = "update AuthenticationSubscription set sequenceNumber='";

        to_json(sequencenumber_j, sequencenumber);
        query += sequencenumber_j.dump() + "'";
        query += " where ueid='" + ue_id + "'";
      } else {
        Logger::udr_server().error(
            "AuthenticationSubscription no data！SQL(%s)",
            select_Authenticationsubscription.c_str());
      }

      Logger::udr_server().info("MySQL Update cmd (%s)", query.c_str());
      mysql_free_result(res);

      if (mysql_real_query(&mysql, query.c_str(),
                           (unsigned long)query.size()) != 0) {
        Logger::udr_server().error("update mysql failure！SQL(%s)",
                                   query.c_str());
        return;
      }
    }

    to_json(tmp_j, patchItem[i]);
    j += tmp_j;
  }

  Logger::udr_server().info("AuthenticationSubscription PATCH - json:\n\"%s\"",
                            j.dump().c_str());

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;
}

//------------------------------------------------------------------------------
void udr_app::handle_read_authentication_subscription(
    const std::string &ue_id, nlohmann::json &response_data, long &code) {
  Logger::udr_server().info("Handle Read Authentication Subscription");
  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};

  AuthenticationSubscription authenticationsubscription = {};
  const std::string query =
      "select * from AuthenticationSubscription WHERE ueid='" + ue_id + "'";
  Logger::udr_server().info("MySQL Query (%s)", query.c_str());

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);

  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("authenticationMethod", field->name)) {
        //                AuthMethod authenticationmethod;
        //                nlohmann::json::parse(row[i]).get_to(authenticationmethod);
        authenticationsubscription.setAuthenticationMethod(row[i]);
      } else if (!strcmp("encPermanentKey", field->name) && row[i] != NULL) {
        authenticationsubscription.setEncPermanentKey(row[i]);
      } else if (!strcmp("protectionParameterId", field->name) &&
                 row[i] != NULL) {
        authenticationsubscription.setProtectionParameterId(row[i]);
      } else if (!strcmp("sequenceNumber", field->name) && row[i] != NULL) {
        SequenceNumber sequencenumber;
        nlohmann::json::parse(row[i]).get_to(sequencenumber);
        authenticationsubscription.setSequenceNumber(sequencenumber);
      } else if (!strcmp("authenticationManagementField", field->name) &&
                 row[i] != NULL) {
        authenticationsubscription.setAuthenticationManagementField(row[i]);
      } else if (!strcmp("algorithmId", field->name) && row[i] != NULL) {
        authenticationsubscription.setAlgorithmId(row[i]);
      } else if (!strcmp("encOpcKey", field->name) && row[i] != NULL) {
        authenticationsubscription.setEncOpcKey(row[i]);
      } else if (!strcmp("encTopcKey", field->name) && row[i] != NULL) {
        authenticationsubscription.setEncTopcKey(row[i]);
      } else if (!strcmp("vectorGenerationInHss", field->name) &&
                 row[i] != NULL) {
        std::cout << row[i] << std::endl;
        if (strcmp(row[i], "0"))
          authenticationsubscription.setVectorGenerationInHss(true);
        else
          authenticationsubscription.setVectorGenerationInHss(false);
      } else if (!strcmp("n5gcAuthMethod", field->name) && row[i] != NULL) {
        //                AuthMethod n5gcauthmethod;
        //                nlohmann::json::parse(row[i]).get_to(n5gcauthmethod);
        authenticationsubscription.setN5gcAuthMethod(row[i]);
      } else if (!strcmp("rgAuthenticationInd", field->name) &&
                 row[i] != NULL) {
        std::cout << row[i] << std::endl;
        if (strcmp(row[i], "0"))
          authenticationsubscription.setRgAuthenticationInd(true);
        else
          authenticationsubscription.setRgAuthenticationInd(false);
      } else if (!strcmp("supi", field->name) && row[i] != NULL) {
        authenticationsubscription.setSupi(row[i]);
      }
    }

    to_json(j, authenticationsubscription);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().info("AuthenticationSubscription GET - json:\n\"%s\"",
                              j.dump().c_str());

  } else {
    Logger::udr_server().error("AuthenticationSubscription no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_query_sdm_subscription(const std::string &ue_id,
                                            const std::string &subs_id,
                                            nlohmann::json &response_data,
                                            long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  SdmSubscription SdmSubscriptions = {};
  const std::string query = "SELECT * from SdmSubscriptions WHERE ueid='" +
                            ue_id + "' AND subsId=" + subs_id;

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);
  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("nfInstanceId", field->name)) {
        SdmSubscriptions.setNfInstanceId(row[i]);
      } else if (!strcmp("implicitUnsubscribe", field->name) &&
                 row[i] != NULL) {
        if (strcmp(row[i], "0"))
          SdmSubscriptions.setImplicitUnsubscribe(true);
        else
          SdmSubscriptions.setImplicitUnsubscribe(false);
      } else if (!strcmp("expires", field->name) && row[i] != NULL) {
        SdmSubscriptions.setExpires(row[i]);
      } else if (!strcmp("callbackReference", field->name)) {
        SdmSubscriptions.setCallbackReference(row[i]);
      } else if (!strcmp("amfServiceName", field->name) && row[i] != NULL) {
        ServiceName amfservicename;
        nlohmann::json::parse(row[i]).get_to(amfservicename);
        SdmSubscriptions.setAmfServiceName(amfservicename);
      } else if (!strcmp("monitoredResourceUris", field->name)) {
        std::vector<std::string> monitoredresourceuris;
        nlohmann::json::parse(row[i]).get_to(monitoredresourceuris);
        SdmSubscriptions.setMonitoredResourceUris(monitoredresourceuris);
      } else if (!strcmp("singleNssai", field->name) && row[i] != NULL) {
        Snssai singlenssai;
        nlohmann::json::parse(row[i]).get_to(singlenssai);
        SdmSubscriptions.setSingleNssai(singlenssai);
      } else if (!strcmp("dnn", field->name) && row[i] != NULL) {
        SdmSubscriptions.setDnn(row[i]);
      } else if (!strcmp("subscriptionId", field->name) && row[i] != NULL) {
        SdmSubscriptions.setSubscriptionId(row[i]);
      } else if (!strcmp("plmnId", field->name) && row[i] != NULL) {
        PlmnId plmnid;
        nlohmann::json::parse(row[i]).get_to(plmnid);
        SdmSubscriptions.setPlmnId(plmnid);
      } else if (!strcmp("immediateReport", field->name) && row[i] != NULL) {
        if (strcmp(row[i], "0"))
          SdmSubscriptions.setImmediateReport(true);
        else
          SdmSubscriptions.setImmediateReport(false);
      } else if (!strcmp("report", field->name) && row[i] != NULL) {
        SubscriptionDataSets report;
        nlohmann::json::parse(row[i]).get_to(report);
        SdmSubscriptions.setReport(report);
      } else if (!strcmp("supportedFeatures", field->name) && row[i] != NULL) {
        SdmSubscriptions.setSupportedFeatures(row[i]);
      } else if (!strcmp("contextInfo", field->name) && row[i] != NULL) {
        ContextInfo contextinfo;
        nlohmann::json::parse(row[i]).get_to(contextinfo);
        SdmSubscriptions.setContextInfo(contextinfo);
      }
    }
    to_json(j, SdmSubscriptions);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug("SdmSubscription GET - json:\n\"%s\"",
                               j.dump().c_str());
  } else {
    Logger::udr_server().error("SdmSubscription no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_remove_sdm_subscription(const std::string &ue_id,
                                             const std::string &subs_id,
                                             nlohmann::json &response_data,
                                             long &code) {
  MYSQL_RES *res = NULL;
  nlohmann::json j = {};
  ProblemDetails problemdetails = {};

  const std::string select_query =
      "SELECT * from SdmSubscriptions WHERE ueid='" + ue_id +
      "' AND subsId=" + subs_id;

  const std::string query = "DELETE from SdmSubscriptions WHERE ueid='" +
                            ue_id + "' AND subsId=" + subs_id;

  if (mysql_real_query(&mysql, select_query.c_str(),
                       (unsigned long)select_query.size())) {
    problemdetails.setCause("USER_NOT_FOUND");
    to_json(j, problemdetails);
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    response_data = j;
    // code = Pistache::Http::Code::Not_Found;
    code = HTTP_STATUS_CODE_404_NOT_FOUND;
    return;
  }
  res = mysql_store_result(&mysql);
  if (res == NULL) {
    problemdetails.setCause("USER_NOT_FOUND");
    to_json(j, problemdetails);
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    response_data = j;
    // code = Pistache::Http::Code::Not_Found;
    code = HTTP_STATUS_CODE_404_NOT_FOUND;
    return;
  }
  if (!mysql_num_rows(res)) {
    problemdetails.setCause("DATA_NOT_FOUND");
    to_json(j, problemdetails);
    response_data = j;
    // code = Pistache::Http::Code::Not_Found;
    code = HTTP_STATUS_CODE_404_NOT_FOUND;
    return;
  }
  mysql_free_result(res);
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    problemdetails.setCause("USER_NOT_FOUND");
    to_json(j, problemdetails);
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    response_data = j;
    // code = Pistache::Http::Code::Not_Found;
    code = HTTP_STATUS_CODE_404_NOT_FOUND;
    return;
  }

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;

  Logger::udr_server().debug("SdmSubscription DELETE - successful");
}

//------------------------------------------------------------------------------
void udr_app::handle_update_sdm_subscription(const std::string &ue_id,
                                             const std::string &subs_id,
                                             SdmSubscription &sdmSubscription,
                                             nlohmann::json &response_data,
                                             long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};

  const std::string select_query =
      "SELECT * from SdmSubscriptions WHERE ueid='" + ue_id +
      "' AND subsId=" + subs_id;
  std::string query = {};
  nlohmann::json j = {};
  ProblemDetails problemdetails = {};

  if (mysql_real_query(&mysql, select_query.c_str(),
                       (unsigned long)select_query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }
  if (mysql_num_rows(res)) {
    nlohmann::json MonitoredResourceUris_json(
        sdmSubscription.getMonitoredResourceUris());

    query =
        "update SdmSubscriptions set nfInstanceId='" +
        sdmSubscription.getNfInstanceId() + "'" +
        (sdmSubscription.implicitUnsubscribeIsSet()
             ? (sdmSubscription.isImplicitUnsubscribe()
                    ? ",implicitUnsubscribe=1"
                    : ",implicitUnsubscribe=0")
             : "") +
        (sdmSubscription.expiresIsSet()
             ? ",expires='" + sdmSubscription.getExpires() + "'"
             : "") +
        ",callbackReference='" + sdmSubscription.getCallbackReference() + "'" +
        (sdmSubscription.dnnIsSet() ? ",dnn='" + sdmSubscription.getDnn() + "'"
                                    : "") +
        (sdmSubscription.subscriptionIdIsSet()
             ? ",subscriptionId='" + sdmSubscription.getSubscriptionId() + "'"
             : "") +
        (sdmSubscription.immediateReportIsSet()
             ? (sdmSubscription.isImmediateReport() ? ",immediateReport=1"
                                                    : ",immediateReport=0")
             : "") +
        (sdmSubscription.supportedFeaturesIsSet()
             ? ",supportedFeatures='" + sdmSubscription.getSupportedFeatures() +
                   "'"
             : "");

    if (sdmSubscription.amfServiceNameIsSet()) {
      to_json(j, sdmSubscription.getAmfServiceName());
      query += ",amfServiceName='" + j.dump() + "'";
    }
    if (sdmSubscription.singleNssaiIsSet()) {
      to_json(j, sdmSubscription.getSingleNssai());
      query += ",singleNssai='" + j.dump() + "'";
    }
    if (sdmSubscription.plmnIdIsSet()) {
      to_json(j, sdmSubscription.getPlmnId());
      query += ",plmnId='" + j.dump() + "'";
    }
    if (sdmSubscription.reportIsSet()) {
      to_json(j, sdmSubscription.getReport());
      query += ",report='" + j.dump() + "'";
    }
    if (sdmSubscription.contextInfoIsSet()) {
      to_json(j, sdmSubscription.getContextInfo());
      query += ",contextInfo='" + j.dump() + "'";
    }

    query +=
        ",monitoredResourceUris='" + MonitoredResourceUris_json.dump() + "'";

    query += " where ueid='" + ue_id + "' AND subsId=" + subs_id;
  } else {
    to_json(j, problemdetails);
    response_data = j;
    // code = Pistache::Http::Code::Not_Found;
    code = HTTP_STATUS_CODE_404_NOT_FOUND;

    mysql_free_result(res);
    return;
  }

  mysql_free_result(res);
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;

  to_json(j, sdmSubscription);
  Logger::udr_server().debug("SdmSubscription PUT - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_create_sdm_subscriptions(const std::string &ue_id,
                                              SdmSubscription &sdmSubscription,
                                              nlohmann::json &response_data,
                                              long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  nlohmann::json j = {};
  int32_t subs_id = 0;
  int32_t count = 0;
  std::string query =
      "SELECT subsId from SdmSubscriptions WHERE ueid='" + ue_id + "'";
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }
  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  while (row = mysql_fetch_row(res)) {
    count++;
    if (strcmp(row[0], std::to_string(count).c_str())) {
      subs_id = count;
      break;
    }
  }
  mysql_free_result(res);

  query =
      "insert into SdmSubscriptions set ueid='" + ue_id + "'" +
      ",nfInstanceId='" + sdmSubscription.getNfInstanceId() + "'" +
      (sdmSubscription.implicitUnsubscribeIsSet()
           ? (sdmSubscription.isImplicitUnsubscribe()
                  ? ",implicitUnsubscribe=1"
                  : ",implicitUnsubscribe=0")
           : "") +
      (sdmSubscription.expiresIsSet()
           ? ",expires='" + sdmSubscription.getExpires() + "'"
           : "") +
      ",callbackReference='" + sdmSubscription.getCallbackReference() + "'" +
      (sdmSubscription.dnnIsSet() ? ",dnn='" + sdmSubscription.getDnn() + "'"
                                  : "") +
      (sdmSubscription.subscriptionIdIsSet()
           ? ",subscriptionId='" + sdmSubscription.getSubscriptionId() + "'"
           : "") +
      (sdmSubscription.immediateReportIsSet()
           ? (sdmSubscription.isImmediateReport() ? ",immediateReport=1"
                                                  : ",immediateReport=0")
           : "") +
      (sdmSubscription.supportedFeaturesIsSet()
           ? ",supportedFeatures='" + sdmSubscription.getSupportedFeatures() +
                 "'"
           : "");

  if (sdmSubscription.amfServiceNameIsSet()) {
    to_json(j, sdmSubscription.getAmfServiceName());
    query += ",amfServiceName='" + j.dump() + "'";
  }
  if (sdmSubscription.singleNssaiIsSet()) {
    to_json(j, sdmSubscription.getSingleNssai());
    query += ",singleNssai='" + j.dump() + "'";
  }
  if (sdmSubscription.plmnIdIsSet()) {
    to_json(j, sdmSubscription.getPlmnId());
    query += ",plmnId='" + j.dump() + "'";
  }
  if (sdmSubscription.reportIsSet()) {
    to_json(j, sdmSubscription.getReport());
    query += ",report='" + j.dump() + "'";
  }
  if (sdmSubscription.contextInfoIsSet()) {
    to_json(j, sdmSubscription.getContextInfo());
    query += ",contextInfo='" + j.dump() + "'";
  }

  nlohmann::json MonitoredResourceUris_json(
      sdmSubscription.getMonitoredResourceUris());
  query += ",monitoredResourceUris='" + MonitoredResourceUris_json.dump() + "'";

  if (subs_id && (subs_id == count)) {
    query += ",subsId=" + std::to_string(subs_id);
  }

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  to_json(j, sdmSubscription);
  response_data = j;
  // code = Pistache::Http::Code::Created;
  code = HTTP_STATUS_CODE_201_CREATED;

  Logger::udr_server().debug("SdmSubscriptions POST - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_query_sdm_subscriptions(const std::string &ue_id,
                                             nlohmann::json &response_data,
                                             long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  std::vector<std::string> fields;

  nlohmann::json j = {};
  nlohmann::json tmp = {};

  const std::string query =
      "SELECT * from SdmSubscriptions WHERE ueid='" + ue_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  while (field = mysql_fetch_field(res)) {
    fields.push_back(field->name);
  }

  j.clear();

  while (row = mysql_fetch_row(res)) {
    SdmSubscription sdmsubscriptions = {};

    tmp.clear();

    for (int i = 0; i < fields.size(); i++) {
      if (!strcmp("nfInstanceId", fields[i].c_str())) {
        sdmsubscriptions.setNfInstanceId(row[i]);
      } else if (!strcmp("implicitUnsubscribe", fields[i].c_str()) &&
                 row[i] != NULL) {
        if (strcmp(row[i], "0"))
          sdmsubscriptions.setImplicitUnsubscribe(true);
        else
          sdmsubscriptions.setImplicitUnsubscribe(false);
      } else if (!strcmp("expires", fields[i].c_str()) && row[i] != NULL) {
        sdmsubscriptions.setExpires(row[i]);
      } else if (!strcmp("callbackReference", fields[i].c_str())) {
        sdmsubscriptions.setCallbackReference(row[i]);
      } else if (!strcmp("amfServiceName", fields[i].c_str()) &&
                 row[i] != NULL) {
        ServiceName amfservicename;
        nlohmann::json::parse(row[i]).get_to(amfservicename);
        sdmsubscriptions.setAmfServiceName(amfservicename);
      } else if (!strcmp("monitoredResourceUris", fields[i].c_str())) {
        std::vector<std::string> monitoredresourceuris;
        nlohmann::json::parse(row[i]).get_to(monitoredresourceuris);
        sdmsubscriptions.setMonitoredResourceUris(monitoredresourceuris);
      } else if (!strcmp("singleNssai", fields[i].c_str()) && row[i] != NULL) {
        Snssai singlenssai;
        nlohmann::json::parse(row[i]).get_to(singlenssai);
        sdmsubscriptions.setSingleNssai(singlenssai);
      } else if (!strcmp("dnn", fields[i].c_str()) && row[i] != NULL) {
        sdmsubscriptions.setDnn(row[i]);
      } else if (!strcmp("subscriptionId", fields[i].c_str()) &&
                 row[i] != NULL) {
        sdmsubscriptions.setSubscriptionId(row[i]);
      } else if (!strcmp("plmnId", fields[i].c_str()) && row[i] != NULL) {
        PlmnId plmnid;
        nlohmann::json::parse(row[i]).get_to(plmnid);
        sdmsubscriptions.setPlmnId(plmnid);
      } else if (!strcmp("immediateReport", fields[i].c_str()) &&
                 row[i] != NULL) {
        if (strcmp(row[i], "0"))
          sdmsubscriptions.setImmediateReport(true);
        else
          sdmsubscriptions.setImmediateReport(false);
      } else if (!strcmp("report", fields[i].c_str()) && row[i] != NULL) {
        SubscriptionDataSets report;
        nlohmann::json::parse(row[i]).get_to(report);
        sdmsubscriptions.setReport(report);
      } else if (!strcmp("supportedFeatures", fields[i].c_str()) &&
                 row[i] != NULL) {
        sdmsubscriptions.setSupportedFeatures(row[i]);
      } else if (!strcmp("contextInfo", fields[i].c_str()) && row[i] != NULL) {
        ContextInfo contextinfo;
        nlohmann::json::parse(row[i]).get_to(contextinfo);
        sdmsubscriptions.setContextInfo(contextinfo);
      }
    }
    to_json(tmp, sdmsubscriptions);
    j += tmp;
  }

  mysql_free_result(res);

  response_data = j;
  // code = Pistache::Http::Code::Ok;
  code = HTTP_STATUS_CODE_200_OK;

  Logger::udr_server().debug("SdmSubscriptions GET - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_query_sm_data(const std::string &ue_id,
                                   const std::string &serving_plmn_id,
                                   nlohmann::json &response_data, long &code,
                                   oai::udr::model::Snssai snssai,
                                   std::string dnn) {
  MYSQL_RES *res = nullptr;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  SessionManagementSubscriptionData sessionmanagementsubscriptiondata = {};
  std::string query =
      "select * from SessionManagementSubscriptionData WHERE ueid='" + ue_id +
      "' and servingPlmnid='" + serving_plmn_id + "' ";
  std::string option_str = {};
  if (snssai.getSst() > 0) {
    option_str += " and JSON_EXTRACT(singleNssai, \"$.sst\")=" +
                  std::to_string(snssai.getSst());
  }
  if (!dnn.empty()) {
    option_str +=
        " and JSON_EXTRACT(dnnConfigurations, \"$." + dnn + "\") IS NOT NULL";
  }

  query += option_str;
  Logger::udr_server().debug("MySQL query: %s", query.c_str());

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure (SQL query %s)!",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure (SQL query %s)！",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);

  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("singleNssai", field->name)) {
        Snssai singlenssai;
        nlohmann::json::parse(row[i]).get_to(singlenssai);
        sessionmanagementsubscriptiondata.setSingleNssai(singlenssai);
      } else if (!strcmp("dnnConfigurations", field->name) && row[i] != NULL) {
        std ::map<std ::string, DnnConfiguration> dnnconfigurations;
        nlohmann::json::parse(row[i]).get_to(dnnconfigurations);
        sessionmanagementsubscriptiondata.setDnnConfigurations(
            dnnconfigurations);
        Logger::udr_server().debug("DNN configurations (row %d): %s", i,
                                   row[i]);
        for (auto d : dnnconfigurations) {
          nlohmann::json temp = {};
          to_json(temp, d.second);
          Logger::udr_server().debug("DNN configurations: %s",
                                     temp.dump().c_str());
        }
      } else if (!strcmp("internalGroupIds", field->name) && row[i] != NULL) {
        std ::vector<std ::string> internalgroupIds;
        nlohmann::json::parse(row[i]).get_to(internalgroupIds);
        sessionmanagementsubscriptiondata.setInternalGroupIds(internalgroupIds);
      } else if (!strcmp("sharedVnGroupDataIds", field->name) &&
                 row[i] != NULL) {
        std ::map<std ::string, std ::string> sharedvngroupdataids;
        nlohmann::json::parse(row[i]).get_to(sharedvngroupdataids);
        sessionmanagementsubscriptiondata.setSharedVnGroupDataIds(
            sharedvngroupdataids);
      } else if (!strcmp("sharedDnnConfigurationsId", field->name) &&
                 row[i] != NULL) {
        sessionmanagementsubscriptiondata.setSharedDnnConfigurationsId(row[i]);
      } else if (!strcmp("odbPacketServices", field->name) && row[i] != NULL) {
        OdbPacketServices odbpacketservices;
        nlohmann::json::parse(row[i]).get_to(odbpacketservices);
        sessionmanagementsubscriptiondata.setOdbPacketServices(
            odbpacketservices);
      } else if (!strcmp("traceData", field->name) && row[i] != NULL) {
        TraceData tracedata;
        nlohmann::json::parse(row[i]).get_to(tracedata);
        sessionmanagementsubscriptiondata.setTraceData(tracedata);
      } else if (!strcmp("sharedTraceDataId", field->name) && row[i] != NULL) {
        sessionmanagementsubscriptiondata.setSharedTraceDataId(row[i]);
      } else if (!strcmp("expectedUeBehavioursList", field->name) &&
                 row[i] != NULL) {
        std ::map<std ::string, ExpectedUeBehaviourData>
            expecteduebehaviourslist;
        nlohmann::json::parse(row[i]).get_to(expecteduebehaviourslist);
        sessionmanagementsubscriptiondata.setExpectedUeBehavioursList(
            expecteduebehaviourslist);
      } else if (!strcmp("suggestedPacketNumDlList", field->name) &&
                 row[i] != NULL) {
        std ::map<std ::string, SuggestedPacketNumDl> suggestedpacketnumdllist;
        nlohmann::json::parse(row[i]).get_to(suggestedpacketnumdllist);
        sessionmanagementsubscriptiondata.setSuggestedPacketNumDlList(
            suggestedpacketnumdllist);
      } else if (!strcmp("3gppChargingCharacteristics", field->name) &&
                 row[i] != NULL) {
        sessionmanagementsubscriptiondata.setR3gppChargingCharacteristics(
            row[i]);
      }
    }
    to_json(j, sessionmanagementsubscriptiondata);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug("SessionManagementSubscriptionData:\n %s",
                               j.dump().c_str());
  } else {
    Logger::udr_server().error(
        "SessionManagementSubscriptionData no data found (SQL query: %s)",
        query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_create_smf_context_non_3gpp(
    const std::string &ue_id, const int32_t &pdu_session_id,
    const SmfRegistration &smfRegistration, nlohmann::json &response_data,
    long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};

  const std::string select_SmfRegistration =
      "SELECT * from SmfRegistrations WHERE ueid='" + ue_id +
      "' AND subpduSessionId=" + std::to_string(pdu_session_id);
  std::string query = {};
  nlohmann::json j = {};

  if (mysql_real_query(&mysql, select_SmfRegistration.c_str(),
                       (unsigned long)select_SmfRegistration.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               select_SmfRegistration.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               select_SmfRegistration.c_str());
    return;
  }
  if (mysql_num_rows(res)) {
    query =
        "update SmfRegistrations set smfInstanceId='" +
        smfRegistration.getSmfInstanceId() + "'" +
        ",pduSessionId=" + std::to_string(smfRegistration.getPduSessionId()) +
        (smfRegistration.smfSetIdIsSet()
             ? ",smfSetId='" + smfRegistration.getSmfSetId() + "'"
             : "") +
        (smfRegistration.supportedFeaturesIsSet()
             ? ",supportedFeatures='" + smfRegistration.getSupportedFeatures() +
                   "'"
             : "") +
        (smfRegistration.dnnIsSet() ? ",dnn='" + smfRegistration.getDnn() + "'"
                                    : "") +
        (smfRegistration.emergencyServicesIsSet()
             ? (smfRegistration.isEmergencyServices() ? ",emergencyServices=1"
                                                      : ",emergencyServices=0")
             : "") +
        (smfRegistration.pcscfRestorationCallbackUriIsSet()
             ? ",pcscfRestorationCallbackUri='" +
                   smfRegistration.getPcscfRestorationCallbackUri() + "'"
             : "") +
        (smfRegistration.pgwFqdnIsSet()
             ? ",pgwFqdn='" + smfRegistration.getPgwFqdn() + "'"
             : "") +
        (smfRegistration.epdgIndIsSet()
             ? (smfRegistration.isEpdgInd() ? ",epdgInd=1" : ",epdgInd=0")
             : "") +
        (smfRegistration.deregCallbackUriIsSet()
             ? ",deregCallbackUri='" + smfRegistration.getDeregCallbackUri() +
                   "'"
             : "") +
        (smfRegistration.registrationTimeIsSet()
             ? ",registrationTime='" + smfRegistration.getRegistrationTime() +
                   "'"
             : "");

    if (smfRegistration.registrationReasonIsSet()) {
      to_json(j, smfRegistration.getRegistrationReason());
      query += ",registrationReason='" + j.dump() + "'";
    }
    if (smfRegistration.contextInfoIsSet()) {
      to_json(j, smfRegistration.getContextInfo());
      query += ",contextInfo='" + j.dump() + "'";
    }

    to_json(j, smfRegistration.getSingleNssai());
    query += ",singleNssai='" + j.dump() + "'";
    to_json(j, smfRegistration.getPlmnId());
    query += ",plmnId='" + j.dump() + "'";
    query += " where ueid='" + ue_id +
             "' AND subpduSessionId=" + std::to_string(pdu_session_id);
  } else {
    query =
        "insert into SmfRegistrations set ueid='" + ue_id + "'" +
        ",subpduSessionId=" + std::to_string(pdu_session_id) +
        ",pduSessionId=" + std::to_string(smfRegistration.getPduSessionId()) +
        ",smfInstanceId='" + smfRegistration.getSmfInstanceId() + "'" +
        (smfRegistration.smfSetIdIsSet()
             ? ",smfSetId='" + smfRegistration.getSmfSetId() + "'"
             : "") +
        (smfRegistration.supportedFeaturesIsSet()
             ? ",supportedFeatures='" + smfRegistration.getSupportedFeatures() +
                   "'"
             : "") +
        (smfRegistration.dnnIsSet() ? ",dnn='" + smfRegistration.getDnn() + "'"
                                    : "") +
        (smfRegistration.emergencyServicesIsSet()
             ? (smfRegistration.isEmergencyServices() ? ",emergencyServices=1"
                                                      : ",emergencyServices=0")
             : "") +
        (smfRegistration.pcscfRestorationCallbackUriIsSet()
             ? ",pcscfRestorationCallbackUri='" +
                   smfRegistration.getPcscfRestorationCallbackUri() + "'"
             : "") +
        (smfRegistration.pgwFqdnIsSet()
             ? ",pgwFqdn='" + smfRegistration.getPgwFqdn() + "'"
             : "") +
        (smfRegistration.epdgIndIsSet()
             ? (smfRegistration.isEpdgInd() ? ",epdgInd=1" : ",epdgInd=0")
             : "") +
        (smfRegistration.deregCallbackUriIsSet()
             ? ",deregCallbackUri='" + smfRegistration.getDeregCallbackUri() +
                   "'"
             : "") +
        (smfRegistration.registrationTimeIsSet()
             ? ",registrationTime='" + smfRegistration.getRegistrationTime() +
                   "'"
             : "");

    if (smfRegistration.registrationReasonIsSet()) {
      to_json(j, smfRegistration.getRegistrationReason());
      query += ",registrationReason='" + j.dump() + "'";
    }
    if (smfRegistration.contextInfoIsSet()) {
      to_json(j, smfRegistration.getContextInfo());
      query += ",contextInfo='" + j.dump() + "'";
    }

    to_json(j, smfRegistration.getSingleNssai());
    query += ",singleNssai='" + j.dump() + "'";
    to_json(j, smfRegistration.getPlmnId());
    query += ",plmnId='" + j.dump() + "'";
  }

  mysql_free_result(res);
  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  to_json(j, smfRegistration);
  response_data = j;
  // code = Pistache::Http::Code::Created;
  code = HTTP_STATUS_CODE_201_CREATED;

  Logger::udr_server().debug("SmfRegistration PUT - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_delete_smf_context(const std::string &ue_id,
                                        const int32_t &pdu_session_id,
                                        nlohmann::json &response_data,
                                        long &code) {
  const std::string query =
      "DELETE from SmfRegistrations WHERE ueid='" + ue_id +
      "' AND subpduSessionId=" + std::to_string(pdu_session_id);

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  response_data = {};
  // code = Pistache::Http::Code::No_Content;
  code = HTTP_STATUS_CODE_204_NO_CONTENT;
  Logger::udr_server().debug("SmfRegistration DELETE - successful");
}

//------------------------------------------------------------------------------
void udr_app::handle_query_smf_registration(const std::string &ue_id,
                                            const int32_t &pdu_session_id,
                                            nlohmann::json &response_data,
                                            long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  SmfRegistration smfregistration = {};
  const std::string query =
      "SELECT * from SmfRegistrations WHERE ueid='" + ue_id +
      "' AND subpduSessionId=" + std::to_string(pdu_session_id);

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);
  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      try {
        if (!strcmp("smfInstanceId", field->name)) {
          smfregistration.setSmfInstanceId(row[i]);
        } else if (!strcmp("smfSetId", field->name) && row[i] != NULL) {
          smfregistration.setSmfSetId(row[i]);
        } else if (!strcmp("supportedFeatures", field->name) &&
                   row[i] != NULL) {
          smfregistration.setSupportedFeatures(row[i]);
        } else if (!strcmp("pduSessionId", field->name)) {
          int32_t a = std::stoi(row[i]);
          smfregistration.setPduSessionId(a);
        } else if (!strcmp("singleNssai", field->name)) {
          Snssai singlenssai;
          nlohmann::json::parse(row[i]).get_to(singlenssai);
          smfregistration.setSingleNssai(singlenssai);
        } else if (!strcmp("dnn", field->name) && row[i] != NULL) {
          smfregistration.setDnn(row[i]);
        } else if (!strcmp("emergencyServices", field->name) &&
                   row[i] != NULL) {
          if (strcmp(row[i], "0"))
            smfregistration.setEmergencyServices(true);
          else
            smfregistration.setEmergencyServices(false);
        } else if (!strcmp("pcscfRestorationCallbackUri", field->name) &&
                   row[i] != NULL) {
          smfregistration.setPcscfRestorationCallbackUri(row[i]);
        } else if (!strcmp("plmnId", field->name)) {
          PlmnId plmnid;
          nlohmann::json::parse(row[i]).get_to(plmnid);
          smfregistration.setPlmnId(plmnid);
        } else if (!strcmp("pgwFqdn", field->name) && row[i] != NULL) {
          smfregistration.setPgwFqdn(row[i]);
        } else if (!strcmp("epdgInd", field->name) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            smfregistration.setEpdgInd(true);
          else
            smfregistration.setEpdgInd(false);
        } else if (!strcmp("deregCallbackUri", field->name) && row[i] != NULL) {
          smfregistration.setDeregCallbackUri(row[i]);
        } else if (!strcmp("registrationReason", field->name) &&
                   row[i] != NULL) {
          RegistrationReason registrationreason;
          nlohmann::json::parse(row[i]).get_to(registrationreason);
          smfregistration.setRegistrationReason(registrationreason);
        } else if (!strcmp("registrationTime", field->name) && row[i] != NULL) {
          smfregistration.setRegistrationTime(row[i]);
        } else if (!strcmp("contextInfo", field->name) && row[i] != NULL) {
          ContextInfo contextinfo;
          nlohmann::json::parse(row[i]).get_to(contextinfo);
          smfregistration.setContextInfo(contextinfo);
        }
      } catch (std::exception e) {
        Logger::udr_server().error(
            " Cannot set values for SMF Registration: %s", e.what());
      }
    }
    to_json(j, smfregistration);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug("SmfRegistration GET - json:\n\"%s\"",
                               j.dump().c_str());
  } else {
    Logger::udr_server().error("SmfRegistration no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}

//------------------------------------------------------------------------------
void udr_app::handle_query_smf_reg_list(const std::string &ue_id,
                                        nlohmann::json &response_data,
                                        long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  std::vector<std::string> fields;
  nlohmann::json j = {};
  nlohmann::json tmp = {};

  const std::string query =
      "SELECT * from SmfRegistrations WHERE ueid='" + ue_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  while (field = mysql_fetch_field(res)) {
    fields.push_back(field->name);
  }

  j.clear();
  while (row = mysql_fetch_row(res)) {
    SmfRegistration smfregistration = {};

    tmp.clear();

    for (int i = 0; i < fields.size(); i++) {
      try {
        if (!strcmp("smfInstanceId", fields[i].c_str())) {
          smfregistration.setSmfInstanceId(row[i]);
        } else if (!strcmp("smfSetId", fields[i].c_str()) && row[i] != NULL) {
          smfregistration.setSmfSetId(row[i]);
        } else if (!strcmp("supportedFeatures", fields[i].c_str()) &&
                   row[i] != NULL) {
          smfregistration.setSupportedFeatures(row[i]);
        } else if (!strcmp("pduSessionId", fields[i].c_str())) {
          int32_t a = std::stoi(row[i]);
          smfregistration.setPduSessionId(a);
        } else if (!strcmp("singleNssai", fields[i].c_str())) {
          Snssai singlenssai;
          nlohmann::json::parse(row[i]).get_to(singlenssai);
          smfregistration.setSingleNssai(singlenssai);
        } else if (!strcmp("dnn", fields[i].c_str()) && row[i] != NULL) {
          smfregistration.setDnn(row[i]);
        } else if (!strcmp("emergencyServices", fields[i].c_str()) &&
                   row[i] != NULL) {
          if (strcmp(row[i], "0"))
            smfregistration.setEmergencyServices(true);
          else
            smfregistration.setEmergencyServices(false);
        } else if (!strcmp("pcscfRestorationCallbackUri", fields[i].c_str()) &&
                   row[i] != NULL) {
          smfregistration.setPcscfRestorationCallbackUri(row[i]);
        } else if (!strcmp("plmnId", fields[i].c_str())) {
          PlmnId plmnid;
          nlohmann::json::parse(row[i]).get_to(plmnid);
          smfregistration.setPlmnId(plmnid);
        } else if (!strcmp("pgwFqdn", fields[i].c_str()) && row[i] != NULL) {
          smfregistration.setPgwFqdn(row[i]);
        } else if (!strcmp("epdgInd", fields[i].c_str()) && row[i] != NULL) {
          if (strcmp(row[i], "0"))
            smfregistration.setEpdgInd(true);
          else
            smfregistration.setEpdgInd(false);
        } else if (!strcmp("deregCallbackUri", fields[i].c_str()) &&
                   row[i] != NULL) {
          smfregistration.setDeregCallbackUri(row[i]);
        } else if (!strcmp("registrationReason", fields[i].c_str()) &&
                   row[i] != NULL) {
          RegistrationReason registrationreason;
          nlohmann::json::parse(row[i]).get_to(registrationreason);
          smfregistration.setRegistrationReason(registrationreason);
        } else if (!strcmp("registrationTime", fields[i].c_str()) &&
                   row[i] != NULL) {
          smfregistration.setRegistrationTime(row[i]);
        } else if (!strcmp("contextInfo", fields[i].c_str()) &&
                   row[i] != NULL) {
          ContextInfo contextinfo;
          nlohmann::json::parse(row[i]).get_to(contextinfo);
          smfregistration.setContextInfo(contextinfo);
        }
      } catch (std::exception e) {
        Logger::udr_server().error(
            " Cannot set values for SMF Registration: %s", e.what());
      }
    }
    to_json(tmp, smfregistration);
    j += tmp;
  }

  mysql_free_result(res);

  response_data = j;
  // code = Pistache::Http::Code::Ok;
  code = HTTP_STATUS_CODE_200_OK;

  Logger::udr_server().debug("SmfRegistrations GET - json:\n\"%s\"",
                             j.dump().c_str());
}

//------------------------------------------------------------------------------
void udr_app::handle_query_smf_select_data(const std::string &ue_id,
                                           const std::string &serving_plmn_id,
                                           nlohmann::json &response_data,
                                           long &code) {
  MYSQL_RES *res = NULL;
  MYSQL_ROW row = {};
  MYSQL_FIELD *field = nullptr;
  nlohmann::json j = {};
  SmfSelectionSubscriptionData smfselectionsubscriptiondata = {};
  const std::string query =
      "select * from SmfSelectionSubscriptionData WHERE ueid='" + ue_id +
      "' and servingPlmnid='" + serving_plmn_id + "'";

  if (mysql_real_query(&mysql, query.c_str(), (unsigned long)query.size())) {
    Logger::udr_server().error("mysql_real_query failure！SQL(%s)",
                               query.c_str());
    return;
  }

  res = mysql_store_result(&mysql);
  if (res == NULL) {
    Logger::udr_server().error("mysql_store_result failure！SQL(%s)",
                               query.c_str());
    return;
  }

  row = mysql_fetch_row(res);

  if (row != NULL) {
    for (int i = 0; field = mysql_fetch_field(res); i++) {
      if (!strcmp("supportedFeatures", field->name) && row[i] != NULL) {
        smfselectionsubscriptiondata.setSupportedFeatures(row[i]);
      } else if (!strcmp("subscribedSnssaiInfos", field->name) &&
                 row[i] != NULL) {
        std ::map<std ::string, SnssaiInfo> subscribedsnssaiinfos;
        nlohmann::json::parse(row[i]).get_to(subscribedsnssaiinfos);
        smfselectionsubscriptiondata.setSubscribedSnssaiInfos(
            subscribedsnssaiinfos);
      } else if (!strcmp("sharedSnssaiInfosId", field->name) &&
                 row[i] != NULL) {
        smfselectionsubscriptiondata.setSharedSnssaiInfosId(row[i]);
      }
    }
    to_json(j, smfselectionsubscriptiondata);
    response_data = j;
    // code = Pistache::Http::Code::Ok;
    code = HTTP_STATUS_CODE_200_OK;

    Logger::udr_server().debug(
        "SmfSelectionSubscriptionData GET - json:\n\"%s\"", j.dump().c_str());
  } else {
    Logger::udr_server().error("SmfSelectionSubscriptionData no data！SQL(%s)",
                               query.c_str());
  }

  mysql_free_result(res);
}
