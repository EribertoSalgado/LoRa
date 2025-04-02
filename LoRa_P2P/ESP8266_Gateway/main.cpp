#include <Arduino.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sendRequest.h> 
#include <ArduinoJson.h>

const char* ssid  = "Erick's iPhone"; 
const char* password = "Ramirez510";

const String url = "https://lightpink-sheep-430801.hostingersite.com/DataBaseUrlDataPushingPageP2P.php?";
const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(3) + 200;
DynamicJsonDocument doc(capacity);

String dataToBeSent = "";
String currentTime;
bool receivedFlag = false;
String nodeValue, lightValue, rssiValue;

String getParamValue(String data, String key) {
  int keyIndex = data.indexOf(key + "=");
  if (keyIndex == -1) return "";

  int valueStart = keyIndex + key.length() + 1;
  int valueEnd = data.indexOf("&", valueStart);
  if (valueEnd == -1) valueEnd = data.length();

  return data.substring(valueStart, valueEnd);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 UART Example\n");

  Serial.println("Connecting to WiFi..."); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi.");
  Serial.println("LoRaP2P Ready! HereIam3!");
}

void loop() {
  if (Serial.available() && !receivedFlag) {
    String receivedData = Serial.readString();
    Serial.println("Received: " + receivedData);
    Serial.println("");

    // Clean parsing
    nodeValue = getParamValue(receivedData, "node");
    lightValue = getParamValue(receivedData, "light");
    rssiValue = getParamValue(receivedData, "rssi");

    if (nodeValue != "" && lightValue != "" && rssiValue != "") {
      receivedFlag = true;
    } else {
      Serial.println("Invalid data format received.");
      return;
    }
  }

  if (receivedFlag) {
    delay(10000); // Allow 10 seconds before fetching time

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

          dataToBeSent = "node=" + nodeValue + "&time=" + currentTime + "&light=" + lightValue + "&rssi=" + rssiValue; 
          Serial.println("Updated dataToBeSent: " + dataToBeSent);
        }
        https.end();
      } else {
        Serial.println("[HTTPS] Unable to connect to time server");
      }
    }

    if (WiFi.status() == WL_CONNECTED && currentTime != "") {
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient https;
      String fullUrl = url + dataToBeSent;
      Serial.println("Requesting: --> " + fullUrl);

      if (https.begin(client, fullUrl)) {
        https.addHeader("Content-Type", "application/x-www-form-urlencoded");

        Serial.println("Final dataToBeSent: " + dataToBeSent);
        int httpCode = https.POST(dataToBeSent);
        Serial.println("Response code <--: " + String(httpCode));

        if (httpCode > 0) {
          Serial.println("Successfully posted new data.\n");
        }

        https.end();
      } else {
        Serial.println("[HTTPS] Unable to connect");
      }

      receivedFlag = false;
    }

    delay(30000);
    Serial.println("Ready for next client input...");
  }
}
