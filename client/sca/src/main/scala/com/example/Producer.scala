package com.example

import org.apache.kafka.clients.producer._
import org.apache.kafka.common.serialization.StringSerializer
import io.circe.syntax._
import java.time.Instant
import java.util.Properties
import scala.util.{Try, Success, Failure}

object Producer extends App {
  val topic = "my-topic"
  val bootstrapServers = "10.0.0.207:9094"

  val props = new Properties()
  props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers)
  props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, classOf[StringSerializer].getName)
  props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, classOf[StringSerializer].getName)
  props.put(ProducerConfig.ACKS_CONFIG, "all")
  
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

  val producer = new KafkaProducer[String, String](props)

  try {
    (0 until 10).foreach { i =>
      val message = Message(
        id = i,
        timestamp = Instant.now().toString,
        message = s"Hello Kafka #$i"
      )

      val jsonMessage = message.asJson.noSpaces
      val record = new ProducerRecord[String, String](
        topic,
        s"key-$i",
        jsonMessage
      )

      val metadata = producer.send(record).get()
      println(s"Sent: $message -> Partition: ${metadata.partition()}, Offset: ${metadata.offset()}")
      
      Thread.sleep(100)
    }
    println("All messages sent!")
  } catch {
    case e: Exception => e.printStackTrace()
  } finally {
    producer.close()
  }
}