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
* Network Slicing Selection Function (**NSSF**)

Each has its own repository: this repository (`oai-cn5g-nssf`) is meant for NSSF.

# Licence info

It is distributed under `OAI Public License V1.1`.
See [OAI Website for more details](https://www.openairinterface.org/?page_id=698).

The text for `OAI Public License V1.1` is also available under [LICENSE](LICENSE)
file at the root of this repository.

# Where to start

The Openair-CN-5G NSSF code is written, executed, and tested on UBUNTU server bionic version.
Other Linux distributions support will be added later on.

More details on the supported feature set is available on this [page](docs/FEATURE_SET.md).

# Collaborative work

This source code is managed through a GITLAB server, a collaborative development platform:

*  URL: [https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-nssf](https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-nssf).

Process is explained in [CONTRIBUTING](CONTRIBUTING.md) file.

# Contribution requests

In a general way, anybody who is willing can contribute on any part of the
code in any network component.

Contributions can be simple bugfixes, advices and remarks on the design,
architecture, coding/implementation.

# Release Notes

They are available on the [CHANGELOG](CHANGELOG.md) file.

# Repository Structure:

The OpenAirInterface CN NSSF software is composed of the following parts: 

<pre>
openair-cn5g-nssf
├── build:         Directory containing build scripts.
├── ci-scripts:    Directory containing the script files for CI framework.
├── docker:        Directory containing the docker files to build images.
├── docs:          Directory containing feature set documentation
├── etc:           Directory containing configuration file templates
├── scripts
└── src:           Directory containing the source files of NSSF
    ├── api-server
    │   ├── api
    │   ├── impl
    │   └── model
    ├── common
    │   ├── msg
    │   └── utils
    ├── nssf_app
    └── oai_nssf
</pre>
