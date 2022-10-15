"""
Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
contributor license agreements.  See the NOTICE file distributed with
this work for additional information regarding copyright ownership.
The OpenAirInterface Software Alliance licenses this file to You under
the OAI Public License, Version 1.1  (the "License"); you may not use this file
except in compliance with the License.
You may obtain a copy of the License at

      http://www.openairinterface.org/?page_id=698

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
------------------------------------------------------------------------------
For more information about the OpenAirInterface (OAI) Software Alliance:
      contact@openairinterface.org
"""

import argparse
import logging
import os
import re
import shutil
import subprocess
import sys
import time
from pcap_check import *

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(asctime)s] %(name)s:%(levelname)s: %(message)s"
)

def main() -> None:
    """Provide command-line options to deploy a simple sanity check"""
    args = _parse_args()

    if args.mode == 'Deploy':
        deployObject(args.service, args.upf_tag, args.docker_compose)

    if args.mode == 'Check':
        checkDeployment(args.service, args.docker_compose)

    if args.mode == 'UnDeploy':
        undeployObject(args.service, args.docker_compose)

def _parse_args() -> argparse.Namespace:
    """Parse the command line args

    Returns:
        argparse.Namespace: the created parser
    """
    parser = argparse.ArgumentParser(description='OAI UPF-VPP Sanity Check Deployment for CI')

    # Service to deploy
    parser.add_argument(
        '--service',
        action='store',
        required=True,
        help='Service to Deploy',
    )
    # UPF tag to deploy
    parser.add_argument(
        '--upf_tag',
        action='store',
        default='develop',
        help='UPF tag to Deploy',
    )
    # Docker-compose file to use
    parser.add_argument(
        '--docker_compose',
        action='store',
        default='docker-compose.yml',
        help='Docker-compose file to use (default: docker-compose.yml)',
    )
    # Mode
    parser.add_argument(
        '--mode',
        action='store',
        required=True,
        choices=['Deploy', 'Check', 'UnDeploy'],
        help='Mode',
    )
    return parser.parse_args()

def deployObject(service, tag, compose_file):
    logging.info ('Deploying ' + service)

    if service == 'gnbsim-vpp':
        logging.info ('Not available for the moment')
        time.sleep(10)
        return

    if not os.path.isdir('./archives'):
        subprocess_run_w_echo('mkdir -p ./archives')

    os.chdir('docker-compose')
    if not os.path.exists('./ci-' + compose_file):
        logging.debug('cp ./' + compose_file + ' ./ci-' + compose_file)
        shutil.copyfile('./' + compose_file, './ci-' + compose_file)
        subprocess_run_w_echo('sed -i -e \'s@container_name: "@container_name: "ci-@\' ./ci-' + compose_file)
    expect_healthy = 0
    nb_healthy = 0
    cnt = 0
    # mysql does not require image tag update
    # Also starting a capture on the public docker network
    if service == 'mysql':
        subprocess_run_w_echo('echo "MYSQL_TAG: mysql:5.7" > ../archives/mysql_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Size = {{.Size}} bytes" mysql:5.7 >> ../archives/mysql_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Date = {{.Created}}" mysql:5.7 >> ../archives/mysql_image_info.log')
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' up -d ' + service)
        expect_healthy = 1
        subprocess_run_w_echo('sudo nohup tshark -i cn5g-public -f "not arp and not port 53 and not host archive.ubuntu.com and not host security.ubuntu.com" -w /tmp/ci-upf-vpp-sanity.pcap > /dev/null 2>&1 &')
    # We might need tag manipulations
    if service == 'oai-nrf':
        subprocess_run_w_echo('echo "OAI_NRF_TAG: oai-nrf:develop" > ../archives/oai_nrf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Size = {{.Size}} bytes" oai-nrf:develop >> ../archives/oai_nrf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Date = {{.Created}}" oai-nrf:develop >> ../archives/oai_nrf_image_info.log')
        subprocess_run_w_echo('sed -i -e "s#oai-nrf:latest#oai-nrf:develop#" ./ci-' + compose_file)
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' up -d ' + service)
        expect_healthy = 2
    if service == 'vpp-upf':
        subprocess_run_w_echo('echo "OAI_UPF_VPP_TAG: oai-upf-vpp:' + tag + '" > ../archives/oai_upf_vpp_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Size = {{.Size}} bytes" oai-upf-vpp:' + tag + ' >> ../archives/oai_upf_vpp_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Date = {{.Created}}" oai-upf-vpp:' + tag + ' >> ../archives/oai_upf_vpp_image_info.log')
        subprocess_run_w_echo('sed -i -e "s#oai-upf-vpp:latest#oai-upf-vpp:' + tag + '#" ./ci-' + compose_file)
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' up -d ' + service)
        expect_healthy = 3
    if service == 'oai-smf':
        subprocess_run_w_echo('echo "OAI_AMF_TAG: oai-amf:develop" > ../archives/oai_amf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Size = {{.Size}} bytes" oai-amf:develop >> ../archives/oai_amf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Date = {{.Created}}" oai-amf:develop >> ../archives/oai_amf_image_info.log')
        subprocess_run_w_echo('echo "OAI_SMF_TAG: oai-smf:develop" > ../archives/oai_smf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Size = {{.Size}} bytes" oai-smf:develop >> ../archives/oai_smf_image_info.log')
        subprocess_run_w_echo('docker image inspect --format="Date = {{.Created}}" oai-smf:develop >> ../archives/oai_smf_image_info.log')
        subprocess_run_w_echo('sed -i -e "s#oai-amf:latest#oai-amf:develop#" -e "s#oai-smf:latest#oai-smf:develop#" ./ci-' + compose_file)
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' up -d ' + service)
        expect_healthy = 5

    # Counting the number of healthy containers
    cmd = 'docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' ps -a'
    while (nb_healthy != expect_healthy) or (cnt > 50):
        time.sleep(5)
        cnt += 1
        res = subprocess.check_output(cmd, shell=True, universal_newlines=True)
        nb_healthy = 0
        for line in res.split('\n'):
            result = re.search('Up.*healthy', line)
            if result is not None:
                nb_healthy += 1
    if nb_healthy == expect_healthy:
        logging.info('OK')
        # Adding a tempo for VPP-UPF to be fully ready
        if service == 'vpp-upf':
            time.sleep(10)
    elif cnt > 50:
        logging.error('KO')
        sys.exit(-1)

