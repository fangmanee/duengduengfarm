// Board repo : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// BOARD: ESP32-WROOM-DA-Module

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT22


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

DHT dht(DHTPIN, DHTTYPE);

void sendUptime(){
  int time =  millis() / 1000;
  Blynk.virtualWrite(V0,time);
  Serial.println(time);
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
  // pinMode(potPin,INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("ESP32-01 Online!");
  Blynk.notify("ESP32-01 Online!");
  
  timer.setInterval(1000, sendUptime);
  // timer.setInterval(3000, moistSensor);
  timer.setInterval(1000, dhtSensor);
}

void loop(){
  // delay(2000);
  // float h = dht.readHumidity();
  // // Read temperature as Celsius (the default)
  // float t = dht.readTemperature();
  // // Read temperature as Fahrenheit (isFahrenheit = true)
  // float f = dht.readTemperature(true);

  // // Check if any reads failed and exit early (to try again).
  // if (isnan(h) || isnan(t) || isnan(f)) {
  //   Serial.println(F("Failed to read from DHT sensor!"));
  //   return;
  // }

  // // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);
  // // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);

  // Serial.print(F("Humidity: "));
  // Serial.print(h);
  // Serial.print(F("%  Temperature: "));
  // Serial.print(t);
  // Serial.print(F("°C "));
  // Serial.print(f);
  // Serial.print(F("°F  Heat index: "));
  // Serial.print(hic);
  // Serial.print(F("°C "));
  // Serial.print(hif);
  // Serial.println(F("°F"));
  Blynk.run();
  timer.run();
}
