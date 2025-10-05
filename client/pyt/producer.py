from kafka import KafkaProducer
import json
from datetime import datetime

producer = KafkaProducer(
    bootstrap_servers=['10.0.0.207:9094'],  # Your bootstrap IP
    security_protocol='SASL_SSL',
    sasl_mechanism='SCRAM-SHA-512',
    sasl_plain_username='kafka-admin',
    sasl_plain_password='...pw...',
    ssl_cafile='ca.crt',
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)

# Send messages
for i in range(10):
    message = {
        'id': i,
        'timestamp': datetime.now().isoformat(),
        'message': f'Hello Kafka #{i}'
    }
    future = producer.send('my-topic', value=message)
    result = future.get(timeout=10)
    print(f"Sent: {message} -> Partition: {result.partition}, Offset: {result.offset}")

producer.flush()
producer.close()