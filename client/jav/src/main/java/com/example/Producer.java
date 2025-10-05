package com.example;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import org.apache.kafka.clients.producer.*;
import org.apache.kafka.common.serialization.StringSerializer;

import java.time.Instant;
import java.util.Properties;

public class Producer {
    private static final String TOPIC = "my-topic";
    private static final String BOOTSTRAP_SERVERS = "10.0.0.207:9094";

    public static void main(String[] args) {
        Properties props = new Properties();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, BOOTSTRAP_SERVERS);
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        props.put(ProducerConfig.ACKS_CONFIG, "all");
        
        // SASL SCRAM-SHA-512
        props.put("security.protocol", "SASL_SSL");
        props.put("sasl.mechanism", "SCRAM-SHA-512");
        props.put("sasl.jaas.config", 
            "org.apache.kafka.common.security.scram.ScramLoginModule required " +
            "username=\"kafka-admin\" " +
            "password=\"...pw...\";");
        
        // TLS
        props.put("ssl.truststore.location", "kafka.truststore.jks");
        props.put("ssl.truststore.password", "changeit");

        ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.registerModule(new JavaTimeModule());

        try (KafkaProducer<String, String> producer = new KafkaProducer<>(props)) {
            for (int i = 0; i < 10; i++) {
                Message message = new Message(
                    i,
                    Instant.now().toString(),
                    String.format("Hello Kafka #%d", i)
                );

                String jsonMessage = objectMapper.writeValueAsString(message);
                ProducerRecord<String, String> record = new ProducerRecord<>(
                    TOPIC,
                    String.format("key-%d", i),
                    jsonMessage
                );

                RecordMetadata metadata = producer.send(record).get();
                System.out.printf("Sent: %s -> Partition: %d, Offset: %d%n",
                    message, metadata.partition(), metadata.offset());

                Thread.sleep(100);
            }
            System.out.println("All messages sent!");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}