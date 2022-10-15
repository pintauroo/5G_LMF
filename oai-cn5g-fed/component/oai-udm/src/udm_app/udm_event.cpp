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

/*! \file udm_event.cpp
 \brief
 \author Tien-Thinh NGUYEN (EURECOM)
 \company
 \date 2022
 \email: contact@openairinterface.org
 */

#include "udm_event.hpp"
using namespace oai::udm::app;

//------------------------------------------------------------------------------
bs2::connection udm_event::subscribe_loss_of_connectivity(
    const loss_of_connectivity_sig_t::slot_type& sig) {
  return loss_of_connectivity.connect(sig);
}

//------------------------------------------------------------------------------
bs2::connection udm_event::subscribe_ue_reachability_for_data(
    const ue_reachability_for_data_sig_t::slot_type& sig) {
  return ue_reachability_for_data.connect(sig);
}
