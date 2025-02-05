// remove Tx and Rx from ESP on each upload then reconnect
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sendRequest.h> 
#include <ArduinoJson.h>

const char* ssid  = "<>"; 
const char* password = "<>";

const String url = "https://lightpink-sheep-430801.hostingersite.com/DataBaseUrlDataPushingPageP2P.php?";
const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument doc(capacity);

String dataToBeSent = "";
String currentTime;
bool receivedFlag = false;
String node = "";  // 🔹 Declared globally
String light = ""; // 🔹 Declared globally

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 UART Example");
  Serial.println("");
  
  Serial.println("Connecting to WiFi"); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
}

void loop() {
  if (Serial.available() && !receivedFlag) {
    String receivedData = Serial.readString();
    Serial.println("Received: " + receivedData);
    Serial.println("");

    dataToBeSent = receivedData;
    receivedFlag = true;

    // Extract node and light values
    node = "";
    light = "";

    int nodeIndex = receivedData.indexOf("node=");
    int lightIndex = receivedData.indexOf("light=");
    
    if (nodeIndex != -1 && lightIndex != -1) {
      node = receivedData.substring(nodeIndex + 5, receivedData.indexOf("&", nodeIndex));
      light = receivedData.substring(lightIndex + 6);

      Serial.println("Extracted Node: " + node);
      Serial.println("Extracted Light: " + light);
    }
  }

  if (receivedFlag) {
    delay(10000);  // Wait before sending requests

    // Fetch the current time
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
          deserializeJson(doc, response);
          currentTime = doc["dateTime"].as<String>();
          Serial.println("The current datetime is: " + currentTime); 
          Serial.println("");
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }

    // Send data to the server
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient https;
      delay(5000);
      String fullUrl = url + "node=" + node + "&time=" + currentTime + "&light=" + light;
      delay(5000);
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
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      receivedFlag = false;
    }

    delay(30000);  // Wait before checking for new data
  }
}
