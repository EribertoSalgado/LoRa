#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import json

# TTN parameters
TTN_APP_ID = 'firstlora3'  # Replace with your TTN application ID
TTN_DEVICE_ID = 'transmitter1'  # Replace with your TTN device ID
TTN_ACCESS_KEY = 'NNSXS.BGHYYIOLYIKTX2ML6UQIKIS4JCNC2WMEDU7ZPXI.QYTUTEOOCFBBPNL3ZZYQZ4YICROES74XZINHITMGQSCEUC2BLDMA'  # Replace with your TTN access key
TTN_SERVER = 'nam1.cloud.thethings.network'  # TTN server (adjust for your region if necessary)
TTN_PORT = 1883  # Default MQTT port (use 8883 for TLS)

# MQTT topic format for TTN uplinks
TTN_TOPIC = f'v3/{TTN_APP_ID}@ttn/devices/{TTN_DEVICE_ID}/up'
global counter
counter = 0

# Callback when message is received
def on_message(client, userdata, message):
    print("Received message from TTN")
    global counter  # Declare that we are using the global variable
    try:
        # Decode the payload
        payload = json.loads(message.payload.decode('utf-8'))
        temp = payload['uplink_message']['decoded_payload'].get('temperature')

        # Check if the temperature value is captured and print it
        if temp is not None:
            print(f"Temperature reading: {temp} degrees")
            # Save the data to a text file
            with open('sensor_data.txt', 'a') as f:  # Open file in append mode
                f.write(f"Temperature: {temp} degrees\n")

        counter += 1
    except Exception as e:
        print(f"Error processing message: {e}")

# Callback when connected to TTN
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to TTN")
        client.subscribe(TTN_TOPIC)
    else:
        print(f"Failed to connect, return code {rc}")

# Setup MQTT client
client = mqtt.Client()

# Use correct authentication format for TTN
client.username_pw_set(f'{TTN_APP_ID}@ttn', password=TTN_ACCESS_KEY)

# Attach callback functions
client.on_connect = on_connect
client.on_message = on_message

# Optional: Enable logging for debugging
client.enable_logger()

# Connect to TTN (Consider using TLS if needed)
client.connect(TTN_SERVER, TTN_PORT, 60)

# Blocking loop to process messages
client.loop_forever()
