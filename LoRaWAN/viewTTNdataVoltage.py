#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import json
from datetime import datetime

# TTN parameters
TTN_APP_ID = 'firstlora3'  # Replace with your TTN application ID
TTN_DEVICE_ID = 'transmitter1'  # Replace with your TTN device ID
TTN_ACCESS_KEY = 'NNSXS.BGHYYIOLYIKTX2ML6UQIKIS4JCNC2WMEDU7ZPXI.QYTUTEOOCFBBPNL3ZZYQZ4YICROES74XZINHITMGQSCEUC2BLDMA'  # Replace with your TTN access key
TTN_SERVER = 'nam1.cloud.thethings.network'  # TTN server (adjust if needed)
TTN_PORT = 1883  # Use 8883 with TLS
# TTN_PORT = 8883  # Uncomment this and client.tls_set() for TLS

# MQTT topic for your device's uplink messages
TTN_TOPIC = f'v3/{TTN_APP_ID}@ttn/devices/{TTN_DEVICE_ID}/up'

# Counter for number of messages received
counter = 0

# Callback for when a message is received
def on_message(client, userdata, message):
    global counter
    print("Received message from TTN")
    try:
        # Parse the incoming JSON payload
        payload = json.loads(message.payload.decode('utf-8'))

        # Extract the voltage value from decoded payload
        voltage = payload['uplink_message']['decoded_payload'].get('voltage')

        if voltage is not None:
            # Display voltage
            print(f"Voltage reading: {voltage} V")

            # Timestamped logging
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            with open('sensor_data.txt', 'a') as f:
                f.write(f"{timestamp} - Voltage: {voltage} V\n")

        counter += 1
        print(f"Messages received: {counter}\n")

    except Exception as e:
        print(f"Error processing message: {e}")

# Callback for when client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to TTN")
        client.subscribe(TTN_TOPIC)
    else:
        print(f"Failed to connect, return code {rc}")

# Create MQTT client and set credentials
client = mqtt.Client()
client.username_pw_set(f'{TTN_APP_ID}@ttn', password=TTN_ACCESS_KEY)

# Optional TLS support (uncomment to enable)
# client.tls_set()

# Assign event callbacks
client.on_connect = on_connect
client.on_message = on_message

# Connect and start listening
client.connect(TTN_SERVER, TTN_PORT, 60)
client.loop_forever()
