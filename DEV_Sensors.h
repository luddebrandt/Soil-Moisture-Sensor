#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

Adafruit_ADS1115 ads;

const int Pw0Pin = 25;
const int Pw1Pin = 26;
const int Pw2Pin = 27;
const int PwADCPin = 19;

//const int timeupdate = 60000;
const int timeupdate = 300000;
//const int timeupdate = 1800000;

#define INFLUXDB_URL ""
#define INFLUXDB_TOKEN ""
#define INFLUXDB_ORG ""
#define INFLUXDB_BUCKET ""
#define TZ_INFO ""

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


Point sensor1("Soil Moisture Sensor 1");

struct DEV_HumSensor : Service::HumiditySensor {

  SpanCharacteristic *hum;
  
  DEV_HumSensor() : Service::HumiditySensor(){       // constructor() method

    pinMode(Pw0Pin, OUTPUT);
    digitalWrite(Pw0Pin, LOW);
    pinMode(PwADCPin, OUTPUT);
    digitalWrite(PwADCPin, LOW);
    
    hum = new Characteristic::CurrentRelativeHumidity(50);
    hum->setRange(0, 100); 
        
    // initialization message
    Serial.print("Configuring Soil Moisture Sensor");           

    sensor1.addTag("Sensor", "Soil Moisture Sensor 1");

  } // end constructor

  void loop(){
    // the temperature refreshes every x seconds by the elapsed time
    if (hum->timeVal() > timeupdate) {

      digitalWrite(Pw0Pin, HIGH);
      digitalWrite(PwADCPin, HIGH);
      delay(2000);
      ads.setGain(GAIN_ONE);
      //ads.setGain(GAIN_TWOTHIRDS);
      ads.begin();
      delay(3000);

      int sum = 0;
      for (int m=0; m < 2; m++) {
        int adc00 = ads.readADC_SingleEnded(0);
        delay(2000);
        int adc01 = ads.readADC_SingleEnded(0);
        delay(2000);
        int adc02 = ads.readADC_SingleEnded(0);
        delay(2000);
        int adc03 = ads.readADC_SingleEnded(0);
        delay(2000);
        int adc04 = ads.readADC_SingleEnded(0);
        delay(2000);

        int values[7] = {adc00, adc01, adc02, adc03, adc04};
        for (int i = 0; i < 4; i++) {
          for (int j = i + 1; j < 5; j++) {
            if (values[j] < values[i]) {
              int temp = values[i];
              values[i] = values[j];
              values[j] = temp;
            }
          }
        }

        sum += values[2];
      }

      int16_t adc0 = sum/2 ;

      digitalWrite(Pw0Pin, LOW);
      digitalWrite(PwADCPin, LOW);
      delay(2000);
      
      float minadc = 0;
      float maxadc = 32767;
      //float maxadc = 4095;
      float humidityI = 100 - ((float)adc0-minadc)/(maxadc-minadc)*100;
      
      hum->setVal(humidityI);

      Serial.print("\n");
      Serial.print(adc0);
      Serial.print("\n");
      Serial.print(humidityI);
      Serial.print("\n");  

      sensor1.clearFields();
      if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());

        timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
        
        sensor1.addField("ADC", adc0);
        sensor1.addField("Humidity", humidityI);

        // Write point
        if (!client.writePoint(sensor1)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(client.getLastErrorMessage());
        }

      } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
      }

    }
  } // loop
  
};

Point sensor2("Soil Moisture Sensor 2");

struct DEV_HumSensor2 : Service::HumiditySensor {

  SpanCharacteristic *hum;
  
  DEV_HumSensor2() : Service::HumiditySensor(){       // constructor() method

    pinMode(Pw1Pin, OUTPUT);
    digitalWrite(Pw1Pin, LOW);
    
    hum = new Characteristic::CurrentRelativeHumidity(50);
    hum->setRange(0, 100); 
        
    // initialization message
    Serial.print("Configuring Soil Moisture Sensor 2");           

    sensor2.addTag("Sensor", "Soil Moisture Sensor 2");

  } // end constructor

