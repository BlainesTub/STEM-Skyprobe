#include <Sleep_n0m1.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_ADXL345_U.h>
#include <SensorCalibration.h>
#include <SPI.h>
#include "SdFat.h"

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground
    
   History
   =======
   2013/JUN/17  - Updated altitude calculations (KTOWN)
   2013/FEB/13  - First version (KTOWN)
*/
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

float xaccelstorage[5] = {0, 0, 0, 0, 0};
float yaccelstorage[5] = {0, 0, 0, 0, 0};
float zaccelstorage[5] = {0, 0, 0, 0, 0};
bool inMotion = true; 
bool hasLaunched = false;
// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = SS;

// Interval between data records in milliseconds.
// The interval must be greater than the maximum SD write latency plus the
// time to acquire and write data to the SD to avoid overrun errors.
// Run the bench example to check the quality of your SD card.
//TODO add this variable to the sensor configuration library
const uint32_t SAMPLE_INTERVAL_MS = 1000;

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "Test"
//------------------------------------------------------------------------------
// File system object.
SdFat sd;

// Log file.
SdFile file;

// Time in micros for next data record.
uint32_t logTime;

uint32_t time;
//==============================================================================
// User functions.  Edit writeHeader() and logData() for your requirements.

//------------------------------------------------------------------------------
// Write data header.
void writeHeader() {
  file.print(F("Time,"));
  file.print("Pressure,");
  file.print("Temperature,");
  file.print("Altitude,");
  file.print("X Acceleration,");
  file.print("Y Acceleration,");
  file.print("Z Acceleration,\n");
  }

void logSensorData(float sensorData) {
  file.print(sensorData);
  file.print(",");
}
#define error(msg) sd.errorHalt(F(msg))

