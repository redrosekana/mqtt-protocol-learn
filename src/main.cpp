// import library
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ชื่อ wifi / รหัส wifi 
#define WIFI_STA_NAME "XXXYYYZZZ"
#define WIFI_STA_PASS "XXXYYYZZZ"

// ชื่อ server ของ mqtt broker ยังใช้เป็นตัวทดลอง
#define MQTT_SERVER "broker.mqttdashboard.com"
// กำหนด port ปกติแล้ว mqtt จะใช้ 1883
#define MQTT_PORT 1883

// ข้อมูลต่างๆ
// #define MQTT_USERNAME "lnbtkana"
// #define MQTT_PASSWORD "XXXXXXX"
// #define MQTT_NAME "kana"

// กำหนด led pin 33
#define LED_PIN 33

// กำหนดให้ใช้เป็น mqtt
WiFiClient client;
PubSubClient mqtt(client);

// ฟังชันก์ callback ซึ่งจะทำเมื่อมีการ publish มาจาก server แล้วเรามีการ subscribe ไว้นั่นเอง
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  Serial.println("[" + topic_str + "]: " + payload_str);

  // สั่งให้ไฟติด หรือ ดับตามข้อมูลที่ได้จากการ subscribe
  digitalWrite(LED_PIN, (payload_str == "ON") ? HIGH : LOW);
}

// ฟังชันก์ในการเชื่อมต่อ wifi
void connectToWiFi() {
  Serial.println("Connected...");
 
  // ทำการเชื่อมต่อ wifi
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
  
  // วนไปเรื่อยถ้ายังเชื่อมต่อไม่ได้
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // เช็ค IP Address
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  // เรียกฟังชันก์เชื่อมต่อ wifi
  connectToWiFi();
  // กำหนด pin ของ led ให้เป็น output
  pinMode(LED_PIN, OUTPUT);

  // เชื่อมต่อ mqtt server
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}

void loop() {
  
  // ถ้ายังไม่ได้เชื่อมต่อก็เข้า if นี้ ก็คือเริ่มการเชื่อมต่อนั่นเอง
  if (!mqtt.connected()) {
    Serial.print("MQTT connection... ");
    
    // วนไปเรื่อยๆ เมื่อยังเชื่อมต่อไม่ได้
    while (!mqtt.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "Kana";
      clientId += String(random(0xffff), HEX);
      
      // เช็คการเชื่อมต่อว่าสำเร็จหรือยัง พร้อมกำหนด public และ subscribe 
      if (mqtt.connect(clientId.c_str())) {
        Serial.println("Connected.... Success");
        mqtt.publish("kana/tep/1","Hello World");
        mqtt.subscribe("kana/tep/2");
      }
    }
  } else {
    mqtt.loop();
  }
}