FROM registry.fedoraproject.org/fedora-minimal:34

ADD conv-stack /

RUN \
    microdnf install -y zeromq


CMD  /conv-stack -n conv -p tcp://localhost:9200 -s tcp://localhost:9210 