void isMoving(float exaccelstorage, float eyaccelstorage, float ezaccelstorage) {
  xaccelstorage[4] = xaccelstorage[3];
  xaccelstorage[3] = xaccelstorage[2];
  xaccelstorage[2] = xaccelstorage[1];
  xaccelstorage[1] = xaccelstorage[0];
  xaccelstorage[0] = exaccelstorage;
  yaccelstorage[4] = yaccelstorage[3];
  yaccelstorage[3] = yaccelstorage[2];
  yaccelstorage[2] = yaccelstorage[1];
  yaccelstorage[1] = yaccelstorage[0];
  yaccelstorage[0] = eyaccelstorage;
  zaccelstorage[4] = zaccelstorage[3];
  zaccelstorage[3] = zaccelstorage[2];
  zaccelstorage[2] = zaccelstorage[1];
  zaccelstorage[1] = zaccelstorage[0];
  zaccelstorage[0] = ezaccelstorage;
  float xmean = (xaccelstorage[0] + xaccelstorage[1] + xaccelstorage[2] + xaccelstorage[3] + xaccelstorage[4]) / 5;
  float ymean = (yaccelstorage[0] + yaccelstorage[1] + yaccelstorage[2] + yaccelstorage[3] + yaccelstorage[4]) / 5;
  float zmean = (zaccelstorage[0] + zaccelstorage[1] + zaccelstorage[2] + zaccelstorage[3] + zaccelstorage[4]) / 5;
  Serial.println(xmean);
  Serial.println(ymean);
  Serial.println(zmean);
  bool isxaccelstorageerating = false;
  bool isyaccelstorageerating = false;
  bool iszaccelstorageerating = false;
  if (xmean >= -0.1 && xmean <= 0.1)  {
      isxaccelstorageerating = false;
  } else {
    isxaccelstorageerating = true;
  } 
  if (ymean >= -0.1 && ymean <= 0.1) {
      isyaccelstorageerating = false;
  } else {
    isyaccelstorageerating = true;
  }
  if (zmean >= -0.1 && zmean <= 0.1) {
      iszaccelstorageerating = false;
  } else {
    iszaccelstorageerating = true;
  }
  if (isxaccelstorageerating && isyaccelstorageerating) {
    inMotion = true;
    hasLaunched = true;
  } else if (isyaccelstorageerating && iszaccelstorageerating) {
    inMotion = true;
    hasLaunched = true;
  } else if (iszaccelstorageerating && isxaccelstorageerating) {
    inMotion = true;
    hasLaunched = true;
  } else {
    inMotion = false;
  }
  
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t bmpsensor;
  bmp.getSensor(&bmpsensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(bmpsensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(bmpsensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(bmpsensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(bmpsensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(bmpsensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(bmpsensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  sensor_t accelsensor;
  accel.getSensor(&accelsensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(accelsensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(accelsensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(accelsensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(accelsensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(accelsensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(accelsensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);

}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}
void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}
/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) 
{

  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";
  time = 0;
  Serial.begin(9600);
/* Begin SD card test code 
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }
  delay(1000);
*/
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

  // Write data header.
  writeHeader();

  // Start on a multiple of the sample interval.
  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;
/*  end sd code */
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  accel.setRange(ADXL345_RANGE_16_G);
  displayDataRate();
  displayRange();
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
  sensors_event_t accelevent; 
  accel.getEvent(&accelevent);
  float xaccel = accelevent.acceleration.x + xoffset;
  float yaccel = accelevent.acceleration.y + yoffset;
  float zaccel = accelevent.acceleration.z + zoffset;
  isMoving(xaccel, yaccel, zaccel);
  if(inMotion && hasLaunched)
  {
     /* Get a new sensor event */ 
     sensors_event_t bmpevent;
     bmp.getEvent(&bmpevent);

     file.print((time));
     file.print(",");
     /* Display the results (barometric pressure is measure in hPa) */
     if (bmpevent.pressure)
     {
       /* Display atmospheric pressue in hPa */
       Serial.print("Pressure:    ");
       Serial.print(bmpevent.pressure);
       logSensorData(bmpevent.pressure);
       Serial.println(" hPa");

       /* Calculating altitude with reasonable accuracy requires pressure    *
       * sea level pressure for your position at the moment the data is     *
       * converted, as well as the ambient temperature in degress           *
       * celcius.  If you don't have these values, a 'generic' value of     *
       * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
       * in sensors.h), but this isn't ideal and will give variable         *
       * results from one day to the next.                                  *
       *                                                                    *
       * You can usually find the current SLP value by looking at weather   *
       * websites or from environmental information centers near any major  *
       * airport.                                                           *
       *                                                                    *
       * For example, for Paris, France you can check the current mean      *
       * pressure and sea level at: http://bit.ly/16Au8ol                   */
        
       /* First we get the current temperature from the BMP085 */
       float temperature;
       bmp.getTemperature(&temperature);
       logSensorData(temperature);
       Serial.print("Temperature: ");
       Serial.print(temperature);
       Serial.println(" C");

       /* Then convert the atmospheric pressure, and SLP to altitude         */
       /* Update this next line with the current SLP for better results      */
       float seaLevelPressure = CURRENTSEALEVELPRESSURE;
       float altitude = bmp.pressureToAltitude(seaLevelPressure, bmpevent.pressure);
       Serial.print("Altitude:    "); 
       Serial.print(altitude); 
       logSensorData(altitude);
       Serial.println(" m");
       Serial.println("");
     }
     else 
     {
       Serial.println("Sensor error");
     }

     logSensorData(xaccel);
     logSensorData(yaccel);
     logSensorData(zaccel);
     Serial.print("X: "); Serial.print(xaccel); Serial.print("  ");
     Serial.print("Y: "); Serial.print(yaccel); Serial.print("  ");
     Serial.print("Z: "); Serial.print(zaccel); Serial.print("  ");Serial.println("m/s^2 ");

  } else { 
    file.print((time));
    file.print(",");
    file.print("Probe is not,");
    file.print("collecting data");
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

  time += 1000;
  Serial.println(time);
  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }
file.print("\n");
  if (inMotion == false && hasLaunched == true) {
    // Close file and stop.
    file.close();
    Serial.println(F("Done"));
    SysCall::halt();
  }
  
}




