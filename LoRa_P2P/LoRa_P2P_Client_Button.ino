#include <Arduino.h>
 #include <ESP8266WiFi.h>
 #include <ESP8266HTTPClient.h>
 #include <sendRequest.h> 
 #include <ArduinoJson.h>
 
 #define BUTTON_PIN PA7
 #define LED_PIN PA4
 
 long startTime;
 bool rx_done = false;
 double myFreq = 905500000;
 uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 5;
 
 uint64_t data = 0;
 String edgeNodeName = "node-1";
 
 void hexDump(uint8_t * buf, uint16_t len)
 {
     char alphabet[17] = "0123456789abcdef";
     Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
     Serial.print(F("   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |\r\n"));
     for (uint16_t i = 0; i < len; i += 16) {
         if (i % 128 == 0)
             Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
         char s[] = "|                                                | |                |\r\n";
         uint8_t ix = 1, iy = 52;
         for (uint8_t j = 0; j < 16; j++) {
             if (i + j < len) {
   	            uint8_t c = buf[i + j];
   	            s[ix++] = alphabet[(c >> 4) & 0x0F];
   	            s[ix++] = alphabet[c & 0x0F];
   	            ix++;
   	            if (c > 31 && c < 128)
   	                s[iy++] = c;
   	            else
   	                s[iy++] = '.';
             }
         }
         uint8_t index = i / 16;
         if (i < 256)
             Serial.write(' ');
         Serial.print(index, HEX);
         Serial.write('.');
         Serial.print(s);
     }
     Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
 }
 const char* ssid  = "Erick's iPhone"; 
 const char* password = "Ramirez510";
 
 void parseHexDump(uint8_t *buf, uint16_t len) {
     Serial.println("Parsed Data:");
     for (uint16_t i = 0; i < len; i++) {
         if (buf[i] >= 32 && buf[i] < 127) {
             Serial.print((char)buf[i]);
         } else {
             Serial.print('.');
         }
     }
     Serial.println();
 }
 const String url = "https://lightpink-sheep-430801.hostingersite.com/DataBaseUrlDataPushingPageP2P.php?";
 const String getTimeUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";
 
 void recv_cb(rui_lora_p2p_recv_t data)
 {
     rx_done = true;
 const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(3) + 200;
 DynamicJsonDocument doc(capacity);
 
     if (data.BufferSize == 0) {
         Serial.println("Empty buffer.");
         return;
     }
 String dataToBeSent = "";
 String currentTime;
 bool receivedFlag = false;
 String nodeValue, lightValue;
 
     char buff[92];
     sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d", data.BufferSize, data.Rssi, data.Snr);
     Serial.println("The raw message:");
     Serial.println(buff);
     Serial.println();
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
 
     hexDump(data.Buffer, data.BufferSize);
   Serial.println("\nConnected to WiFi.");
 }
 
 void send_cb(void)
 {
   Serial.println("DATA SENT!!!");
   // Turn off the LED now that the data has been sent.
   digitalWrite(LED_PIN, LOW);
   delay(1000);
   Serial.printf("P2P set Rx mode %s\r\n", api.lora.precv(10000) ? "Success" : "Fail");
 }
 void loop() {
   if (Serial.available() && !receivedFlag) {
     String receivedData = Serial.readString();
     Serial.println("Received: " + receivedData);
     Serial.println("");
 
     // Parse receivedData if it's in the format "node=1&light=300"
     int nodeIndex = receivedData.indexOf("node=");
     int lightIndex = receivedData.indexOf("light=");
     
     if (nodeIndex != -1 && lightIndex != -1) {
         nodeValue = receivedData.substring(nodeIndex + 5, receivedData.indexOf("&", nodeIndex)); 
         lightValue = receivedData.substring(lightIndex + 6);
     } else {
         Serial.println("Invalid data format received.");
         return;
     }
 
 void setup()
 {
   Serial.begin(115200);
   Serial.println("LoRa Client Set up!");
   Serial.println("------------------------------------------------------");
   delay(2000);
   pinMode(LED_PIN, OUTPUT);
   pinMode(BUTTON_PIN, INPUT);
   startTime = millis();
 
   if(api.lora.nwm.get() != 0)
   {
     Serial.printf("Set Node device work mode %s\r\n",
     api.lora.nwm.set() ? "Success" : "Fail");
     api.system.reboot();
     receivedFlag = true;
   }
 
   Serial.println("P2P Start");
   Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
   Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
   Serial.printf("RUI API Version: %s\r\n", api.system.apiVersion.get().c_str());
   Serial.printf("Firmware Version: %s\r\n", api.system.firmwareVersion.get().c_str());
   Serial.printf("AT Command Version: %s\r\n", api.system.cliVersion.get().c_str());
   Serial.printf("Set P2P mode frequency %3.3f: %s\r\n", (myFreq / 1e6),
   api.lora.pfreq.set(myFreq) ? "Success" : "Fail");
   Serial.printf("Set P2P mode spreading factor %d: %s\r\n", sf,
   api.lora.psf.set(sf) ? "Success" : "Fail");
   Serial.printf("Set P2P mode bandwidth %d: %s\r\n", bw,
   api.lora.pbw.set(bw) ? "Success" : "Fail");
   Serial.printf("Set P2P mode code rate 4/%d: %s\r\n", (cr + 5),
   api.lora.pcr.set(cr) ? "Success" : "Fail");
   Serial.printf("Set P2P mode preamble length %d: %s\r\n", preamble,
   api.lora.ppl.set(preamble) ? "Success" : "Fail");
   Serial.printf("Set P2P mode tx power %d: %s\r\n", txPower,
   api.lora.ptp.set(txPower) ? "Success" : "Fail");
   api.lora.registerPRecvCallback(recv_cb);
   api.lora.registerPSendCallback(send_cb);
   Serial.printf("P2P set Rx mode %s\r\n", api.lora.precv(3000) ? "Success" : "Fail");
 
   rx_done = true;
 }
 
 bool ReadSensor() {
   // Return true if the button is pressed (HIGH)
   return (digitalRead(BUTTON_PIN) == HIGH);
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
             // dB set up to only take 2 digit light values
             //dataToBeSent = "node=" + nodeValue + "&time=" + currentTime + "&light=" + lightValue;
             //there may be an empty space or null operator from the lightValue
             dataToBeSent = "node=" + nodeValue + "&time=" + currentTime + "&light=1";
             Serial.println("Updated dataToBeSent: " + dataToBeSent);
           }
           https.end();
         } 
         else {
           Serial.printf("[HTTPS] Unable to connect to time server\n");
         }
     }
 
 void loop()
 {   
     digitalWrite(LED_PIN, HIGH);
     if (ReadSensor()){
         // Turn off the LED immediately when the button is pressed
         digitalWrite(LED_PIN, LOW);
     if (WiFi.status() == WL_CONNECTED && currentTime != "") { // Ensure time is set
         WiFiClientSecure client;
         client.setInsecure();
         HTTPClient https;
         String fullUrl = url + dataToBeSent;
         Serial.println("Requesting: --> " + fullUrl);
 
         // Generate payload string with a fixed sensor value ("1" when pressed)
         String temp = "node=" + edgeNodeName + "&" + "light=1";
         if (https.begin(client, fullUrl)) {
             https.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
         Serial.println("Generated Payload String:");
         Serial.println(temp);
             Serial.println("Final dataToBeSent: " + dataToBeSent);
             int httpCode = https.POST(dataToBeSent);
             Serial.println("Response code <--: " + String(httpCode));
 
         uint8_t payload[temp.length()];
         temp.getBytes(payload, temp.length() + 1); 
             if (httpCode > 0) {
                 Serial.println("Successfully posted new data.");
                 Serial.println("");
             }
 
         Serial.println("Converted Payload (HEX):");
         for (size_t i = 0; i < temp.length(); i++) {
             Serial.printf("%02X ", payload[i]);
             https.end();
         } 
         else {
           Serial.printf("[HTTPS] Unable to connect\n");
         }
         Serial.println();
 
         Serial.println("Converted Payload (ASCII):");
         for (size_t i = 0; i < temp.length(); i++) {
             Serial.print((char)payload[i]);
         }
         Serial.println();
         
         if (rx_done) {
             rx_done = false;
             bool send_result = api.lora.psend(temp.length(), payload);
             Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
         }
         
         delay(5000);  // Debounce delay
         receivedFlag = false;
     }
 
     delay(30000);
   }
 }
