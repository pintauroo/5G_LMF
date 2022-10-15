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

/*! \file api_conversions.cpp
 \brief
 \author  Tien-Thinh NGUYEN
 \company Eurecom
 \date 2020
 \email: tien-thinh.nguyen@eurecom.fr
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>
#include <nlohmann/json.hpp>
#include <regex>

#include "api_conversions.hpp"
#include "logger.hpp"
#include "udm.h"
#include "string.hpp"

//------------------------------------------------------------------------------
patch_op_type_t util::api_conv::string_to_patch_operation(
    const std::string& str) {
  if (str.compare("add") == 0) return PATCH_OP_ADD;
  if (str.compare("copy") == 0) return PATCH_OP_COPY;
  if (str.compare("move") == 0) return PATCH_OP_MOVE;
  if (str.compare("remove") == 0) return PATCH_OP_REMOVE;
  if (str.compare("replace") == 0) return PATCH_OP_REPLACE;
  if (str.compare("test") == 0) return PATCH_OP_TEST;
  // default
  return PATCH_OP_UNKNOWN;
}
