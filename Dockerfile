FROM ubuntu:22.10 AS builder

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
	make \
	pkg-config \
	openmpi-bin \
	libopenmpi3 \
	libopenmpi-dev \
	libsqlite3-dev

# add content
ADD . /src

# install
WORKDIR /src
RUN make

FROM ubuntu:22.10
COPY --from=builder /src/bin/hpc /src/bin/hpc
COPY --from=builder /lib/x86_64-linux-gnu /lib/x86_64-linux-gnu
CMD [ "mpirun", "-n", "1", "/src/bin/hpc" ]
