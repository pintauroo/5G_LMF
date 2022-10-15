Location Management Function for 5g core implementation

This repo implements the LMF module in OAI. It implements a client-server mechanism, and the code is implemented as follows: the client is implemented in the already available AMF implementation, and the server is under lmf-server.

LMF and AMF Docker images must be compiled manually to make the environment fully work.


To compile the AMF image refer to https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-fed/-/blob/master/docs/BUILD_IMAGES.md

$ docker build --target oai-amf --tag oai-amf:test --file component/oai-amf/docker/Dockerfile.amf.ubuntu18 component/oai-amf

The code is under oai-cn5g-fed/component/oai-amf/amf-server

To compile LMF image:

$ docker run --name oai-lmf --network=demo-oai-public-net -t lmf

The code is under lmf-server





Once you have compiled both the images with the whole OAI environment, you can start the environment.

The test folder contains a whole test suite for the localization function.

If you run the lmf_tester.sh you can run multiple actions:

./lmf_tester.sh -c -> starts the environment checking for its healthiness

./lmf_tester.sh -s -> stops the environment

./lmf_tester.sh -l -> runs the locust emulation

./lmf_tester.sh -j -> runs the jmeter emulation

The test suite contains all the code and the tests to run the emulations.
