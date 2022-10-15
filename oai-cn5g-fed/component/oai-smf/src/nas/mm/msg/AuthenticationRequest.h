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

#ifndef AUTHENTICATION_REQUEST_H_
#define AUTHENTICATION_REQUEST_H_

#include <stdint.h>

#include "ExtendedProtocolDiscriminator.h"
#include "SecurityHeaderType.h"
#include "MessageType.h"
#include "NASKeySetIdentifier.h"
#include "ABBA.h"
#include "AuthenticationParameterRAND.h"
#include "AuthenticationParameterAUTN.h"
#include "EAPMessage.h"

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define AUTHENTICATION_REQUEST_MINIMUM_LENGTH                                  \
  (EXTENDED_PROTOCOL_DISCRIMINATOR_MINIMUM_LENGTH +                            \
   SECURITY_HEADER_TYPE_MINIMUM_LENGTH + MESSAGE_TYPE_MINIMUM_LENGTH +         \
   ABBA_MINIMUM_LENGTH + NAS_KEY_SET_IDENTIFIER_MINIMUM_LENGTH + 0)
/*
 AUTHENTICATION_PARAMETER_RAND_MINIMUM_LENGTH + \
                AUTHENTICATION_PARAMETER_AUTN_MINIMUM_LENGTH + \
                EAP_MESSAGE_MINIMUM_LENGTH +
 0)
 */

/* Maximum length macro. Formed by maximum length of each field */
#define AUTHENTICATION_REQUEST_MAXIMUM_LENGTH                                  \
  (EXTENDED_PROTOCOL_DISCRIMINATOR_MAXIMUM_LENGTH +                            \
   SECURITY_HEADER_TYPE_MAXIMUM_LENGTH + MESSAGE_TYPE_MAXIMUM_LENGTH +         \
   NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH +                                     \
   AUTHENTICATION_PARAMETER_RAND_MAXIMUM_LENGTH +                              \
   AUTHENTICATION_PARAMETER_AUTN_MAXIMUM_LENGTH + EAP_MESSAGE_MAXIMUM_LENGTH + \
   0)

#define AUTHENTICATION_PARAMETER_RAND_IEI 0x21
#define AUTHENTICATION_PARAMETER_AUTN_IEI 0x20
#define EAP_MESSAGE_IEI 0x78

#define AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_PRESENT (1 << 0)
#define AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_PRESENT (1 << 1)
#define AUTHENTICATION_REQUEST_EAP_MESSAGE_PRESENT (1 << 2)

typedef struct authentication_request_msg_tag {
  ExtendedProtocolDiscriminator extendedprotocoldiscriminator;
  SecurityHeaderType securityheadertype;
  MessageType messagetype;
  NASKeySetIdentifier naskeysetidentifier;
  ABBA abba;
  uint8_t presence;
  AuthenticationParameterRAND authenticationparameterrand;
  AuthenticationParameterAUTN authenticationparameterautn;
  EAPMessage eapmessage;
} authentication_request_msg;

int decode_authentication_request(
    authentication_request_msg* authenticationrequest, uint8_t* buffer,
    uint32_t len);
int encode_authentication_request(
    authentication_request_msg* authenticationrequest, uint8_t* buffer,
    uint32_t len);

#endif
