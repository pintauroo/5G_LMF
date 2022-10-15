# Get the base Ubuntu image from Docker Hub
FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get -y install software-properties-common 
RUN add-apt-repository ppa:pistache+team/unstable
RUN apt-get -y update
RUN apt-get -y install g++ cmake pkg-config libpistache-dev
RUN apt-get install python3 python3-pip python3-setuptools python3-wheel ninja-build -y
RUN pip3 install meson
RUN apt-get install libblas-dev libopenblas-dev liblapack-dev libsuperlu-dev libarpack2-dev libparpack2-dev libarpack++2-dev -y

COPY src /src 
WORKDIR /src

RUN tar -xvf armadillo-11.2.0.tar.xz
WORKDIR armadillo-11.2.0
RUN ./configure
RUN make
RUN make install
WORKDIR ..

RUN meson setup buildServer -Dcpp_std=c++20
WORKDIR buildServer
RUN meson compile
EXPOSE 80
CMD /src/buildServer/httpServer 80 1
