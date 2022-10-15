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

/*! \file ausf_app.hpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2021
 \email:
 */

#ifndef FILE_AUSF_APP_HPP_SEEN
#define FILE_AUSF_APP_HPP_SEEN

#include <string>
#include "AuthenticationInfo.h"
#include "UEAuthenticationCtx.h"
#include "ConfirmationData.h"
#include "ausf.h"
#include <pistache/http.h>
#include <map>
#include <shared_mutex>

namespace oai {
namespace ausf {
namespace app {

using namespace oai::ausf_server::model;

class security_context {
 public:
  security_context() : xres_star() {
    // supi       = {};
    ausf_av_s  = {};
    supi_ausf  = "";
    auth_type  = "";
    serving_nn = "";
    kausf_tmp  = "";
  }

  // supi64_t supi;
  AUSF_AV_s ausf_av_s;
  uint8_t xres_star[16];   // store xres*
  std::string supi_ausf;   // store supi
  std::string auth_type;   // store authType
  std::string serving_nn;  // store serving network name
  std::string kausf_tmp;   // store Kausf(string)
};

// class ausf_config;
class ausf_app {
 public:
  explicit ausf_app(const std::string& config_file);
  ausf_app(ausf_app const&) = delete;
  void operator=(ausf_app const&) = delete;

  virtual ~ausf_app();

  void handle_ue_authentications(
      const AuthenticationInfo& authenticationInfo, nlohmann::json& json_data,
      std::string& location, Pistache::Http::Code& code,
      uint8_t http_version = 1);

  void handle_ue_authentications_confirmation(
      const std::string& authCtxId, const ConfirmationData& confirmation_data,
      nlohmann::json& json_data, Pistache::Http::Code& code);

  bool is_supi_2_security_context(const std::string& supi) const;
  std::shared_ptr<security_context> supi_2_security_context(
      const std::string& supi) const;
  void set_supi_2_security_context(
      const std::string& supi, std::shared_ptr<security_context> sc);

  bool is_contextId_2_security_context(const std::string& contextId) const;
  std::shared_ptr<security_context> contextId_2_security_context(
      const std::string& contextId) const;
  void set_contextId_2_security_context(
      const std::string& contextId, std::shared_ptr<security_context> sc);

 private:
  std::map<supi64_t, std::shared_ptr<security_context>> imsi2security_context;
  mutable std::shared_mutex m_imsi2security_context;

  std::map<std::string, std::shared_ptr<security_context>>
      supi2security_context;
  mutable std::shared_mutex m_supi2security_context;

  std::map<std::string, std::shared_ptr<security_context>>
      contextId2security_context;
  mutable std::shared_mutex m_contextId2security_context;
};
}  // namespace app
}  // namespace ausf
}  // namespace oai
#include "ausf_config.hpp"

#endif /* FILE_AUSF_APP_HPP_SEEN */
