#include<ArduinoJson.h>
#include<WiFi.h>
#include<WebSocketsClient.h>
#include<MQTTPubSubClient.h>
//// DECLARATION ////
// LED1-LED2
#define led1 2
#define led2 15
// WebSockets & MQTTPubSub
WebSocketsClient client;
MQTTPubSubClient mqtt;
// WIFI SSID & pass
const char* ssid = "<YOUR_WIFI_SSID>"; 
const char* password = "<YOUR_WIFI_PASS>"; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  // Connecting to WIFI network with SSID and password
  Serial.print("WIFI-Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  // Connecting to Host
  Serial.println("HOST-Connecting...");
  mqtt.begin(client);
  client.disconnect();
  const char* mqtt_server = "<YOUR_URL@OVERVIEW>";
  client.beginSSL(mqtt_server, <YOUR_WEBSOCKETPORT@OVERVIEW>, "/mqtt");
  client.setReconnectInterval(2000);
  // Connecting to MQTT Broker
  Serial.print("MQTT Broker-Connecting...");
  while(!mqtt.connect("ESP32-mikro", "<YOUR_USERNAME@WEBCLIENT>", "<YOUR_PASS@WEBCLIENT>")){
    Serial.print(".");
  }
  Serial.println("Connected!");
  // Subscribe
  mqtt.subscribe("daten", [](const String& payload, const size_t size){
    Serial.print("Daten: "); 
    Serial.println(payload);
  // Publish
    StaticJsonDocument<200> doc;
    // JSON syntax check
    DeserializationError error = deserializeJson(doc, payload);
    // Error check
    if(error){
      Serial.print(F("deserializeJson() failed:"));
      Serial.println(error.f_str());
      return;
    }
    // LED1-LED2 lesen & Schreiben
    JsonObject obj = doc.as<JsonObject>();
    if(obj.containsKey("led1")){
      bool ledState1 = doc["led1"];
      digitalWrite(led1, ledState1);
      int state = digitalRead(led1);
      mqtt.publish("led1", String(state));
    }
    if(obj.containsKey("led2")){
      bool ledState2 = doc["led2"];
      digitalWrite(led2, ledState2);
      int state = digitalRead(led2);
      mqtt.publish("led2", String(state));
    }
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.update();
  static uint32_t prev_ms = millis();
  if(millis() > prev_ms + 10000){
    prev_ms = millis();
    //mqtt.publish("Hallo", "Hello world!");
    String zeit = String(millis()/1000);
    mqtt.publish("uptime", zeit);
  }
}
