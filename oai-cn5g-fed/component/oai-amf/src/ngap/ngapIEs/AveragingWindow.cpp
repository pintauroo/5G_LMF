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

/*! \file
 \brief
 \author  Keliang DU, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */

#include "AveragingWindow.hpp"

#include <iostream>
using namespace std;

namespace ngap {

//------------------------------------------------------------------------------
AveragingWindow::AveragingWindow() {
  averagingwindow = 0;
}

//------------------------------------------------------------------------------
AveragingWindow::~AveragingWindow() {}

//------------------------------------------------------------------------------
void AveragingWindow::setAveragingWindow(long value) {
  averagingwindow = value;
}

//------------------------------------------------------------------------------
bool AveragingWindow::getAveragingWindow(long& value) {
  value = averagingwindow;

  return true;
}

//------------------------------------------------------------------------------
bool AveragingWindow::encode2AveragingWindow(
    Ngap_AveragingWindow_t* averagingWindow) {
  *averagingWindow = averagingwindow;

  return true;
}

//------------------------------------------------------------------------------
bool AveragingWindow::decodefromAveragingWindow(
    Ngap_AveragingWindow_t* averagingWindow) {
  averagingwindow = *averagingWindow;

  return true;
}
}  // namespace ngap
