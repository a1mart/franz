const { Kafka } = require('kafkajs');
const fs = require('fs');

async function main() {
  const kafka = new Kafka({
    clientId: 'my-consumer',
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

  const consumer = kafka.consumer({
    groupId: 'my-consumer-group',
  });

  await consumer.connect();
  console.log('Consumer connected');

  await consumer.subscribe({
    topic: 'my-topic',
    fromBeginning: true,
  });

  console.log('Waiting for messages...');

  await consumer.run({
    eachMessage: async ({ topic, partition, message }) => {
      try {
        const value = message.value?.toString();
        if (value) {
          const msg = JSON.parse(value);
          console.log(`Partition: ${partition}, Offset: ${message.offset}`);
          console.log(`Value: ${JSON.stringify(msg, null, 2)}\n`);
        }
      } catch (error) {
        console.error('Error parsing message:', error);
      }
    },
  });
}

main().catch(console.error);