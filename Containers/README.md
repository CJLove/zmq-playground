# Containerization

## Proxy
```bash
$ kubectl apply -f zmq-proxy.yaml
$ kubectl apply -f zmq-proxy-service.yaml
```

Note: currently this exposes an external service, which allows zmq apps outside of the K8S cluster to reach the proxy.



## Stack A
Deploy the `zmq-stack` container with a ConfigMap as "stackA":
```bash
$ kubectl apply -f zmq-stackA-config-map.yaml
$ kubectl apply -f zmq-stackA.yaml
```

## Stack B
Deploy the `zmq-stack` container with a ConfigMap as "stackB":
```bash
$ kubectl apply -f zmq-stackB-config-map.yaml
$ kubectl apply -f zmq-stackB.yaml
```

## Converter
Deploy the `conv-stack` container with a ConfigMap as "converter":
```bash
$ kubectl apply -f conv-stack-config-map.yaml
$ kubectl apply -f conv-stack.yaml
```

## Topics
- stack-a-ingress (from stackA to converter)
- stack-a-egress (from converter to stackA)
- stack-b-ingress (from stackB to converter)
- stack-b-egress (from converter to stackB)