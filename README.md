# Basic-Air-Quality-Monitor-With-MQ135-And-DHT11
A basic Internet of Things (IoT) project that uses ESP8266, MQ135 Air Quality Sensor, and temperature and humidity sensor DHT11 to process and evaluate air quality based on CO2 parts per million (PPM). Those value will be send to Firebase Real-time Database and Blynk IoT software platform to be monitored.

## Installation
- ESP8266 Board for Arduino IDE (https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
- CH340 Driver (https://sparks.gogo.co.nz/ch340.html)
- LiquidCrystal I2C Library by Frank de Brabander
- NTP Client Library by Fabrice Weinberg
- WiFiNINA Library by Arduino
- Blynk Library by Volodymyr Shymanskyy
- BlynkNcpDriver by Volodymyr Shymanskyy
- DHT sensor Library by Adafruit
- Firebase Arduino Client Library for ESP8266 by mobizt
- Firebase ESP8266 Client by mobizt
- MQ135 Library by GeorgeK,ViliusKraujutis,..
- Time Librayr by Michael Margolis

## Devices
- An I2C LiquidCrystal LCD Screen
- An ESP8266
- An MQ135 Air Quality Monitor Sensor
- An DHT11 Temperature and Humidity Sensor

## Setup Blynk
- Create an account and sign in
- Go to developer zone and create a template for Air Quality Monitor
- Go into Datastream and setup virtual pins to push data onto Blynk
- Virtual pins will be setup as follows:
  -  V0 is for CO2 PPM, Data Type: Double, Min: 0, Max:2000, Decimals=#.##,Default value: None
  -  V1 is for Temperature, Data Type: Double,Units: Celsius, Min: 0, Max:100, Decimals=#.##,Default value: None
  -  V2 is for Humidity, Data Type: Double, Units: Percentage, Min: 0, Max:100, Decimals=#.##,Default value: None
  -  V3 is for Air Quality, Data Type: String
- After that, go back to homepage and create a device with Air Quality Monitor template
- Go into the device info and copy your BLYNK_TEMPLATE_ID,BLYNK_TEMPLATE_NAME,BLYNK_AUTH_TOKEN and replace it in the code

## Setup Firebase
- Create a Firebase Project
- Create Real-time Database in Test mode
- Copy database credentials (Web API, Database URL, Email and Password(if needed)) (You just need Web API and Database URL if anonymous sign in is available)
- Create a variable to store your database credentials for database connection

## References
- https://github.com/mobizt/Firebase-ESP8266/tree/master#store-data
- https://github.com/blynkkk/blynk-library
