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

#ifndef FILE_NSSF_SEEN
#define FILE_NSSF_SEEN

#include "3gpp_29.571.h"
#include <nlohmann/json.hpp>

// SMF + AMF + 3GPP TS 29.571 (Common data)
enum class http_response_codes_e {
  HTTP_RESPONSE_CODE_OK                     = 200,
  HTTP_RESPONSE_CODE_CREATED                = 201,
  HTTP_RESPONSE_CODE_ACCEPTED               = 202,
  HTTP_RESPONSE_CODE_NO_CONTENT             = 204,
  HTTP_RESPONSE_CODE_BAD_REQUEST            = 400,
  HTTP_RESPONSE_CODE_UNAUTHORIZED           = 401,
  HTTP_RESPONSE_CODE_FORBIDDEN              = 403,
  HTTP_RESPONSE_CODE_NOT_FOUND              = 404,
  HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED     = 405,
  HTTP_RESPONSE_CODE_REQUEST_TIMEOUT        = 408,
  HTTP_RESPONSE_CODE_406_NOT_ACCEPTED       = 406,
  HTTP_RESPONSE_CODE_CONFLICT               = 409,
  HTTP_RESPONSE_CODE_GONE                   = 410,
  HTTP_RESPONSE_CODE_LENGTH_REQUIRED        = 411,
  HTTP_RESPONSE_CODE_PRECONDITION_FAILED    = 412,
  HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE      = 413,
  HTTP_RESPONSE_CODE_URI_TOO_LONG           = 414,
  HTTP_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE = 415,
  HTTP_RESPONSE_CODE_TOO_MANY_REQUESTS      = 429,
  HTTP_RESPONSE_CODE_INTERNAL_SERVER_ERROR  = 500,
  HTTP_RESPONSE_CODE_NOT_IMPLEMENTED        = 501,
  HTTP_RESPONSE_CODE_SERVICE_UNAVAILABLE    = 503,
  HTTP_RESPONSE_CODE_GATEWAY_TIMEOUT        = 504
};

#define ROAMING_IND_NON_ROAMING (1)
#define ROAMING_IND_LOCAL_BREAKOUT (2)
#define ROAMING_IND_HOME_ROUTED_ROAMING (3)

// NF TYPES
#define NF_TYPE_SMF "SMF"
#define NF_TYPE_NSSF "NSSF"
#define NF_TYPE_AMF "AMF"

// NRF
#define NNRF_NFM_BASE "/nnrf-nfm/"
#define NNRF_NF_REGISTER_URL "/nf-instances/"
#define NNRF_NF_STATUS_SUBSCRIBE_URL "/subscriptions"

// NSSF
#define NSSF_NSS_BASE "/nnssf-nsselection/"
#define NSSF_NS_INFO_URL "/network-slice-information"
#define NSSF_NSSAI_AVAILABILITY_BASE "/nnssf-nssaiavailability/"
#define NSSF_NSSAI_AVAILABILITY_URL "/nssai-availability/"
#define NSSF_NSSAI_AVAILABILITY_SUBSCRIPTION_URL                               \
  "/nssai-availability/subscriptions"

// for CURL
#define NF_CURL_TIMEOUT_MS 100L
#define MAX_WAIT_MSECS 10000  // 1 second
#define AMF_NUMBER_RETRIES 3
#define UDM_NUMBER_RETRIES 3

// for API server query parameters
#define NF_TYPE "nf-type"
#define NF_ID "nf-id"
#define HOME_PLMN_ID "home-plmn-id"
#define TAI "tai"
#define SUPPORTED_FEATURES "supported-features"

#define SLICE_INFO_REGISTRATION "slice-info-request-for-registration"
#define SLICE_INFO_PDU_SESSION "slice-info-request-for-pdu-session"
#define SLICE_INFO_UE_CU "slice-info-request-for-registration"

#endif
