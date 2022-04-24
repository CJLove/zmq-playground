FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq-stack /

RUN \
    microdnf install -y zeromq


CMD  /zmq-stack -n stack -p tcp://localhost:9200 -s tcp://localhost:9210
