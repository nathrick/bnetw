FROM ubuntu:latest as build

LABEL description="Build container - bnetw"

RUN apt-get -qq update && apt-get -qqy install build-essential cmake curl file gcc g++ git unzip wget googletest python-pip

ENV LD_LIBRARY_PATH="/usr/local/lib:${LD_LIBRARY_PATH}"

ARG TEST=OFF

RUN cd /home && wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz \
  && tar xfz boost_1_67_0.tar.gz \
  && rm boost_1_67_0.tar.gz \
  && cd boost_1_67_0 \
  && ./bootstrap.sh --prefix=/usr/local --with-libraries=atomic,chrono,context,date_time,fiber,filesystem,program_options,regex,serialization,system,thread,math \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_67_0

RUN pip install gcovr

RUN cd /usr/src/googletest ; cmake -DBUILD_SHARED_LIBS=ON . ; make 
RUN mv /usr/src/googletest/googlemock/*.so /usr/lib/; mv /usr/src/googletest/googlemock/gtest/*.so /usr/lib/

COPY src /bnetw/src
COPY example /bnetw/example
COPY test /bnetw/test
COPY CMakeLists.txt /bnetw
COPY docker-compose.yml /bnetw
COPY Dockerfile /bnetw
COPY README.md /bnetw

WORKDIR /bnetw

RUN mkdir -p build \
  && cd build \
  && cmake .. \
  && make install \
  && make example

RUN if [ "$TEST" = "ON" ] ; then \
  cd /bnetw/test/ \
  && mkdir -p build \
  && cd build \
  && cmake -DBUILD_TEST=ON ../.. \
  && make \
  && ../../bin/bnetw_test; \
  # && gcovr -r ../../ -e ".*/test/.*"; \ 
  # && gcovr -r ../../ -e ".*/test/.*" --html --html-details -o coverage.html; \
  # && gcovr -r ../ . -e ".*\.hpp" -e ".*/test/.*"; \
  fi
