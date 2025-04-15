// Final Draft
#include <Arduino.h>

// LoRa Configuration
long startTime;
bool rx_done = false;
bool received = false;
double myFreq = 905500000;
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 5;
String LoRaMessage = "";
int lastRSSI = 0;

// Node identification
enum RegisteredNodes {
  node_1, node_2, node_3, node_4, node_5,
  node_6, node_7, node_8, node_9, node_10,
  node_unknown
};

// Extract query values like "key=value"
String getQueryValue(String query, String key) {
  int startIndex = query.indexOf(key + "=");
  if (startIndex == -1) return "";
  startIndex += key.length() + 1;
  int endIndex = query.indexOf("&", startIndex);
  if (endIndex == -1) endIndex = query.length();
  return query.substring(startIndex, endIndex);
}

// HEX to ASCII and capture message
void hexDump(uint8_t *buf, uint16_t len) {
  LoRaMessage = "";
  received = true;

  Serial.println("\n--- Raw Data HEX Dump ---");
  for (uint16_t i = 0; i < len; i++) {
    Serial.printf("%02X ", buf[i]);
    if (buf[i] >= 32 && buf[i] <= 126) {
      LoRaMessage += (char)buf[i];
    } else {
      LoRaMessage += '.';
    }
  }
  Serial.println("\n-------------------------");
}

// Callback when message received
void recv_cb(rui_lora_p2p_recv_t data) {
  if (data.BufferSize == 0) {
    Serial.println("[DEBUG] Empty message received. Ignored.");
    return;
  }

  rx_done = true;
  lastRSSI = data.Rssi;

  Serial.println("\nIncoming message received!");
  Serial.printf("Message Length: %d, RSSI: %d dBm, SNR: %d dB\n",
                data.BufferSize, data.Rssi, data.Snr);

  hexDump(data.Buffer, data.BufferSize);
}

// Callback when message sent
void send_cb(void) {
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lora.precv(3000) ? "Success" : "Fail");
}

// Setup
void setup() {
  Serial.begin(115200);
  Serial.println("Board is booting...");
  delay(1000);

  Serial1.begin(115200);
  pinMode(PA4, OUTPUT); // LED indicator

  Serial.println("Checking LoRa Initialization...");

  if (api.lora.nwm.get() != 0) {
    Serial.println("[ERROR] LoRa not in correct mode. Trying to set...");
    bool success = api.lora.nwm.set();
    Serial.printf("[DEBUG] LoRa mode set: %s\r\n", success ? "Success" : "Fail");
    if (!success) {
      Serial.println("[ERROR] LoRa initialization failed. Rebooting...");
      api.system.reboot();
    }
  }

  Serial.println("[DEBUG] LoRa Initialization Complete. Setting Parameters...");

  Serial.printf("Set Frequency %3.3f MHz: %s\r\n", myFreq / 1e6, api.lora.pfreq.set(myFreq) ? "Success" : "Fail");
  Serial.printf("Set Spreading Factor %d: %s\r\n", sf, api.lora.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set Bandwidth %d: %s\r\n", bw, api.lora.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set Code Rate 4/%d: %s\r\n", (cr + 5), api.lora.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set Preamble Length %d: %s\r\n", preamble, api.lora.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set TX Power %d dBm: %s\r\n", txPower, api.lora.ptp.set(txPower) ? "Success" : "Fail");

  api.lora.registerPRecvCallback(recv_cb);
  api.lora.registerPSendCallback(send_cb);

  bool rx_status = api.lora.precv(5000);
  Serial.printf("[DEBUG] RX Mode Status: %s\n", rx_status ? "Active" : "Fail");

  if (!rx_status) {
    Serial.println("[ERROR] RX mode failed! Stopping execution...");
    while (true) { delay(1000); }
  }

  Serial.println("[DEBUG] Setup Complete!");
}

// Main loop
void loop() {
  Serial.println("Loop is running... Checking for messages.");

  bool rx_status = api.lora.precv(5000);
  Serial.printf("RX Mode Status: %s\n", rx_status ? "Active" : "Fail");

  if (received) {
    received = false;

    Serial.println("Message Received!");
    Serial.print("LoRaMessage: ");
    Serial.println(LoRaMessage);

    String nodeValue = getQueryValue(LoRaMessage, "node");
    if (nodeValue == "") {
      Serial.println("[ERROR] No 'node' value found in message.");
      return;
    }

    Serial.println("Node Value Extracted: " + nodeValue);

    digitalWrite(PA4, HIGH);
    delay(5000);
    digitalWrite(PA4, LOW);
    delay(1000);

    String ack = "ack";
    uint8_t payload[ack.length() + 1];
    ack.getBytes(payload, ack.length() + 1);

    bool send_result = false;
    int attempts = 0;
    const int max_attempts = 1;

    while (!send_result && attempts < max_attempts) {
      send_result = api.lora.psend(ack.length() + 1, payload);
      Serial.printf("Acknowledgment Sent: %s\r\n", send_result ? "Success" : "Fail");
      if (!send_result) delay(1000);
      attempts++;
    }

    // Send final message with RSSI to ESP
    String extendedMessage = LoRaMessage + "&rssi=" + String(lastRSSI);
    Serial1.println(extendedMessage);
  }

  delay(2000); // avoid serial flooding
}
