#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sendRequest.h> 
#include <ArduinoJson.h>
const char* ssid  = "WIN-PNG6LLSRRPS 2281"; 
const char* password = "V6i=9";

String key = "&time=";
String value = emptyString;

const String url = "youwebpage.php?";
const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

const size_t capacity = JSON_ARRAY_SIZE(10) + 10*JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument doc(capacity);

String dataToBeSent = "";
String currentTime;
bool receivedFlag = false;

void setup () {
  Serial.begin(115200);
  Serial.println("ESP8266 UART Example");
  Serial.println("");
  
  Serial.println("Connecting to WiFi"); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  connectionDetails();
}

void loop() {
  if (Serial.available() && !receivedFlag) {
    String receivedData = Serial.readString();
    Serial.println("Received: " + receivedData);
    Serial.println("");
    dataToBeSent = receivedData;
    receivedFlag = true;
  }

  if (receivedFlag) {
    delay(10000);
  
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;

        Serial.println("Requesting: --> " + getTimeUrl);

        if (https.begin(client, getTimeUrl)) {
          int httpCode = https.GET();

          Serial.println("Response code <--: " + String(httpCode));
          Serial.println("");

          if (httpCode > 0) {
            String response = https.getString();
            deserializeJson(doc, https.getString());
            String time = doc["dateTime"];
            currentTime = time;
            Serial.println("The current datetime is: " + currentTime); 
            Serial.println("");
          }

          https.end();
        } 
        else {
          Serial.printf("[HTTPS] Unable to connect\n");
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;
        String fullUrl = url + key + currentTime;
        Serial.println("Requesting: --> " + fullUrl);

        if (https.begin(client, fullUrl)) {
            https.addHeader("Content-Type", "application/x-www-form-urlencoded");

            int httpCode = https.POST(dataToBeSent);

            Serial.println("Response code <--: " + String(httpCode));

            if (httpCode > 0) {
                String response = https.getString();
                Serial.println("Successfully posted new data.");
                Serial.println("");
            }

            https.end();
        } 
        else {
          Serial.printf("[HTTPS] Unable to connect\n");
        }

        receivedFlag = false;
    }

    delay(30000);
  }
}