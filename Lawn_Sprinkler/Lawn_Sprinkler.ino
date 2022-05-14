#include <ESP8266WiFi.h>
#include <LoRa.h>

// WiFi settings
#define WIFI_SSID "<YOUR_SSID>"
#define WIFI_PASSWORD "<YOUR_PASSWORD>"

// Thingspeak settings
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_API_KEY "<YOUR_API_KEY>"

// Pin definitions
#define RELAIS_PIN 5
#define LORA_CS_PIN 2
#define LORA_RST_PIN -1
#define LORA_DIO_PIN 15

// LoRa settings
#define LORA_FREQUENCY 868e6
#define LORA_SYNC_WORD 0x2B

// Lawn sprinkler settings
#define CRITICAL_MOISTURE 15
#define LAWN_SPRINKLER_DURATION 600000

// Lawn sprinkler state
unsigned long start = 0;
boolean active = false;

// Messurement values
float moisture = 25;
float groundTemperature = 20;

// Program setup
void setup() {
  Serial.begin(115200);
  pinMode(RELAIS_PIN, OUTPUT);

  // Start LoRa
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DIO_PIN);
  LoRa.begin(LORA_FREQUENCY);
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(20);

  // Start WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  
  Serial.print("\nWiFi connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP Adress: " + WiFi.localIP().toString());
}

// Program loop
void loop() {
  int size = LoRa.parsePacket();

  // Receive LoRa data
  if (size > 0) {    
    while (LoRa.available()) {
      yield();
      
      String data = LoRa.readString();
      float temperature, humidity, pressure, iaq;
      sscanf(data.c_str(), "%f,%f,%f,%f,%f,%f", &temperature, &humidity, &pressure, &iaq, &moisture, &groundTemperature);

      // Update Thingspeak
      String cmd = "/update?api_key=" + String(THINGSPEAK_API_KEY);
      cmd += "&field1=" + String(temperature);
      cmd += "&field2=" + String(humidity);
      cmd += "&field3=" + String(pressure);
      cmd += "&field4=" + String(iaq);
      cmd += "&field5=" + String(moisture);
      cmd += "&field6=" + String(groundTemperature);
      cmd += "\n\r";
      
      httpGET(THINGSPEAK_HOST, cmd, 80);

      // Activate lawn sprinkler
      if (moisture < CRITICAL_MOISTURE) {
        start = millis();
        active = true;
        digitalWrite(RELAIS_PIN, HIGH);
      }
    }
  }
  // Deactivate lawn sprinkler after duration
  if (active && moisture > CRITICAL_MOISTURE && (millis() - start) > LAWN_SPRINKLER_DURATION) {
    active = false;
    digitalWrite(RELAIS_PIN, LOW);
  }
}

// Send HTTP GET request
String httpGET(String host, String cmd, int port) {
  WiFiClient client;
  
  String request = "GET http://"+ host + cmd + " HTTP/1.1\r\n";
  request += "Host: " + host + "\r\n";
  request += "Connection: close\r\n\r\n";
  String answer = "";
  
  int code = client.connect(host.c_str(), port);
  if(code) {
    client.print(request);
    
    if(client.available() > 0) {
      while(client.available()) {
        answer += client.readStringUntil('\r');
      }
    }
    client.stop();
  }
  else {
    Serial.println("\nNo connection to \"" + host + "\"!");
  }
  return answer;
}
