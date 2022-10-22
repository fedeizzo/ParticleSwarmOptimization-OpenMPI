FROM ubuntu:22.10

# set locate
RUN apt-get clean && apt-get update && apt-get install -y locales
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Rome

# install deps
RUN apt-get install -y \
	gcc \
	make \
	libglib2.0-dev \
	pkg-config \
	openmpi-bin \
	libopenmpi3 \
	libopenmpi-dev

# add content
ADD . /src

# install
WORKDIR /src
RUN make

ENTRYPOINT [ "/src/bin/hpc" ]
