

//USE DOIT ESP32 DEV Kit V1 Board
#define BLYNK_TEMPLATE_ID "TMPL2OaGAXTP"
#define BLYNK_DEVICE_NAME "NodeMCU01"
#define BLYNK_AUTH_TOKEN "XfoG-PdCl5fpXjGdm-7zXIdh3BYgt0DI"
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <DHT_U.h>


#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>


#define LED 2
#define PUMP 23
#define BTN 15

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MOISTPIN 34

#define LIGHTPIN 5

#define PUMP 2


int tempValue = 0;

const char* ssid = "Kin Bed&coffee wifi";
const char* password = "12345678";

const int intervalTime = 15 * 1000;

const String deviceName = "NodeMCU01";
const String getUrl = "http://ec2-54-251-164-224.ap-southeast-1.compute.amazonaws.com:3000/api/iot/get";
const String postUrl = "http://ec2-54-251-164-224.ap-southeast-1.compute.amazonaws.com:3000/api/iot/post";


unsigned long lastTime = 0;


char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

float  moiPercentage = 50.0;
int pMoist = 0;
const int dry = 4095;
const int wet = 0;

// This function is called every time the Virtual Pin V11 state changes
int value;
BLYNK_WRITE(V11)
{
  // Set incoming value from pin V0 to a variable
  value = param.asInt();

  // Update state
//  Blynk.virtualWrite(V1, value);
  Blynk.logEvent("PUMP", "Water Pump Trigger");
  digitalWrite(PUMP,0);
  Serial.println(value);
}


void dhtSensor()
{

//  String timeStamp = getTimeStamp();
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
//
//  Blynk.virtualWrite(V0, temp);
//  Blynk.virtualWrite(V1, humidity);
//  Blynk.virtualWrite(V2, millis() / 1000);

  Blynk.virtualWrite(V3, temp);
  Blynk.virtualWrite(V4, humidity);
  Blynk.virtualWrite(V5, millis() / 1000);

//  Blynk.virtualWrite(V6, temp);
//  Blynk.virtualWrite(V7, humidity);
//  Blynk.virtualWrite(V8, millis() / 1000);

  
}

void moistSensor()
{
  analogReadResolution(12);
  int moiture = 0;
  moiture = analogRead(MOISTPIN);
  pMoist = map(moiture,wet,dry,100,00);
  if(pMoist > 0){
//    Blynk.virtualWrite(V9, pMoist);
    Blynk.virtualWrite(V10, pMoist);
   }
  
  
}

void switcher()
{
  int pumpStart = digitalRead(19);
  int pumpRevert = digitalRead(33);
  
  if(pumpStart > 0){
      Blynk.logEvent("PUMPSTART", "Water Pump Activate!");
      Serial.println("PUMPSTART");
      Blynk.virtualWrite(V12, 1);
      digitalWrite(PUMP,0);
      Blynk.virtualWrite(V13, 0);
      pumpRevert = 0;
    }
  if(pumpRevert > 0){
      Blynk.logEvent("PUMPREVERT", "Water Pump Revert Activate!");
      Serial.println("PUMPREVERT");
      Blynk.virtualWrite(V12, 0);
      Blynk.virtualWrite(V13, 1);
      pumpStart = 0;
    }
  if(pumpStart == 0 && pumpRevert == 0){
      Blynk.virtualWrite(V12, 0);
      Blynk.virtualWrite(V13, 0);
      if(value == 0){
        digitalWrite(PUMP,1);
        }
      
    }
}



void setup() {
  Serial.begin(115200); 
  pinMode(PUMP,OUTPUT);
  digitalWrite(PUMP,HIGH);
  pinMode(19,INPUT);
  pinMode(33,INPUT);
  Blynk.begin(auth, ssid, password);
  Blynk.notify("NodeMCU03 online");
  timer.setInterval(1000, dhtSensor);
  timer.setInterval(1000, moistSensor);
  timer.setInterval(100, switcher);
}

void loop() {
  Blynk.run();
  timer.run();
}

String getTimeStamp(){
      configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
      time_t now = time(nullptr);
      while (now < 8 * 3600 * 2) {
        delay(500);
        now = time(nullptr);
      }
      struct tm timeinfo;
      gmtime_r(&now, &timeinfo);
      char buffer[35];
      return(itoa(now, buffer, 10));
}

String postDataToServer(String url,String deviceName){
  
    WiFiClient client;

    HTTPClient http;

    int wifi_rssi = WiFi.RSSI();
    String timeStamp = getTimeStamp();
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();

//    int moisturePercentage = ( 100 - ( (analogRead(MOI) / 1023.00) * 100 ) );
    
    Serial.printf("[Humidity] read Humidity %f\n", humidity);
    Serial.printf("[Temp] read Temp %f\n", temp);
//    Serial.printf("[Moisture] read moisturePercentage %d\n", moisturePercentage);
    
    if (http.begin(client, url)) {  // HTTP


      Serial.printf("[HTTP] POST...%s\n",url.c_str());
      
      http.addHeader("Content-Type", "application/json");

      String httpRequestData = "{\"data\": { \"device\":\""+ deviceName +"\",\"wifi_rssi\":\""+ deviceName +"\",\"temp\":\""+ String(temp) +"\",\"humidity\":\""+ String(humidity) +"\",\"timeStamp\":\""+ timeStamp +"\"}}";
      Serial.println(httpRequestData);
      int httpCode = http.POST(httpRequestData);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          return(payload);
        }
      } else { 
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  return "none";
}

String getDataFromServer(String url,String deviceName){
  
    WiFiClient client;

    HTTPClient http;

    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();

//    int moisturePercentage = ( 100 - ( (analogRead(MOI) / 1023.00) * 100 ) );
    
    Serial.printf("[Humidity] read Humidity %f\n", humidity);
    Serial.printf("[Temp] read Temp %f\n", temp);
//    Serial.printf("[Moisture] read moisturePercentage %d\n", moisturePercentage);
    
    Serial.print("[HTTP] begin..\n");
    if (http.begin(client, url + "?device="+ deviceName + "&humidity="+ humidity + "&temp="+ temp)) {  // HTTP


      Serial.printf("[HTTP] GET...%s\n",url.c_str());
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
//          Serial.println(payload);
          return(payload);
        }
      } else { 
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  return "none";
}
