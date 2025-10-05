
# Convert ca.crt to JKS truststore
keytool -import -alias kafka-ca -file ca.crt -keystore kafka.truststore.jks -storepass changeit -noprompt


# Build with Maven
mvn clean package

# Run Producer
mvn exec:java -Dexec.mainClass="com.example.Producer"

# Run Consumer
mvn exec:java -Dexec.mainClass="com.example.Consumer"

# Or with Gradle
./gradlew run --args="producer"
./gradlew run --args="consumer"