  void loop(){
    // the temperature refreshes every x seconds by the elapsed time
    if (hum->timeVal() > timeupdate) {

      digitalWrite(Pw1Pin, HIGH);
      digitalWrite(PwADCPin, HIGH);
      delay(2000);
      ads.setGain(GAIN_ONE);
      ads.begin();
      delay(3000);

      int sum = 0;
      for (int m=0; m < 2; m++) {
        int adc10 = ads.readADC_SingleEnded(1);
        delay(2000);
        int adc11 = ads.readADC_SingleEnded(1);
        delay(2000);
        int adc12 = ads.readADC_SingleEnded(1);
        delay(2000);
        int adc13 = ads.readADC_SingleEnded(1);
        delay(2000);
        int adc14 = ads.readADC_SingleEnded(1);
        delay(2000);

        int values[7] = {adc10, adc11, adc12, adc13, adc14};
        for (int i = 0; i < 4; i++) {
          for (int j = i + 1; j < 5; j++) {
            if (values[j] < values[i]) {
              int temp = values[i];
              values[i] = values[j];
              values[j] = temp;
            }
          }
        }

        sum += values[2];
      }

      int16_t adc1 = sum/2 ;

      digitalWrite(Pw1Pin, LOW);
      digitalWrite(PwADCPin, LOW);
      delay(2000);

      float minadc = 0;
      float maxadc = 32767;
      //float maxadc = 4095;
      float humidityI = 100 - ((float)adc1-minadc)/(maxadc-minadc)*100;
      
      hum->setVal(humidityI);

      Serial.print("\n");
      Serial.print(adc1);
      Serial.print("\n");
      Serial.print(humidityI);
      Serial.print("\n");  

      sensor2.clearFields();
      if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());

        //timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
        
        sensor2.addField("ADC", adc1);
        sensor2.addField("Humidity", humidityI);

        // Write point
        if (!client.writePoint(sensor2)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(client.getLastErrorMessage());
        }

      } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
      }

    }
  } // loop
  
};

Point sensor3("Soil Moisture Sensor 3");

struct DEV_HumSensor3 : Service::HumiditySensor {

  SpanCharacteristic *hum;
  
  DEV_HumSensor3() : Service::HumiditySensor(){       // constructor() method
    
    pinMode(Pw2Pin, OUTPUT);
    digitalWrite(Pw2Pin, LOW);

    hum = new Characteristic::CurrentRelativeHumidity(50);
    hum->setRange(0, 100); 
        
    // initialization message
    Serial.print("Configuring Soil Moisture Sensor 3");           

    sensor3.addTag("Sensor", "Soil Moisture Sensor 3");

  } // end constructor

  void loop(){
    // the temperature refreshes every x seconds by the elapsed time
    if (hum->timeVal() > timeupdate) {

      digitalWrite(Pw2Pin, HIGH);
      digitalWrite(PwADCPin, HIGH);
      delay(2000);
      ads.setGain(GAIN_ONE);
      ads.begin();
      delay(3000);


      int sum = 0;
      for (int m=0; m < 2; m++) {
        int adc20 = ads.readADC_SingleEnded(2);
        delay(2000);
        int adc21 = ads.readADC_SingleEnded(2);
        delay(2000);
        int adc22 = ads.readADC_SingleEnded(2);
        delay(2000);
        int adc23 = ads.readADC_SingleEnded(2);
        delay(2000);
        int adc24 = ads.readADC_SingleEnded(2);
        delay(2000);

        int values[7] = {adc20, adc21, adc22, adc23, adc24};
        for (int i = 0; i < 4; i++) {
          for (int j = i + 1; j < 5; j++) {
            if (values[j] < values[i]) {
              int temp = values[i];
              values[i] = values[j];
              values[j] = temp;
            }
          }
        }

        sum += values[2];
      }

      int16_t adc2 = sum/2 ;

      digitalWrite(Pw2Pin, LOW);
      digitalWrite(PwADCPin, LOW);
      delay(2000);

      float minadc = 0;
      float maxadc = 32767;
      //float maxadc = 4095;
      float humidityI = 100 - ((float)adc2-minadc)/(maxadc-minadc)*100;
      
      hum->setVal(humidityI);

      Serial.print("\n");
      Serial.print(adc2);
      Serial.print("\n");
      Serial.print(humidityI);
      Serial.print("\n");  

      sensor3.clearFields();
      if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());

        //timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
    
        sensor3.addField("ADC", adc2);
        sensor3.addField("Humidity", humidityI);

        // Write point
        if (!client.writePoint(sensor3)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(client.getLastErrorMessage());
        }

      } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
      }

    }
  } // loop
  
};
