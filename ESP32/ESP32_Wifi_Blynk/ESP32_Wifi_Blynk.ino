// Board repo : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// BOARD: ESP32-WROOM-DA-Module

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define BLYNK_PRINT Serial
//#define BLYNK_TEMPLATE_ID   "TMPLQwo8gNKs"
const int potPin = 2;
float  moiPercentage = 50.0;
int pMoist = 0;
const int dry = 4095;
const int wet = 0;
int moiture = 0;

char auth[] = "TzSKwYR-rLHARZl6yTXF5UgK-vSFUjyw";
char ssid[] = "Kin Bed&coffee wifi";
char pass[] = "12345678";

BlynkTimer timer;

void sendUptime(){
  int time =  millis() / 1000;
  Blynk.virtualWrite(V2,time);
  Serial.println(time);
}

void dhtSensor(){

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
//  Serial.printf("[Humidity] read Humidity %f\n", humidity);
//  Serial.printf("[Temp] read Temp %f\n",temp);
  Serial.println(humidity);
  Serial.println(temp);
  
}

void moistSensor()
{
  //analogReadResolution(12);
  moiture = analogRead(potPin);
  pMoist = map(moiture,wet,dry,100,00);
  Serial.println(moiture);
  if(pMoist > 0){
    Blynk.virtualWrite(V10, pMoist);
   }
  
  
}

void setup()
{
  // Debug console
  
  Serial.begin(115200);
  pinMode(potPin,INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Blynk.notify("ESP32-01 Online!");
  
  timer.setInterval(1000, sendUptime);
  timer.setInterval(1000, moistSensor);
  //timer.setInterval(1000, dhtSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
