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

#ifndef FILE_AUSF_SEEN
#define FILE_AUSF_SEEN

#define HEART_BEAT_TIMER 10

#define _unused(x) ((void) (x))

typedef enum nf_type_s {
  NF_TYPE_NRF     = 0,
  NF_TYPE_AMF     = 1,
  NF_TYPE_SMF     = 2,
  NF_TYPE_AUSF    = 3,
  NF_TYPE_NEF     = 4,
  NF_TYPE_PCF     = 5,
  NF_TYPE_SMSF    = 6,
  NF_TYPE_NSSF    = 7,
  NF_TYPE_UDR     = 8,
  NF_TYPE_LMF     = 9,
  NF_TYPE_GMLC    = 10,
  NF_TYPE_5G_EIR  = 11,
  NF_TYPE_SEPP    = 12,
  NF_TYPE_UPF     = 13,
  NF_TYPE_N3IWF   = 14,
  NF_TYPE_AF      = 15,
  NF_TYPE_UDSF    = 16,
  NF_TYPE_BSF     = 17,
  NF_TYPE_CHF     = 18,
  NF_TYPE_NWDAF   = 19,
  NF_TYPE_UNKNOWN = 20
} nf_type_t;

static const std::vector<std::string> nf_type_e2str = {
    "NRF",   "AMF", "SMF",  "AUSF", "NEF",    "PCF",   "SMSF",
    "NSSF",  "UDR", "LMF",  "GMLC", "5G_EIR", "SEPP",  "UPF",
    "N3IWF", "AF",  "UDSF", "BSF",  "CHF",    "NWDAF", "UNKNOWN"};

typedef enum patch_op_type_s {
  PATCH_OP_ADD     = 0,
  PATCH_OP_REMOVE  = 1,
  PATCH_OP_REPLACE = 2,
  PATCH_OP_MOVE    = 3,
  PATCH_OP_COPY    = 4,
  PATCH_OP_TEST    = 5,
  PATCH_OP_UNKNOWN = 6

} patch_op_type_t;

static const std::vector<std::string> patch_op_type_e2str = {
    "ADD", "REMOVE", "REPLACE", "MOVE", "COPY", "TEST", "UNKNOWN"};

#define CURL_TIMEOUT_MS 1000L
#define MAX_WAIT_MSECS 20000  // 1 second

typedef struct {
  uint8_t rand[16];
  uint8_t autn[16];
  uint8_t hxresStar[16];
  uint8_t kseaf[32];
} AUSF_AV_s;

typedef uint64_t supi64_t;

// 3GPP TS 29.571 (Common data)
enum http_response_codes_e {
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

#define NAUSF_AUTH_BASE "/nausf-auth/"
#define NAUSF_UE_AUTHS "/ue-authentications"
#define NAUSF_UE_AUTHS_DEREG "/ue-authentications/deregister"
#define NAUSF_RG_AUTH "/rg-authentications"

typedef struct supi_range_s {
  std::string start;
  std::string end;
  std::string pattern;
} supi_range_t;

typedef struct supi_range_ausf_info_item_s {
  supi_range_t supi_range;
} supi_range_ausf_info_item_t;

typedef struct ausf_info_s {
  std::string groupid;
  std::vector<supi_range_ausf_info_item_t> supi_ranges;
  std::vector<std::string> routing_indicators;
} ausf_info_t;

#endif
