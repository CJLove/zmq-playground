FROM registry.fedoraproject.org/fedora-minimal:37

WORKDIR /conv-stack

COPY . .

RUN \
    microdnf install -y zeromq


CMD  ./conv-stack -f ./config/conv-stack.yaml
