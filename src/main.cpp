/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */

#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <OneWire.h>
#include <DallasTemperature.h>


// Data wire is connected to ESP32 GPIO 21
#define ONE_WIRE_BUS 7
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// change this to match your SD shield or module;
const int chipSelect = 4; 

// Create a file to store the data
File myFile;
float temperature;

void setup() {
  //initializing Serial monitor
  Serial.begin(9600);
  
  // setup for the
  while(!Serial); // for Leonardo/Micro/Zero
  // setup for the SD card
  Serial.print("Initializing SD card...");

  if(!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
    
  //open file
  myFile=SD.open("data.txt", FILE_WRITE);

  // if the file opened ok, write to it:
  if (myFile) {
    Serial.println("File opened ok");
  }
  myFile.close();
}

void loggingTime() {
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(temperature);
    myFile.print('\n');
  }
  Serial.print(temperature);   
  Serial.print('\n');
  myFile.close();

  delay(1000);  
}

void loggingTemperature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  sensors.requestTemperatures(); 
  temperature = sensors.getTempCByIndex(0); // Temperature in Celsius

  // Check if any reads failed and exit early (to try again).
  if  (isnan(temperature) /*|| isnan(f)*/) {
    Serial.println("Failed to read from Dallas sensor!");
    return;
  }
}

void loop() {
  loggingTime();
  loggingTemperature();
  delay(6000000); 
}
