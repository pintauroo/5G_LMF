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

/*! \file ausf_client.hpp
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email:
 */

#ifndef FILE_AUSF_CLIENT_HPP_SEEN
#define FILE_AUSF_CLIENT_HPP_SEEN

#include <map>
#include <thread>

#include <curl/curl.h>

#include "logger.hpp"
#include "ausf_config.hpp"

namespace oai {
namespace ausf {
namespace app {

class ausf_client {
 private:
 public:
  ausf_client();
  virtual ~ausf_client();

  ausf_client(ausf_client const&) = delete;

  void curl_http_client(
      std::string remoteUri, std::string method, std::string msgBody,
      std::string& response);
};
}  // namespace app
}  // namespace ausf
}  // namespace oai
#endif /* FILE_AUSF_CLIENT_HPP_SEEN */
