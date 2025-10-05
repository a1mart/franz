#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <librdkafka/rdkafka.h>
#include <cjson/cJSON.h>

void get_current_timestamp(char *buffer, size_t size) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    struct tm *tm_info = gmtime(&ts.tv_sec);
    size_t len = strftime(buffer, size, "%Y-%m-%dT%H:%M:%S", tm_info);
    snprintf(buffer + len, size - len, ".%03ldZ", ts.tv_nsec / 1000000);
}

void delivery_report_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err) {
        fprintf(stderr, "Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
    } else {
        printf("Sent: Message delivered to partition %d at offset %lld\n",
                rkmessage->partition, (long long)rkmessage->offset);
    }
}

int main() {
    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    char errstr[512];
    
    conf = rd_kafka_conf_new();
    
    // Basic configuration
    rd_kafka_conf_set(conf, "bootstrap.servers", "10.0.0.207:9094", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "client.id", "c-producer", errstr, sizeof(errstr));
    
    // SASL SCRAM-SHA-512
    rd_kafka_conf_set(conf, "security.protocol", "SASL_SSL", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.mechanism", "SCRAM-SHA-512", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.username", "kafka-admin", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "sasl.password", "...pw...", errstr, sizeof(errstr));
    
    // TLS
    rd_kafka_conf_set(conf, "ssl.ca.location", "ca.crt", errstr, sizeof(errstr));
    
    // Set delivery report callback
    rd_kafka_conf_set_dr_msg_cb(conf, delivery_report_cb);
    
    // Create producer
    producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer) {
        fprintf(stderr, "Failed to create producer: %s\n", errstr);
        return 1;
    }
    
    const char *topic = "my-topic";
    
    // Send messages
    for (int i = 0; i < 10; i++) {
        char timestamp[64];
        get_current_timestamp(timestamp, sizeof(timestamp));
        
        // Create JSON message
        cJSON *json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "id", i);
        cJSON_AddStringToObject(json, "timestamp", timestamp);
        
        char message_text[128];
        snprintf(message_text, sizeof(message_text), "Hello Kafka #%d", i);
        cJSON_AddStringToObject(json, "message", message_text);
        
        char *payload = cJSON_PrintUnformatted(json);
        
        char key[32];
        snprintf(key, sizeof(key), "key-%d", i);
        
        printf("Producing: %s\n", payload);
        
        // Produce message
        rd_kafka_resp_err_t err = rd_kafka_producev(
            producer,
            RD_KAFKA_V_TOPIC(topic),
            RD_KAFKA_V_KEY(key, strlen(key)),
            RD_KAFKA_V_VALUE(payload, strlen(payload)),
            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
            RD_KAFKA_V_END
        );
        
        if (err) {
            fprintf(stderr, "Failed to produce: %s\n", rd_kafka_err2str(err));
        }
        
        free(payload);
        cJSON_Delete(json);
        
        rd_kafka_poll(producer, 0);
        usleep(100000); // 100ms
    }
    
    // Flush messages
    printf("\nFlushing final messages...\n");
    rd_kafka_flush(producer, 10000);
    
    printf("All messages sent!\n");
    
    rd_kafka_destroy(producer);
    return 0;
}