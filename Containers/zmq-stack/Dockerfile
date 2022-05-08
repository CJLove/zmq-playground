FROM registry.fedoraproject.org/fedora-minimal:34

WORKDIR /zmq-stack

COPY . .

RUN \
    microdnf install -y zeromq

CMD ./zmq-stack -f ./config/zmq-stack.yaml
