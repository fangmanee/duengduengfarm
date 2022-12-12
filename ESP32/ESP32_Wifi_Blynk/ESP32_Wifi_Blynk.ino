// Board repo : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// BOARD: ESP32-WROOM-DA-Module

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 15
#define DHTTYPE DHT22


#define BLYNK_PRINT Serial
//#define BLYNK_TEMPLATE_ID   "TMPLQwo8gNKs"

const int moistPin = 36;
int pMoist = 0;
const int dry = 4095;
const int wet = 0;
int moisture = 0;

char auth[] = "TzSKwYR-rLHARZl6yTXF5UgK-vSFUjyw";
char ssid[] = "Kin Bed&coffee wifi";
char pass[] = "12345678";

BlynkTimer timer;

DHT dht(DHTPIN, DHTTYPE);

void sendUptime(){
  int time =  millis() / 1000;
  Blynk.virtualWrite(V0,time);
//  Serial.println(time);
  Serial.printf("[Uptime] %d s\n",time);
}

void dhtSensor(){
  dht.begin();
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.printf("[Humidity] read Humidity %f\n", humidity);
  Serial.printf("[Temp] read Temp %f\n",temp);
  Blynk.virtualWrite(V3,humidity);
  Blynk.virtualWrite(V4,temp);
  // Serial.println(humidity);
  // Serial.println(temp);
  
}

void moistSensor(){
  //analogReadResolution(12);
  moisture = analogRead(moistPin);
  pMoist = map(moisture,wet,dry,100,0);
//  Serial.println(moisture);
  Serial.printf("[Moisture] read Moist %d\n",moisture);
  Serial.printf("[Moisture] Calculated pMoist %d\n",pMoist);
  if(pMoist > 0){
    Blynk.virtualWrite(V5, pMoist);
   }
  
  
}

void setup()
{
  // Debug console
  
  Serial.begin(115200);
  pinMode(DHTPIN,INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("ESP32-01 Online!");
  Blynk.notify("ESP32-01 Online!");
  
  timer.setInterval(1000, sendUptime);
  timer.setInterval(1000, moistSensor);
  timer.setInterval(1000, dhtSensor);
}

void loop(){

  Blynk.run();
  timer.run();
}
