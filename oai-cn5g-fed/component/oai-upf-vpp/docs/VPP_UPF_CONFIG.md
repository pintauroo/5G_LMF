 <br/>
 <br/>
 <br/>
We need create/use a host interface that will attach to a linux AF_PACKET interface, one side of a veth pair. The veth pair or host interaface must already exist. Once created, a new host interface will exist in VPP with the name &#39;<em>host-<ifname></em>&#39;, where &#39;<em><ifname></em>&#39; is the name of the specified veth pair. Use the &#39;<em>show interface</em>&#39; command to display host interface details. <br/>
 <br/>
Below is sample vpp-upf configuration. We use at least three veth-pairs as UPF interfaces (e.g. N3, N4, N6/N9 etc). <br/>
 <br/>


```bash
ip table add 1
ip table add 2

####### Create N4 interface
create host-interface name core
set interface ip table host-core 0
set interface ip address host-core 192.168.61.201/24
set interface state host-core up

####### Create N3 interface
create host-interface name access
set interface ip table host-access 1
set interface ip address host-access 192.168.62.201/24
set interface state host-access up

####### Create N6 interface
create host-interface name sgi
set interface ip table host-sgi 2
set interface ip address host-sgi 192.168.63.201/24
set interface state host-sgi up

####### Add ip routes
ip route add 0.0.0.0/0 table 0 via 192.168.61.196 host-core
ip route add 0.0.0.0/0 table 1 via 192.168.62.210 host-access
ip route add 0.0.0.0/0 table 2 via 192.168.63.194 host-sgi

####### Configure PFCP enpoint
upf pfcp endpoint ip 192.168.61.201 vrf 0

####### Add network instance 
upf nwi name core vrf 0
upf nwi name access vrf 1
upf nwi name sgi vrf 2

####### Add fqdn
upf node-id fqdn gwu1.vpp.upg.node.epc.mnc095.mcc208.3gppnetwork.org

####### Specify release for TS 3GPP 29.244 (15 or 16)
upf specification release 16

trace add af-packet-input 100

upf gtpu endpoint ip 192.168.62.201 nwi access teid 0x000004d2/2
```



