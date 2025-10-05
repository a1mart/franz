package main

import (
	"crypto/tls"
	"crypto/x509"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"time"

	"github.com/IBM/sarama"
)

type Message struct {
	ID        int       `json:"id"`
	Timestamp time.Time `json:"timestamp"`
	Message   string    `json:"message"`
}

func main() {
	config := sarama.NewConfig()
	config.Version = sarama.V4_0_0_0
	config.Producer.Return.Successes = true
	config.Producer.RequiredAcks = sarama.WaitForAll

	// SASL SCRAM-SHA-512
	config.Net.SASL.Enable = true
	config.Net.SASL.Mechanism = sarama.SASLTypeSCRAMSHA512
	config.Net.SASL.User = "kafka-admin"
	config.Net.SASL.Password = "...pw..."
	config.Net.SASL.SCRAMClientGeneratorFunc = func() sarama.SCRAMClient { return &XDGSCRAMClient{HashGeneratorFcn: SHA512} }

	// TLS
	config.Net.TLS.Enable = true
	tlsConfig, err := createTLSConfig("ca.crt")
	if err != nil {
		log.Fatal(err)
	}
	config.Net.TLS.Config = tlsConfig

	producer, err := sarama.NewSyncProducer([]string{"10.0.0.207:9094"}, config)
	if err != nil {
		log.Fatalf("Failed to create producer: %v", err)
	}
	defer producer.Close()

	for i := 0; i < 10; i++ {
		msg := Message{
			ID:        i,
			Timestamp: time.Now(),
			Message:   fmt.Sprintf("Hello Kafka #%d", i),
		}

		jsonBytes, err := json.Marshal(msg)
		if err != nil {
			log.Printf("Failed to marshal message: %v", err)
			continue
		}

		partition, offset, err := producer.SendMessage(&sarama.ProducerMessage{
			Topic: "my-topic",
			Value: sarama.ByteEncoder(jsonBytes),
		})

		if err != nil {
			log.Printf("Failed to send message: %v", err)
		} else {
			fmt.Printf("Sent: %+v -> Partition: %d, Offset: %d\n", msg, partition, offset)
		}
	}
}

func createTLSConfig(caFile string) (*tls.Config, error) {
	caCert, err := os.ReadFile(caFile)
	if err != nil {
		return nil, err
	}

	caCertPool := x509.NewCertPool()
	caCertPool.AppendCertsFromPEM(caCert)

	return &tls.Config{
		RootCAs: caCertPool,
	}, nil
}