def undeployObject(service, compose_file):
    logging.info ('Un-Deploying ' + service)

    if service == 'gnbsim-vpp':
        logging.info ('Not available for the moment')
        return

    os.chdir('docker-compose')
    if os.path.exists('./ci-' + compose_file):
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' down')
    else:
        subprocess_run_w_echo('docker-compose -p vpp-sanity -f ./' + compose_file + ' down')

    if os.path.exists('/tmp/ci-upf-vpp-sanity.pcap'):
        subprocess_run_w_echo('sudo rm -f /tmp/ci-upf-vpp-sanity.pcap')

def checkDeployment(service, compose_file):
    logging.info ('Performing Check')

    if not os.path.isdir('./archives'):
        subprocess_run_w_echo('mkdir -p ./archives')

    if os.path.exists('/tmp/ci-upf-vpp-sanity.pcap'):
        subprocess_run_w_echo('sudo chmod 666 /tmp/ci-upf-vpp-sanity.pcap')
        subprocess_run_w_echo('cp /tmp/ci-upf-vpp-sanity.pcap archives')

    upf_register = False
    pfcp_association = False
    if os.path.exists('./archives/ci-upf-vpp-sanity.pcap'):
        res1 = check_if_upf_registers_to_nrf('./archives/ci-upf-vpp-sanity.pcap')
        if res1['upf_nrf_reg_req'] and res1['upf_nrf_reg_res']:
            logging.debug('UPF did register at NRF')
            upf_register = True
        else:
            logging.error('UPF did NOT register at NRF')
        res2 = check_pcfp_association('./archives/ci-upf-vpp-sanity.pcap')
        if res2['pfcp_ass_req'] and res2['pfcp_ass_res']:
            logging.debug('PCFP association b/w SMF and UPF was successful')
            pfcp_association = True
        else:
            logging.error('PCFP association b/w SMF and UPF failed')

    os.chdir('docker-compose')
    if not os.path.exists('./ci-' + compose_file):
        return

    cmd = 'docker-compose -p vpp-sanity -f ./ci-' + compose_file + ' ps -a'
    res = subprocess.check_output(cmd, shell=True, universal_newlines=True)
    healthy_cnt = 0
    for line in res.split('\n'):
        result = re.search('Up.*healthy', line)
        if result is not None:
            subprocess_run_w_echo('docker logs ' + line.split(' ')[0] + ' > ../archives/' + line.split(' ')[0] + '.log 2>&1')
            healthy_cnt += 1

    if upf_register and (healthy_cnt == 5):
        subprocess_run_w_echo('echo "SANITY-CHECK-DEPLOYMENT: OK" > ../archives/deployment_status.log')
    else:
        subprocess_run_w_echo('echo "SANITY-CHECK-DEPLOYMENT: KO" > ../archives/deployment_status.log')

def subprocess_run_w_echo(cmd):
    logging.debug(cmd)
    subprocess.run(cmd, shell=True)

if __name__ == '__main__':
    main()
