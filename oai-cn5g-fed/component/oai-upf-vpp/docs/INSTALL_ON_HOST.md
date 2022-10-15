# VPP-UPF Native Installation

Tested and validated on Ubuntu Bionic arch amd64.

## Download OAI UPF (VPP-UPF) source code

```bash
$ git clone https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-upf-vpp.git

$ cd oai-cn5g-upf-vpp

$ git checkout develop
```

## Install VPP-UPF

### Install VPP-UPF software dependencies

```bash
$ cd oai-cn5g-upf-vpp/
/oai-cn5g-upf-vpp$ cd ./build/scripts
/oai-cn5g-upf-vpp/build/scripts$ ./build_vpp_upf -I -f
```

After successful dependencies install, 
```bash
==========================================================
make[1]: Leaving directory '/tmp/oai-cn5g-upf-vpp/vpp/build/external'
VPP UPF deps installation successful
VPP UPF not compiled, to compile it, re-run ./build_vpp_upf without -I option
```
### Build VPP-UPF 

```bash
/oai-cn5g-upf-vpp/build/scripts$ ./build_vpp_upf -c -V 
```
After successful dependencies install, 
```bash
@@@@ Installing vpp @@@@
[0/1] Install the project...
-- Install configuration: "debug"
make[1]: Leaving directory '/home/rohan/oai-cn5g-upf-vpp/vpp/build-root'
##### VPP compiled #####
VPP UPG initializing
Installing VPP
```
### Wipe VPP source
```bash
/oai-cn5g-upf-vpp/build/scripts$ ./build_vpp_upf -w
```
