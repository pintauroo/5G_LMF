#!/usr/bin/env bash

#"""
#/*
# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this
# * file except in compliance with the License. You may obtain a copy of the
# * License at
# *
# *      http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *      contact@openairinterface.org
# */
#
#/*! \file nrf_client.py
#   \author  Rohan KHARADE
#   \date 2021
#   \email: rohan.kharade@openairinterface.org
#*/
#"""

# files are common to docker and native installation
#____________
# init.conf          => upf config
# startup_debug.conf => vpp config (To Do -> sed location of init.conf file)
# run_sh             => run vpp
#____________

set -euo pipefail

CONFIG_DIR="/openair-upf/etc"
SGI_IPV4=$(ifconfig $INTERFACE_CORE | grep "inet " | awk '{print $2}')
ACCESS_IPV4=$(ifconfig $INTERFACE_ACCESS | grep "inet " | awk '{print $2}')
CORE_IPV4=$(ifconfig $INTERFACE_CP | grep "inet " | awk '{print $2}')

N3_IPV4_ADDRESS_LOCAL=$(ifconfig $INTERFACE_ACCESS | grep "inet " | awk '{print $2}' | cut -d"." -f1-3)".202"
N4_IPV4_ADDRESS_LOCAL=$(ifconfig $INTERFACE_CP | grep "inet " | awk '{print $2}' | cut -d"." -f1-3)".202"
N6_IPV4_ADDRESS_LOCAL=$(ifconfig $INTERFACE_CORE | grep "inet " | awk '{print $2}' | cut -d"." -f1-3)".202"

###############################
# UPF Config
###############################
array=(${CONFIG_DIR}/*.conf ${CONFIG_DIR}/*.json)
for c in "${array[@]}"; do
    # grep variable names (format: ${VAR}) from template to be rendered
    VARS=$(grep -oP '@[a-zA-Z0-9_]+@' ${c} | sort | uniq | xargs)
    # create sed expressions for substituting each occurrence of ${VAR}
    # with the value of the environment variable "VAR"
    EXPRESSIONS=""
    for v in ${VARS}; do
	NEW_VAR=`echo $v | sed -e "s#@##g"`
        if [[ "${!NEW_VAR}x" == "x" ]]; then
            echo "Error: Environment variable '${NEW_VAR}' is not set." \
                "Config file '$(basename $c)' requires all of $VARS."
            exit 1
        fi
        EXPRESSIONS="${EXPRESSIONS};s|${v}|${!NEW_VAR}|g"
    done
    EXPRESSIONS="${EXPRESSIONS#';'}"
    # render template and inline replace config file
    sed -i "${EXPRESSIONS}" ${c}
done

###############################
# VPP Routes
###############################

# Assumption ->
# UPF has only three interfaces viz. n3 (access) eth0, n4 (core) eth1, n6 (sgi) eth2
# Near future we will have multiple interfaces (e.g. two n6 interface for edge computing case)
# We define in this order in docker-compose -> it is alphabetical order
#

ip link set $INTERFACE_ACCESS down
ip link set $INTERFACE_ACCESS name n3
ip link set n3 up

ip link set $INTERFACE_CP down
ip link set $INTERFACE_CP name n4
ip link set n4 up

ip link set $INTERFACE_CORE down
ip link set $INTERFACE_CORE name n6
ip link set n6 up

ip route add $NETWORK_UE_IP via $SGI_IPV4 dev n6

echo "Done setting the configuration"

exec "$@"
