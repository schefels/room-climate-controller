# Room Climate Controller
![picture of assembled room climate controller](https://github.com/schefels/room-climate-controller/blob/main/room-climate-controller.jpg?raw=true)

## Circuit board from Make
https://www.heise.de/news/Aktion-Make-verschenkt-Platinen-fuer-die-CO2-Ampel-4938738.html

## Also inspired by
https://www.heise.de/select/make/2020/5/2022015381334973804

## Features
* Controller: ESP8266WiFi

* In a room, it measures (SparkFun SCD30):
  - CO2 level
  - Temperatur
  - Humidity

* Displays all measured values on a Adafruit 7-segment matrix
* Displays measured CO2 level on a traffic light (three LEDs: green, yellow, red)
  - see Pettenkofer-Zahl: https://www.svlw.ch/427-pettenkofer-zahl-und-co2-konzentrationen

## Python script
* Live plot of:
  - CO2 level
  - Temperatur
  - Humidity

![Time plot of the Python script](https://github.com/schefels/room-climate-controller/blob/main/time-plot.png?raw=true)
