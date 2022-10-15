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

/*! \file
 \brief
 \author  Keliang DU, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */

#ifndef __Extended_DRX_Parameters_H_
#define __Extended_DRX_Parameters_H_

#include <stdint.h>

namespace nas {

class Extended_DRX_Parameters {
 public:
  Extended_DRX_Parameters();
  Extended_DRX_Parameters(uint8_t iei);
  Extended_DRX_Parameters(
      const uint8_t iei, uint8_t paging_time, uint8_t value);
  ~Extended_DRX_Parameters();
  int encode2buffer(uint8_t* buf, int len);
  int decodefrombuffer(uint8_t* buf, int len, bool is_option);
  void setValue(uint8_t value);
  uint8_t getValue();
  void setPaging_time(uint8_t value);
  uint8_t getPaging_time();

 private:
  uint8_t _iei;
  uint8_t _paging_time;
  uint8_t _value;
};
}  // namespace nas

#endif
