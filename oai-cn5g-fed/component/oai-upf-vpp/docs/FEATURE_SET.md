User Plane Gateway (UPG) based on VPP
=====================================

UPG  implements a GTP-U user plane based on [3GPP TS 23.214][TS23214]
and [3GPP TS 29.244][TS29244] Release 15 & 16. It is implemented as an
out-of-tree plugin for [FD.io VPP][VPP].

UPG is project by Travelping GmbH and more official information please visit [UPG github page](https://github.com/travelping/upg-vpp)

* VPP_BRANCH  = stable/2101
* VPP_RELEASE = v21.01

#### Supported Features

* PFCP protocol
  * en/decoding of most IEs
  * heartbeat
  * node related messages
  * session related messages
* Uplink and Downlink Packet Detection Rules (PDR) and
  Forward Action Rules (FAR) -- (some parts)
* IPv4 -- inner and outer
* IPv6 -- inner and outer
* Usage Reporting Rules (URR)
* PFCP Session Reports
* Linked Usage Reports
* GTP Extension Header (partial)

No yet working
--------------

* Buffer Action Rules (BAR)
* QoS Enforcement Rule (QER)

Limitations
-----------

* FAR action with destination LI are not implemented
* Ethernet bearer support

