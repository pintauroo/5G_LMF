#/*
# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this file
# * except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *       http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *       contact@openairinterface.org
# */
#---------------------------------------------------------------------


import time
import sys 
import logging
import uuid
import pycurl
import argparse
import copy
import os
import subprocess
import yaml

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(asctime)s] %(name)s:%(levelname)s: %(message)s"
)

def main() -> None:
    """Provide command-line options to deploy a simple sanity check"""
    args = _parse_args()
    compose_file = './ci-scripts/docker-compose/docker-compose.yaml'
    deploy(compose_file, args.nssf_tag, 1)
    value_initialize(args.ip, args.port, args.apiv, args.http_version)

    undeploy(compose_file)


def deploy(compose_file, tag, ct):
    """Deploy the containers using the docker-compose template

    Returns:
        None
    """
    if not os.path.isdir('./archives'):
        run_cmd('mkdir -p ./archives')

    run_cmd('echo "OAI_NSSF_TAG: oai-nssf:' + tag + '" > ./archives/oai_nssf_image_info.log')
    run_cmd('docker image inspect --format="Size = {{.Size}} bytes" oai-nssf:' + tag + ' >> ./archives/oai_nssf_image_info.log')
    run_cmd('docker image inspect --format="Date = {{.Created}}" oai-nssf:' + tag + ' >> ./archives/oai_nssf_image_info.log')
    run_cmd(f'sed -e "s#NSSF_TAG#{tag}#g" ci-scripts/docker-compose/docker-compose.tplt > {compose_file}')

    logging.info ('Deploying NSSF Container')
    cmd = f'docker-compose -f {compose_file} up -d | tee ./archives/deployment.log'
    res = run_cmd(cmd)
    if res is None:
        exit(f'Incorrect/Unsupported executing command {cmd}')
    logging.info('Deployed NSSF Container, checking the health status of the container... takes few secs')
    for x in range(20):
        cmd = f'docker-compose -f {compose_file} ps -a'
        res = run_cmd(cmd)
        if res is None:
            undeploy(compose_file)
            exit(f'Incorrect/Unsupported executing command "{cmd}"')
        time.sleep(2)
        cnt = res.count('(healthy)')
        if cnt == ct:
            logging.info('NSSF Container is healthy')
            run_cmd('echo "SANITY-CHECK-DEPLOYMENT: OK" > ./archives/deployment_status.log')
            break
    if cnt != ct:
        logging.error('NSSF Container is un-healthy')
        run_cmd('echo "SANITY-CHECK-DEPLOYMENT: KO" > ./archives/deployment_status.log')
        undeploy(compose_file)
        exit(-1)

def run_cmd(cmd):
    """terminal command execution

    Returns:
        string: terminal command execution output
    """
    result = None
    logging.debug(cmd)
    try:
        res = subprocess.run(cmd,
                        shell=True, check=True,
                        stdout=subprocess.PIPE, 
                        universal_newlines=True)
        result = res.stdout.strip()
    except:
        pass
    return result
        
def value_initialize(ip, port, apiv, httpv):
    """passing the required input parameters

    Returns:
        None
    """
    # Default/correct values to test
    defaultset = {'type': 'AMF', 'sst': '222', 'roamingIndication': 'NON_ROAMING', 'mcc': '208', 'mnc': '95', 'home_plmnid': ''}
    
    # Input parameters values to test
    input_parm = [{'type': 'AMF', 'sst': '222', 'roamingIndication': 'NON_ROAMING', 'mcc': '208', 'mnc': '95', 'home_plmnid': '', 'name': 'Valid Slice Info', 'code': 200, 'msg': 'Authorized Network Slice Info Returned'},
            {'type': 'UPF', 'name': 'Invalid NF Type', 'code': 400, 'msg': 'Invalid NF_Type (Valid NF_Type is AMF, NSSF, NWDAP, SMF)'}, 
            {'sst': '223', 'name': 'Invalid Snssai from SliceInfoRequestForPduSession', 'code': 400, 'msg': 'S-NSSAI from SliceInfoForPDUSession is not authorised'}, 
            {'roamingIndication': 'LOCAL_BREAKOUT', 'name': 'Invalid Roaming Indication', 'code': 503, 'msg': 'Roming/Local Breakout is not Supported yet'}, 
            {'mcc': '207', 'mnc': '98', 'name': 'Invalid TAI', 'code': 400, 'msg': 'TAI is not authorised'}, 
            {'home_plmnid': '&home-plmn-id={"mcc":"208","mnc":"95"}', 'name': 'Invalid Home PLMN Id', 'code': 503, 'msg': 'Roming is not Supported yet. HomePlmnId can not be validated'}
        ]
    final_result = {}
    final_result[httpv] = []
    result = {}
    for subset in input_parm:
        temp = copy.deepcopy(defaultset)
        for key in subset:
            if (key is not 'code') and (key is not 'msg'):
                temp[key] = subset[key]
        response_code = url_gen(ip, port, apiv, httpv, temp['type'], temp['sst'], temp['roamingIndication'], temp['mcc'], temp['mnc'], temp['home_plmnid'])
        if response_code == subset['code']:
            result['status'] = 'OK'
            result['test_description'] = str(subset['code']) + ': ' + subset['msg']
            result['name'] = subset['name']
        else:
            result['status'] = 'KO'
            result['test_description'] = str(subset['code']) + ': ' + subset['msg']
            result['name'] = subset['name']
        result1 = result.copy()
        final_result[httpv].append(result1)
    yaml_file = open("archives/sanity_check_result.yaml", "w")
    yaml.dump(final_result, yaml_file)
    yaml_file.close()

