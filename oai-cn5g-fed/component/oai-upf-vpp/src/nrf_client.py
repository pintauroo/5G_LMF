#!/usr/bin/env python
"""
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

/*! \file nrf_client.py
   \author  Rohan KHARADE
   \date 2018
   \email: rohan.kharade@openairinterface.org
*/
"""

import time, sys, json, logging, uuid, pycurl, argparse, atexit
from termcolor import colored

logging.basicConfig(format='%(asctime)s] %(filename)s: %(levelname)s '
                           '- %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')

class upf_profile(object):
        def __init__(self, nrf_ip = None, nrf_port = None, http_version = None):
            self.logger = logging.getLogger("upf")
            atexit.register(self.goodbye)  # register a message to print out when exit

            message = " NRF UPF client started "
            self.logger.info(message)
            #@@@@@@@@  Initialize arguments #@@@@@@@@ 
            self.nrf_ip         = nrf_ip
            self.nrf_port       = nrf_port 
            self.http_version   = http_version
            self.status_code    = 0
            self.uuid           = uuid.uuid4()
            self.url            = 'http://'+str(self.nrf_ip)+':'+str(self.nrf_port)+'/nnrf-nfm/v1/nf-instances/'+str(self.uuid) 
            self.curl           = pycurl.Curl()
            self.headers        = ["Content-Type:application/json"]
            self.dir_config     = '/openair-upf/'
            self.file_name      = self.dir_config+'etc/upf_profile.json'
            self.conf_file      = open(self.file_name,)
            self.upf_profile    = json.load(self.conf_file)
            #@@@@@@@@ Initialize upf profile #@@@@@@@@ 
            self.upf_profile['nfInstanceId'] = str(self.uuid)
            self.capacity       = self.upf_profile['capacity']
            self.fqdn           = self.upf_profile['fqdn']
            self.heartBeatTimer = self.upf_profile['heartBeatTimer']
            self.ipv4Addresses  = self.upf_profile['ipv4Addresses']
            self.nfInstanceId   = self.upf_profile['nfInstanceId']
            self.nfInstanceName = self.upf_profile['nfInstanceName']
            self.nfStatus       = self.upf_profile['nfStatus']
            self.nfType         = self.upf_profile['nfType']
            self.priority       = self.upf_profile['priority']
            self.sNssais        = self.upf_profile['sNssais']
            self.upfInfo        = self.upf_profile['upfInfo']

            message = " UPF profile is parsed "
            self.logger.info(message)

        def display_upf_profile(self):
            message = " Display UPF profile "
            self.logger.info(message)
            print(colored('[*] UPF Profile \n \t fqdn = '+self.fqdn+ \
            '\n \t capacity       = '+str(self.capacity)+ \
            '\n \t heartBeatTimer = '+str(self.heartBeatTimer)+ \
            '\n \t ipv4Addresses  = '+u", ".join(self.ipv4Addresses)+ \
            '\n \t nfInstanceId   = '+self.nfInstanceId+ \
            '\n \t nfInstanceName = '+self.nfInstanceName+ \
            '\n \t nfStatus       = '+self.nfStatus+ \
            '\n \t nfType         = '+self.nfType+ \
            '\n \t priority       = '+str(self.priority)+ \
            '\n \t sNssais        = '+json.dumps(self.sNssais)+ \
            '\n \t upfInfo        = '+json.dumps(self.upfInfo,indent=6) \
            ,'green'))

        def trigger_nf_registration(self):
                message = " Sending NF registration request (HTTP Version - "+str(self.http_version)+")"
                self.logger.info(message)
                self.curl.setopt(self.curl.URL, self.url)
                self.curl.setopt(self.curl.HTTPHEADER, self.headers)
                self.curl.setopt(self.curl.CUSTOMREQUEST, 'PUT')
                self.curl.setopt(self.curl.POSTFIELDS, json.dumps(self.upf_profile))
                if(str(self.http_version) == '2'):
                    self.curl.setopt(self.curl.HTTP_VERSION, pycurl.CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE)
                response=self.curl.perform()
                self.status_code=self.curl.getinfo(self.curl.RESPONSE_CODE)
                if self.status_code == 201 or self.status_code == 200:
                    message = " Succussfully registered at NRF !!"
                    self.logger.info(message)
                    self.trigger_nf_heartbeat()
                else:
                    print(colored('\n\n NF registration failed \n\n', 'red'))
        
        def trigger_nf_heartbeat(self):
                patch_data = [{"op": "replace","path": "/nfStatus","value": "REGISTERED"}]
                while True:
                    message = " Sending NF heartbeat requset (HTTP Version - "+str(self.http_version)+") !!"
                    self.logger.info(message)
                    time.sleep(5)
                    self.curl.setopt(self.curl.CUSTOMREQUEST, 'PATCH')
                    self.curl.setopt(self.curl.POSTFIELDS, json.dumps(patch_data))
                    response=self.curl.perform()
                    self.status_code=self.curl.getinfo(self.curl.RESPONSE_CODE)
                    if self.status_code == 204:
                        message = " Succussfully received NF heartbeat response !!"
                        self.logger.info(message)
                    else:
                        print(colored('\n\n NF heartbeat procedure failed \n\n', 'red'))

        def goodbye(self):
            print(colored('\n\n\n [*] You are now leaving OAI-NRF framework .....\n\n\n', 'yellow'))
            sys.exit(0)

if __name__ == '__main__':
        parser = argparse.ArgumentParser(
            description='Process commandline arguments and override configurations')
        parser.add_argument('--nrf_ip', metavar='[number]', action='store', type=str,
                            required=False, default='192.168.71.130',
                            help='set the nrf ip address. default = 192.168.71.130')
    
        parser.add_argument('--nrf_port', metavar='[number]', action='store', type=str,
                            required=False, default='8080',
                            help='set the nrf port. default = 8080')
                    
        parser.add_argument('--http_version', metavar='[number]', action='store', type=str,
                            required=False, default='2',
                            help='set the nrf ip address. default = 2')
        args = parser.parse_args()

        nrf_client = upf_profile(args.nrf_ip, args.nrf_port, args.http_version)
        nrf_client.display_upf_profile()
        nrf_client.trigger_nf_registration()

"""
* Usage of nrf client -->

        $ python nrf_client.py -h

        usage: nrf_client.py [-h] [--nrf_ip [number]] [--nrf_port [number]]
                            [--http_version [number]]

        Process commandline arguments and override configurations

        optional arguments:
        -h, --help            show this help message and exit
        
        --nrf_ip [number]       set the nrf ip address. default = 192.168.71.130
        --nrf_port [number]     set the nrf port. default = 8080
        --http_version [number] set the nrf ip address. default = 2

"""
