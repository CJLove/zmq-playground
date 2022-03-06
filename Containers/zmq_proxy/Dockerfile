FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq_proxy /

RUN \
    microdnf install -y zeromq


CMD  /zmq_proxy
