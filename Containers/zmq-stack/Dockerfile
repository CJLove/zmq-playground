FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq-proxy /

RUN \
    microdnf install -y zeromq


CMD  /zmq-proxy -i
