/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "asn.1/Information Element Definitions.asn1"
 * 	`asn1c -pdu=all -fcompound-names -fno-include-deps -findirect-choice
 * -gen-PER -D src`
 */

#include "Ngap_PDUSessionResourceSetupResponseTransfer.h"

#include "Ngap_QosFlowPerTNLInformation.h"
#include "Ngap_SecurityResult.h"
#include "Ngap_QosFlowList.h"
#include "Ngap_ProtocolExtensionContainer.h"
static asn_TYPE_member_t
    asn_MBR_Ngap_PDUSessionResourceSetupResponseTransfer_1[] = {
        {ATF_NOFLAGS,
         0,
         offsetof(
             struct Ngap_PDUSessionResourceSetupResponseTransfer,
             qosFlowPerTNLInformation),
         (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
         -1, /* IMPLICIT tag at current level */
         &asn_DEF_Ngap_QosFlowPerTNLInformation,
         0,
         {0, 0, 0},
         0,
         0, /* No default value */
         "qosFlowPerTNLInformation"},
        {ATF_POINTER,
         4,
         offsetof(
             struct Ngap_PDUSessionResourceSetupResponseTransfer,
             additionalQosFlowPerTNLInformation),
         (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
         -1, /* IMPLICIT tag at current level */
         &asn_DEF_Ngap_QosFlowPerTNLInformation,
         0,
         {0, 0, 0},
         0,
         0, /* No default value */
         "additionalQosFlowPerTNLInformation"},
        {ATF_POINTER,
         3,
         offsetof(
             struct Ngap_PDUSessionResourceSetupResponseTransfer,
             securityResult),
         (ASN_TAG_CLASS_CONTEXT | (2 << 2)),
         -1, /* IMPLICIT tag at current level */
         &asn_DEF_Ngap_SecurityResult,
         0,
         {0, 0, 0},
         0,
         0, /* No default value */
         "securityResult"},
        {ATF_POINTER,
         2,
         offsetof(
             struct Ngap_PDUSessionResourceSetupResponseTransfer,
             qosFlowFailedToSetupList),
         (ASN_TAG_CLASS_CONTEXT | (3 << 2)),
         -1, /* IMPLICIT tag at current level */
         &asn_DEF_Ngap_QosFlowList,
         0,
         {0, 0, 0},
         0,
         0, /* No default value */
         "qosFlowFailedToSetupList"},
        {ATF_POINTER,
         1,
         offsetof(
             struct Ngap_PDUSessionResourceSetupResponseTransfer,
             iE_Extensions),
         (ASN_TAG_CLASS_CONTEXT | (4 << 2)),
         -1, /* IMPLICIT tag at current level */
         &asn_DEF_Ngap_ProtocolExtensionContainer_175P117,
         0,
         {0, 0, 0},
         0,
         0, /* No default value */
         "iE-Extensions"},
};
static const int asn_MAP_Ngap_PDUSessionResourceSetupResponseTransfer_oms_1[] =
    {1, 2, 3, 4};
static const ber_tlv_tag_t
    asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1[] = {
        (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))};
static const asn_TYPE_tag2member_t
    asn_MAP_Ngap_PDUSessionResourceSetupResponseTransfer_tag2el_1[] = {
        {(ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0,
         0}, /* qosFlowPerTNLInformation */
        {(ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0,
         0}, /* additionalQosFlowPerTNLInformation */
        {(ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0}, /* securityResult */
        {(ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0,
         0}, /* qosFlowFailedToSetupList */
        {(ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0} /* iE-Extensions */
};
static asn_SEQUENCE_specifics_t
    asn_SPC_Ngap_PDUSessionResourceSetupResponseTransfer_specs_1 = {
        sizeof(struct Ngap_PDUSessionResourceSetupResponseTransfer),
        offsetof(struct Ngap_PDUSessionResourceSetupResponseTransfer, _asn_ctx),
        asn_MAP_Ngap_PDUSessionResourceSetupResponseTransfer_tag2el_1,
        5, /* Count of tags in the map */
        asn_MAP_Ngap_PDUSessionResourceSetupResponseTransfer_oms_1, /* Optional
                                                                       members
                                                                     */
        4,
        0, /* Root/Additions */
        5, /* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer = {
    "PDUSessionResourceSetupResponseTransfer",
    "PDUSessionResourceSetupResponseTransfer",
    &asn_OP_SEQUENCE,
    asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1,
    sizeof(asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1) /
        sizeof(asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1
                   [0]),                                         /* 1 */
    asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1, /* Same as
                                                                    above */
    sizeof(asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1) /
        sizeof(asn_DEF_Ngap_PDUSessionResourceSetupResponseTransfer_tags_1
                   [0]), /* 1 */
    {0, 0, SEQUENCE_constraint},
    asn_MBR_Ngap_PDUSessionResourceSetupResponseTransfer_1,
    5, /* Elements count */
    &asn_SPC_Ngap_PDUSessionResourceSetupResponseTransfer_specs_1 /* Additional
                                                                     specs */
};
