#include <Arduino.h>
#include <ESP8266TrueRandom.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "base64.h"

ESP8266WiFiMulti WiFiMulti;

#define ssid "DIPEX_LAB"
#define password "1234567890"
#define addr "http://192.168.0.106/"

String data_string;
String a;

void setup() {

    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.println();

    delay(4000);
    WiFiMulti.addAP(ssid, password);

}

String sendtoserver(String data){
  String response = "None";
  if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;                  //Http begin
        http.setTimeout(500000);
        
        String request = addr + base64::encode(data);
        http.begin(request); //HTTP
  
        int httpCode = http.GET();      //Http Get
        
        if(httpCode > 0) {
          if(httpCode == HTTP_CODE_OK) {
            response = http.getString();
          }
          else{
            response = "Unknown Response";
          }
        } 
        else {
           response = http.errorToString(httpCode).c_str();
        }
        http.end();
    }
    return response;
}

void loop() {
  data_string = String(random(1,100));
  a = sendtoserver(data_string);
  Serial.println(a);
}

