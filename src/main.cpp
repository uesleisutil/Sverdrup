#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <OneWire.h>
#include <DallasTemperature.h>
#include <./Narcoleptic.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

// Data wire is connected to ESP32 GPIO 21
#define ONE_WIRE_BUS 8
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// change this to match your SD shield or module;
const int chipSelect = 10; 

// BME280 
Adafruit_BME280 bme;

//LCD.
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
 
// Create a file to store the data
File myFile;

// Strings and floats. 
float temp_ds18b20;
float temp_bme280;
float rh_bme280;
float pres_bme280; 
String dataMessage;

// Sleep time
#define SLEEP_TIME 30000

void setup() {
  //initializing Serial monitor
  Serial.begin(9600);

  // Initializing BME280 sensor.
  bme.begin(0x76);

  //LCD.
  lcd.begin (16,2);

  // SD begin.
  if(!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
    

  // setup for the dSD card
  myFile=SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("File opened ok");
    // print the headings for our data
    myFile.println("DS18B20_temp (ºC),BME_temp (ºC),BME_rh (ºC),BME_pres (hPa)");
  }
  myFile.close();
}

void printingdata() {
  Serial.print("DS18B20: ");
  Serial.print(temp_ds18b20);
  Serial.print(" BME_temp: ");
  Serial.print(temp_bme280);
  Serial.print(" BME_rh: ");
  Serial.print(rh_bme280);
  Serial.print(" BME_pres: ");
  Serial.print(pres_bme280);
  Serial.println();  

}

void loggingDS18B20() {
  sensors.requestTemperatures(); 
  temp_ds18b20 = sensors.getTempCByIndex(0); // Temperature in Celsius

  // Check if any reads failed and exit early (to try again).
  if  (isnan(temp_ds18b20) /*|| isnan(f)*/) {
    Serial.println("Failed to read from Dallas sensor!");
    return;
  }
}

void loggingBME280() {
  temp_bme280 = bme.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if  (isnan(temp_bme280) /*|| isnan(f)*/) {
    Serial.println("Failed to read temperature from BME280 sensor!");
    return;
  }
  rh_bme280   = bme.readHumidity();
  if  (isnan(rh_bme280) /*|| isnan(f)*/) {
    Serial.println("Failed to read humidity from BME280 sensor!");
    return;
  }
  pres_bme280 = bme.readPressure() / 100.0F;
  if  (isnan(pres_bme280) /*|| isnan(f)*/) {
    Serial.println("Failed to read pressure from BME280 sensor!");
    return;
  }
}

void savingdata() {
  myFile=SD.open("data.txt", FILE_WRITE);
  dataMessage = String(temp_ds18b20) + "," + String(temp_bme280) + 
                  "," + String(rh_bme280) + "," + String(pres_bme280) + "\r\n";
  myFile.print(dataMessage);
  myFile.close();

}

void loop() {
  delay(2000);
  loggingDS18B20();
  delay(2000);
  loggingBME280();
  delay(2000);
  savingdata();
  delay(2000);
  printingdata();
  delay(2000);

  lcd.setCursor(0, 0);
  lcd.print(temp_ds18b20); 
  lcd.setCursor(5, 0);   
  lcd.print(char(223));
  lcd.setCursor(6, 0);   
  lcd.print("C");
  lcd.setCursor(10, 0);   
  lcd.print(rh_bme280); 
  lcd.setCursor(15, 0); 
  lcd.print("%");
  lcd.setCursor(4, 1);  
  lcd.print(pres_bme280); 
  lcd.setCursor(9, 1);  
  lcd.print("hPa");

  Narcoleptic.delay(SLEEP_TIME); // During this time power consumption is minimised
}
