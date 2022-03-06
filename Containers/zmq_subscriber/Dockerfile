FROM registry.fedoraproject.org/fedora-minimal:34

ADD zmq_subscriber /

RUN \
    microdnf install -y zeromq 

CMD /zmq_subscriber -p "tcp://zmq_proxy:9200" -s "tcp://zmq_proxy:9210" -r REQ1 -R RESP1
