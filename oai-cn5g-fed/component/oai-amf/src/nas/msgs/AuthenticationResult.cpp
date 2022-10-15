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

#include "AuthenticationResult.hpp"

#include "3gpp_ts24501.hpp"
#include "logger.hpp"

using namespace nas;

//------------------------------------------------------------------------------
AuthenticationResult::AuthenticationResult() {
  plain_header   = NULL;
  ie_ngKSI       = NULL;
  ie_abba        = NULL;
  ie_eap_message = NULL;
}

//------------------------------------------------------------------------------
AuthenticationResult::~AuthenticationResult() {}

//------------------------------------------------------------------------------
void AuthenticationResult::setHeader(uint8_t security_header_type) {
  plain_header = new NasMmPlainHeader();
  plain_header->setHeader(
      EPD_5GS_MM_MSG, security_header_type, AUTHENTICATION_RESULT);
}

//------------------------------------------------------------------------------
void AuthenticationResult::setngKSI(uint8_t tsc, uint8_t key_set_id) {
  ie_ngKSI = new NasKeySetIdentifier(0x00, tsc, key_set_id);
}

//------------------------------------------------------------------------------
void AuthenticationResult::setABBA(uint8_t length, uint8_t* value) {
  ie_abba = new ABBA(0x38, length, value);
}

//------------------------------------------------------------------------------
void AuthenticationResult::setEAP_Message(bstring eap) {
  ie_eap_message = new EAP_Message(0x00, eap);
}

//------------------------------------------------------------------------------
int AuthenticationResult::encode2buffer(uint8_t* buf, int len) {
  Logger::nas_mm().debug("Encoding AuthenticationResult message");
  int encoded_size = 0;
  if (!plain_header) {
    Logger::nas_mm().error("Mandatory IE missing Header");
    return 0;
  }
  if (!(plain_header->encode2buffer(buf, len))) return 0;
  encoded_size += 3;
  if (!ie_ngKSI) {
    Logger::nas_mm().warn("IE ie_ngKSI is not available");
  } else {
    if (int size =
            ie_ngKSI->encode2buffer(buf + encoded_size, len - encoded_size)) {
      encoded_size += size;
    } else {
      Logger::nas_mm().error("Encoding ie_ngKSI error");
      return 0;
    }
  }
  if (!ie_eap_message) {
    Logger::nas_mm().warn("IE ie_eap_message is not available");
  } else {
    if (int size = ie_eap_message->encode2buffer(
            buf + encoded_size, len - encoded_size)) {
      encoded_size += size;
    } else {
      Logger::nas_mm().error("Encoding ie_eap_message error");
      return 0;
    }
  }
  if (!ie_abba) {
    Logger::nas_mm().warn("IE ie_abba is not available");
  } else {
    if (int size =
            ie_abba->encode2buffer(buf + encoded_size, len - encoded_size)) {
      encoded_size += size;
    } else {
      Logger::nas_mm().error("encoding ie_abba error");
      return 0;
    }
  }
  Logger::nas_mm().debug(
      "Encoded AuthenticationResult message len (%d)", encoded_size);
  return 1;
}

//------------------------------------------------------------------------------
int AuthenticationResult::decodefrombuffer(
    NasMmPlainHeader* header, uint8_t* buf, int len) {
  Logger::nas_mm().debug("Decoding AuthenticationResult message");
  int decoded_size = 3;
  plain_header     = header;
  ie_ngKSI         = new NasKeySetIdentifier();
  decoded_size += ie_ngKSI->decodefrombuffer(
      buf + decoded_size, len - decoded_size, false, false);
  decoded_size++;
  ie_eap_message = new EAP_Message();
  decoded_size += ie_eap_message->decodefrombuffer(
      buf + decoded_size, len - decoded_size, false);
  Logger::nas_mm().debug("Decoded_size (%d)", decoded_size);
  uint8_t octet = *(buf + decoded_size);
  Logger::nas_mm().debug("First option IEI (0x%x)", octet);
  while ((octet != 0x0)) {
    switch (octet) {
      case 0x38: {
        Logger::nas_mm().debug("Decoding IEI (0x38)");
        ie_abba = new ABBA();
        decoded_size += ie_abba->decodefrombuffer(
            buf + decoded_size, len - decoded_size, true);
        octet = *(buf + decoded_size);
        Logger::nas_mm().debug("Next IEI (0x%x)", octet);
      } break;
    }
  }
  Logger::nas_mm().debug(
      "Decoded AuthenticationResult message len (%d)", decoded_size);
  return 1;
}
