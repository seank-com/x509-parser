FROM ubuntu:latest AS base

RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y git && \
  apt-get install -y cmake && \
  apt-get install -y build-essential && \
  apt-get install -y curl && \
  apt-get install -y libcurl4-openssl-dev && \
  apt-get install -y libssl-dev && \
  apt-get install -y uuid-dev && \
  apt-get install -y nano && \
  cmake --version && \
  gcc --version && \
  mkdir /src && \
  cd /src && \
  git clone -b 2018-03-16 --recursive https://github.com/Azure/azure-iot-sdk-c.git && \
  cd /src/azure-iot-sdk-c && \
  mkdir cmake

ADD custom_hsm/ /src/custom_hsm

COPY azure-iot-sdk/dice_device_enrollment.c \
  /src/azure-iot-sdk-c/provisioning_client/tools/dice_device_enrollment/dice_device_enrollment.c

RUN \
  sed -i -e 's/\[ID Scope\]/0ne00012BA4/g' /src/azure-iot-sdk-c/provisioning_client/samples/prov_dev_client_ll_sample/prov_dev_client_ll_sample.c

WORKDIR /src

FROM base AS dice

RUN \
  cd /src/azure-iot-sdk-c/cmake && \
  cmake -Duse_prov_client=ON .. && \
  cmake --build .

CMD ["bash"]

FROM base AS azure

RUN \
  cd /src/custom_hsm && \
  make && \
  cd /src/azure-iot-sdk-c/cmake && \
  cmake -Duse_prov_client=ON -Dhsm_custom_lib=/src/custom_hsm/custom_hsm.a .. && \
  cmake --build .

CMD ["bash"]

FROM base AS mine

RUN \
  sed -i -e 's/define MSR/define NOT_MSR/g' /src/custom_hsm/custom_hsm.c && \
  cd /src/custom_hsm && \
  make && \
  cd /src/azure-iot-sdk-c/cmake && \
  cmake -Duse_prov_client=ON -Dhsm_custom_lib=/src/custom_hsm/custom_hsm.a .. && \
  cmake --build .

CMD ["bash"]
