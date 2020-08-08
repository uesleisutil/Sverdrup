#include "RTClib.h"
#include <SD.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <./Narcoleptic.h>

// SD card resources.
int CS_SD = 10;

// File resources.
File file;
String dataMessage;

// RTC resources.
RTC_DS1307 rtc;
char gettime[20];

// BME280 resources.
float temp_bme280;
float rh_bme280;
float pres_bme280; 
Adafruit_BME280 bme;

// DS18b20 resources.
float temp_ds18b20;
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Sleep time (milliseconds)
#define SLEEP_TIME 30000

// -- Do not change above --

void setup() {
  Serial.begin(57600);
  
  //Start SD card.
  if (!SD.begin(CS_SD))
  {
    Serial.println("SD card failed!");
    return;
  }
  Serial.println("SD card ok.");
  Serial.println();

  //Check RTC.
  if (! rtc.begin())
  {
    Serial.println("RTC not found.!");
    while (1);
  }
  if (! rtc.isrunning())
  {
    Serial.println("RTC not running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //Format: DateTime(year, month, day, hour, minute, second)
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Init sensor.
  bme.begin(0x76);  
}

void loggingDS18B20() {
  sensors.requestTemperatures(); 
  temp_ds18b20 = sensors.getTempCByIndex(0); // Temperature in Celsius
}

void loggingBME280() {
  temp_bme280 = bme.readTemperature();
  rh_bme280   = bme.readHumidity();
  pres_bme280 = bme.readPressure() / 100.0F;
}

void savingdata() {
  file=SD.open("data.txt", FILE_WRITE);
  DateTime now = rtc.now();
  sprintf(gettime, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  dataMessage = String(gettime) +"," + String(temp_ds18b20) + "," + String(temp_bme280) + 
                  "," + String(rh_bme280) + "," + String(pres_bme280) + "\r\n";
  file.print(dataMessage);
  file.close();
}

void printingdata() {
  Serial.println(gettime);
  Serial.print(" DS18B20: ");
  Serial.print(temp_ds18b20);
  Serial.print(" BME_temp: ");
  Serial.print(temp_bme280);
  Serial.print(" BME_rh: ");
  Serial.print(rh_bme280);
  Serial.print(" BME_pres: ");
  Serial.print(pres_bme280);
  Serial.println();  
}

void loop() {
  loggingDS18B20();
  loggingBME280();
  savingdata();
  printingdata();
  delay(500);
  Narcoleptic.delay(SLEEP_TIME);
}
