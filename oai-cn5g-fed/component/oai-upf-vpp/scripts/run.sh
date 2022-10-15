#!/bin/bash

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

if [ $(id -u) -ne 0 ]; then
    exec sudo -E "$0" "$@"
fi

base=$(dirname $0)

APP="$base/bin/vpp"
ARGS="-c $base/etc/startup_debug.conf"

USAGE="Usage: run.sh [-r] [ debug ]
       debug:	executes vpp under gdb"

while getopts ":r" opt; do
    case $opt in
	r)
	    APP="$base/bin/vpp"
	    ARGS="-c $base/etc/startup.conf"
	    ;;
	\?)
	    echo "Invalid option: -$OPTARG\n" >&2
	    echo "$USAGE" >&2
	    exit 1
	    ;;
    esac
done

shift $((OPTIND-1))

if test -z "$1"; then
    $APP $ARGS &
elif test "$1" = "debug"; then
    shift
    gdb -ex 'set print pretty on' -ex 'run' --args $APP $ARGS $@
else
    echo "$USAGE" >&2
    exit 1
fi

while : 
do
   echo "waiting for vpp-upf service"
   RES=$(bin/vppctl sh upf specification release | awk {'print $3'})
   echo $RES
   if [[ $RES =~ 16 ]]; then
       echo "vpp-upf service is running now"
       break
   fi
   sleep 3
done

if [[ ${REGISTER_NRF} == "yes" ]];then
    NRF_APP="$base/bin/nrf_client.py"
    NRF_ARGS=" --nrf_ip="$NRF_IP_ADDR" --nrf_port="$NRF_PORT" --http_version="$HTTP_VERSION
    python $NRF_APP $NRF_ARGS
fi
sleep infinity

