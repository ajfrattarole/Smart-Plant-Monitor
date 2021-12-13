/// Author: ajfrattarole
/// Last Edited: 12/13/2021

#include <SPI.h>
#include <ArduinoMqttClient.h>
#include "WiFi101.h"

// data needed to connect to wifi
WiFiClient client;
char ssid[] = "ARRIS-B33D";
char pass[] = "5G5344106449";
int status = WL_IDLE_STATUS;

// data needed to connect to MQTT broker
MqttClient mqttClient(client);
char broker[] = "broker.emqx.io";
char topic[] = "plant/SoilMoisture";
char topic2[] = "plant/Temperature";
int port = 1883;

// sensor and LED pins
int soilPin = A0;
int temperaturePin = A1;
int greenPin = 6;
int yellowPin = 7;
int redPin = 8;


void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);

  // outputs appropriate message based on if
  // the wifi was connected or not
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

  // displays appropriate message based on
  // if the broker was connected to or not
  if (!mqttClient.connect(broker, port))
  {
    Serial.println("MQTT Connection failed :(");
  }
  else
  {
    Serial.println("Connected to MQTT Broker");
  }

    // sets the sensor pins as input, and the
    // LED pins as output
    pinMode(temperaturePin, INPUT);
    pinMode(soilPin, INPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(redPin, OUTPUT);

  // output a buffer
  Serial.println(" ");
  Serial.println("===============================");
}

void loop() 
{
  // while the wifi is connected, proceed
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }

  // keeps the MQTT connection going
  mqttClient.poll();

  // int variable for temperature reading
  int soilInput = analogRead(soilPin);
  int tempInput = analogRead(temperaturePin);

  // changes the range of the soil moisture sensor
  soilInput = map(soilInput,0,1023,0,1000);

  // the next 4 lines convert the temperature from
  // voltage, to celsius, to fahrenheit
  float voltage = tempInput * 3.300;
  voltage /= 1024.0;
 
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;

  // if the soil moisture level is 500 or below, turn off
  // green LED and turn on yellow LED. if it is above 500,
  // turn off yellow LED and turn on green LED
  if (soilInput <= 500)
  {
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, LOW);
  }
  else
  {
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
  }

  // if the temperature is below 70 or above 90
  // degrees F, turn the red LED on. otherwise, 
  // keep it off
  if (temperatureF < 70 || temperatureF > 90)
  {
    digitalWrite(redPin, HIGH);
  }
  else
  {
    digitalWrite(redPin, LOW);
  }

  // sends the "Temperature" topic name
  // and the temperature data
  mqttClient.beginMessage(topic);
  mqttClient.print(soilInput);
  mqttClient.endMessage();

  // sends the "SoilMoisture" topic name
  // and the soil moisture data
  mqttClient.beginMessage(topic2);
  mqttClient.print(temperatureF);
  mqttClient.endMessage();

  // 5 second delay
  delay(5000);
}
