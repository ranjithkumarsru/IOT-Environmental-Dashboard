#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "aws_secrets.h" // Create locally from aws_secrets.example.h; never commit credentials.
#include "../aws_config.h"

// Wi-Fi configuration. Set these in a local, untracked header for physical hardware.
#ifndef WIFI_SSID
#define WIFI_SSID "Wokwi-GUEST"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

// DHT sensor configuration
#define DHT_PIN 15
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// Secure MQTT connection
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// Connect to WiFi
void connectWiFi() {
  Serial.println("Connecting to WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
}

// Connect to AWS IoT Core
void connectAWS() {

  if (mqttClient.connected()) {
    return;
  }

  while (!mqttClient.connected()) {

    Serial.println("Connecting to AWS IoT Core...");

    if (mqttClient.connect(CLIENT_ID)) {

      Serial.println("AWS IoT Connected!");

    } else {

      Serial.print("AWS connection failed. State: ");
      Serial.println(mqttClient.state());

      delay(3000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  // Start DHT sensor
  dht.begin();

  // Start OLED
  Wire.begin(21, 22);

  if (!display.begin(
    SSD1306_SWITCHCAPVCC,
    OLED_ADDRESS
  )) {

    Serial.println("OLED initialization failed!");

    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 10);
  display.println("IoT Environmental");
  display.println("Dashboard");
  display.println("Connecting...");
  display.display();

  // Connect WiFi
  connectWiFi();

  // Configure AWS certificates
  secureClient.setCACert(AWS_ROOT_CA);
  secureClient.setCertificate(AWS_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);

  // Configure MQ%
  mqttClient.setServer(
    AWS_IOT_ENDPOINT,
    AWS_IOT_PORT
  );

  mqttClient.setBufferSize(512);

  // Connect AWS
  connectAWS();

  Serial.println("IoT Dashboard Started!");
}

void loop() {

  // Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Maintain AWS connection
  if (!mqttClient.connected()) {
    connectAWS();
  }

  mqttClient.loop();

  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {

    Serial.println("Sensor reading failed!");

    delay(2000);
    return;
  }

  // Display Serial readings
  Serial.println("--------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Prepare JSON message
  char payload[150];

  snprintf(
    payload,
    sizeof(payload),
    "{\"temperature\":%.2f,\"humidity\":%.2f}",
    temperature,
    humidity
  );

  // Publish readings to AWS
  if (mqttClient.publish(MQTT_TOPIC, payload)) {

    Serial.println("AWS Publish Successful!");
    Serial.println(payload);

  } else {

    Serial.println("AWS Publish Failed!");
  }

  // Update OLED
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ENVIRONMENT MONITOR");

  display.drawLine(0, 13, 127, 13, SSD1306_WHITE);

  display.setCursor(0, 22);
  display.print("Temperature:");

  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print(temperature, 1);
  display.print(" C");

  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.print("%");

  display.display();

  delay(2000);
}