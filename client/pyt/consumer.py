from kafka import KafkaConsumer
import json

consumer = KafkaConsumer(
    'my-topic',
    bootstrap_servers=['10.0.0.207:9094'],
    security_protocol='SASL_SSL',
    sasl_mechanism='SCRAM-SHA-512',
    sasl_plain_username='kafka-admin',
    sasl_plain_password='...pw...',
    ssl_cafile='ca.crt',
    value_deserializer=lambda m: json.loads(m.decode('utf-8')),
    auto_offset_reset='earliest',
    group_id='my-consumer-group'
)

print("Waiting for messages...")
for message in consumer:
    print(f"Partition: {message.partition}, Offset: {message.offset}")
    print(f"Value: {message.value}\n")