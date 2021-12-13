/// Author: ajfrattarole
/// Last Edited: 12/13/2021

#include <SPI.h>
#include <ArduinoMqttClient.h>
#include <BlynkSimpleMKR1000.h>
#include <BlynkSimpleWiFiShield101.h>
#include "ThingSpeak.h"
#include "WiFi101.h"

// information needed to connect to Blynk
#define BLYNK_TEMPLATE_ID "TMPLZdixb3Jn"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "jGfTovCm53rIrx6hxX_Irp9cX_83yof7"

// information needed to connect to wifi
WiFiClient client;
char ssid[] = "ARRIS-B33D";
char pass[] = "5G5344106449";
char auth[] = BLYNK_AUTH_TOKEN;
int status = WL_IDLE_STATUS;

// information needed to connect to MQTT broker
MqttClient mqttClient(client);
char broker[] = "broker.emqx.io";
char topic[] = "plant/SoilMoisture";
char topic2[] = "plant/Temperature";
int port = 1883;

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);

  // displays an error message or a success message
  if (status != WL_CONNECTED)
  {
    Serial.println("Couldn't get a wifi connection :(");
    while(true);
  }
  else
  {
    Serial.print("Connected to network: ");
    Serial.println(ssid);
  }

  // attempts to connect to broker, displays proper message
  Serial.println("Attempting to connect to MQTT Broker..");
  if (!mqttClient.connect(broker, port))
  {
    Serial.println("MQTT Connection failed :(");
    Serial.print("Error Code: ");
    Serial.println(mqttClient.connectError());
  }
  else
  {
    Serial.println("Connected to broker");
  }

  // connects to Blynk
  Blynk.begin(auth, ssid, pass);

  // outputs to the user that the subscriber is 
  // subscribing to the topics
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topic2);
  Serial.println();

  // the subscriber subscribes to the temperature
  // and soil moisture topics
  mqttClient.subscribe(topic);
  mqttClient.subscribe(topic2);

  // connects to ThingSpeak
  ThingSpeak.begin(client);

  // outputs buffer
  Serial.println(" ");
  Serial.println("===============================");
}

void loop() 
{
  // while internet is connected, proceed
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }
  // keeps MQTT connection active
  mqttClient.poll();
  // runs Blynk connection
  Blynk.run();

  // while the subscriber is connected to 
  // the broker
  while (mqttClient.available())
  {
    // outputs the topic that the message was received from
    Serial.print("Received message from topic ");
    Serial.println(mqttClient.messageTopic());

    // if the topic was temperature
    if (mqttClient.messageTopic() == topic)
    {
      // the message will be put into soilInput, and sent
      // to ThingSpeak and Blynk. then outputted to user
      int soilInput = (int)mqttClient.read();
      int soilMoist = ThingSpeak.writeField(1559328, 1, soilInput, "LLM73JUD853RCETL");
      Blynk.virtualWrite(V0, soilInput);
      Serial.print("Soil Moisture Level: ");
      Serial.println(soilInput);
    }

    // if the topic is soil moisture
    if (mqttClient.messageTopic() == topic2)
    {
      // the message will be put into tempInput, and sent
      // to ThingSpeak and Blynk, then outputted to the user
      int tempInput = (int)mqttClient.read();
      float temperature = ThingSpeak.writeField(1559328, 2, tempInput, "LLM73JUD853RCETL");
      Blynk.virtualWrite(V1, tempInput);
      Serial.print("Temperature (F): ");
      Serial.println(tempInput);
    }
  }

  // output buffer
  Serial.println(); Serial.println();
}
