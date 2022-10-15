 <br/>
 <br/>
 <br/>

UPF sould have at least three host or veth interfaces for viz. access, core and sgi (or N3, N4 and N6/N9 respectively).
Here we have three interfaces to docker container. We rename them just for sec of simplicity. There is
additional route added for UE traffic 10.10.10.0/24 network via sgi interface as gateway.<br/>
 <br/>
 Below is sample interfaces configuration for host machine/container -
 <br/>

```bash
ip link set eth0 down
ip link set eth0 name access
ip link set access up 

ip link set eth1 down
ip link set eth1 name core 
ip link set core up

ip link set eth2 down
ip link set eth2 name sgi
ip link set sgi up 

ip route add 10.10.10.0/24 via 192.168.63.198 dev sgi

ip route add 192.168.61.0/24 dev core 
ip route add 192.168.62.0/24 dev access
ip route add 192.168.63.0/24 dev sgi 
```

