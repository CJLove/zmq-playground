FROM registry.fedoraproject.org/fedora-minimal:37

WORKDIR /zmq-proxy

COPY . .

RUN \
    microdnf install -y zeromq

CMD  ./zmq-proxy -f ./config/zmq-proxy.yaml
