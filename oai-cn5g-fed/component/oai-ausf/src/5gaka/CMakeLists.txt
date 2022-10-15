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

set(5GAKA_DIR "${SRC_TOP_DIR}/5gaka")

include_directories(${SRC_TOP_DIR}/ausf_app)
include_directories(${SRC_TOP_DIR}/common)
include_directories(${SRC_TOP_DIR}/common/utils)
include_directories(${SRC_TOP_DIR}/common/libngap)
include_directories(${SRC_TOP_DIR}/5gaka)
include_directories(${SRC_TOP_DIR}/../build/ext/spdlog/include)

file(GLOB 5GAKA_src_files
    ${SRC_TOP_DIR}/common/libngap/*.c
    ${5GAKA_DIR}/*.cpp
)
 
add_library(5GAKA STATIC
           ${5GAKA_src_files})
