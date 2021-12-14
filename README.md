# Smart-Plant-Monitor

This project used three LEDS (red, yellow, green), two nodes (two Arduino boards or similar microcontrollers), two sensors (soil moisture and temperature), a cloud to upload data (ThingSpeak), and a mobile app for the user to monitor the data anywhere (Blynk).

The system monitors the soil moisture level as well as the temperature near the plant. The range of the soil moisture sensor is 0 - 1000, so if the sensor reads 500 or below, the yellow LED will light up to show the user that the plant must be watered. If it is above 500, the green LED will light up to show the user that the plant does not need to be watered. The optimal range for my house plant is 70 - 90 degrees Fahrenheit. So, if the temperature goes above or below that range, the red LED will light up to show the user that the temperature is too hot or too cold.

Using the MQTT protocol, the publisher would read the data from the sensors and send this information to the subscriber. Then, the subscriber would output the data to the user as well as upload it to a ThingSpeak channel and to the Blynk app on my phone. 

In terms of IoT architecture, the long-haul communicator and the agent was the subscriber. The subscriber was the last point before the cloud, and it also was where all the information was sent to. The publisher was both a thing (node) and a controller. The sensors were the soil moisture sensor and the temperature sensor. These were directly connected to the publisher. 


ThingSpeak Channel: https://thingspeak.com/channels/1559328
