FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq_pub /

RUN \
    microdnf install -y zeromq 

CMD  /zmq_pub -p 9300 -t TOPIC1