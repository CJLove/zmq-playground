FROM registry.fedoraproject.org/fedora-minimal:36

WORKDIR /zmq-proxy

COPY zmq-proxy /zmq-proxy/
COPY config/zmq-proxy.yaml /zmq-proxy/config/
COPY lib* /usr/lib64/

RUN \
    microdnf install -y zeromq

CMD  ./zmq-proxy -f ./config/zmq-proxy.yaml
