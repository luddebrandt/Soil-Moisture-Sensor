#include "HomeSpan.h" 
#include "DEV_Identify.h"      
#include "DEV_Sensors.h"

void setup() {
 
  //Serial.begin(115200);
  Serial.begin(9600);

  //homeSpan.begin(Category::Sensors,"Soil Moisture Sensor");
  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  new SpanAccessory();  
    new DEV_Identify("Greenhouse","HomeSpan","000","HomeSpan Bridge","1.0",3);
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
     
  new SpanAccessory();                                                          
    new DEV_Identify("Soil Moisture Sensor 1","AZDelivery","001","ESP32","1.0",0);
    //new Service::HAPProtocolInformation();
      //new Characteristic::Version("1.1.0");
    new DEV_HumSensor();

  new SpanAccessory();                                                          
    new DEV_Identify("Soil Moisture Sensor 2","AZDelivery","002","ESP32","1.0",0);
    new DEV_HumSensor2();                                                                   

  new SpanAccessory();                                                          
    new DEV_Identify("Soil Moisture Sensor 3","AZDelivery","003","ESP32","1.0",0);
    new DEV_HumSensor3();   

}

void loop(){
  
  homeSpan.poll();
  
}