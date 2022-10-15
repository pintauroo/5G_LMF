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

#ifndef FILE_3GPP_29_503_SMF_SEEN
#define FILE_3GPP_29_503_SMF_SEEN

#include "smf.h"
#include "3gpp_29.571.h"

enum ssc_mode_e {
  SSC_MODE_1 = 1,
  SSC_MODE_2 = 2,
  SSC_MODE_3 = 3,
};
static const std::vector<std::string> ssc_mode_e2str = {
    "Error", "SSC_MODE_1", "SSC_MODE_2", "SSC_MODE_3"};

typedef struct ssc_mode_s {
  uint8_t ssc_mode;
  ssc_mode_s() : ssc_mode(SSC_MODE_1) {}
  ssc_mode_s(ssc_mode_e mode) : ssc_mode(mode) {}
  ssc_mode_s(const struct ssc_mode_s& p) : ssc_mode(p.ssc_mode) {}

  ssc_mode_s(const std::string& s) {
    if (s.compare("SSC_MODE_1") == 0) {
      ssc_mode = ssc_mode_e::SSC_MODE_1;
    } else if (s.compare("SSC_MODE_2") == 0) {
      ssc_mode = ssc_mode_e::SSC_MODE_2;
    } else if (s.compare("SSC_MODE_3") == 0) {
      ssc_mode = ssc_mode_e::SSC_MODE_3;
    } else {
      ssc_mode = ssc_mode_e::SSC_MODE_1;  // default mode
    }
  }

  ssc_mode_s& operator=(const ssc_mode_s& s) {
    ssc_mode = s.ssc_mode;
    return *this;
  }

  virtual ~ssc_mode_s(){};

} ssc_mode_t;

typedef struct pdu_session_types_s {
  pdu_session_type_t default_session_type;
  std::vector<pdu_session_type_t> allowed_session_types;
} pdu_session_types_t;

typedef struct ssc_modes_s {
  ssc_mode_t default_ssc_mode;
  std::vector<ssc_mode_t> allowed_ssc_modes;
} ssc_modes_t;

enum ip_address_type_value_e {
  IP_ADDRESS_TYPE_IPV4_ADDRESS = 0,
  IP_ADDRESS_TYPE_IPV6_ADDRESS = 1,
  IP_ADDRESS_TYPE_IPV6_PREFIX  = 2
};

typedef struct ipv6_prefix_s {
  struct in6_addr prefix;
  uint8_t prefix_len;
  std::string to_string() const {
    return conv::toString(prefix) + "/" + std::to_string(prefix_len);
  }

} ipv6_prefix_t;

