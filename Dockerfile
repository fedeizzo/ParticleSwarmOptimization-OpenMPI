FROM ubuntu:16.04 AS builder

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
	mpich \
	libsqlite3-dev

# add content
ADD . /src

# install
WORKDIR /src
RUN make
WORKDIR /src/bin

RUN cp ../pso-data.ini .

# FROM ubuntu:22.10
# COPY --from=builder /src/bin/particle-swarm-optimization /src/bin/particle-swarm-optimization
# COPY --from=builder /lib/x86_64-linux-gnu /lib/x86_64-linux-gnu
# CMD [ "mpirun", "-n", "1", "/src/bin/particle-swarm-optimization" ]
