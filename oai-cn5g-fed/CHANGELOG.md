# RELEASE NOTES: #

## v1.3.0 -- January 2022 ##

* Incorporation of new public network functions:
  - NSSF
* New tutorials:
  - Slicing --> `docs/DEPLOY_SA5G_SLICING.md`
  - Static UE IP address to emulate private network --> `docs/DEPLOY_SA5G_BASIC_STATIC_UE_IP.md`
* `AMF` Changes:
  - Periodic Registration Update
  - Support for Event Exposure (Registration State Report)
  - Implicit Deregistration Timer
  - Mobile Reachable Timer
  - Mobility Registration Update
  - NRF Selection (via NSSF)
  - Fix for validation of SMF Info
  - Fix RAN UE NGAP Id
  - Fix SMF Selection SD
  - Fix NSSAI mismatch
  - Fix Service Request
  - Fix HTTP2
  - Fix MCC such as 001
  - Docker optimization
* `AUSF` Changes:
  - HTTP2 Support
  - Docker optimization
* `NRF` Changes:
  - NF profile update
  - Docker optimization
* `NSSF` Changes:
  - Initial Public Release
  - Docker Optimization
* `SMF` Changes:
  - Update NWI from NF profile
  - Update SMF profile
  - Added retry for PFCP association request
  - More flexible DNN/IP ranges when deploying container
  - Fix retrieving the list of NWI
  - Fix entreprise IE decoding
  - Fix issue for UPF selection
  - Fix for IE Measurement Period
  - Docker optimization
* `UDM` Changes:
  - Experimental support for Event Exposure
  - Added HTTP2 support
  - Fix static addr allocation
  - Fix build issue
  - Docker build optimizations
* `UDR` Changes:
  - Added HTTP2 support
  - Fix build issue
  - Fix static addr allocation
  - Docker build optimizations
* `SPGWU-TINY` Changes:
  - Disable association request if NF registration is enabled
* `UPF-VPP` Changes:
  - Upgrade to UPG plugin stable/1.2
  - Build fixes
  - Deployment fixes (wait launch of NRF Client until VPP is getting ready)

## v1.2.1 -- October 2021 ##

* Incorporation of new public network functions:
  - UPF-VPP
* New tutorial with this new function
* `AMF` Changes:
  - Fix build issue
  - Tentative fix for ngKSI already in use
  - Initial implementation for Mobility registration update
* `AUSF` Changes:
  - Fix build issue
* `NRF` Changes:
  - Fix build issue
* `SMF` Changes:
  - Timers handling: T3591/T3952
  - Fix build issue
  - Fix UPF selection
* `UDM` Changes:
  - Fix build issue
* `UDR` Changes:
  - Fix build issue
* `SPGWU-TINY` Changes:
  - Fix build issue
  - Fix GTPU DL encapsulation: 8 extraneous bytes
* `UPF-VPP` Changes:
  - Initial Public Release
  - Full support for Ubuntu18 and RHEL7
  - CI Build support
  - Tutorial validated

## v1.2.0 -- September 2021 ##

* Incorporation of new public network functions:
  - AUSF
  - UDM
  - UDR
* New tutorials with these new functions
* CI improvements
* `AMF` Changes:
  - AUSF connection
  - Support PDU Session Resource Modify procedure
  - Support HTTP2
  - Support AMF Event Exposure Services
  - Fix NIA0, NEA2
  - Fix potential AMF crash with UE/NGAP/NAS context
  - Fix N2 Handover
  - Fix Paging procedures
* `AUSF` Changes:
  - Initial public release
  - NRF registration
    - with FQDN DNS resolution
  - Full support for Ubuntu18 and RHEL8
* `NRF` Changes:
  - Added AUSF, UDR, UDM profiles
  - Updated UPF profile
* `SMF` Changes:
  - Event Exposure implemented
  - UPF profile update
  - Support for URR query
  - Bug fixes
* `UDM` Changes:
  - Initial public release
  - NRF registration
    - with FQDN DNS resolution
  - Full support for Ubuntu18 and RHEL8
* `UDR` Changes:
  - Initial public release
  - NRF registration
    - with FQDN DNS resolution
  - Proper mySQL DB deployment management
  - Full support for Ubuntu18 and RHEL8
* `SPGWU-TINY` Changes:
  - Adding 5G features
    - HTTP2 support

## v1.1.0 -- July 2021 ##

* Improvements on Continuous Integration:
  - DsTester validation on Ubuntu18 docker deployment
  - DsTester validation on RHEL8 / OpenShift deployment
  - Some components (such as AMF/NRF) have bracket-testing at unit level
* A lot of tutorials have been added
* `AMF` Changes:
  - Session Release
  - NRF registration
    - with FQDN DNS resolution
  - Multiple PDU support
  - Bug fixes
  - Full support for Ubuntu18 and RHEL8
* `NRF` Changes:
  - FQDN DNS resolution
  - Bug fixes
  - Full support for Ubuntu18 and RHEL8
* `SMF` Changes:
  - PFCP Release 16 support
  - NRF registration
    - with FQDN DNS resolution
  - Support for multiple UPF instances
  - Dotted DNN support
  - Use (SST, SD) to get the subscription information
  - Bug fixes
  - Full support for Ubuntu18 and RHEL8
* `SPGWU-TINY` Changes:
  - Adding 5G features
    - NRF discovery and FQDN support

## v1.0.0 -- September 2020 ##

* Initial release

