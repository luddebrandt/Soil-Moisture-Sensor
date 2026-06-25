# Soil Moisture Sensor
An ESP32-based HomeKit bridge that monitors soil moisture across capacitive sensors, exposes them to Apple Home via HomeSpan, and logs readings to InfluxDB. Reading can be visualized using Grafana and alerts can be generated when plants need to be watered.

## Overview

This project turns an ESP32 into a HomeKit bridge (using the HomeSpan library) that reads three capacitive soil moisture sensors through an ADS1115 ADC. Each sensor is powered only during a reading to reduce corrosion and power consumption. Readings are reported to Apple Home as humidity values and pushed to an InfluxDB instance for long-term logging and dashboarding.

## Hardware

- ESP32 microcontroller (e.g. AZDelivery ESP32) 
- ADS1115 16-bit I²C ADC (reads up to 4 analog channels)
- Capacitive soil moisture sensors

Sensors are powered via dedicated GPIO pins so they are only energized during a reading cycle. This reduces electrolytic corrosion of the sensor probes.
Pin Assignments:

GPIO Pins:
- 25 Power control Sensor 1
- 26 Power control Sensor 2
- 27 Power control Sensor 3
- 19 Power control ADS1115
- (default) ADS1115 SDA/SCL

## Software Dependencies

Install these libraries through the Arduino Library Manager or PlatformIO:

- HomeSpan
- Adafruit ADS1X15
- InfluxDB Client for Arduino

## Configuration

Open DEV_Sensors.h and fill in your InfluxDB credentials and timezone:

```
#define INFLUXDB_URL   ""   // e.g. "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN ""   // your InfluxDB API token
#define INFLUXDB_ORG   ""   // your organisation name
#define INFLUXDB_BUCKET ""  // target bucket
#define TZ_INFO        ""   // e.g. "CET-1CEST,M3.5.0,M10.5.0/3"
```

The update interval is also set in DEV_Sensors.h:

```
const int timeupdate = 300000;  // milliseconds — default is 5 minutes
```

## How It Works

Reading cycle (per sensor):

GPIO pin powers up the sensor and ADS1115.
A 2–3 second warm-up delay allows the ADC to stabilise.
Five ADC readings are taken with 2-second gaps between each.
The readings are sorted and the median is selected (outlier rejection).
This is repeated twice and the two medians are averaged.
The sensor and ADC are powered down.
The raw ADC value is mapped to a 0–100% moisture range and sent to HomeKit.
The ADC value and humidity percentage are written to InfluxDB.

Moisture calculation:

```
humidity% = 100 - ((adc_value / 32767) × 100)
```

A fully dry sensor reads near 32767 (maximum ADC output with GAIN_ONE), giving 0% humidity. A fully saturated sensor reads near 0, giving 100% humidity. If your sensors behave differently, adjust minadc / maxadc in DEV_Sensors.h to calibrate.

## HomeKit integration

The bridge presents itself to HomeKit as a Bridge accessory named HomeSpan Bridge / Greenhouse, with three child accessories (Soil Moisture Sensor 1–3), each exposing a CurrentRelativeHumidity characteristic. Apple Home will display them as humidity sensors.

## File Structure

```
.
├── Soil-Moisture-Sensor.ino    # Main sketch with HomeSpan setup and accessory tree
├── DEV_Sensors.h               # Sensor service definitions and InfluxDB logging
└── DEV_Identify.h              # Accessory identification
```

## Pairing with Apple Home

Flash the sketch to your ESP32.
Open the Apple Home app and tap Add Accessory.
Choose More options and select HomeSpan Bridge.
Enter the default HomeSpan pairing code (printed to Serial on first boot, or see the HomeSpan docs).

## Notes & Tips

Run the device in air (dry) and submerged in water to find your sensor's actual min/max ADC values, then update minadc / maxadc accordingly. The code uses GAIN_ONE (±4.096 V range). If your sensors output a narrower voltage range, switching to GAIN_TWO or GAIN_FOUR will improve resolution. NTP time sync (timeSync) is called only during the Sensor 1 reading cycle. Sensors 2 and 3 reuse the synchronised clock.
