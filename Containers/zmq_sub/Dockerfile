FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq_sub /

RUN \
    microdnf install -y zeromq 

CMD  /zmq_sub -s "tcp://zmq_pub:9300" -t TOPIC1