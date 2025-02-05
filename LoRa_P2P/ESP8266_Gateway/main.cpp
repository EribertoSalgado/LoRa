#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid  = "<>"; 
const char* password = "<>";

String key = "&time=";
String value = "";

const String url = "https://lightpink-sheep-430801.hostingersite.com/DataBaseUrlDataPushingPageP2P.php?";
const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

const size_t capacity = JSON_ARRAY_SIZE(10) + 10*JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument doc(capacity);

String dataToBeSent = "";
String currentTime;
bool receivedFlag = false;

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

  Serial.println("Connected to WiFi");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void loop() {
  if (Serial.available() && !receivedFlag) {
    String receivedData = Serial.readString();
    Serial.println("Received: " + receivedData);
    Serial.println("");

    // Example received data: node=node-1&light=0
    String node = "";
    String light = "";

    // Split the received string by '&' to separate node and light
    int nodeIndex = receivedData.indexOf("node=");
    int lightIndex = receivedData.indexOf("light=");
    
    if (nodeIndex != -1 && lightIndex != -1) {
      // Extract the 'node' value
      node = receivedData.substring(nodeIndex + 5, receivedData.indexOf("&", nodeIndex));
      // Extract the 'light' value
      light = receivedData.substring(lightIndex + 6);

      Serial.println("Extracted Node: " + node);
      Serial.println("Extracted Light: " + light);

      // Get current time
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

      // Construct the URL with node, light, and time values
      String fullUrl = url + "node=" + node + "&light=" + light + "&time=" + currentTime;
      delay(1000);
      Serial.println("Requesting: --> " + fullUrl);
      Serial.println("ERIBERTO");


      // Send the POST request
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;

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
      }
    }

    receivedFlag = false;
  }

  delay(30000);  // Wait for 30 seconds before the next iteration
}
