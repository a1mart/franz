# Franz - Multi-Language Kafka Client Examples

A comprehensive collection of Kafka producers and consumers implemented in 9 different programming languages, all configured with SASL_SSL (SCRAM-SHA-512) authentication. Produce an event from one language and consume it with another.

## Languages Included

- **Python** (`pyt/`) - kafka-python library
- **Bash** (`bas/`) - kcat/kafkacat CLI tool
- **Go** (`gol/`) - Sarama library
- **Rust** (`rus/`) - rdkafka library
- **Node.js/TypeScript** (`ts/`) - kafkajs library
- **Java** (`jav/`) - Apache Kafka clients (Maven)
- **Scala** (`sca/`) - Apache Kafka clients (sbt)
- **C++** (`cpp/`) - librdkafka C++ API
- **C** (`c/`) - librdkafka C API
- **C#** (`csh/`) - Confluent.Kafka library

## Prerequisites

### Common Requirements

- Kafka cluster running version 4.1.0
- CA certificate file (`ca.crt`) for TLS/SSL
- For Java/Scala: JKS truststore (`kafka.truststore.jks`)
- Kafka broker accessible at `10.0.0.207:9094`
- SASL credentials: `kafka-admin` / `...pw...`

### Creating Java Truststore

```bash
keytool -import -alias kafka-ca -file ca.crt \
  -keystore kafka.truststore.jks -storepass changeit -noprompt
```

## Installation & Setup

### Python

```bash
cd pyt
pip install kafka-python
python producer.py
python consumer.py
```

### Bash (kcat)

```bash
cd bas
# macOS
brew install kcat

# Ubuntu/Debian
sudo apt-get install kafkacat

./producer.sh
./consumer.sh
```

### Go

```bash
cd gol
go mod init kafka-go-client
go get github.com/IBM/sarama
go get github.com/xdg-go/scram
go run producer.go scram.go
go run consumer.go scram.go
```

### Rust

```bash
cd rus
cargo build --release
cargo run --bin producer
cargo run --bin consumer
```

### Node.js/TypeScript

```bash
cd ts
npm install
npm run producer
npm run consumer
```

### Java

```bash
cd jav
mvn clean package
mvn exec:java -Dexec.mainClass="com.example.Producer"
mvn exec:java -Dexec.mainClass="com.example.Consumer"
```

### Scala

**Note:** Requires Java 17 or use security manager flag with Java 23+

```bash
cd sca
sbt compile

# With Java 23
sbt -J-Djava.security.manager=allow "runMain com.example.Producer"
sbt -J-Djava.security.manager=allow "runMain com.example.Consumer"

# With Java 17
export JAVA_HOME=/path/to/java17
sbt "runMain com.example.Producer"
sbt "runMain com.example.Consumer"
```

### C++

```bash
cd cpp
# Install dependencies
brew install librdkafka nlohmann-json  # macOS
# sudo apt-get install librdkafka-dev nlohmann-json3-dev  # Ubuntu

make
./producer
./consumer
```

### C

```bash
cd c
# Install dependencies
brew install librdkafka cjson  # macOS
# sudo apt-get install librdkafka-dev libcjson-dev  # Ubuntu

make
./producer
./consumer
```

### C#

```bash
cd csh
dotnet restore
dotnet run --project Producer
dotnet run --project Consumer
```

## Configuration

All clients are configured with:

- **Bootstrap Server:** `10.0.0.207:9094`
- **Topic:** `my-topic`
- **Consumer Group:** `my-consumer-group`
- **Security Protocol:** SASL_SSL
- **SASL Mechanism:** SCRAM-SHA-512
- **Username:** `kafka-admin`
- **Password:** `...pw...`
- **SSL CA Location:** `ca.crt` (or `kafka.truststore.jks` for Java/Scala)

## Message Format

All producers send messages in the following JSON format:

```json
{
  "id": 0,
  "timestamp": "2025-10-04T17:00:00.123Z",
  "message": "Hello Kafka #0"
}
```

## Usage

### Running Producers

Producers send 10 test messages to the `my-topic` topic:

```bash
# Choose your language
python pyt/producer.py           # Python
./bas/producer.sh                # Bash
go run gol/producer.go gol/scram.go  # Go
cargo run --bin producer         # Rust (from rus/)
npm run producer                 # Node.js (from ts/)
mvn exec:java -Dexec.mainClass="com.example.Producer"  # Java (from jav/)
sbt "runMain com.example.Producer"  # Scala (from sca/)
./cpp/producer                   # C++
./c/producer                     # C
dotnet run --project csh/Producer  # C#
```

### Running Consumers

Consumers listen for messages from the `my-topic` topic:

```bash
# Choose your language
python pyt/consumer.py           # Python
./bas/consumer.sh                # Bash
go run gol/consumer.go gol/scram.go  # Go
cargo run --bin consumer         # Rust (from rus/)
npm run consumer                 # Node.js (from ts/)
mvn exec:java -Dexec.mainClass="com.example.Consumer"  # Java (from jav/)
sbt "runMain com.example.Consumer"  # Scala (from sca/)
./cpp/consumer                   # C++
./c/consumer                     # C
dotnet run --project csh/Consumer  # C#
```

## Troubleshooting

### Java 23 Security Manager Issue

If you see `getSubject is supported only if a security manager is allowed`:

```bash
# Add JVM flag
sbt -J-Djava.security.manager=allow "runMain com.example.Producer"

# Or use Java 17
export JAVA_HOME=/path/to/java17
```

### SSL Certificate Issues

Ensure `ca.crt` is in the correct directory and readable:

```bash
ls -la ca.crt
# Should show the certificate file exists
```

### Connection Refused

Verify Kafka broker is accessible:

```bash
nc -zv 10.0.0.207 9094
# Should show "succeeded!"
```

### SASL Authentication Failure

Double-check credentials and ensure the user has proper ACLs configured in Kafka.

## Project Structure

```
franz/
├── README.md                    # This file
├── deploy/                      # Kubernetes deployment configs
│   ├── cluster.yaml
│   └── topics.yaml
└── client/                      # Client implementations
    ├── ca.crt                   # SSL CA certificate
    ├── kafka.truststore.jks     # Java truststore
    ├── pyt/                     # Python
    ├── bas/                     # Bash
    ├── gol/                     # Go
    ├── rus/                     # Rust
    ├── ts/                      # TypeScript/Node.js
    ├── jav/                     # Java
    ├── sca/                     # Scala
    ├── cpp/                     # C++
    ├── c/                       # C
    └── csh/                     # C#
```

## Performance Notes

- **C/C++:** Lowest latency, highest throughput
- **Rust/Go:** Near-native performance, excellent concurrency
- **Java/Scala/C#:** JVM/.NET optimized, good for enterprise
- **Node.js/Python:** Easiest to use, sufficient for most use cases
- **Bash:** Best for quick testing and ops automation

## License

This project is for demonstration purposes. Adjust security credentials and configuration for production use.

## Additional Resources

- [Apache Kafka Documentation](https://kafka.apache.org/documentation/)
- [Confluent Platform](https://docs.confluent.io/)
- [librdkafka](https://github.com/confluentinc/librdkafka)
- [Sarama (Go)](https://github.com/IBM/sarama)
- [KafkaJS](https://kafka.js.org/)