def url_gen(ip, port, apiv, httpv, typ, sst, roamingIndication, mcc, mnc, home_plmnid):
    """Preparing the URL for curl http request

    Returns:
        int: http response code
    """
    uid = uuid.uuid4()   
    # Test for invalid NF-Type   # Response -> Invalid NF_Type (Valid NF_Type is AMF, NSSF, NWDAP, SMF)
    url = 'http://' + ip + ':' + port + '/nnssf-nsselection/' + apiv + '/network-slice-information?nf-type=' + typ + '&nf-id=' + str(uid) + '&slice-info-request-for-pdu-session={"sNssai":{"sst":' + sst + '},"roamingIndication":"' + roamingIndication + '"}' + home_plmnid + '&tai={"plmnId":{"mcc":"' + mcc + '","mnc":"' + mnc + '"},"tac":"33456","nid":"123456"}&supported-features=abc123'
    response_code = curl_call(url, httpv)
    return response_code

def curl_call(url, httpv):
    """Calling the Http request with the Container 

    Returns:
        int: http response code
    """
    curl = pycurl.Curl()
    headers = ["Content-Type:application/json"]
    curl.setopt(curl.URL, url)
    curl.setopt(curl.HTTPHEADER, headers)
    curl.setopt(curl.CUSTOMREQUEST, 'GET')

    if(str(httpv) == '2'):
        curl.setopt(curl.HTTP_VERSION, pycurl.CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE)
    curl.perform()
    return curl.getinfo(pycurl.RESPONSE_CODE)

def undeploy(compose_file):
    """UnDeploy the docker container

    Returns:
        None
    """

    run_cmd('docker logs cicd-oai-nssf > ./archives/oai_nssf.log')
    logging.debug('UnDeploying NSSF Container')
    cmd = f'docker-compose -f {compose_file} down'
    res = run_cmd(cmd)
    if res is None:
        exit(f'Incorrect/Unsupported executing command {cmd}')
    logging.info('NSSF Container UnDeployed')

def _parse_args() -> argparse.Namespace:
    """Parse the command line args

    Returns:
        argparse.Namespace: the created parser
    """
    example_text = '''example:
        1. Test with HTTP1
            python3 sanityCheckDeployment.py --ip 192.168.70.134 --port 80 --http_version 1 --apiv v1 --nssf_tag develop
        2. Test with HTTP2
            python3 sanityCheckDeployment.py --ip 192.168.70.134 --port 8080 --http_version 2 --apiv v1 --nssf_tag develop
        '''
    parser = argparse.ArgumentParser(description='OAI NSSF Sanity Check Deployment for CI')

    # NSSF ip address
    parser.add_argument(
        '--ip',
        metavar='[number]',
        action='store',
        required=False,
        type=str,
        default='192.168.70.134',
        help='nssf ip address',
    )
    # NSSF port number
    parser.add_argument(
        '--port',
        metavar='[number]',
        action='store',
        required=False,
        type=str,
        default='80',
        choices=['80', '8080'],
        help='NSSF port number',
    )
    # NSSF http version
    parser.add_argument(
        '--http_version',
        metavar='[number]',
        action='store',
        required=False,
        type=str,
        default='1',
        choices=['1', '2'],
        help='NSSF http version',
    )
    # NSSF api version
    parser.add_argument(
        '--apiv',
        action='store',
        default='v1',
        help='NSSF api version',
    )
    # NSSF tag to deploy
    parser.add_argument(
        '--nssf_tag',
        action='store',
        default='develop',
        help='NSSF tag to Deploy',
    )

    return parser.parse_args()


if __name__ == '__main__':
    main()