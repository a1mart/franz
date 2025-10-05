CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lrdkafka -lcjson

all: producer consumer

producer: producer.c
	$(CC) $(CFLAGS) -o producer producer.c $(LDFLAGS)

consumer: consumer.c
	$(CC) $(CFLAGS) -o consumer consumer.c $(LDFLAGS)

clean:
	rm -f producer consumer

.PHONY: all clean