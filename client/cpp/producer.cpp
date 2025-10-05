#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <librdkafka/rdkafkacpp.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
    void dr_cb(RdKafka::Message &message) {
        if (message.err()) {
            std::cerr << "Message delivery failed: " << message.errstr() << std::endl;
        } else {
            std::cout << "Sent: Message delivered to partition " << message.partition()
                      << " at offset " << message.offset() << std::endl;
        }
    }
};

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm = *std::gmtime(&now_time_t);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm);
    
    return std::string(buffer) + "." + std::to_string(now_ms.count()) + "Z";
}

int main() {
    std::string brokers = "10.0.0.207:9094";
    std::string topic = "my-topic";

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    
    std::string errstr;
    
    // Basic configuration
    conf->set("bootstrap.servers", brokers, errstr);
    conf->set("client.id", "cpp-producer", errstr);
    
    // SASL SCRAM-SHA-512
    conf->set("security.protocol", "SASL_SSL", errstr);
    conf->set("sasl.mechanism", "SCRAM-SHA-512", errstr);
    conf->set("sasl.username", "kafka-admin", errstr);
    conf->set("sasl.password", "...pw...", errstr);
    
    // TLS
    conf->set("ssl.ca.location", "ca.crt", errstr);
    
    // Delivery report callback
    DeliveryReportCb dr_cb;
    conf->set("dr_cb", &dr_cb, errstr);

    // Create producer
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        delete conf;
        return 1;
    }

    delete conf;

    // Send messages
    for (int i = 0; i < 10; i++) {
        json message = {
            {"id", i},
            {"timestamp", getCurrentTimestamp()},
            {"message", "Hello Kafka #" + std::to_string(i)}
        };

        std::string payload = message.dump();
        std::string key = "key-" + std::to_string(i);

        std::cout << "Producing: " << message.dump() << std::endl;

        RdKafka::ErrorCode resp = producer->produce(
            topic,                              // topic name
            RdKafka::Topic::PARTITION_UA,       // partition (unassigned)
            RdKafka::Producer::RK_MSG_COPY,     // message flags
            const_cast<char *>(payload.c_str()), // payload
            payload.size(),                     // payload size
            const_cast<char *>(key.c_str()),    // key
            key.size(),                         // key size
            0,                                  // timestamp (0 = now)
            nullptr                             // message opaque
        );

        if (resp != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to produce: " << RdKafka::err2str(resp) << std::endl;
        }

        producer->poll(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait for any outstanding messages to be delivered
    std::cout << "\nFlushing final messages..." << std::endl;
    producer->flush(10000);

    std::cout << "All messages sent!" << std::endl;

    delete producer;
    return 0;
}