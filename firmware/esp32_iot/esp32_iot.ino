#include <WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "iot.eie.ucr.ac.cr";
const int mqtt_port = 1883;
const char* clientId = "DoesntMatter";
const char* user = "6coq0bk8am03mfc0hf5s";
const char* pass = ""; // No password needed

WiFiClient wifi;
PubSubClient client(wifi);

void setup_wifi() {
  WiFi.begin("Labo201", "labo201!");
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

int reconnect() {
  Serial.println("Reconnect requested");
  if (client.connected()) {
    Serial.println("MQTT client is still connected");
    return 0;
  }
  
  Serial.print("Reconnecting to MQTT server... ");
  if (client.connect(clientId, user, pass)) {
    Serial.println("connected");
    client.subscribe("v1/devices/me/attributes/request/1/");
    Serial.println("resubscribed");
    return 0;
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    delay(5000);
    return client.state();
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  float temperature = random(200, 301) / 10.0;

  int err = reconnect();
  if (err != 0) {
    // Buffer the measurement to send next time
  } else {
    client.loop(); // Process incoming messages and maintain connection to server
    
    String json = "{\"temperature\":" + String(temperature, 1) + "}";
    Serial.println("Publishing: " + json);
    if (client.publish("v1/devices/me/telemetry", json.c_str())) {
      Serial.println("Publish success");
    } else {
      Serial.println("Publish failed");
    }
  }
  delay(2000);
}