package com.example;

import com.fasterxml.jackson.annotation.JsonProperty;
import java.time.Instant;

public class Message {
    @JsonProperty("id")
    private int id;
    
    @JsonProperty("timestamp")
    private String timestamp;
    
    @JsonProperty("message")
    private String message;

    public Message() {}

    public Message(int id, String timestamp, String message) {
        this.id = id;
        this.timestamp = timestamp;
        this.message = message;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return "Message{" +
                "id=" + id +
                ", timestamp='" + timestamp + '\'' +
                ", message='" + message + '\'' +
                '}';
    }
}