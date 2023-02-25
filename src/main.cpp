#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_STA_NAME "XXXXXXXXX"
#define WIFI_STA_PASS "XXXXXXXXXX"

#define MQTT_SERVER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
// #define MQTT_USERNAME "lnbtkana"
// #define MQTT_PASSWORD "XXXXXXX"
// #define MQTT_NAME "kana"

#define LED_PIN 33

WiFiClient client;
PubSubClient mqtt(client);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  Serial.println("[" + topic_str + "]: " + payload_str);

  digitalWrite(LED_PIN, (payload_str == "ON") ? HIGH : LOW);
}

void connectToWiFi() {
  Serial.println("Connected.");
 
  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
  Serial.println(WIFI_STA_NAME);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectToWiFi();
  pinMode(LED_PIN, OUTPUT);

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("kuy");
  // delay(1000);
  if (!mqtt.connected()) {
    Serial.print("MQTT connection... ");
    
    while (!mqtt.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqtt.connect(clientId.c_str())) {
        Serial.println("Connected.");
        mqtt.subscribe("kana/tep");
      }
    }
  } else {
    mqtt.loop();
  }
}