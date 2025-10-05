const { Kafka, Partitioners } = require('kafkajs');
const fs = require('fs');

async function main() {
  const kafka = new Kafka({
    clientId: 'my-producer',
    brokers: ['10.0.0.207:9094'],
    ssl: {
      rejectUnauthorized: true,
      ca: [fs.readFileSync('./ca.crt', 'utf-8')],
    },
    sasl: {
      mechanism: 'scram-sha-512',
      username: 'kafka-admin',
      password: '...pw...',
    },
  });

  const producer = kafka.producer({
    createPartitioner: Partitioners.DefaultPartitioner,
  });

  await producer.connect();
  console.log('Producer connected');

  try {
    for (let i = 0; i < 10; i++) {
      const message = {
        id: i,
        timestamp: new Date().toISOString(),
        message: `Hello Kafka #${i}`,
      };

      const result = await producer.send({
        topic: 'my-topic',
        messages: [
          {
            key: `key-${i}`,
            value: JSON.stringify(message),
          },
        ],
      });

      console.log(
        `Sent: ${JSON.stringify(message)} -> Partition: ${result[0].partition}, Offset: ${result[0].baseOffset}`
      );

      await new Promise((resolve) => setTimeout(resolve, 100));
    }

    console.log('All messages sent!');
  } catch (error) {
    console.error('Error sending message:', error);
  } finally {
    await producer.disconnect();
  }
}

main().catch(console.error);