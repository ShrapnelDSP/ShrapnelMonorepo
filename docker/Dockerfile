FROM ubuntu:21.04

ARG DEBIAN_FRONTEND=noninteractive
ENV IDF_PATH=/opt/esp-idf
ENV IDF_TOOLS_PATH=/opt/esp-idf-tools

# Install the depenencies from Linux Dependencies.md in the juce repo, and for
# ESP-IDF
RUN apt update
RUN apt install -y \
    libasound2-dev \
    libjack-jackd2-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev \
    libx11-dev \
    libxcomposite-dev \
    libxcursor-dev \
    libxext-dev \
    libxinerama-dev \
    libxrandr-dev \
    libxrender-dev \
    libwebkit2gtk-4.0-dev \
    libglu1-mesa-dev \
    mesa-common-dev \
    build-essential \
    cmake \
    git \
    wget \
    flex \
    bison \
    gperf \
    python3 \
    python3-pip \
    python3-setuptools \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0

RUN rm -rf /var/lib/apt/lists/* && \
    apt clean

RUN git clone --shallow-submodules --recurse-submodules --depth 1 \
    --branch v4.3.2 \
    https://github.com/espressif/esp-idf.git \
    $IDF_PATH

RUN $IDF_PATH/install.sh esp32

ENTRYPOINT ["/bin/bash"]