#include <Sleep_n0m1.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_ADXL345_U.h>
#include <SensorCalibration.h>
#include <SPI.h>
#include "SdFat.h"

//These two lines initialize and assign the sensors to specific IDs.      
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

//These arrays are used to calculate when the probe is moving and when it is not. 
float xaccelstorage[5] = {0, 0, 0, 0, 0};
float yaccelstorage[5] = {0, 0, 0, 0, 0};
float zaccelstorage[5] = {0, 0, 0, 0, 0};

//Array that stores altitude data for calculating apogee
float altitudestorage[3] = {0, 0, 0};

//This bool stores the state of motion for the probe.
bool inMotion = true; 

//if this is true, you better have your parachute deployed
bool atApogee = false;

//This bool stores whether the probe has launched or not.
bool hasLaunched = false;

// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = SS;

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "Test"

// File system object.
SdFat sd;

// Log file.
SdFile file;

// Time used to detect if the SD card is functioning correctly.
uint32_t logTime;

// Time used to log to the SD card
uint32_t time;

// Writes the top row to the csv file for the spreadsheet. 
void writeHeader() {
  file.print(F("Time (ms),"));
  file.print("Pressure (hPa),");
  file.print("Temperature (C),");
  file.print("Altitude (m),");
  file.print("X Acceleration (g),");
  file.print("Y Acceleration (g),");
  file.print("Z Acceleration (g),\n");
}

// This function is used to write sensor data to the csv file.
void logSensorData(float sensorData) {
  file.print(sensorData);
  file.print(",");
}

// This is used by the SD library when there is an error.
#define error(msg) sd.errorHalt(F(msg))

// Quick and dirty function to check if we've hit apogee yet, parachute triggering code should be added here or in an 
// if statement in void loop()
void isAtApogee(float exaltitude) {
  altitudestorage[2] = altitudestorage[1];
  altitudestorage[1] = altitudestorage[0];
  altitudestorage[0] = exaltitude;
  
    if(altitudestorage[2] > altitudestorage[0] || altitudestorage[1] > altitudestorage[0]) {
      atApogee == true;
    } else {
      atApogee == false;
    }
  
}

/* This function determines whether the probe is moving or not by taking
the mean of the previous 5 values for each axis on the accelerometer 
and then checking if 2 out of the 3 show acceleration greater than 0.1g. 
If that is the case, then the probe thinks it is moving and beings recording data. */
void isMoving(float exaccel, float eyaccel, float ezaccel) {
  
  xaccelstorage[4] = xaccelstorage[3];
  xaccelstorage[3] = xaccelstorage[2];
  xaccelstorage[2] = xaccelstorage[1];
  xaccelstorage[1] = xaccelstorage[0];
  xaccelstorage[0] = exaccel;
  
  yaccelstorage[4] = yaccelstorage[3];
  yaccelstorage[3] = yaccelstorage[2];
  yaccelstorage[2] = yaccelstorage[1];
  yaccelstorage[1] = yaccelstorage[0];
  yaccelstorage[0] = eyaccel;
  
  zaccelstorage[4] = zaccelstorage[3];
  zaccelstorage[3] = zaccelstorage[2];
  zaccelstorage[2] = zaccelstorage[1];
  zaccelstorage[1] = zaccelstorage[0];
  zaccelstorage[0] = ezaccel ;
  
  float xmean = (xaccelstorage[0] + xaccelstorage[1] + xaccelstorage[2] + xaccelstorage[3] + xaccelstorage[4]) / 5;
  float ymean = (yaccelstorage[0] + yaccelstorage[1] + yaccelstorage[2] + yaccelstorage[3] + yaccelstorage[4]) / 5;
  float zmean = (zaccelstorage[0] + zaccelstorage[1] + zaccelstorage[2] + zaccelstorage[3] + zaccelstorage[4]) / 5;
  Serial.print("Mean of the last 5 X acceleration values");
  Serial.println(xmean);
  
  Serial.print("Mean of the last 5 Y acceleration values");
  Serial.println(ymean);
  
  Serial.print("Mean of the last 5 Z acceleration values");
  Serial.println(zmean);
  bool isxaccelerating = false;
  bool isyaccelerating = false;
  bool iszaccelerating = false;
  if (xmean >= -0.1 && xmean <= 0.1)  {
      isxaccelerating = false;
  } else {
    isxaccelerating = true;
  } 
  if (ymean >= -0.1 && ymean <= 0.1) {
      isyaccelerating = false;
  } else {
    isyaccelerating = true;
  }
  if (zmean >= -0.1 && zmean <= 0.1) {
      iszaccelerating = false;
  } else {
    iszaccelerating = true;
  }
  if (isxaccelerating && isyaccelerating) {
    inMotion = true;
    hasLaunched = true;
  } else if (isyaccelerating && iszaccelerating) {
    inMotion = true;
    hasLaunched = true;
  } else if (iszaccelerating && isxaccelerating) {
    inMotion = true;
    hasLaunched = true;
  } else {
    inMotion = false;
  }
  
}


