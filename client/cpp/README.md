# Setup
```bash
# macOS
brew install librdkafka

# Ubuntu/Debian
sudo apt-get install librdkafka-dev

# Or build from source
git clone https://github.com/confluentinc/librdkafka.git
cd librdkafka
./configure
make
sudo make install

# macOS
brew install nlohmann-json

# Ubuntu/Debian
sudo apt-get install nlohmann-json3-dev
```

# Run
```bash
make

# Run producer
./producer

# Run consumer (in another terminal)
./consumer
```