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

import glob
import os
import re
import sys
import subprocess
from pcap_check import *

class HtmlReport():
	def __init__(self):
		self.job_name = ''
		self.job_id = ''
		self.job_url = ''
		self.job_start_time = 'TEMPLATE_TIME'
		self.git_url = ''
		self.git_src_branch = ''
		self.git_src_commit = ''
		self.git_src_commit_msg = None
		self.git_pull_request = False
		self.git_target_branch = ''
		self.git_target_commit = ''
		self.nb_warnings = 0
		self.warning_rows = ''

	def generate(self):
		cwd = os.getcwd()
		self.file = open(cwd + '/test_results_oai_upf.html', 'w')
		self.generateHeader()

		self.buildSummaryHeader()
		self.initialGitSetup()
		self.cloningAnPatching()
		self.installLibsPackagesRow()
		self.buildCompileRows()
		self.copyToTargetImage()
		self.copyConfToolsToTargetImage()
		self.imageSizeRow()
		self.buildSummaryFooter()

		self.testSummaryHeader()
		self.testSummaryFooter()

		self.generateFooter()
		self.file.close()

	def generateHeader(self):
		# HTML Header
		self.file.write('<!DOCTYPE html>\n')
		self.file.write('<html class="no-js" lang="en-US">\n')
		self.file.write('<head>\n')
		self.file.write('  <meta name="viewport" content="width=device-width, initial-scale=1">\n')
		self.file.write('  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">\n')
		self.file.write('  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>\n')
		self.file.write('  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>\n')
		self.file.write('  <title>OAI Core Network Test Results for ' + self.job_name + ' job build #' + self.job_id + '</title>\n')
		self.file.write('</head>\n')
		self.file.write('<body><div class="container">\n')
		self.file.write('  <table width = "100%" style="border-collapse: collapse; border: none;">\n')
		self.file.write('	<tr style="border-collapse: collapse; border: none;">\n')
		self.file.write('	  <td style="border-collapse: collapse; border: none;">\n')
		self.file.write('		<a href="http://www.openairinterface.org/">\n')
		self.file.write('		   <img src="http://www.openairinterface.org/wp-content/uploads/2016/03/cropped-oai_final_logo2.png" alt="" border="none" height=50 width=150>\n')
		self.file.write('		   </img>\n')
		self.file.write('		</a>\n')
		self.file.write('	  </td>\n')
		self.file.write('	  <td style="border-collapse: collapse; border: none; vertical-align: center;">\n')
		self.file.write('		<b><font size = "6">Job Summary -- Job: ' + self.job_name + ' -- Build-ID: <a href="' + self.job_url + '">' + self.job_id + '</a></font></b>\n')
		self.file.write('	  </td>\n')
		self.file.write('	</tr>\n')
		self.file.write('  </table>\n')
		self.file.write('  <br>\n')

		# Build Info Summary
		buildSummary = ''
		buildSummary += '  <table class="table-bordered" width = "80%" align = "center" border = "1">\n'
		buildSummary += '	 <tr>\n'
		buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-time"></span> Build Start Time</td>\n'
		#date_formatted = re.sub('\..*', '', self.created)
		buildSummary += '	   <td>' + self.job_start_time + '</td>\n'
		buildSummary += '	 </tr>\n'
		buildSummary += '	 <tr>\n'
		buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-wrench"></span> Build Trigger</td>\n'
		if self.git_pull_request:
			buildSummary += '	   <td>Pull Request</td>\n'
		else:
			buildSummary += '	   <td>Push Event</td>\n'
		buildSummary += '	 </tr>\n'
		buildSummary += '	 <tr>\n'
		buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-cloud-upload"></span> GIT Repository</td>\n'
		buildSummary += '	   <td><a href="' + self.git_url + '">' + self.git_url + '</a></td>\n'
		buildSummary += '	 </tr>\n'
		if self.git_pull_request:
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-log-out"></span> Source Branch</td>\n'
			buildSummary += '	   <td><a href="TEMPLATE_MERGE_REQUEST_LINK">TEMPLATE_MERGE_REQUEST_LINK</a></td>\n'
			buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-header"></span> Merge Request Title</td>\n'
			buildSummary += '	   <td>TEMPLATE_MERGE_REQUEST_TEMPLATE</td>\n'
			buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-log-out"></span> Source Branch</td>\n'
			buildSummary += '	   <td>' + self.git_src_branch + '</td>\n'
			buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-tag"></span> Source Commit ID</td>\n'
			buildSummary += '	   <td>' + self.git_src_commit + '</td>\n'
			buildSummary += '	 </tr>\n'
			if (self.git_src_commit_msg is not None):
				buildSummary += '	 <tr>\n'
				buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-comment"></span> Source Commit Message</td>\n'
				buildSummary += '	   <td>' + self.git_src_commit_msg + '</td>\n'
				buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-log-in"></span> Target Branch</td>\n'
			buildSummary += '	   <td>' + self.git_target_branch + '</td>\n'
			buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-tag"></span> Target Commit ID</td>\n'
			buildSummary += '	   <td>' + self.git_target_commit + '</td>\n'
			buildSummary += '	 </tr>\n'
		else:
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-tree-deciduous"></span> Branch</td>\n'
			buildSummary += '	   <td>' + self.git_src_branch + '</td>\n'
			buildSummary += '	 </tr>\n'
			buildSummary += '	 <tr>\n'
			buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-tag"></span> Commit ID</td>\n'
			buildSummary += '	   <td>' + self.git_src_commit + '</td>\n'
			buildSummary += '	 </tr>\n'
			if (self.git_src_commit_msg is not None):
				buildSummary += '	 <tr>\n'
				buildSummary += '	   <td bgcolor="lightcyan" > <span class="glyphicon glyphicon-comment"></span> Commit Message</td>\n'
				buildSummary += '	   <td>' + self.git_src_commit_msg + '</td>\n'
				buildSummary += '	 </tr>\n'
		buildSummary += '  </table>\n'
		buildSummary += '  <br>\n'
		self.file.write(buildSummary)

		cwd = os.getcwd()
		for reportFile in glob.glob('./*results_oai_cn5g.html'):
			newEpcReport = open(cwd + '/' + str(reportFile) + '.new', 'w')
			buildSummaryDone = True
			with open(cwd + '/' + str(reportFile), 'r') as originalEpcReport:
				for line in originalEpcReport:
					result = re.search('Deployment Summary', line)
					if (result is not None) and buildSummaryDone:
						newEpcReport.write(buildSummary)
						buildSummaryDone = False
					newEpcReport.write(line)
				originalEpcReport.close()
			newEpcReport.close()
			os.rename(cwd + '/' + str(reportFile) + '.new', cwd + '/' + str(reportFile))

	def generateFooter(self):
		self.file.write('  <div class="well well-lg">End of Build Report -- Copyright <span class="glyphicon glyphicon-copyright-mark"></span> 2020 <a href="http://www.openairinterface.org/">OpenAirInterface</a>. All Rights Reserved.</div>\n')
		self.file.write('</div></body>\n')
		self.file.write('</html>\n')

	def buildSummaryHeader(self):
		self.file.write('  <h2>Docker / Podman Image Build Summary</h2>\n')
		self.file.write('  <table class="table-bordered" width = "100%" align = "center" border = "1">\n')
		self.file.write('	  <tr bgcolor="#33CCFF" >\n')
		self.file.write('		<th>Stage Name</th>\n')
		self.file.write('		<th>Image Kind</th>\n')
		self.file.write('		<th>OAI UPF-VPP <font color="Gold">Ubuntu18</font> Image</th>\n')
		self.file.write('		<th>OAI UPF-VPP <font color="Gold">RHEL7</font> Image</th>\n')
		self.file.write('	  </tr>\n')

	def buildSummaryFooter(self):
		self.file.write('  </table>\n')
		self.file.write('  <br>\n')
		if self.nb_warnings > 0:
			self.file.write('  <h3>Compilation Warnings Details</h3>\n')
			self.file.write('  <button data-toggle="collapse" data-target="#oai-compilation-details">Details for Compilation Errors and Warnings </button>\n')
			self.file.write('  <div id="oai-compilation-details" class="collapse">\n')
			self.file.write('  <table class="table-bordered">\n')
			self.file.write('    <tr bgcolor = "#33CCFF" >\n')
			self.file.write('      <th>File</th>\n')
			self.file.write('      <th>Line Number</th>\n')
			self.file.write('      <th>Status</th>\n')
			self.file.write('      <th>Message</th>\n')
			self.file.write(self.warning_rows)
			self.file.write('    </tr>\n')
			self.file.write('  </table>\n')
			self.file.write('  </div>\n')

	def initialGitSetup(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >Initial Git Setup</td>\n')
		self.analyze_docker_build_git_part('UPF-VPP')
		self.file.write('	 </tr>\n')

	def analyze_docker_build_git_part(self, nfType):
		if nfType != 'UPF-VPP':
			self.file.write('      <td>N/A</td>\n')
			self.file.write('      <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		self.file.write('      <td>Builder Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				status = False
				section_start_pattern = 'git config --global http'
				section_end_pattern = 'WORKDIR /vpp-upf'
				section_status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
							status = True
					logfile.close()

				if status:
					cell_msg = '      <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
					cell_msg += ' -- All Git Operations went successfully</b></pre></td>\n'
				else:
					cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO::\n'
					cell_msg += ' -- Some Git Operations went WRONG</b></pre></td>\n'
			else:
				cell_msg = '      <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def cloningAnPatching(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >Cloning and Patching VPP</td>\n')
		self.analyze_docker_clone_patch('UPF-VPP')
		self.file.write('	 </tr>\n')

	def analyze_docker_clone_patch(self, nfType):
		if nfType != 'UPF-VPP':
			self.file.write('      <td>N/A</td>\n')
			self.file.write('      <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		self.file.write('      <td>Builder Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				status = False
				section_start_pattern = 'git clone -b stable/2101 https://github.com/fdio/vpp.git'
				section_end_pattern = 'RUN make install-dep build-release -C vpp'
				section_status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
							status = True
					logfile.close()

				if status:
					cell_msg = '      <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
					cell_msg += ' -- All Git Operations went successfully</b></pre></td>\n'
				else:
					cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO::\n'
					cell_msg += ' -- Some Git Operations went WRONG</b></pre></td>\n'
			else:
				cell_msg = '      <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)


	def installLibsPackagesRow(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >SW libs and packages Installation</td>\n')
		self.analyze_install_log('UPF-VPP')
		self.file.write('	 </tr>\n')

	def analyze_install_log(self, nfType):
		if nfType != 'UPF-VPP':
			self.file.write('      <td>N/A</td>\n')
			self.file.write('      <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		self.file.write('      <td>Builder Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				status = False
				section_start_pattern = 'RUN make install-dep build-release -C vpp'
				if variant == 'docker':
					section_end_pattern = 'FROM ubuntu:bionic as oai-upf-vpp'
				else:
					section_end_pattern = 'FROM registry.access.redhat.com/ubi7/ubi:latest AS oai-upf-vpp'
				section_status = False
				package_install = False
				dpdk_build_start = False
				dpdk_build_status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
						if section_status:
							result = re.search('Arch for platform \'vpp\' is native', line)
							if result is not None:
								package_install = True
								dpdk_build_start = True
							result = re.search('Downloading https://github.com/vpp-quic/quicly/releases/download', line)
							if result is not None:
								dpdk_build_status = True
								status = True
					logfile.close()
				if status:
					cell_msg = '	  <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
				else:
					cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO:\n'
				cell_msg += ' -- make install-dep build-release -C vpp\n'
				if package_install:
					cell_msg += '   ** Packages Installation: OK\n'
				else:
					cell_msg += '   ** Packages Installation: KO\n'
				if dpdk_build_status:
					cell_msg += '   ** dpdk Installation: OK\n'
				else:
					cell_msg += '   ** dpdk Installation: KO\n'
				cell_msg += '</b></pre></td>\n'
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def buildCompileRows(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td rowspan=2 bgcolor="lightcyan" >cNF Compile / Build</td>\n')
		self.analyze_build_log('UPF-VPP', True)
		self.file.write('	 </tr>\n')
		self.file.write('	 <tr>\n')
		self.analyze_compile_log('UPF-VPP', True)
		self.file.write('	 </tr>\n')

	def analyze_build_log(self, nfType, imageKind):
		if nfType != 'UPF-VPP':
			if imageKind:
				self.file.write('      <td>N/A</td>\n')
			self.file.write('	   <td colspan="2">Wrong NF Type for this Report</td>\n')
			return
		if imageKind:
			self.file.write('      <td>Builder Image</td>\n')
		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				status = False
				if nfType == 'UPF-VPP':
					section_start_pattern = 'Downloading https://github.com/vpp-quic/quicly/releases/download'
					section_end_pattern = '[aA][sS] oai-upf-vpp$'
					pass_pattern = 'Install configuration: "release"'
				section_status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
						if section_status:
							result = re.search(pass_pattern, line)
							if result is not None:
								status = True
					logfile.close()
				if status:
					cell_msg = '	  <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
				else:
					cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO:\n'
				cell_msg += ' -- make install-dep build-release -C vpp\n'
				if status:
					cell_msg += '   ** vpp Build: OK\n'
				else:
					cell_msg += '   ** vpp Build: KO\n'
				cell_msg += '</b></pre></td>\n'
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def analyze_compile_log(self, nfType, imageKind):
		if nfType != 'UPF-VPP':
			if imageKind:
				self.file.write('      <td>N/A</td>\n')
			self.file.write('	   <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		if imageKind:
			self.file.write('      <td>Builder Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			nb_errors = 0
			nb_warnings = 0
			if os.path.isfile(cwd + '/archives/' + logFileName):
				if nfType == 'UPF-VPP':
					section_start_pattern = 'Building vpp in /vpp-upf/vpp/build-root/build-vpp-native/vpp'
					section_end_pattern = '[aA][sS] oai-upf-vpp$'
				section_status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
						if section_status:
							result = re.search('error:', line)
							if result is not None:
								nb_errors += 1
							result = re.search('warning:', line)
							if result is not None:
								nb_warnings += 1
								if nfType == 'UPF-VPP':
									correctLine = re.sub("^.*/vpp-upf/vpp/","/vpp-upf/vpp/",line.strip())
									wordsList = correctLine.split(None,2)
									filename = re.sub(":[0-9]*:[0-9]*:","", wordsList[0])
									linenumber = re.sub(filename + ':',"", wordsList[0])
									linenumber = re.sub(':[0-9]*:',"", linenumber)
									error_warning_status = re.sub(':',"", wordsList[1])
									error_warning_msg = re.sub('^.*' + error_warning_status + ':', '', correctLine)
									self.warning_rows += '<tr><td>' + filename + '</td><td>' + linenumber + '</td><td>' + error_warning_status + '</td><td>' + error_warning_msg + '</td></tr>\n'
					logfile.close()
				if nb_warnings == 0 and nb_errors == 0:
					cell_msg = '	   <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
				elif nb_warnings < 20 and nb_errors == 0:
					cell_msg = '	   <td bgcolor="Orange"><pre style="border:none; background-color:Orange"><b>'
				else:
					cell_msg = '	   <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				if nb_errors > 0:
					cell_msg += str(nb_errors) + ' errors found in compile log\n'
				cell_msg += str(nb_warnings) + ' warnings found in compile log</b></pre></td>\n'
				if nfType == 'UPF-VPP':
					self.nb_warnings = nb_warnings
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def copyToTargetImage(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >SW libs Installation / Copy from Builder</td>\n')
		self.analyze_copy_log('UPF-VPP')
		self.file.write('	 </tr>\n')

	def analyze_copy_log(self, nfType):
		if nfType != 'UPF-VPP':
			self.file.write('      <td>N/A</td>\n')
			self.file.write('	   <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		self.file.write('      <td>Target Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				section_start_pattern = '[aA][sS] oai-upf-vpp$'
				section_end_pattern = 'WORKDIR /openair-upf/etc'
				section_status = False
				status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
							status = True
					logfile.close()
				if status:
					cell_msg = '	   <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
				else:
					cell_msg = '	   <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO:\n'
				cell_msg += '</b></pre></td>\n'
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def copyConfToolsToTargetImage(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >Copy Template Conf / Tools from Builder</td>\n')
		self.analyze_copy_conf_tool_log('UPF-VPP')
		self.file.write('	 </tr>\n')

	def analyze_copy_conf_tool_log(self, nfType):
		if nfType != 'UPF-VPP':
			self.file.write('      <td>N/A</td>\n')
			self.file.write('	   <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		self.file.write('      <td>Target Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				section_start_pattern = 'WORKDIR /openair-upf/etc'
				if variant == 'docker':
					section_end_pattern = 'Successfully tagged oai-upf-vpp:'
				else:
					section_end_pattern = 'COMMIT oai-upf-vpp:'
				section_status = False
				status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
							status = True
					logfile.close()
				if status:
					cell_msg = '	   <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:\n'
				else:
					cell_msg = '	   <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO:\n'
				cell_msg += '</b></pre></td>\n'
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def imageSizeRow(self):
		self.file.write('	 <tr>\n')
		self.file.write('	   <td bgcolor="lightcyan" >Image Size</td>\n')
		self.analyze_image_size_log('UPF-VPP', True)
		self.file.write('	 </tr>\n')

	def analyze_image_size_log(self, nfType, imageKind):
		if nfType != 'UPF-VPP':
			if imageKind:
				self.file.write('      <td>N/A</td>\n')
			self.file.write('	   <td colspan="2">Wrong NF Type for this Report</td>\n')
			return

		if imageKind:
			self.file.write('      <td>Target Image</td>\n')

		cwd = os.getcwd()
		variants = ['docker', 'podman']
		for variant in variants:
			logFileName = 'upf_' + variant + '_image_build.log'
			if os.path.isfile(cwd + '/archives/' + logFileName):
				if nfType == 'UPF-VPP':
					if variant == 'docker':
						section_start_pattern = 'Successfully tagged oai-upf-vpp:'
						section_end_pattern = 'OAI-UPF-VPP DOCKER IMAGE BUILD'
					else:
						section_start_pattern = 'COMMIT oai-upf-vpp:'
						section_end_pattern = 'OAI-UPF-VPP PODMAN RHEL7 IMAGE BUILD'
				section_status = False
				status = False
				with open(cwd + '/archives/' + logFileName, 'r') as logfile:
					for line in logfile:
						result = re.search(section_start_pattern, line)
						if result is not None:
							section_status = True
						result = re.search(section_end_pattern, line)
						if result is not None:
							section_status = False
						if section_status:
							if nfType == 'UPF-VPP':
								if self.git_pull_request:
									result = re.search('oai-upf-vpp *ci-tmp', line)
								else:
									result = re.search('oai-upf-vpp *develop', line)
							if result is not None:
								if variant == 'docker':
									result = re.search('ago *([0-9A-Z\.]+)', line)
								else:
									result = re.search('ago *([0-9\.]+ [A-Z]+)', line)
								if result is not None:
									size = result.group(1)
									status = True
					logfile.close()
				if status:
					cell_msg = '	   <td bgcolor="LimeGreen"><pre style="border:none; background-color:LimeGreen"><b>'
					cell_msg += 'OK:  ' + size + '\n'
				else:
					cell_msg = '	   <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
					cell_msg += 'KO:\n'
				cell_msg += '</b></pre></td>\n'
			else:
				cell_msg = '	  <td bgcolor="Tomato"><pre style="border:none; background-color:Tomato"><b>'
				cell_msg += 'KO: logfile (' + logFileName + ') not found</b></pre></td>\n'

			self.file.write(cell_msg)

	def testSummaryHeader(self):
		self.file.write('  <h2>Test Summary</h2>\n')
		cwd = os.getcwd()
		if os.path.isfile(cwd + '/archives/deployment_status.log'):
			cmd = 'egrep -c "DEPLOYMENT: OK" archives/deployment_status.log || true'
			status = False
			ret = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, encoding='utf-8')
			if ret.stdout is not None:
				status = True
			if status:
				self.file.write('  <div class="alert alert-success">\n')
				self.file.write('	  <strong>Successful Sanity Check Deployment! <span class="glyphicon glyphicon-warning-sign"></span></strong>\n')
				self.file.write('  </div>\n')
			else:
				self.file.write('  <div class="alert alert-danger">\n')
				self.file.write('	  <strong>Failed Sanity Check Deployment! <span class="glyphicon glyphicon-warning-sign"></span></strong>\n')
				self.file.write('  </div>\n')
		else:
			self.file.write('  <div class="alert alert-warning">\n')
			self.file.write('	  <strong>Not performed. <span class="glyphicon glyphicon-warning-sign"></span></strong>\n')
			self.file.write('  </div>\n')
		self.file.write('  <br>\n')
		self.file.write('  <button data-toggle="collapse" data-target="#deployment-details">More details on Deployment</button>\n')
		self.file.write('  <br>\n')
		self.file.write('  <div id="deployment-details" class="collapse">\n')
		self.file.write('  <br>\n')
		self.file.write('  <table class="table-bordered" width = "80%" align = "center" border = 1>\n')
		self.file.write('     <tr bgcolor = "#33CCFF" >\n')
		self.file.write('       <th>Container Name</th>\n')
		self.file.write('       <th>Used Image Tag</th>\n')
		self.file.write('       <th>Image Creation Date</th>\n')
		self.file.write('       <th>Used Image Size</th>\n')
		self.file.write('     </tr>\n')
		self.addImageRow('mysql')
		self.addImageRow('oai_nrf')
		self.addImageRow('oai_amf')
		self.addImageRow('oai_smf')
		self.addImageRow('oai_upf_vpp')
		self.file.write('  </table>\n')

		if os.path.isfile(cwd + '/archives/ci-upf-vpp-sanity.pcap'):
			res1 = check_if_upf_registers_to_nrf('./archives/ci-upf-vpp-sanity.pcap')
			res2 = check_pcfp_association('./archives/ci-upf-vpp-sanity.pcap')
			self.file.write('  <br>\n')
			self.file.write('  <table class="table-bordered" width = "90%" align = "center" border = 1>\n')
			self.file.write('     <tr bgcolor = "#33CCFF" >\n')
			self.file.write('       <th>Test Name</th>\n')
			self.file.write('       <th>Test Status</th>\n')
			self.file.write('       <th>Test Details</th>\n')
			self.file.write('     </tr>\n')
			self.file.write(self.addSectionRow('UPF registration to NRF'))
			self.file.write(self.addDetailsRow('UPF Request', res1['upf_nrf_reg_req'], 'n/a'))
			self.file.write(self.addDetailsRow('NRF Answer', res1['upf_nrf_reg_res'], 'UPF_FQDN   = ' + res1['upf_nrf_fqdn']))
			self.file.write(self.addSectionRow('PFCP Association'))
			self.file.write(self.addDetailsRow('SMF Request', res2['pfcp_ass_req'], 'PFCP IPv4  = ' + res2['pfcp_ass_ipv4']))
			details  = 'FQDN       = ' + res2['pfcp_ass_upf_fqdn'] + '\n'
			details += 'Entreprise = ' + res2['pfcp_ass_entreprise'] + '\n'
			details += 'Build      = ' + res2['pfcp_ass_build_str']
			self.file.write(self.addDetailsRow('UPF Answer', res2['pfcp_ass_res'], details))
			self.file.write('  </table>\n')

		self.file.write('  </div>\n')

	def addImageRow(self, imageInfoPrefix):
		cwd = os.getcwd()
		if imageInfoPrefix == 'oai_amf':
			containerName = 'ci-oai-amf'
			tagPattern = 'OAI_AMF_TAG'
		if imageInfoPrefix == 'oai_smf':
			containerName = 'ci-oai-smf'
			tagPattern = 'OAI_SMF_TAG'
		if imageInfoPrefix == 'oai_nrf':
			containerName = 'ci-oai-nrf'
			tagPattern = 'OAI_NRF_TAG'
		if imageInfoPrefix == 'oai_upf_vpp':
			containerName = 'ci-vpp-upf'
			tagPattern = 'OAI_UPF_VPP_TAG'
		if imageInfoPrefix == 'mysql':
			containerName = 'ci-mysql'
			tagPattern = 'MYSQL_TAG'
		if os.path.isfile(cwd + '/archives/' + imageInfoPrefix + '_image_info.log'):
			usedTag = ''
			createDate = ''
			size = ''
			with open(cwd + '/archives/' + imageInfoPrefix + '_image_info.log') as imageLog:
				for line in imageLog:
					line = line.strip()
					result = re.search(tagPattern + ': (?P<tag>[a-zA-Z0-9\.\-\_:]+)', line)
					if result is not None:
						usedTag = result.group('tag')
					result = re.search('Date = (?P<date>[a-zA-Z0-9\-\_:]+)', line)
					if result is not None:
						createDate = result.group('date')
					result = re.search('Size = (?P<size>[0-9]+) bytes', line)
					if result is not None:
						sizeInt = int(result.group('size'))
						if sizeInt < 1000000:
							sizeInt = int(sizeInt / 1000)
							size = str(sizeInt) + ' kB'
						else:
							sizeInt = int(sizeInt / 1000000)
							size = str(sizeInt) + ' MB'
			imageLog.close()
			self.file.write('     <tr>\n')
			self.file.write('       <td>' + containerName + '</td>\n')
			self.file.write('       <td>' + usedTag + '</td>\n')
			self.file.write('       <td>' + createDate + '</td>\n')
			self.file.write('       <td>' + size + '</td>\n')
			self.file.write('     </tr>\n')

	def addSectionRow(self, sectionName):
		sectionRow = ''
		sectionRow += '     <tr bgcolor = "LightGray">\n'
		sectionRow += '       <td align = "center" colspan = "3">' + sectionName + '</td>\n'
		sectionRow += '     </tr>\n'
		return sectionRow

	def addDetailsRow(self, testName, status, details):
		detailsRow = ''
		detailsRow += '     <tr>\n'
		detailsRow += '       <td>' + testName + '</td>\n'
		if status:
			detailsRow += '       <td bgcolor = "Green"><font color="white"><b>OK</b></font></td>\n'
		else:
			detailsRow += '       <td bgcolor = "Red"><font color="white"><b>KO</b></font></td>\n'
		detailsRow += '       <td><pre>'+ details + '</td>\n'
		detailsRow += '     </tr>\n'
		return detailsRow

	def testSummaryFooter(self):
		self.file.write('  <br>\n')

def Usage():
	print('----------------------------------------------------------------------------------------------------------------------')
	print('generateHtmlReport.py')
	print('   Generate an HTML report for the Jenkins pipeline on openair-smf.')
	print('----------------------------------------------------------------------------------------------------------------------')
	print('Usage: python3 generateHtmlReport.py [options]')
	print('  --help  Show this help.')
	print('---------------------------------------------------------------------------------------------- Mandatory Options -----')
	print('  --job_name=[Jenkins Job name]')
	print('  --job_id=[Jenkins Job Build ID]')
	print('  --job_url=[Jenkins Job Build URL]')
	print('  --git_url=[Git Repository URL]')
	print('  --git_src_branch=[Git Source Branch Name]')
	print('  --git_src_commit=[Git Source Commit SHA-ONE]')
	print('----------------------------------------------------------------------------------------------- Optional Options -----')
	print('  --git_pull_request=True')
	print('  --git_target_branch=[Git Target Branch Name]')
	print('  --git_target_commit=[Git Target Commit SHA-ONE]')

#--------------------------------------------------------------------------------------------------------
#
# Start of main
#
#--------------------------------------------------------------------------------------------------------

argvs = sys.argv
argc = len(argvs)

HTML = HtmlReport()

while len(argvs) > 1:
	myArgv = argvs.pop(1)
	if re.match('^\-\-help$', myArgv, re.IGNORECASE):
		Usage()
		sys.exit(0)
	elif re.match('^\-\-job_name=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-job_name=(.+)$', myArgv, re.IGNORECASE)
		HTML.job_name = matchReg.group(1)
	elif re.match('^\-\-job_id=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-job_id=(.+)$', myArgv, re.IGNORECASE)
		HTML.job_id = matchReg.group(1)
	elif re.match('^\-\-job_url=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-job_url=(.+)$', myArgv, re.IGNORECASE)
		HTML.job_url = matchReg.group(1)
	elif re.match('^\-\-git_url=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-git_url=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_url = matchReg.group(1)
	elif re.match('^\-\-git_src_branch=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-git_src_branch=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_src_branch = matchReg.group(1)
	elif re.match('^\-\-git_src_commit=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-git_src_commit=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_src_commit = matchReg.group(1)
	elif re.match('^\-\-git_src_commit_msg=(.+)$', myArgv, re.IGNORECASE):
		# Not Mandatory
		matchReg = re.match('^\-\-git_src_commit_msg=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_src_commit_msg = matchReg.group(1)
	elif re.match('^\-\-git_pull_request=(.+)$', myArgv, re.IGNORECASE):
		# Can be silent: would be false!
		matchReg = re.match('^\-\-git_pull_request=(.+)$', myArgv, re.IGNORECASE)
		if matchReg.group(1) == 'true' or matchReg.group(1) == 'True':
			HTML.git_pull_request = True
	elif re.match('^\-\-git_target_branch=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-git_target_branch=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_target_branch = matchReg.group(1)
	elif re.match('^\-\-git_target_commit=(.+)$', myArgv, re.IGNORECASE):
		matchReg = re.match('^\-\-git_target_commit=(.+)$', myArgv, re.IGNORECASE)
		HTML.git_target_commit = matchReg.group(1)
	else:
		sys.exit('Invalid Parameter: ' + myArgv)

if HTML.job_name == '' or HTML.job_id == '' or HTML.job_url == '':
	sys.exit('Missing Parameter in job description')

if HTML.git_url == '' or HTML.git_src_branch == '' or HTML.git_src_commit == '':
	sys.exit('Missing Parameter in Git Repository description')

if HTML.git_pull_request:
	if HTML.git_target_commit == '' or HTML.git_target_branch == '':
		 sys.exit('Missing Parameter in Git Pull Request Repository description')

HTML.generate()
