FROM registry.fedoraproject.org/fedora-minimal:34

WORKDIR /net-stack

COPY . .

RUN \
    microdnf install -y zeromq


CMD  ./net-stack -f ./config/net-stack.yaml
