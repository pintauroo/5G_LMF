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

/*! \file comUt.cpp
 \brief
 \author
 \company Eurecom
 */
#include "comUt.hpp"

//------------------------------------------------------------------------------
void comUt::print_buffer(
    const std::string app, const std::string commit, uint8_t* buf, int len) {
  if (!app.compare("udm_ueau")) Logger::udm_ueau().info(commit.c_str());
  for (int i = 0; i < len; i++) printf("%x ", buf[i]);
  printf("\n");
}

//------------------------------------------------------------------------------
void comUt::print_buffer(
    const string app, const string commit, const uint8_t* buf, int len) {
  if (!app.compare("udm_ueau")) cout << commit.c_str() << endl;
  Logger::udm_ueau().debug(commit.c_str());

  for (int i = 0; i < len; i++) printf("%x ", buf[i]);
  printf("\n");
}

//------------------------------------------------------------------------------
void comUt::hexStr2Byte(const char* src, unsigned char* dest, int len) {
  short i;
  unsigned char hBy, lBy;
  for (i = 0; i < len; i += 2) {
    hBy = toupper(src[i]);
    lBy = toupper(src[i + 1]);
    if (hBy > 0x39)
      hBy -= 0x37;
    else
      hBy -= 0x30;
    if (lBy > 0x39)
      lBy -= 0x37;
    else
      lBy -= 0x30;
    dest[i / 2] = (hBy << 4) | lBy;
  }
}
