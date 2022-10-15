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

import os
import re
import socket
import struct
import sys
import subprocess
import pyshark

# IP addresses
NRF_IP_ADDRESS = '192.168.71.130'
SMF_IP_ADDRESS = '192.168.71.133'
UPF_IP_ADDRESS = '192.168.71.134'
UPF_N4_IP_ADDRESS = '192.168.71.202'

# PGCP Constants
PFCP_MSG_TYPE__SX_HEARTBEAT_REQUEST = '1'
PFCP_MSG_TYPE__SX_HEARTBEAT_RESPONSE = '2'
PFCP_MSG_TYPE__SX_ASSOCIATION_SETUP_REQUEST = '5'
PFCP_MSG_TYPE__SX_ASSOCIATION_SETUP_RESPONSE = '6'
PFCP_NODE_ID_TYPE__IPV4 = '0'
PFCP_NODE_ID_TYPE__FQDN = '2'
PFCP_CAUSE__SUCCESS = '1'

def check_if_upf_registers_to_nrf(pcap_file):
	res = {}
	res['upf_nrf_reg_req'] = False
	res['upf_nrf_reg_res'] = False
	res['upf_nrf_fqdn'] = ''
	try:
		cap = {}
		#cap = pyshark.FileCapture(pcap_file, keep_packets=True, display_filter="http2", use_json=True)
		cap = pyshark.FileCapture(pcap_file, keep_packets=True, display_filter="http2")
		cnt = 0
		for pkt in cap:
			if pkt is not None:
				cnt += 1
				if pkt.ip.src == UPF_IP_ADDRESS and pkt.ip.dst == NRF_IP_ADDRESS:
					result = re.search('headers_method', str(pkt.http2.field_names))
					if result is not None:
						if pkt.http2.headers_method == 'PUT':
							res['upf_nrf_reg_req'] = True
				if pkt.ip.src == NRF_IP_ADDRESS and pkt.ip.dst == UPF_IP_ADDRESS:
					status201 = False
					upf_fqdn = False
					result = re.search('header', str(pkt.http2.field_names))
					if result is not None:
						if pkt.http2.header == 'Header: :status: 201 Created':
							status201 = True
					result = re.search('json_value_string', str(pkt.http2.field_names))
					if result is not None:
						if pkt.http2.json_value_string == 'gw1.vppupf.node.5gcn.mnc95.mcc208.3gppnetwork.org':
							upf_fqdn = True
						res['upf_nrf_fqdn'] = pkt.http2.json_value_string
					if status201 and upf_fqdn:
						res['upf_nrf_reg_res'] = True
		cap.close()
	except Exception as e:
		print(e.message, e.args) 
		print('Could not open PCAP file')
	return res

def check_pcfp_association(pcap_file):
	res = {}
	res['pfcp_ass_req'] = False
	res['pfcp_ass_res'] = False
	res['pfcp_ass_ipv4'] = ''
	res['pfcp_ass_upf_fqdn'] = ''
	res['pfcp_ass_entreprise'] = ''
	res['pfcp_ass_build_str'] = ''
	try:
		cap = {}
		cap = pyshark.FileCapture(pcap_file, keep_packets=True, display_filter="pfcp")
		cnt = 0
		for pkt in cap:
			if pkt is not None:
				cnt += 1
				if pkt.ip.src == SMF_IP_ADDRESS and pkt.ip.dst == UPF_N4_IP_ADDRESS:
					if pkt.pfcp.msg_type == PFCP_MSG_TYPE__SX_ASSOCIATION_SETUP_REQUEST and pkt.pfcp.node_id_type == PFCP_NODE_ID_TYPE__IPV4:
						if pkt.pfcp.node_id_ipv4 == SMF_IP_ADDRESS:
							res['pfcp_ass_req'] = True
							res['pfcp_ass_ipv4'] = pkt.pfcp.node_id_ipv4
						else:
							res['pfcp_ass_ipv4'] = pkt.pfcp.node_id_ipv4
				if pkt.ip.src == UPF_N4_IP_ADDRESS and pkt.ip.dst == SMF_IP_ADDRESS:
					if pkt.pfcp.msg_type == PFCP_MSG_TYPE__SX_ASSOCIATION_SETUP_RESPONSE and pkt.pfcp.node_id_type == PFCP_NODE_ID_TYPE__FQDN:
						if pkt.pfcp.cause == PFCP_CAUSE__SUCCESS:
							res['pfcp_ass_res'] = True
						res['pfcp_ass_upf_fqdn'] = pkt.pfcp.node_id_fqdn
						if pkt.pfcp.enterprise_id == '18681':
							res['pfcp_ass_entreprise'] = 'Travelping GmbH'
						res['pfcp_ass_build_str'] = pkt.pfcp.travelping_build_id_str
		cap.close()
	except Exception as e:
		print(e.message, e.args) 
		print('Could not open PCAP file')
	return res
