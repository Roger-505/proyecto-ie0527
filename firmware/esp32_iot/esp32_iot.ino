#include <TFT_eSPI.h> 
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Thingsboard/WiFi stuff
const char* mqtt_server = "iot.eie.ucr.ac.cr";
const int mqtt_port = 1883;
const char* clientId = "DoesntMatter";
const char* user = "6coq0bk8am03mfc0hf5s";
const char* pass = ""; // No password needed

WiFiClient wifi;
PubSubClient client(wifi);

// TFT Screen stuff
TFT_eSPI tft = TFT_eSPI(); 

#define TFT_GREY 0x5AEB

#define LOOP_PERIOD 35 // Display updates every 35 ms

float ltx = 0;                  // Saved x coord of bottom of needle
uint16_t osx = 120, osy = 120;  // Saved x & y coords
uint32_t updateTime = 0;        // time for next update

int old_analog =  -999; // Value last displayed
int old_digital = -999; // Value last displayed

float value[3] = {0.0, 0.0, 0.0};
float old_value[3] = { -1.0, -1.0, -1.0};
int d = 0;
float HR, SPO2, TEMP;

void setup(void) {
  Serial.begin(57600); // For debug
 
  // TFT Init
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  header(); // print header with info

  // Draw 3 linear meters
  byte d = 80;
  plotLinear("HR(bpm)", 0, 160);
  plotLinear("SPO2(%)", 1 * d, 160);
  plotLinear("TEMP(oC)", 2 * d, 160);

 // WiFi Init
  randomSeed(analogRead(D5));
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  updateTime = millis(); // Next update time
}


void loop() {
  // TFT Loop
  if (updateTime <= millis()) {
    updateTime = millis() + LOOP_PERIOD;

    // Generate random values for HR, SDPO2, and TEMP
    HR = 90 + random(-2,2);
    SPO2 = 100 - random(1,5);
    TEMP = 37 + random(-1,1);

    // Vital signs values (randomly chosen)
    value[0] = HR; 
    value[1] = SPO2; 
    value[2] = TEMP; 

    // Plot vital signs
    plotPointer();
  }

  // WiFi Loop
  float temperature = random(200, 301) / 10.0;
  int err = reconnect();

  if (err != 0) 
  {
    // Buffer the measurement to send next time
  } else 
  {
    client.loop(); // Process incoming messages and maintain connection to server
    String json = "{\"HR\":" + String(value[0], 1) + ", \"SPO2\":" + String(value[1], 1) +", \"TEMP\":" + String(value[2], 1) +"}";
    //Serial.println("Publishing: " + json);
    if (client.publish("v1/devices/me/telemetry", json.c_str())) 
    {
      // Serial.println("Publish success");
    } else 
    {
      // Serial.println("Publish failed");
    }
  }
  
}

void header()
{
  // Meter outline
  tft.fillRect(0, 0, 239, 126, TFT_GREY);
  tft.fillRect(5, 3, 230, 119, TFT_WHITE);
  tft.drawRect(5, 3, 230, 119, TFT_BLACK);  // Draw bezel line
  tft.setTextColor(TFT_BLACK);              // Text colour

  // Draw info
  // tft.drawString("¡Conectado! IP: 128.106.201", 20, 20, 2); 
  tft.drawString("IE0527", 20, 40, 2);
  tft.drawString("Ingenieria de Comunicaciones", 20, 60, 2);
  tft.drawString("Proyecto Integrador", 20, 80, 2);
  tft.drawString("I2024, GR001, UCR-EIE", 20, 100, 2);
}

void plotLinear(char *label, int x, int y)
{
  int w = 72;
  int h = 155;
  tft.drawRect(x, y, w, h, TFT_GREY);
  tft.fillRect(x + 2, y + 19, w - 3, h - 38, TFT_WHITE);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(label, x + w / 2, y + 2, 2);

  for (int i = 0; i < 110; i += 10)
  {
    tft.drawFastHLine(x + 20, y + 27 + i, 6, TFT_BLACK);
  }

  for (int i = 0; i < 110; i += 50)
  {
    tft.drawFastHLine(x + 20, y + 27 + i, 9, TFT_BLACK);
  }

  tft.fillTriangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 - 5, TFT_RED);
  tft.fillTriangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 + 5, TFT_RED);

  tft.drawCentreString("---", x + w / 2, y + h - 18, 2);
}

void plotPointer(void)
{
  int dy = 187;
  byte pw = 16;

  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // Move the 3 pointers one pixel towards new value
  for (int i = 0; i < 3; i++)
  {
    char buf[8]; dtostrf(value[i], 4, 0, buf);
    tft.drawRightString(buf, i * 80 + 54 - 5, 187 - 27 + 155 - 18, 2);

    int dx = 3 + 80 * i;
    if (value[i] < 0) value[i] = 0; // Limit value to emulate needle end stops
    if (value[i] > 100) value[i] = 100;

    while (!(value[i] == old_value[i])) {
      dy = 187 + 100 - old_value[i];
      if (old_value[i] > value[i])
      {
        tft.drawLine(dx, dy - 5, dx + pw, dy, TFT_WHITE);
        old_value[i]--;
        tft.drawLine(dx, dy + 6, dx + pw, dy + 1, TFT_RED);
      }
      else
      {
        tft.drawLine(dx, dy + 5, dx + pw, dy, TFT_WHITE);
        old_value[i]++;
        tft.drawLine(dx, dy - 6, dx + pw, dy - 1, TFT_RED);
      }
    }
  }
}

void setup_wifi() {
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  WiFi.begin("Labo201", "labo201!");
  tft.drawString("Conectando a WiFi", 20, 20, 2); 
  while (WiFi.status() != WL_CONNECTED) {
    tft.drawString("   ", 140, 20, 2); 
    tft.drawString(".", 140, 20, 2); 
    delay(500);
    tft.drawString(".", 148, 20, 2);
    delay(500); 
    tft.drawString(".", 156, 20, 2); 
    delay(500);
  }
  // Serial
  tft.drawString("Conectado! IP: ", 20, 20, 2); 
  String localIPStr = WiFi.localIP().toString();
  tft.drawString(localIPStr.c_str(), 110, 20, 2); 
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
  //Serial.println("Reconnect requested");
  if (client.connected()) {
    // Serial.println("MQTT client is still connected");
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
