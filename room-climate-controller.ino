#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <ESP8266WiFi.h>

// https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
SCD30 airSensorSCD30; // Objekt SDC30 sensor
unsigned short co2 = 0; // CO2
float temp = 0; // temperature
float humidity = 0; // humidity
const byte measurementInterval = 30;  // CO2-measurement intervall in seconds (max 1800)
const float tempOffset = 2; // temperature offset up to 5°C, stored in non-volatile memory of SCD30
const short alt = 520; // Set altitude of the sensor in m, stored in non-volatile memory of SCD30

// Adafruit Feather 7 Segment https://learn.adafruit.com/adafruit-7-segment-led-featherwings/overview
Adafruit_7segment matrix7Seg = Adafruit_7segment();
const unsigned int matrixLag = measurementInterval/3 * 1000;
const byte brightness = 3; // 0-15
const byte blinkRate = 0; // 0-3 

// Traffic Lights
const byte greenLED = 14;
const byte yellowLED = 13;
const byte redLED = 12;

// Traffic Lights limits (Pettenkofer-Zahl)
const short goodAir = 800;
const short moderateAir = 1200;
const short unhealthyAir = 1400;
const short veryUnhealthyAir = 2000;

void setup() {
  // ESP8266WiFi config
  WiFi.forceSleepBegin(); // WIFI off
  pinMode( greenLED, OUTPUT);
  pinMode( yellowLED, OUTPUT);
  pinMode( redLED, OUTPUT);
  Wire.begin(); // ---- Initialisiere den I2C-Bus
  if ( Wire.status() != I2C_OK ) Serial.println("Something wrong with I2C.");
  if ( airSensorSCD30.begin() == false ) {
    Serial.println("The SCD30 did not respond. Please check wiring.");
    while (1) {
      yield();
      delay(1);
    }
  }
  Wire.setClock( 100000L ); // 100 kHz SCD30
  Wire.setClockStretchLimit( 200000L ); // CO2-SCD30
  Serial.begin( 115200 );
  digitalWrite( greenLED, LOW );
  digitalWrite( yellowLED, LOW );
  digitalWrite( redLED, LOW );
  // SCD30 config
  airSensorSCD30.setAutoSelfCalibration(false); // Sensirion no auto calibration
  if (airSensorSCD30.getAltitudeCompensation() != alt ) airSensorSCD30.setAltitudeCompensation( alt );
  if (airSensorSCD30.getTemperatureOffset() != tempOffset ) airSensorSCD30.setTemperatureOffset( tempOffset );
  airSensorSCD30.setMeasurementInterval( measurementInterval );
  // Adafruit Feather 7 Segment config
  matrix7Seg.begin( 0x70 ); // initialize 7Segment Matrix
  matrix7Seg.clear();
  matrix7Seg.setBrightness( brightness );
  matrix7Seg.blinkRate( blinkRate );
  matrix7Seg.writeDisplay();
}

void loop() {
  if (airSensorSCD30.dataAvailable()) {
    temp = airSensorSCD30.getTemperature();
    humidity = airSensorSCD30.getHumidity();
    co2 = airSensorSCD30.getCO2();
  }
  serialOutput();
  trafficLights();
  matrix7SegOutput( matrixLag );
}

// Adafruit Feather 7 Segment Output
void matrix7SegOutput(int lag) {
  // matrix7Seg.print(0xC02, HEX);  // CO2
  // matrix7Seg.writeDisplay();
  // delay( lag/2 );
  matrix7Seg.print(co2);
  matrix7Seg.writeDisplay();
  delay( lag );
  // matrix7Seg.clear();
  // matrix7Seg.writeDigitRaw(1, 0x76); // H
  // matrix7Seg.writeDigitRaw(3, 0x3E); // U
  // matrix7Seg.writeDigitRaw(4, 0x37); // M
  // matrix7Seg.writeDisplay();
  // delay( lag/2 );
  matrix7Seg.print(temp);
  matrix7Seg.writeDisplay();
  delay( lag );
  // matrix7Seg.clear();
  // matrix7Seg.writeDigitRaw(0, 0x7);  // T
  // matrix7Seg.writeDigitRaw(1, 0x79); // E
  // matrix7Seg.writeDigitRaw(3, 0x37); // M
  // matrix7Seg.writeDigitRaw(4, 0x73); // P
  // matrix7Seg.writeDisplay();
  // delay( lag/2 );
  matrix7Seg.print(humidity);
  matrix7Seg.writeDisplay();
  delay( lag );
  // matrix7Seg.clear();
  // matrix7Seg.writeDigitNum(0, -1, true);
  // matrix7Seg.writeDisplay();
  // delay( lag );
}

// Traffic Lights Output (3 LEDs)
void trafficLights() {
  if ( co2 <= goodAir )  {
    digitalWrite( greenLED, HIGH );
    digitalWrite( yellowLED, LOW );
    digitalWrite( redLED, LOW );
  } else if ( co2 <= moderateAir ) {
    digitalWrite( greenLED, LOW );
    digitalWrite( yellowLED, HIGH );
    digitalWrite( redLED, LOW );
  } else if ( co2 <= unhealthyAir ) {
    digitalWrite( greenLED, LOW );
    digitalWrite( yellowLED, HIGH );
    digitalWrite( redLED, HIGH );
  } else if (co2 <= veryUnhealthyAir ) {
    digitalWrite( greenLED, LOW );
    digitalWrite( yellowLED, LOW );
    digitalWrite( redLED, HIGH );
  } else {
    digitalWrite( greenLED, HIGH );
    digitalWrite( yellowLED, HIGH );
    digitalWrite( redLED, HIGH );
  }
}

// Serial Monitor Output (e.g., cu -l /dev/ttyUSB0 -s 115200 | xargs --null -IL date +"%Y-%m-%d %H:%M:%S | L")
void serialOutput() {
  Serial.println();
  Serial.printf("%012u | ", millis());
  Serial.printf("Temperature: %6.2f°C | ", temp);
  Serial.printf("Humidity: %6.2f%% | ", humidity);
  if ( co2 <= goodAir )  {
    Serial.printf("CO2-Concentration: %5ippm \033[0;32m■   \033[0;39m(Good Air Quality)", co2);
  } else if ( co2 <= moderateAir ) {
    Serial.printf("CO2-Concentration: %5ippm  \033[1;33m■  \033[0;39m(Moderate Air Quality)", co2);
  } else if ( co2 <= unhealthyAir ) {
    Serial.printf("CO2-Concentration: %5ippm  \033[1;33m■\033[0;31m■ \033[0;39m(Poor Air Quality)", co2);
  } else if ( co2 <= veryUnhealthyAir ) {
    Serial.printf("CO2-Concentration: %5ippm   \033[0;31m■ \033[0;39m(Unhealthy Air Quality)", co2);
  } else {
    Serial.printf("CO2-Concentration: %5ippm \033[0;32m■\033[1;33m■\033[0;31m■ \033[0;39m(Very Unhealthy Air Quality)", co2);
  }
  Serial.flush();
}
