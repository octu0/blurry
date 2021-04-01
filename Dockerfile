# syntax=docker/dockerfile:experimental
FROM golang:1.16-alpine as goget

COPY go.mod go.sum /go/src/github.com/octu0/blurry/

RUN set -eux && \
    apk add --no-cache --virtual .build-deps git wget openssh-client && \
    cd /go/src/github.com/octu0/blurry && \
    go mod download && \
    apk del .build-deps

# ----------------------------------
FROM golang:1.16 as builder
COPY --from=goget /go/bin/  /go/bin/
COPY --from=goget /go/src/  /go/src/
COPY --from=goget /go/pkg/  /go/pkg/

WORKDIR /build

RUN set -eux && \
    apt-get clean && \
    apt-get update -y && \
    apt-get install -y gcc libc-dev libc++-dev g++ clang wget libtinfo5 libclang-dev

ADD . /go/src/github.com/octu0/blurry/

RUN set -eux && \
    cd /go/src/github.com/octu0/blurry && \
    GOOS=linux GOARCH=amd64 go build -o /build/blurry \
      cmd/bridge/main.go \
      && \
    /build/blurry --version

RUN set -eux && \
    apt-get remove -y gcc libc-dev && \
    apt-get purge -y && \
    apt-get autoclean

# ----------------------------------

FROM golang:1.16

WORKDIR /app
COPY --from=builder /build/blurry /app

RUN set -eux && \
    /app/blurry --version

COPY docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
ENTRYPOINT [ "docker-entrypoint.sh" ]
