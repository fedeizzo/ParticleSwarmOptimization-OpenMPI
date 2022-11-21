FROM nlknguyen/alpine-mpich

USER root
RUN apk update
RUN apk add sqlite-dev argp-standalone
# add content
ADD . /src

# install
WORKDIR /src
RUN make LINKING_LIBRARIES=/usr/lib/libargp.a
WORKDIR /src/bin

RUN cp ../pso-data.ini .

#CMD [ "/src/bin/particle-swarm-optimization" ]
