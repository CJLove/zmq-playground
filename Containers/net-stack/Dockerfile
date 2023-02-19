FROM registry.fedoraproject.org/fedora-minimal:37

WORKDIR /net-stack

COPY . .

RUN \
    microdnf install -y zeromq


CMD  ./net-stack -f ./config/net-stack.yaml
