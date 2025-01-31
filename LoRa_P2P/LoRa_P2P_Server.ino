long startTime;
bool rx_done = false;
bool received = false;
double myFreq = 905500000;
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 5;
String LoRaMessage = "";

enum RegisteredNodes{
  node_1,
  node_2,
  node_3,
  node_4,
  node_5,
  node_6,
  node_7,
  node_8,
  node_9,
  node_10,
  node_unknown
};


// Function to parse value from query string
String getQueryValue(String query, String key) {
  int startIndex = query.indexOf(key + "=");
  if (startIndex == -1) {
    return ""; // Key not found
  }
  startIndex += key.length() + 1; // Move index to the start of the value
  int endIndex = query.indexOf("&", startIndex);
  if (endIndex == -1) {
    endIndex = query.length(); // If no '&' is found, use the end of the string
  }
  return query.substring(startIndex, endIndex);
}

// Function to convert string to enum
RegisteredNodes stringToNodeEnum(String nodeValue) {
  if (nodeValue == "node-1") return node_1;
  else if (nodeValue == "node-2") return node_2;
  else if (nodeValue == "node-3") return node_3;
  else if (nodeValue == "node-4") return node_4;
  else if (nodeValue == "node-5") return node_5;
  else if (nodeValue == "node-6") return node_6;
  else if (nodeValue == "node-7") return node_7;
  else if (nodeValue == "node-8") return node_8;
  else if (nodeValue == "node-9") return node_9;
  else if (nodeValue == "node-10") return node_10;
  else return node_unknown;
}

void hexDump(uint8_t * buf, uint16_t len)
{
    LoRaMessage = "";

    received = true;
    
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
                {
  	                s[iy++] = c;
                    LoRaMessage += (char)c;
                }
  	            else{
  	                s[iy++] = '.';
                }
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

void recv_cb(rui_lora_p2p_recv_t data)
{
    rx_done = true;
    if (data.BufferSize == 0) {
        Serial.println("Empty buffer.");
        return;
    }
    char buff[92];
    sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
  	    data.BufferSize, data.Rssi, data.Snr);

    Serial.println();
    Serial.println(buff);
    //LoRaMessage = RxMessage(buff);
    hexDump(data.Buffer, data.BufferSize);
}

void send_cb(void)
{
    Serial.printf("P2P set Rx mode %s\r\n",
		api.lora.precv(3000) ? "Success" : "Fail");
}

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200); 
    Serial.println("Starting the LoRa Server");
    Serial.println("------------------------------------------------------");
    delay(2000);
    startTime = millis();

    pinMode(PA7, OUTPUT);

    if(api.lora.nwm.get() != 0)
    {
        Serial.printf("Set Node device work mode %s\r\n",
            api.lora.nwm.set() ? "Success" : "Fail");
        api.system.reboot();
    }

    Serial.println("P2P Start");
    Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
    Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
    Serial.printf("RUI API Version: %s\r\n",
  		api.system.apiVersion.get().c_str());
    Serial.printf("Firmware Version: %s\r\n",
  		api.system.firmwareVersion.get().c_str());
    Serial.printf("AT Command Version: %s\r\n",
  		api.system.cliVersion.get().c_str());
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
    Serial.printf("P2P set Rx mode %s\r\n",
  		api.lora.precv(3000) ? "Success" : "Fail");
}

void loop()
{
  bool registeredNode = true;
  api.lora.precv(5000);

  if (received){
    received = false;

    Serial.println("Message Received!");
    Serial.println("The message is: " + LoRaMessage);

    String query = LoRaMessage;
    String nodeValue = getQueryValue(query, "node");

    RegisteredNodes nodeEnum = stringToNodeEnum(nodeValue);

    if (nodeEnum == node_unknown) {
      Serial.println("Node found: " + nodeValue + " is not registered... Dropping received data!");
      registeredNode = false;
    }
    
    digitalWrite(PA7, HIGH);
    delay(5000);
    digitalWrite(PA7, LOW);
    delay(1000);

    int attempts = 0;

    const int max_attempts = 1;

    String ack = "ack";

    uint8_t payload[ack.length() + 1];

    ack.getBytes(payload, ack.length() + 1);

    bool send_result = false;

    while (!send_result && attempts < max_attempts) {
      send_result = api.lora.psend(ack.length() + 1, payload);

      Serial.printf("Server to Client Acknowledge %s\r\n", send_result ? "Success" : "Fail");

      attempts++;

      if (!send_result)
      {
        delay(1000);
      }

    }
    if (registeredNode){
      Serial1.println(LoRaMessage);
    }
  }  
}