void setup(void) 
{

  time = 0;

  // Begin SD Card file creation and debugging. 
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";
  
  Serial.begin(9600);
 
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }
  delay(1000);
  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }

  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
    error("file.open");
  }
  // Read any Serial data.
  do {
    delay(10);
  } while (Serial.available() && Serial.read() >= 0);

  Serial.print(F("Logging to: "));
  Serial.println(fileName);
  Serial.println(F("Type any character to stop"));

  // End SD Card file creation and debugging.
  
  // Write the top row of data to the csv file. 
  writeHeader();

  // Establishing logTime.
  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;


  /* Initialise the barometer/thermometer sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on the sensors */
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("");
}

/**************************************************************************/
/*
    Arduino loop functi on, called once 'setup' is complete (your own code
    should go here)temperature                                                                
*/
/**************************************************************************/
void loop(void) 
{
  //Get the current acceleration values from the accelerometer and check if the probe is moving.
  sensors_event_t accelevent; 
  accel.getEvent(&accelevent);
  float xaccel = accelevent.acceleration.x + xoffset;
  float yaccel = accelevent.acceleration.y + yoffset;
  float zaccel = accelevent.acceleration.z + zoffset;
  isMoving(xaccel, yaccel, zaccel);
  //If the Probe has launched and is moving, begin recording data
  if(inMotion && hasLaunched)
  {
     //Get the current temperature and pressure from the barometer/thermometer
     sensors_event_t bmpevent;
     bmp.getEvent(&bmpevent);

     file.print((time));
     file.print(",");
     //Check if there is a detected air pressure.
     if (bmpevent.pressure)
     {
       // Display and record atmospheric pressure in hPa 
       Serial.print("Pressure:    ");
       Serial.print(bmpevent.pressure);
       logSensorData(bmpevent.pressure);
       Serial.println(" hPa");

      //Get and log the current temperature from the bmp180

      float temperature;
      bmp.getTemperature(&temperature);
      logSensorData(temperature);
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");

      /*Take the SLP(from SensorCalibration.h) and the current pressure to calculate the altitude.
      You must update CURRENTSEALEVELPRESSURE in SensorCalibration.h in order to get an accurate result. 
      SensorCalibration.h contains accurate instructions on how to get this data and will need to be moved
      from the root directory for this project and moved to $HOME(Your user directory)/Arduino/libraries/SensorCalibration.
      More detailed instructions on how to get every library located in the correct place for the project are found in README.md
      as well as the youtube tutorial also linked in README.md*/
       float seaLevelPressure = CURRENTSEALEVELPRESSURE;
       float altitude = bmp.pressureToAltitude(seaLevelPressure, bmpevent.pressure);
       Serial.print("Altitude:    "); 
       Serial.print(altitude); 
       logSensorData(altitude);
       Serial.println(" m");
       Serial.println("");
       isAtApogee(altitude);
     }
     else 
     {
       Serial.println("Sensor error");
     }
     if(atApogee) {
      // do yo parachute code
    }
     //Logging and recording all the data from the accelerometer to the csv file. 
     logSensorData(xaccel);
     logSensorData(yaccel);
     logSensorData(zaccel);
     Serial.print("X: "); Serial.print(xaccel); Serial.print("  ");
     Serial.print("Y: "); Serial.print(yaccel); Serial.print("  ");
     Serial.print("Z: "); Serial.print(zaccel); Serial.print("  ");Serial.println("m/s^2 ");

  } else { 
    /*In the event that isMoving() decides that the probe isn't moving, the time is still printed
    along with a small message that specifies that the probe isn't gonna collect data while it's
    not moving */
    file.print((time));
    file.print(",");
    file.print("Probe is motionless,");
    file.print("and is not ");
    file.print("collecting data");
    //Printing the time and error to the Serial bus. 
    Serial.print((time));
    Serial.print(",");
    Serial.print("Probe is motionless,");
    Serial.print("and is not ");
    Serial.print("collecting data");
  }
  

  // Time for next record.
  logTime += 1000UL*SAMPLE_INTERVAL_MS;
  // Wait for log time.
  int32_t diff;
  do {
    diff = micros() - logTime;
  } while (diff < 0);

  // Check for data rate too high.
  if (diff > 10) {
    error("Missed data record");
  }

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }
  
  time += SAMPLE_INTERVAL_MS;
  Serial.println(time);


  file.print("\n");
  if (inMotion == false && hasLaunched == true) {
    // Close file and stop.
    file.close();
    Serial.println(F("Done"));
    SysCall::halt();
  }
  
}




