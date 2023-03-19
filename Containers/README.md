# Containerization

## Proxy
```bash
$ kubectl apply -f zmq-proxy.yaml
```

Note: this is a ClusterIP service 

## Stack A
Deploy the `zmq-stack` container with a ConfigMap as "stackA":
```bash
$ kubectl apply -f zmq-stackA.yaml
```

## Stack B
Deploy the `zmq-stack` container with a ConfigMap as "stackB":
```bash
$ kubectl apply -f zmq-stackB.yaml
```

## Converter
Deploy the `conv-stack` container with a ConfigMap as "converter":
```bash
$ kubectl apply -f conv-stack.yaml
```

## Netstack A
Deploy the `net-stack` container with a ConfigMap as "net-stack-a":
```bash
$ kubectl apply -f net-stack-a.yaml
```

NodePort service listening on port 30000

## Netstack B
Deply the `net-stack` container with a ConfigMap as "net-stack-b":
```bash
$ kubectl apply -f net-stack-b.yaml
```

NodePort service listening on port 30001

## Topics
- stack-a-ingress (from stackA to converter)
- stack-a-egress (from converter to stackA)
- stack-b-ingress (from stackB to converter)
- stack-b-egress (from converter to stackB)
- net-stack-a-ingress (from net-stack-a to converter)
- net-stack-a-egress (from converter to net-stack-a)
- net-stack-b-ingress (from net-stack-b to converter)
- net-stack-b-egress (from converter to net-stack-b)