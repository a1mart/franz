# Deploy Kafka Cluster to Kubernetes with Operator

1. Install Operator
```bash
# Create namespace
kubectl create namespace data-streaming

# Add Strimzi Helm repo
helm repo add strimzi https://strimzi.io/charts/
helm repo update

# Install the operator
helm install strimzi-kafka-operator strimzi/strimzi-kafka-operator \
  --namespace data-streaming \
  --set watchNamespaces="{data-streaming}"
```

2. Verify Operator
```bash
kubectl get pods -n data-streaming
# Should see: strimzi-cluster-operator-xxxxx running

kubectl get crd | grep kafka
# Should see multiple kafka CRDs
```

3. Deploy
```bash
# Apply the Kafka cluster configuration
kubectl apply -f cluster.yaml

# Watch the deployment
kubectl get kafka -n data-streaming -w
```

4. Verify Service
```bash
kubectl get svc -n data-streaming

# You should see services like:
# my-cluster-kafka-external-bootstrap    LoadBalancer   10.x.x.x   192.168.1.100
# my-cluster-kafka-0                     LoadBalancer   10.x.x.x   192.168.1.101
# my-cluster-kafka-1                     LoadBalancer   10.x.x.x   192.168.1.102
# my-cluster-kafka-2                     LoadBalancer   10.x.x.x   192.168.1.103
```

5. Create Users and Topics
```bash
# Apply users and topics
kubectl apply -f topics.yaml

# Get the user passwords (stored in secrets)
kubectl get secret my-kafka-user -n data-streaming -o jsonpath='{.data.password}' | base64 -d
kubectl get secret kafka-admin -n data-streaming -o jsonpath='{.data.password}' | base64 -d
```

6. Extract TLS
```bash
# Extract the CA certificate for TLS
kubectl get secret my-cluster-cluster-ca-cert -n data-streaming \
  -o jsonpath='{.data.ca\.crt}' | base64 -d > ca.crt

# Or get the CA password
kubectl get secret my-cluster-cluster-ca-cert -n data-streaming \
  -o jsonpath='{.data.ca\.password}' | base64 -d
```

## Connect from External Clients
```python
from kafka import KafkaProducer

producer = KafkaProducer(
    bootstrap_servers=['192.168.1.100:9094'],
    security_protocol='SASL_SSL',
    sasl_mechanism='SCRAM-SHA-512',
    sasl_plain_username='my-kafka-user',
    sasl_plain_password='YOUR_PASSWORD',
    ssl_cafile='ca.crt'
)
```