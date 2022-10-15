# 1.  Retrieve the proper code version #

```bash
$ git clone https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-upf-vpp.git

$ cd oai-cn5g-upf-vpp

$ git checkout develop
```

# 2. Generic Parameters #

Here in our network configuration, we need to pass the "GIT PROXY" configuration.

*   If you do not need, remove the `--build-arg NEEDED_GIT_PROXY=".."` option.
*   If you do need it, change with your proxy value.

# 3. Build vpp-upf image #
## 3.1 On a Ubuntu 18.04 Host ##

```bash
$ docker build --target oai-upf-vpp --tag oai-upf-vpp:latest \
               --file docker/Dockerfile.upf-vpp.ubuntu18 \
               --build-arg NEEDED_GIT_PROXY="http://proxy.eurecom.fr:8080" .
```

##  3.2 On a RHEL 7 Host ##

```bash
$ docker build --target oai-upf-vpp --tag oai-upf-vpp:latest \
               --file docker/Dockerfile.upf-vpp.rhel7 \
               --build-arg NEEDED_GIT_PROXY="http://proxy.eurecom.fr:8080" .
```
