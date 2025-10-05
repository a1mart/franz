CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lrdkafka++

all: producer consumer

producer: producer.cpp
	$(CXX) $(CXXFLAGS) -o producer producer.cpp $(LDFLAGS)

consumer: consumer.cpp
	$(CXX) $(CXXFLAGS) -o consumer consumer.cpp $(LDFLAGS)

clean:
	rm -f producer consumer

.PHONY: all clean