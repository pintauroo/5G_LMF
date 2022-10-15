/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "asn.1/Information Element Definitions.asn1"
 * 	`asn1c -pdu=all -fcompound-names -fno-include-deps -findirect-choice
 * -gen-PER -D src`
 */

#include "Ngap_CoreNetworkAssistanceInformation.h"

#include "Ngap_ExpectedUEBehaviour.h"
#include "Ngap_ProtocolExtensionContainer.h"
asn_TYPE_member_t asn_MBR_Ngap_CoreNetworkAssistanceInformation_1[] = {
    {ATF_NOFLAGS,
     0,
     offsetof(
         struct Ngap_CoreNetworkAssistanceInformation, uEIdentityIndexValue),
     (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
     +1, /* EXPLICIT tag at current level */
     &asn_DEF_Ngap_UEIdentityIndexValue,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "uEIdentityIndexValue"},
    {ATF_POINTER,
     1,
     offsetof(struct Ngap_CoreNetworkAssistanceInformation, uESpecificDRX),
     (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_PagingDRX,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "uESpecificDRX"},
    {ATF_NOFLAGS,
     0,
     offsetof(
         struct Ngap_CoreNetworkAssistanceInformation,
         periodicRegistrationUpdateTimer),
     (ASN_TAG_CLASS_CONTEXT | (2 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_PeriodicRegistrationUpdateTimer,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "periodicRegistrationUpdateTimer"},
    {ATF_POINTER,
     1,
     offsetof(struct Ngap_CoreNetworkAssistanceInformation, mICOModeIndication),
     (ASN_TAG_CLASS_CONTEXT | (3 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_MICOModeIndication,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "mICOModeIndication"},
    {ATF_NOFLAGS,
     0,
     offsetof(struct Ngap_CoreNetworkAssistanceInformation, tAIListForInactive),
     (ASN_TAG_CLASS_CONTEXT | (4 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_TAIListForInactive,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "tAIListForInactive"},
    {ATF_POINTER,
     2,
     offsetof(
         struct Ngap_CoreNetworkAssistanceInformation, expectedUEBehaviour),
     (ASN_TAG_CLASS_CONTEXT | (5 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_ExpectedUEBehaviour,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "expectedUEBehaviour"},
    {ATF_POINTER,
     1,
     offsetof(struct Ngap_CoreNetworkAssistanceInformation, iE_Extensions),
     (ASN_TAG_CLASS_CONTEXT | (6 << 2)),
     -1, /* IMPLICIT tag at current level */
     &asn_DEF_Ngap_ProtocolExtensionContainer_175P28,
     0,
     {0, 0, 0},
     0,
     0, /* No default value */
     "iE-Extensions"},
};
static const int asn_MAP_Ngap_CoreNetworkAssistanceInformation_oms_1[] = {1, 3,
                                                                          5, 6};
static const ber_tlv_tag_t
    asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1[] = {
        (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))};
static const asn_TYPE_tag2member_t
    asn_MAP_Ngap_CoreNetworkAssistanceInformation_tag2el_1[] = {
        {(ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0,
         0}, /* uEIdentityIndexValue */
        {(ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0}, /* uESpecificDRX */
        {(ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0,
         0}, /* periodicRegistrationUpdateTimer */
        {(ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0}, /* mICOModeIndication */
        {(ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0}, /* tAIListForInactive */
        {(ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0}, /* expectedUEBehaviour */
        {(ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0}  /* iE-Extensions */
};
asn_SEQUENCE_specifics_t asn_SPC_Ngap_CoreNetworkAssistanceInformation_specs_1 =
    {
        sizeof(struct Ngap_CoreNetworkAssistanceInformation),
        offsetof(struct Ngap_CoreNetworkAssistanceInformation, _asn_ctx),
        asn_MAP_Ngap_CoreNetworkAssistanceInformation_tag2el_1,
        7, /* Count of tags in the map */
        asn_MAP_Ngap_CoreNetworkAssistanceInformation_oms_1, /* Optional members
                                                              */
        4,
        0, /* Root/Additions */
        7, /* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_Ngap_CoreNetworkAssistanceInformation = {
    "CoreNetworkAssistanceInformation",
    "CoreNetworkAssistanceInformation",
    &asn_OP_SEQUENCE,
    asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1,
    sizeof(asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1) /
        sizeof(asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1[0]), /* 1 */
    asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1, /* Same as above */
    sizeof(asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1) /
        sizeof(asn_DEF_Ngap_CoreNetworkAssistanceInformation_tags_1[0]), /* 1 */
    {0, 0, SEQUENCE_constraint},
    asn_MBR_Ngap_CoreNetworkAssistanceInformation_1,
    7,                                                     /* Elements count */
    &asn_SPC_Ngap_CoreNetworkAssistanceInformation_specs_1 /* Additional specs
                                                            */
};
