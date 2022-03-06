FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq_producer /

RUN \
    microdnf install -y zeromq 

CMD  /zmq_producer -p "tcp://zmq_proxy:9200" -s "tcp://zmq_proxy:9210" -r REQ1 -R RESP1