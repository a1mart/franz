#!/bin/bash

# consumer.sh - Consume messages from Kafka

BROKER="10.0.0.207:9094"
TOPIC="my-topic"
USERNAME="kafka-admin"
PASSWORD="...pw..."
CA_CERT="ca.crt"
GROUP_ID="my-consumer-group"

echo "Waiting for messages..."

kcat -C \
  -b "$BROKER" \
  -X security.protocol=SASL_SSL \
  -X sasl.mechanism=SCRAM-SHA-512 \
  -X sasl.username="$USERNAME" \
  -X sasl.password="$PASSWORD" \
  -X ssl.ca.location="$CA_CERT" \
  -X auto.offset.reset=earliest \
  -G "$GROUP_ID" "$TOPIC"