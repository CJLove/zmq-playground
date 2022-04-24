FROM registry.fedoraproject.org/fedora-minimal:34

ADD conv-stack /

RUN \
    microdnf install -y zeromq


CMD  /conv-stack -n conv -p tcp://localhost:9200 -s tcp://localhost:9210 -S jreap.ingress.jreap1 -S xtor.jreap1.uci -P "jreap.ingress.jreap1|platform.jreap1.uci" -P "xtor.jreap1.uci|jreap.egress.jreap1"