typedef struct ip_address_s {
  uint8_t ip_address_type;
  union {
    struct in_addr ipv4_address;
    struct in6_addr ipv6_address;
    ipv6_prefix_t ipv6_prefix;
  } u1;

  bool operator==(const struct ip_address_s& i) const {
    if ((i.ip_address_type == this->ip_address_type) &&
        (i.u1.ipv4_address.s_addr == this->u1.ipv4_address.s_addr) &&
        (i.u1.ipv6_address.s6_addr32[0] ==
         this->u1.ipv6_address.s6_addr32[0]) &&
        (i.u1.ipv6_address.s6_addr32[1] ==
         this->u1.ipv6_address.s6_addr32[1]) &&
        (i.u1.ipv6_address.s6_addr32[2] ==
         this->u1.ipv6_address.s6_addr32[2]) &&
        (i.u1.ipv6_address.s6_addr32[3] ==
         this->u1.ipv6_address.s6_addr32[3]) &&
        (i.u1.ipv6_prefix.prefix_len == this->u1.ipv6_prefix.prefix_len) &&
        (i.u1.ipv6_prefix.prefix.s6_addr32[0] ==
         this->u1.ipv6_prefix.prefix.s6_addr32[0]) &&
        (i.u1.ipv6_prefix.prefix.s6_addr32[1] ==
         this->u1.ipv6_prefix.prefix.s6_addr32[1]) &&
        (i.u1.ipv6_prefix.prefix.s6_addr32[2] ==
         this->u1.ipv6_prefix.prefix.s6_addr32[2]) &&
        (i.u1.ipv6_prefix.prefix.s6_addr32[3] ==
         this->u1.ipv6_prefix.prefix.s6_addr32[3])) {
      return true;
    } else {
      return false;
    }
  };

  bool operator==(const struct in_addr& a) const {
    if ((IP_ADDRESS_TYPE_IPV4_ADDRESS == this->ip_address_type) &&
        (a.s_addr == u1.ipv4_address.s_addr)) {
      return true;
    } else {
      return false;
    }
  };
  bool operator==(const struct in6_addr& i) const {
    if ((IP_ADDRESS_TYPE_IPV6_ADDRESS == this->ip_address_type) &&
        (i.s6_addr32[0] == this->u1.ipv6_address.s6_addr32[0]) &&
        (i.s6_addr32[1] == this->u1.ipv6_address.s6_addr32[1]) &&
        (i.s6_addr32[2] == this->u1.ipv6_address.s6_addr32[2]) &&
        (i.s6_addr32[3] == this->u1.ipv6_address.s6_addr32[3])) {
      return true;
    } else {
      return false;
    }
  };

  bool operator==(const ipv6_prefix_t& i) const {
    if ((IP_ADDRESS_TYPE_IPV6_PREFIX == this->ip_address_type) &&
        (i.prefix_len == this->u1.ipv6_prefix.prefix_len) &&
        (i.prefix.s6_addr32[0] == this->u1.ipv6_prefix.prefix.s6_addr32[0]) &&
        (i.prefix.s6_addr32[1] == this->u1.ipv6_prefix.prefix.s6_addr32[1]) &&
        (i.prefix.s6_addr32[2] == this->u1.ipv6_prefix.prefix.s6_addr32[2]) &&
        (i.prefix.s6_addr32[3] == this->u1.ipv6_prefix.prefix.s6_addr32[3])) {
      return true;
    } else {
      return false;
    }
  };

  ip_address_s& operator=(const struct in_addr& a) {
    ip_address_type        = IP_ADDRESS_TYPE_IPV4_ADDRESS;
    u1.ipv4_address.s_addr = a.s_addr;
    return *this;
  }

  ip_address_s& operator=(const struct in6_addr& a) {
    ip_address_type              = IP_ADDRESS_TYPE_IPV6_ADDRESS;
    u1.ipv6_address.s6_addr32[0] = a.s6_addr32[0];
    u1.ipv6_address.s6_addr32[1] = a.s6_addr32[1];
    u1.ipv6_address.s6_addr32[2] = a.s6_addr32[2];
    u1.ipv6_address.s6_addr32[3] = a.s6_addr32[3];
    return *this;
  }

  ip_address_s& operator=(const ipv6_prefix_t& a) {
    ip_address_type                    = IP_ADDRESS_TYPE_IPV6_PREFIX;
    u1.ipv6_prefix.prefix_len          = a.prefix_len;
    u1.ipv6_prefix.prefix.s6_addr32[0] = a.prefix.s6_addr32[0];
    u1.ipv6_prefix.prefix.s6_addr32[1] = a.prefix.s6_addr32[1];
    u1.ipv6_prefix.prefix.s6_addr32[2] = a.prefix.s6_addr32[2];
    u1.ipv6_prefix.prefix.s6_addr32[3] = a.prefix.s6_addr32[3];
    return *this;
  }

  virtual ~ip_address_s(){};

  std::string to_string() const {
    if (IP_ADDRESS_TYPE_IPV4_ADDRESS == this->ip_address_type) {
      return conv::toString(u1.ipv4_address);
    } else if (IP_ADDRESS_TYPE_IPV6_ADDRESS == this->ip_address_type) {
      return conv::toString(u1.ipv6_address);
    } else if (IP_ADDRESS_TYPE_IPV6_PREFIX == this->ip_address_type) {
      return u1.ipv6_prefix.to_string();
    }
    return std::string("Unknown IP Address Type");
  }
} ip_address_t;

typedef struct dnn_configuration_s {
  pdu_session_types_t pdu_session_types;
  ssc_modes_t ssc_modes;
  session_ambr_t session_ambr;
  subscribed_default_qos_t _5g_qos_profile;
  std::vector<ip_address_t> static_ip_addresses;
} dnn_configuration_t;

#endif
