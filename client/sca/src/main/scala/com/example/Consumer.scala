package com.example

import org.apache.kafka.clients.consumer._
import org.apache.kafka.common.serialization.StringDeserializer
import io.circe.parser._
import java.time.Duration
import java.util.Properties
import scala.jdk.CollectionConverters._

object Consumer extends App {
  val topic = "my-topic"
  val bootstrapServers = "10.0.0.207:9094"
  val groupId = "my-consumer-group"

  val props = new Properties()
  props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers)
  props.put(ConsumerConfig.GROUP_ID_CONFIG, groupId)
  props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, classOf[StringDeserializer].getName)
  props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, classOf[StringDeserializer].getName)
  props.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "earliest")
  props.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "true")
  
  // SASL SCRAM-SHA-512
  props.put("security.protocol", "SASL_SSL")
  props.put("sasl.mechanism", "SCRAM-SHA-512")
  props.put("sasl.jaas.config",
    """org.apache.kafka.common.security.scram.ScramLoginModule required
      |username="kafka-admin"
      |password="...pw...";""".stripMargin)
  
  // TLS
  props.put("ssl.truststore.location", "kafka.truststore.jks")
  props.put("ssl.truststore.password", "changeit")

  val consumer = new KafkaConsumer[String, String](props)
  consumer.subscribe(List(topic).asJava)
  
  println("Waiting for messages...")

  try {
    while (true) {
      val records = consumer.poll(Duration.ofMillis(100))
      
      records.asScala.foreach { record =>
        decode[Message](record.value()) match {
          case Right(message) =>
            println(s"Partition: ${record.partition()}, Offset: ${record.offset()}")
            println(s"Value: $message\n")
          case Left(error) =>
            System.err.println(s"Error parsing message: ${error.getMessage}")
        }
      }
    }
  } catch {
    case e: Exception => e.printStackTrace()
  } finally {
    consumer.close()
  }
}