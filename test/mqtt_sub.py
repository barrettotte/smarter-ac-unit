import paho.mqtt.client as mqtt
import random

broker = 'giza.agartha'
port = 1883
ttl = 60
client_id = f'test-mqtt={random.randint(0, 1000)}'
# topic = 'smarter_ac_unit/temperature/set'
topic = 'homeassistant/climate/smarter-ac-unit/config'

def connect_mqtt() -> mqtt:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def subscribe(client: mqtt):
    def on_message(client, user_data, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(topic)
    client.on_message = on_message

def main():
    client = connect_mqtt()
    subscribe(client)
    try:
        client.loop_forever()
    except KeyboardInterrupt as e:
        pass

if __name__ == '__main__':
    main()
