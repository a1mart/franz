# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project does not yet adhere to [Semantic Versioning](https://semver.org/).

---

## [Unreleased]

- Add Docker Compose setup for local Kafka cluster
- Add performance benchmarking scripts
- Expand examples with Avro/Protobuf schema serialization

---

## [0.1.0] - 2025-10-04

### Added
- Initial release of **Franz - Multi-Language Kafka Client Examples**
- **Common Configurations**:
  - SASL_SSL (SCRAM-SHA-512) authentication
  - Shared topic `my-topic` and consumer group `my-consumer-group`
  - JSON message format `{ id, timestamp, message }`
- **Languages Implemented**:
  - **Python** (`pyt/`) using `kafka-python`
  - **Bash** (`bas/`) using `kcat/kafkacat`
  - **Go** (`gol/`) using `Sarama`
  - **Rust** (`rus/`) using `rdkafka`
  - **Node.js/TypeScript** (`ts/`) using `KafkaJS`
  - **Java** (`jav/`) using Apache Kafka clients (Maven)
  - **Scala** (`sca/`) using Apache Kafka clients (sbt)
  - **C++** (`cpp/`) using librdkafka C++ API
  - **C** (`c/`) using librdkafka C API
  - **C#** (`csh/`) using `Confluent.Kafka`
- **Deployment Configurations**:
  - Kubernetes manifests for Kafka cluster (`deploy/cluster.yaml`)
  - Topic creation manifests (`deploy/topics.yaml`)
- **Setup Guides**:
  - Installation instructions per language
  - Java truststore creation steps
- **Troubleshooting Documentation**:
  - Java 23 security manager workaround
  - SSL certificate validation issues
  - Connection testing with `nc`
  - SASL authentication failure tips

### Notes
- Tested against **Kafka 4.1.0** with broker at `10.0.0.207:9094`
- Requires valid `ca.crt` for TLS and SASL credentials (`kafka-admin`)
- JVM clients (Java/Scala) require special handling on **Java 23**

---

## [0.0.1] - 2025-09-15

### Prototype
- Verified SASL_SSL connectivity with Python and Bash
- Produced and consumed test messages across two languages
- Established project structure and design goals
