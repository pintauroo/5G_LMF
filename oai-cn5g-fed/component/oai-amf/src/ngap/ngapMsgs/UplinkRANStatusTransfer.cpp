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
 \author  Keliang Du, BUPT
 \date 2020
 \email: contact@openairinterface.org
 */
#include "UplinkRANStatusTransfer.hpp"

#include "logger.hpp"

#include <iostream>
#include <vector>

using namespace std;
namespace ngap {

//------------------------------------------------------------------------------
UplinkRANStatusTransfer::UplinkRANStatusTransfer() {
  amfUeNgapId                            = nullptr;
  ranUeNgapId                            = nullptr;
  ranStatusTransfer_TransparentContainer = nullptr;
  UplinkRANStatusTransferPDU             = nullptr;
  UplinkRANStatusTransferIEs             = nullptr;
}

//------------------------------------------------------------------------------
UplinkRANStatusTransfer::~UplinkRANStatusTransfer() {}

//------------------------------------------------------------------------------
unsigned long UplinkRANStatusTransfer::getAmfUeNgapId() {
  if (amfUeNgapId)
    return amfUeNgapId->getAMF_UE_NGAP_ID();
  else
    return 0;
}

//------------------------------------------------------------------------------
uint32_t UplinkRANStatusTransfer::getRanUeNgapId() {
  if (ranUeNgapId)
    return ranUeNgapId->getRanUeNgapId();
  else
    return 0;
}

//------------------------------------------------------------------------------
void UplinkRANStatusTransfer::getRANStatusTransfer_TransparentContainer(
    RANStatusTransferTransparentContainer*&
        ranstatustransfer_transparentcontainer) {
  ranstatustransfer_transparentcontainer =
      ranStatusTransfer_TransparentContainer;
}

//------------------------------------------------------------------------------
bool UplinkRANStatusTransfer::defromPDU(Ngap_NGAP_PDU_t* ngap_msg_pdu) {
  UplinkRANStatusTransferPDU = ngap_msg_pdu;
  if (UplinkRANStatusTransferPDU->present ==
      Ngap_NGAP_PDU_PR_initiatingMessage) {
    if (UplinkRANStatusTransferPDU->choice.initiatingMessage &&
        UplinkRANStatusTransferPDU->choice.initiatingMessage->procedureCode ==
            Ngap_ProcedureCode_id_UplinkRANStatusTransfer &&
        UplinkRANStatusTransferPDU->choice.initiatingMessage->criticality ==
            Ngap_Criticality_ignore &&
        UplinkRANStatusTransferPDU->choice.initiatingMessage->value.present ==
            Ngap_InitiatingMessage__value_PR_UplinkRANStatusTransfer) {
      UplinkRANStatusTransferIEs =
          &UplinkRANStatusTransferPDU->choice.initiatingMessage->value.choice
               .UplinkRANStatusTransfer;
    } else {
      Logger::ngap().error("Check UplinkRANStatusTransfer message error");
      return false;
    }
  } else {
    Logger::ngap().error("UplinkRANStatusTransfer message type error");
    return false;
  }
  for (int i = 0; i < UplinkRANStatusTransferIEs->protocolIEs.list.count; i++) {
    switch (UplinkRANStatusTransferIEs->protocolIEs.list.array[i]->id) {
      case Ngap_ProtocolIE_ID_id_AMF_UE_NGAP_ID: {
        if (UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_UplinkRANStatusTransferIEs__value_PR_AMF_UE_NGAP_ID) {
          amfUeNgapId = new AMF_UE_NGAP_ID();
          if (!amfUeNgapId->decodefromAMF_UE_NGAP_ID(
                  UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                      ->value.choice.AMF_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP AMF_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP AMF_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_RAN_UE_NGAP_ID: {
        if (UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_UplinkRANStatusTransferIEs__value_PR_RAN_UE_NGAP_ID) {
          ranUeNgapId = new RAN_UE_NGAP_ID();
          if (!ranUeNgapId->decodefromRAN_UE_NGAP_ID(
                  UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                      ->value.choice.RAN_UE_NGAP_ID)) {
            Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
            return false;
          }
        } else {
          Logger::ngap().error("Decoded NGAP RAN_UE_NGAP_ID IE error");
          return false;
        }
      } break;
      case Ngap_ProtocolIE_ID_id_RANStatusTransfer_TransparentContainer: {
        if (UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->criticality == Ngap_Criticality_reject &&
            UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                    ->value.present ==
                Ngap_UplinkRANStatusTransferIEs__value_PR_RANStatusTransfer_TransparentContainer) {
          ranStatusTransfer_TransparentContainer =
              new RANStatusTransferTransparentContainer();
          if (!ranStatusTransfer_TransparentContainer
                   ->decoderanstatustransfer_transparentcontainer(
                       &UplinkRANStatusTransferIEs->protocolIEs.list.array[i]
                            ->value.choice
                            .RANStatusTransfer_TransparentContainer)) {
            Logger::ngap().error(
                "Decoded NGAP RANStatusTransfer_TransparentContainer IE error");
            return false;
          }
          Logger::ngap().error(
              "Decoded NGAP RANStatusTransfer_TransparentContainer IE error");
        } else {
          Logger::ngap().error(
              "Decoded NGAP RANStatusTransfer_TransparentContainer IE error");
        }
      } break;
      default: {
        Logger::ngap().error("Decoded NGAP message PDU error");
        return false;
      }
    }
  }
  return true;
}

}  // namespace ngap
