FROM nlknguyen/alpine-mpich

USER root
RUN apk update
RUN apk add --no-cache sqlite-dev argp-standalone check git cmake git automake autoconf libtool texinfo diffutils
RUN git clone https://github.com/libcheck/check.git /check
WORKDIR /check
RUN git checkout tags/0.15.2
RUN autoreconf --install && ./configure
RUN make && make install

ADD . /src

WORKDIR /src
RUN make LINKING_LIBRARIES=/usr/lib/libargp.a
WORKDIR /src/bin
