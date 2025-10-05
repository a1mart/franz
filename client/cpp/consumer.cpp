#include <iostream>
#include <string>
#include <csignal>
#include <librdkafka/rdkafkacpp.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static volatile sig_atomic_t run = 1;

static void sigterm(int sig) {
    run = 0;
}

class ConsumerRebalanceCb : public RdKafka::RebalanceCb {
public:
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
                      RdKafka::ErrorCode err,
                      std::vector<RdKafka::TopicPartition*> &partitions) {
        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
        } else {
            consumer->unassign();
        }
    }
};

int main() {
    std::string brokers = "10.0.0.207:9094";
    std::string topic = "my-topic";
    std::string group_id = "my-consumer-group";

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    
    std::string errstr;
    
    // Basic configuration
    conf->set("bootstrap.servers", brokers, errstr);
    conf->set("group.id", group_id, errstr);
    conf->set("client.id", "cpp-consumer", errstr);
    conf->set("auto.offset.reset", "earliest", errstr);
    conf->set("enable.auto.commit", "true", errstr);
    
    // SASL SCRAM-SHA-512
    conf->set("security.protocol", "SASL_SSL", errstr);
    conf->set("sasl.mechanism", "SCRAM-SHA-512", errstr);
    conf->set("sasl.username", "kafka-admin", errstr);
    conf->set("sasl.password", "...pw...", errstr);
    
    // TLS
    conf->set("ssl.ca.location", "ca.crt", errstr);
    
    // Rebalance callback
    ConsumerRebalanceCb rebalance_cb;
    conf->set("rebalance_cb", &rebalance_cb, errstr);

    // Create consumer
    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        delete conf;
        return 1;
    }

    delete conf;

    // Subscribe to topic
    std::vector<std::string> topics;
    topics.push_back(topic);
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err) {
        std::cerr << "Failed to subscribe: " << RdKafka::err2str(err) << std::endl;
        delete consumer;
        return 1;
    }

    std::cout << "Waiting for messages..." << std::endl;

    // Consume messages
    while (run) {
        RdKafka::Message *msg = consumer->consume(1000);
        
        switch (msg->err()) {
            case RdKafka::ERR_NO_ERROR: {
                std::string payload(static_cast<const char*>(msg->payload()), msg->len());
                
                try {
                    json j = json::parse(payload);
                    std::cout << "Partition: " << msg->partition()
                              << ", Offset: " << msg->offset() << std::endl;
                    std::cout << "Value: " << j.dump(2) << std::endl << std::endl;
                } catch (json::parse_error& e) {
                    std::cerr << "JSON parse error: " << e.what() << std::endl;
                }
                break;
            }
            case RdKafka::ERR__TIMED_OUT:
                // No message within timeout, continue
                break;
            case RdKafka::ERR__PARTITION_EOF:
                // Reached end of partition
                break;
            default:
                std::cerr << "Consumer error: " << msg->errstr() << std::endl;
                run = 0;
                break;
        }
        
        delete msg;
    }

    // Close consumer
    consumer->close();
    delete consumer;

    std::cout << "Consumer closed" << std::endl;
    return 0;
}