#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sendRequest.h> 
#include <ArduinoJson.h>

const char* ssid  = ""; 
const char* password = "";

const String url = "https://lightpink-sheep-430801.hostingersite.com/DataBaseUrlDataPushingPageP2P.php?";
const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument doc(capacity);

String dataToBeSent = "";
String currentTime;
bool receivedFlag = false;
String nodeValue, lightValue;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 UART Example");
  Serial.println("");
  
  Serial.println("Connecting to WiFi..."); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi.");
}

void loop() {
  if (Serial.available() && !receivedFlag) {
    String receivedData = Serial.readString();
    Serial.println("Received: " + receivedData);
    Serial.println("");

    // Parse received if it's in the format "node=1&light=300"
    int nodeIndex = receivedData.indexOf("node=");
    int lightIndex = receivedData.indexOf("light=");
    
    if (nodeIndex != -1 && lightIndex != -1) {
        nodeValue = receivedData.substring(nodeIndex + 5, receivedData.indexOf("&", nodeIndex)); 
        lightValue = receivedData.substring(lightIndex + 6);
    } else {
        Serial.println("Invalid data format received.");
        return;
    }

    receivedFlag = true;
  }

  if (receivedFlag) {
    delay(10000);
  
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;

        Serial.println("Requesting current time: --> " + getTimeUrl);

        if (https.begin(client, getTimeUrl)) {
          int httpCode = https.GET();
          Serial.println("Response code <--: " + String(httpCode));
          Serial.println("");

          if (httpCode > 0) {
            String response = https.getString();
            deserializeJson(doc, response);
            currentTime = String(doc["dateTime"]);
            Serial.println("The current datetime is: " + currentTime); 
            Serial.println("");

            // **Update `dataToBeSent` now that we have `currentTime`**
            dataToBeSent = "node=" + nodeValue + "&time=" + currentTime + "&light=" + lightValue;
            Serial.println("Updated dataToBeSent: " + dataToBeSent);
          }
          https.end();
        } 
        else {
          Serial.printf("[HTTPS] Unable to connect to time server\n");
        }
    }

    if (WiFi.status() == WL_CONNECTED && currentTime != "") { // Ensure time is set
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;
        String fullUrl = url + "time=" + currentTime;
        Serial.println("Requesting: --> " + fullUrl);

        if (https.begin(client, fullUrl)) {
            https.addHeader("Content-Type", "application/x-www-form-urlencoded");

            Serial.println("Final dataToBeSent: " + dataToBeSent);
            int httpCode = https.POST(dataToBeSent);

            Serial.println("Response code <--: " + String(httpCode));

            if (httpCode > 0) {
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
