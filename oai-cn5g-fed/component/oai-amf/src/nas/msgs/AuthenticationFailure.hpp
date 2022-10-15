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

#ifndef _AuthenticationFailure_H_
#define _AuthenticationFailure_H_

#include "nas_ie_header.hpp"

namespace nas {

class AuthenticationFailure {
 public:
  AuthenticationFailure();
  ~AuthenticationFailure();
  int encode2buffer(uint8_t* buf, int len);
  int decodefrombuffer(NasMmPlainHeader* header, uint8_t* buf, int len);
  void setHeader(uint8_t security_header_type);
  void set_5GMM_Cause(uint8_t value);
  void setAuthentication_Failure_Parameter(bstring auts);

  uint8_t get5GMmCause();
  bool getAutsInAuthFailPara(bstring& auts);

 public:
  NasMmPlainHeader* plain_header;
  _5GMM_Cause* ie_5gmm_cause;
  Authentication_Failure_Parameter* ie_authentication_failure_parameter;
};

}  // namespace nas

#endif
