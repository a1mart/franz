#!/bin/bash

# producer.sh - Send messages to Kafka

BROKER="10.0.0.207:9094"
TOPIC="my-topic"
USERNAME="kafka-admin"
PASSWORD="...pw..."
CA_CERT="ca.crt"

# Send 10 messages
for i in {0..9}; do
  MESSAGE=$(cat <<EOF
{
  "id": $i,
  "timestamp": "$(date -u +"%Y-%m-%dT%H:%M:%S.%3NZ")",
  "message": "Hello Kafka #$i"
}
EOF
)
  
  echo "$MESSAGE" | kcat -P \
    -b "$BROKER" \
    -t "$TOPIC" \
    -X security.protocol=SASL_SSL \
    -X sasl.mechanism=SCRAM-SHA-512 \
    -X sasl.username="$USERNAME" \
    -X sasl.password="$PASSWORD" \
    -X ssl.ca.location="$CA_CERT"
  
  echo "Sent message #$i"
  sleep 0.5
done

echo "All messages sent!"