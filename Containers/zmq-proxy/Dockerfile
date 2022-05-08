FROM registry.fedoraproject.org/fedora-minimal:34

WORKDIR /zmq-proxy

COPY . .

RUN \
    microdnf install -y zeromq

CMD  ./zmq-proxy -f ./config/zmq-proxy.yaml
