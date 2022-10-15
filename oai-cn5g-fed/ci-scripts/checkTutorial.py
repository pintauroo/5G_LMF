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

------------------------------------------------------------------------------
"""

from subprocess import PIPE,STDOUT
import time
import subprocess
import logging
import argparse
import re 
import sys 

logging.basicConfig(
    level=logging.DEBUG,
    format="%(message)s"
)

## Global variables
DOCUMENT_FOLDER = '../docs'
SLEEP_BETWEEN_COMMANDS = 2
SLEEP_BETWEEN_HEADERS = 5
DOCKER_COMPOSE_DIRECTORY='../docker-compose'
COMMAND_STATUS = dict()         # A mapping of command and its exit code status

def _parse_args() -> argparse.Namespace:
    """Parse the command line args

    Returns:
        argparse.Namespace: the created parser
    """
    example_text = '''example:
        python3 checkTutorial.py --tutorial DEPLOY_SA5G_BASIC_STATIC_UE_IP.md'''

    parser = argparse.ArgumentParser(description='Run the tutorials to see they are executing fine',
                                    epilog=example_text,
                                    formatter_class=argparse.RawDescriptionHelpFormatter)

    # Tutorial Name
    parser.add_argument(
        '--tutorial', '-t',
        action='store',
        required=True,
        help='name of the tutorial markdown file',
    )
    return parser.parse_args()


def subprocess_call(command,cwd):
    popen = subprocess.Popen(command,shell=True,universal_newlines=True,cwd=cwd,stdout=PIPE)
    for stdout_line in iter(popen.stdout.readline, ""):
        yield stdout_line
    popen.stdout.close()
    return_code = popen.wait()
    if return_code:
        COMMAND_STATUS.update({command:'FAIL'})
        #raise subprocess.CalledProcessError(return_code, command) # This will stop the tutorial at the command which returned exit code>0
    else:
        COMMAND_STATUS.update({command:'PASS'})


def extract_h2_blocks(headers,text):
    text = text.split('\n')
    positions = []
    header_blocks = dict()
    temp_header_blocks = dict()
    for header in headers:
        header_blocks.update({header:[]})
        for position,line in enumerate(text):
            if header in line:
                temp_header_blocks[position]=header
                positions.append(position)
    for key,value in enumerate(positions):
        if key < len(positions)-1:
            for n in range(positions[key],positions[key+1]):
                header_blocks[temp_header_blocks[value]].append(text[n])
        else:
            for n in range(positions[key],len(text)-key):
                header_blocks[temp_header_blocks[value]].append(text[n])

    return header_blocks

def execute_shell_command(h2_blocks):
    for value in h2_blocks:
        logging.info('\033[0;32m {}\033[0m'.format(value))
        shell_blocks = re.findall(r"`{3} shell\n([\S\s]+?)`{3}",'\n'.join(h2_blocks[value]))
        for block in shell_blocks:
            commands = re.findall(r"\$: (.*)",block)
            for command in commands:
                logging.info('\033[0;31m Executing command "{}"\033[0m'.format(command))
                for output in subprocess_call(command=command, cwd=DOCKER_COMPOSE_DIRECTORY):
                    print(output)
                time.sleep(SLEEP_BETWEEN_COMMANDS)
            time.sleep(SLEEP_BETWEEN_HEADERS)

def print_tutorial_summary(command_status,name):
    count = 0
    final_result = 'FAIL'
    for command in command_status:
        if command_status[command] == 'PASS':
            count+=1
    if count == len(command_status):
        final_result = 'PASS'
    statement = "\nFinal result for the tutorial {} is {}\nCommand execution summary:".format(name,final_result)
    print(statement)
    for command in command_status:
        print("{}: {}".format(command,command_status[command]))

def check_tutorial(name):
    filename = DOCUMENT_FOLDER + '/' + name
    with open(filename, 'r') as f:
        text = f.read()
    h2 = re.findall(r"## (.*)\n",text)
    h2_blocks = extract_h2_blocks(h2,text)
    execute_shell_command(h2_blocks)
    print_tutorial_summary(COMMAND_STATUS,name)

if __name__ == '__main__':
    # Parse the arguments to get the deployment instruction
    args = _parse_args()
    check_tutorial(args.tutorial)

