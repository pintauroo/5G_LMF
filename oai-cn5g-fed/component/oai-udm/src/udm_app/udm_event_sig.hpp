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

/*! \file udm_event_sig.hpp
 \brief
 \author  Tien-Thinh NGUYEN (EURECOM)
 \company
 \date 2022
 \email: contact@openairinterface.org
 */

#ifndef FILE_UDM_EVENT_SIG_HPP_SEEN
#define FILE_UDM_EVENT_SIG_HPP_SEEN

#include <boost/signals2.hpp>
#include <string>

namespace bs2 = boost::signals2;

namespace oai::udm::app {

// Signal for Loss of Connectivity
// SUPI, Connectivity status, HTTP version
typedef bs2::signal_type<
    void(std::string, uint8_t, uint8_t),
    bs2::keywords::mutex_type<bs2::dummy_mutex>>::type
    loss_of_connectivity_sig_t;

// Signal for UE Reachability for Data
// SUPI, Reachability status, HTTP version
typedef bs2::signal_type<
    void(std::string, uint8_t, uint8_t),
    bs2::keywords::mutex_type<bs2::dummy_mutex>>::type
    ue_reachability_for_data_sig_t;

// UE_REACHABILITY_FOR_SMS
// LOCATION_REPORTING
// CHANGE_OF_SUPI_PEI_ASSOCIATION
// ROAMING_STATUS
// COMMUNICATION_FAILURE
// AVAILABILITY_AFTER_DNN_FAILURE
// CN_TYPE_CHANGE

}  // namespace oai::udm::app
#endif
