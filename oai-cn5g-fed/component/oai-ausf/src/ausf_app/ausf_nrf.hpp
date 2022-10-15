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

#ifndef FILE_AUSF_NRF_SEEN
#define FILE_AUSF_NRF_SEEN

#include <map>
#include <thread>

#include <curl/curl.h>

#include "logger.hpp"
#include "ausf_config.hpp"
#include "ausf_profile.hpp"

namespace oai {
namespace ausf {
namespace app {

class ausf_nrf {
 private:
 public:
  ausf_profile ausf_nf_profile;  // AUSF profile
  std::string ausf_instance_id;  // AUSF instance id
  // timer_id_t timer_ausf_heartbeat;

  ausf_nrf();
  ausf_nrf(ausf_nrf const&) = delete;
  void operator=(ausf_nrf const&) = delete;

  void generate_uuid();

  /*
   * Generate a AUSF profile for this instance
   * @param [void]
   * @return void
   */
  void generate_ausf_profile(
      ausf_profile& ausf_nf_profile, std::string& ausf_instance_id);

  /*
   * Trigger NF instance registration to NRF
   * @param [void]
   * @return void
   */
  void register_to_nrf();
  /*
   * Get ausf API Root
   * @param [std::string& ] api_root: ausf's API Root
   * @return void
   */
  void get_ausf_api_root(std::string& api_root);
};
}  // namespace app
}  // namespace ausf
}  // namespace oai
#endif /* FILE_AUSF_NRF_SEEN */
