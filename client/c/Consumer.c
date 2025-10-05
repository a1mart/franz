#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <librdkafka/rdkafka.h>
#include <cjson/cJSON.h>

static volatile sig_atomic_t run = 1;

void sigterm(int sig) {
    run = 0;
}

int main() {
    rd_kafka_t *consumer;
    rd_kafka_conf_t *conf;
    rd_kafka_topic_partition_list_t *topics;
    char errstr[512];
    
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    
    conf = rd_kafka_conf_new();
    
    // Basic configuration
    rd_kafka_conf_set(conf, "bootstrap.servers", "10.0.0.207:9094", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", "my-consumer-group", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "client.id", "c-consumer", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "enable.auto.commit", "true", errstr, sizeof(errstr));
    
    // SASL SCRAM-SHA-512
    rd_kafka_conf_set(conf, "security.protocol", "SASL_SSL", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.mechanism", "SCRAM-SHA-512", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.username", "kafka-admin", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.password", "...pw...", errstr, sizeof(errstr));
    
    // TLS
    rd_kafka_conf_set(conf, "ssl.ca.location", "ca.crt", errstr, sizeof(errstr));
    
    // Create consumer
    consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!consumer) {
        fprintf(stderr, "Failed to create consumer: %s\n", errstr);
        return 1;
    }
    
    // Subscribe to topic
    topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, "my-topic", RD_KAFKA_PARTITION_UA);
    
    rd_kafka_resp_err_t err = rd_kafka_subscribe(consumer, topics);
    if (err) {
        fprintf(stderr, "Failed to subscribe: %s\n", rd_kafka_err2str(err));
        rd_kafka_topic_partition_list_destroy(topics);
        rd_kafka_destroy(consumer);
        return 1;
    }
    
    rd_kafka_topic_partition_list_destroy(topics);
    
    printf("Waiting for messages...\n");
    
    // Consume messages
    while (run) {
        rd_kafka_message_t *msg = rd_kafka_consumer_poll(consumer, 1000);
        
        if (!msg)
            continue;
        
        if (msg->err) {
            if (msg->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                // Reached end of partition
            } else {
                fprintf(stderr, "Consumer error: %s\n", rd_kafka_message_errstr(msg));
                run = 0;
            }
        } else {
            // Parse JSON
            cJSON *json = cJSON_ParseWithLength((const char *)msg->payload, msg->len);
            if (json) {
                printf("Partition: %d, Offset: %lld\n", 
                        msg->partition, (long long)msg->offset);
                
                char *json_str = cJSON_Print(json);
                printf("Value: %s\n\n", json_str);
                
                free(json_str);
                cJSON_Delete(json);
            } else {
                fprintf(stderr, "Failed to parse JSON\n");
            }
        }
        
        rd_kafka_message_destroy(msg);
    }
    
    // Close consumer
    rd_kafka_consumer_close(consumer);
    rd_kafka_destroy(consumer);
    
    printf("Consumer closed\n");
    return 0;
}