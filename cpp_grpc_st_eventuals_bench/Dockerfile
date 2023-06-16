# FROM gcr.io/bazel-public/bazel:5.1.1
FROM ubuntu:jammy

USER root

ARG PYTHON_VERSION=3.10

# Installs `python-is-python3` to make sure we're using python3 (not python2).
RUN apt-get update && apt-get install -y --no-install-recommends \
    python-is-python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/* \
    # Confirm that python3 is installed and that it has the expected version.
    && python3 --version | grep -Eq "^Python ${PYTHON_VERSION}.*" \
    # And that "python" (aka python3) is installed and has the expected version.
    && python --version | grep -Eq "^Python ${PYTHON_VERSION}.*"

RUN apt-get update && apt-get install -y --no-install-recommends autoconf clang wget git zlib1g-dev

WORKDIR /app
COPY . /app/src

RUN rm -rf /usr/bin/python && ln -s /usr/bin/python3 /usr/bin/python

# Bazel version specified at .bazelversion (5.1.1)
RUN wget -O /usr/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/v1.17.0/bazelisk-linux-amd64
RUN chmod +x /usr/bin/bazel

WORKDIR /app/src/cpp_grpc_st_eventuals_bench

# RUN export EVENTUALS_GRPC_LOG=1
RUN bazel build :test

# RUN ls && ls bazel-bin

ENTRYPOINT bazel-bin/test