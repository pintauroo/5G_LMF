################################################################################
# Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The OpenAirInterface Software Alliance licenses this file to You under
# the OAI Public License, Version 1.1  (the "License"); you may not use this file
# except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.openairinterface.org/?page_id=698
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------------
# For more information about the OpenAirInterface (OAI) Software Alliance:
#      contact@openairinterface.org
################################################################################

set(UDM_API_SERVER_DIR "${SRC_TOP_DIR}/api_server")

include_directories(${UDM_API_SERVER_DIR}/api)
include_directories(${UDM_API_SERVER_DIR}/impl)
include_directories(${UDM_API_SERVER_DIR}/model)
include_directories(${UDM_API_SERVER_DIR}/)
include_directories(${SRC_TOP_DIR}/udm_app)
include_directories(${SRC_TOP_DIR}/common)
include_directories(${SRC_TOP_DIR}/common/utils)
include_directories(${SRC_TOP_DIR}/common/libngap)
include_directories(${SRC_TOP_DIR}/5gaka)
include_directories(${SRC_TOP_DIR}/../build/ext/spdlog/include)

file(GLOB UDM_API_SERVER_src_files
    ${UDM_API_SERVER_DIR}/udm-api-server.cpp
    ${UDM_API_SERVER_DIR}/udm-http2-server.cpp
    ${UDM_API_SERVER_DIR}/model/*.cpp
    ${UDM_API_SERVER_DIR}/api/*.cpp
    ${UDM_API_SERVER_DIR}/impl/*.cpp
)
 
add_library(UDM_API STATIC
           ${UDM_API_SERVER_src_files})
