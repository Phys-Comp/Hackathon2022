#include <ESP8266WiFi.h>
#include <Ticker.h>

#include <bsec.h>
#include <LoRa.h>
#include <Wire.h>

// Pin definitions
#define LORA_CS_PIN 2
#define LORA_RST_PIN -1
#define LORA_DIO_PIN 15
#define ADC_I2C_ADDRESS 0x50

// LoRa settings
#define LORA_FREQUENCY 868e6
#define LORA_SYNC_WORD 0x2B

// Update interval
#define BSEC_INTERVAL 3000
#define UPDATE_INTERVAL 10000

extern "C" {
#include "user_interface.h"
}

const uint8_t bsecConfig[] = {
#include "config/generic_33v_3s_28d_2d_iaq_50_200/bsec_iaq.txt"
};

// Library instances
Bsec bme680;
Ticker timer;

// Program setup
void setup() {
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  Wire.begin();

  // Setup the BME680
  Serial.println("\nConnecting BME680...");
  bme680.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  bme680.setConfig(bsecConfig);
  checkStatus();

  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  bme680.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);  
  bme680.run();
  timer.attach_ms(BSEC_INTERVAL, runBME680);
  Serial.println("BME680 connected!");

  // Setup the ADC over I2C
  if (Wire.status() != I2C_OK) {
    Serial.println("Something went wrong with I2C!");
  }
  initADC(ADC_I2C_ADDRESS);

  // Start LoRa
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DIO_PIN);
  LoRa.begin(LORA_FREQUENCY);
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(20);
}

// Program loop
void loop() {
  // Messure values
  float temperature = bme680.temperature;
  float humidity = bme680.humidity;
  float pressure = bme680.pressure / 100.0;
  float iaq = bme680.iaq;
  float moisture = 50.0 * readADC(ADC_I2C_ADDRESS) / 2176.0;
  float groundTemperature = (300.0 * analogRead(0) / 1024.0) - 55.0;

  // Send LoRa data
  LoRa.beginPacket();
  LoRa.print(temperature);
  LoRa.print(',');
  LoRa.print(humidity);
  LoRa.print(',');
  LoRa.print(pressure);
  LoRa.print(',');
  LoRa.print(iaq);
  LoRa.print(',');
  LoRa.print(moisture);
  LoRa.print(',');
  LoRa.print(groundTemperature);
  LoRa.endPacket();
  
  ESP.deepSleep((long) UPDATE_INTERVAL * 1000UL, WAKE_RF_DISABLED);
}

// Check status of BME680
void checkStatus(void) { 
  String output;
  
  while (bme680.status < BSEC_OK) {
    output = "BSEC error code : " + String(bme680.status);
    Serial.println(output);
    delay(5000);
  }
  if (bme680.status > BSEC_OK) {
    output = "BSEC warning code : " + String(bme680.status);
    Serial.println(output);
  }

  while (bme680.bme680Status < BME680_OK) {
    output = "BME680 error code : " + String(bme680.bme680Status);
    Serial.println(output);
    delay(5000);
  }
  if (bme680.bme680Status > BME680_OK) {
    output = "BME680 warning code : " + String(bme680.bme680Status);
    Serial.println(output);
  }
}

// Run the BME680
void runBME680(){
  bme680.run();
}

// Init the ADC over I2C
void initADC(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x02);
  Wire.write(0x20);
  Wire.endTransmission();
}

// Read the ADC over I2C
int readADC(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  delay(1);
  
  int data = 0;
  if (Wire.available() <= 2) {
    data = (Wire.read() & 0x0F) << 8;
    data |= Wire.read();
  }
  return data;
}
