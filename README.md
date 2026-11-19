# IoT Environmental Dashboard

ESP32-based environmental monitoring using a DHT22 temperature/humidity sensor, SSD1306 OLED, AWS IoT Core MQTT, and a web dashboard backed by an AWS API.

## Features
- Reads temperature and humidity from a DHT22 sensor
- Displays readings on a 128x64 SSD1306 OLED
- Publishes JSON readings to AWS IoT Core over MQTT/TLS
- Dashboard displays current readings, charts, and history from an API endpoint
- Wokwi wiring diagram included

## Firmware setup
See `platformio.ini` for PlatformIO dependencies. Set your AWS IoT endpoint in `aws_config.h`. Copy `src/aws_secrets.example.h` to `src/aws_secrets.h` and populate it with your own Amazon Root CA, device certificate, and private key. Keep that local file untracked.

**Never commit AWS private keys, device certificates, Wi-Fi passwords, or other credentials.** This repository intentionally excludes them.

## Dashboard
Host the `Dashboard` folder as a static website. Update the API endpoint in `Dashboard/script.js` for your own deployment and configure API CORS.

## Hardware
DHT22 data: GPIO 15. OLED I2C: SDA GPIO 21, SCL GPIO 22, address 0x3C. Both modules use 3V3 and GND.

## License
Add a license before permitting reuse or redistribution.
