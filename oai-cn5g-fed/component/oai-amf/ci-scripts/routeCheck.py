#/*
# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this file
# * except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *   http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *   contact@openairinterface.org
# */
#---------------------------------------------------------------------

import os
import re
import sys
import subprocess
import yaml

class IpRouteCheck():
	def __init__(self):
		self.mode = ''
		self.userName = ''
		self.hostName = ''
		self.subnet = ''
		self.gatwayIp = ''
		self.interfaceName = ''

	def getSubnet(self):
		cmd = "egrep 'subnet' ci-scripts/dsTesterDockerCompose/docker-compose.yml"
		ret = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding='utf-8')
		if ret.returncode == 0:
			if ret.stdout is not None:
				result = re.search("(?:[0-9]{1,3}[.]){3}[0-9]{1,3}/[0-9]{1,2}", ret.stdout.strip())
				if result is not None:
					self.subnet = result.group(0)
					#print("found subnet:", self.subnet)
			else:
				print("subnet not found in docker compose")
				sys.exit(-1)
		else:
			print("docker-compose file not found")
			sys.exit(-1)

	def getGatwayIp(self):
		cmd = "ifconfig | grep 192.168.18"
		ret = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding='utf-8')
		if ret.returncode == 0:
			result = re.search("inet (?:[0-9]{1,3}[.]){3}[0-9]{1,3}", ret.stdout.strip())
			self.gatwayIp = result.group(0)
			#print("Gatway IP Address:", self.gatwayIp)
		else:
			print("No Gatway IP address starts with 196.168.18")
			sys.exit(-1)	

	def routeCheck(self):
		# Get the interface name
		cmd = "ip route | grep 192.168.18.0/24 | awk {'print $3'}"
		ret = subprocess.run(f'ssh {self.userName}@{self.hostName} {cmd} || true', shell=True, stdout=subprocess.PIPE, encoding='utf-8')
		#print('interface name:', ret.stdout.strip())
		self.interfaceName = ret.stdout.strip()
		# Check whether or not ip route exist 
		cmd = f'ip route | grep -c "{self.subnet}"'
		ret = subprocess.run(f'ssh {self.userName}@{self.hostName} {cmd} || true', shell=True, stdout=subprocess.PIPE, encoding='utf-8')
		if ret.stdout is not None:
			if ret.stdout.strip() == '1':
				#print('Route exits')
				if IPRC.mode == 'Delete':
					IPRC.routeDel()
				else:
					sys.exit(0)
			else:
				#print("Route not found")
				if IPRC.mode == 'Add':
					IPRC.routeAdd()
				else:
					sys.exit(0)

	def routeAdd(self):
		# Add the route
		cmd = f"sudo ip route add {self.subnet} via {self.gatwayIp} dev {self.interfaceName}"
		ret = subprocess.run(f'ssh {self.userName}@{self.hostName} {cmd} || true', shell=True, stdout=subprocess.PIPE, encoding='utf-8')
		print("Added ip route")

	def routeDel(self):
		# Delete the route
		cmd = f"sudo ip route del {self.subnet} via {self.gatwayIp} dev {self.interfaceName}"
		ret = subprocess.run(f'ssh {self.userName}@{self.hostName} {cmd} || true', shell=True, stdout=subprocess.PIPE, encoding='utf-8')
		print("Deleted ip route")

def Usage():
	print('----------------------------------------------------------------------------------------------------------------------')
	print('routecheck.py')
	print('   Add and Delete the ip route on the Server.')
	print('----------------------------------------------------------------------------------------------------------------------')
	print('Usage: python3 routecheck.py [options]')
	print('  --help  Show this help.')
	print('---------------------------------------------------------------------------------------------- Mandatory Options -----')
	print('  --mode=[Add/Delete]')
	print('  --userName=[server userName where to add/delete]')
	print('  --hostName=[server hostName where to add/delete]')
	print('------------------------------------------------------------------------------------------------- Actions Syntax -----')
	print('python3 routeCheck.py --mode=Add [Mandatory Options]')
	print('python3 routeCheck.py --mode=Delete [Mandatory Options]')	


#--------------------------------------------------------------------------------------------------------
#
# Start of main
#
#--------------------------------------------------------------------------------------------------------

argvs = sys.argv

IPRC = IpRouteCheck()

while len(argvs) > 1:
	myArgv = argvs.pop(1)
	if re.match('^\-\-help$', myArgv, re.IGNORECASE):
		Usage()
		sys.exit(0)
	elif re.match('^\-\-mode=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-mode=(.+)$', myArgv, re.IGNORECASE)
		IPRC.mode = matchReg.group(1)
	elif re.match('^\-\-userName=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-userName=(.+)$', myArgv, re.IGNORECASE)
		IPRC.userName = matchReg.group(1)
	elif re.match('^\-\-hostName=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-hostName=(.+)$', myArgv, re.IGNORECASE)
		IPRC.hostName = matchReg.group(1)
	else:
		sys.exit('Invalid Parameter: ' + myArgv)

if IPRC.mode == '' or IPRC.userName == '' or IPRC.hostName == '':
	sys.exit('Missing Parameter in job description')

IPRC.getSubnet()
IPRC.getGatwayIp()
IPRC.routeCheck()
