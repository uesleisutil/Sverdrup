#include <SD.h>
#include "SPI.h"
#include "DHT.h"
#include <RTClibExtended.h>
#include <Wire.h>
#include <LowPower.h>
#include <stdlib.h>
#include <avr/interrupt.h> 
#include <avr/sleep.h> 
#include <avr/power.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DHT22 sensor.
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// DS18B20 module.
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 

// RTC module.
#define wakePin 2
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const int chipSelect = 10;
int interval_sec = 0; //Alarm interval in seconds
int interval_min = 1; //Alarm interval in minutes
int interval_hr = 00; //Alarm interval in hours
char file_name[25] = "Readings.csv";

// Led.
const int led = 8;

void setup () {
  Serial.begin(9600);
  delay(3000);

  // Led. 
  pinMode(led, OUTPUT);

  // RTC.
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    digitalWrite(led, HIGH);
    delay(5000);
    digitalWrite(led, LOW);   
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(wakePin, INPUT_PULLUP);
  rtc.armAlarm(1, false);
  rtc.clearAlarm(1);
  rtc.alarmInterrupt(1, false);
  rtc.armAlarm(2, false);
  rtc.clearAlarm(2);
  rtc.alarmInterrupt(2, false);
  rtc.writeSqwPinMode(DS3231_OFF);

  //Initialize DS18B20, DHT22 sensors.
  sensors.begin();
  dht.begin();

  // SD Card.
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    digitalWrite(led, HIGH);
    delay(5000);
    digitalWrite(led, LOW);   
    while (1);
  }
  SD.remove(file_name);
  File dataFile = SD.open(file_name, FILE_WRITE);
  dataFile.println("Date,Hour,Minute,DHT22_T,DHT22_H,DS18B20_T"); 
  dataFile.close();
  delay(100);
}

void wakeUp() {
 
}

void loop () {
  // Print time.
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Get data from DHT22, check it and print.
  delay(5000);
  float dht22_h = dht.readHumidity();
  float dht22_t = dht.readTemperature();
    if (isnan(dht22_h) || isnan(dht22_t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      digitalWrite(led, HIGH);
      delay(5000);
      digitalWrite(led, LOW);   
      return;
    }
  Serial.print(("DHT22 H: "));
  Serial.print(dht22_h);
  Serial.print((" %\t"));
  Serial.print(("DHT22 T: "));
  Serial.print(dht22_t);
  Serial.print((" *C\t"));
  delay(3000);

  // Get data from DS18B20, check it and print.
  sensors.requestTemperatures();
  float ds18b20_t =sensors.getTempCByIndex(0);
  if (isnan(ds18b20_t)) {
    Serial.println(F("Failed to read from DS18B20 sensor!"));
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(led, HIGH);
    delay(2000);
    return;
  }    
  Serial.print("DS18B20 T: ");
  Serial.print(ds18b20_t);  
  Serial.print((" *C\t"));
  delay(3000);

  // SD card.
  File dataFile = SD.open(file_name, FILE_WRITE);
  String dataString = now.day() + String("/") + now.month() + String("/") + now.year() + String(",") + now.hour() + String(",") + now.minute() + String(",") + dht22_t + String(",") + dht22_h + String(",") + ds18b20_t;
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println(F("error opening file to log data"));
    digitalWrite(led, HIGH);
    delay(5000);
    digitalWrite(led, LOW);   
    return;
  }

  // Sleep from RTC
  DateTime nextAlarm = now + TimeSpan(0, interval_hr, interval_min, interval_sec);
  Serial.print("Alarm at: ");
  Serial.print(nextAlarm.hour());
  Serial.print(":");    
  Serial.print(nextAlarm.minute());
  Serial.print(" ");     
  Serial.println(nextAlarm.second());

  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500); 
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500); 
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);  
  digitalWrite(led, HIGH);
  delay(500);
  rtc.setAlarm(ALM1_MATCH_HOURS, nextAlarm.second(), nextAlarm.minute(), nextAlarm.hour (), 1);
  rtc.alarmInterrupt(1, true);
  delay(3000);
  attachInterrupt(0, wakeUp, LOW);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0);
  rtc.armAlarm(1, false);
  rtc.clearAlarm(1);
  rtc.alarmInterrupt(1, false);
  Serial.println();
  delay(3000);
}