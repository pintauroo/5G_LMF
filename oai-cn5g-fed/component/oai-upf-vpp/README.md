------------------------------------------------------------------------------

                             OPENAIR-CN-5G
 An implementation of the 5G Core network by the OpenAirInterface community.

------------------------------------------------------------------------------

OPENAIR-CN-5G is an implementation of the 3GPP specifications for the 5G Core Network.
At the moment, it contains the following network elements:

* Access and Mobility Management Function (**AMF**)
* Authentication Server Management Function (**AUSF**)
* Network Repository Function (**NRF**)
* Session Management Function (**SMF**)
* Unified Data Management (**UDM**)
* Unified Data Repository (**UDR**)
* User Plane Function (**UPF**)

Each has its own repository: this repository (`oai-cn5g-upf-vpp`) is meant for UPF.

This `UPF` repository contains mainly patches / hacks over 2 open-source projects:

- [Vector Packet Processing](https://github.com/fdio/vpp.git)
- [User Plane Gateway (UPG) based on VPP](https://github.com/travelping/upg-vpp)

# Licence info

As this repository contains mainly patches over 2 open-source projects that are
distributed under Apache V2, it is distributed under `Apache V2.0 License`.

See [Apache Website for more details](http://www.apache.org/licenses/LICENSE-2.0).

The text for `Apache V2.0 License` is also available under [LICENSE](LICENSE)
file at the root of this repository.

Some part(s) of the repository that are decorrelated from the 2 original open-source
projects may be under another LICENSE type.

Check the [NOTICE](NOTICE.md) file for more details.

# Where to start

The Openair-CN-5G UPF code is written, executed, and tested on UBUNTU server bionic version.
Other Linux distributions support will be added later on.

More details on the supported feature set is available on this [page](docs/FEATURE_SET.md).

# Collaborative work

This source code is managed through a GITLAB server, a collaborative development platform:

*  URL: [https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-upf-vpp](https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-upf-vpp).

Process is explained in [CONTRIBUTING](CONTRIBUTING.md) file.

# Contribution requests

In a general way, anybody who is willing can contribute on any part of the
code in any network component.

Contributions can be simple bugfixes, advices and remarks on the design,
architecture, coding/implementation.

# Release Notes

They are available on the [CHANGELOG](CHANGELOG.md) file.

# Repository Structure:

The OpenAirInterface CN UPF software is composed of the following parts: 

<pre>
openair-cn5g-upf-vpp
├── build
│   └── scripts
├── ci-scripts
├── docker
├── docker-compose
├── docs
│   └── images
├── scripts
│   ├── patches
│   └── upf_conf
└── src
</pre>
