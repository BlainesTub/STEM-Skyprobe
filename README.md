# STEM-Skyprobe
The code for our senior STEM Skyprobe Project. 

This repository contains all of the relevant files needed to install and run the project, and includes:

* Separate Arduino Sketches for both launching and testing the probe
* 2 complete Fritzing sketches with all the necessary wiring laid out in the correct colors for both testing on a breadboard and launch. 
* A custom Sensor Calibration library that makes calibration of the sensors easy with all the values located in one file

There are youtube tutorials that are on the way, as well as a full text guide on this document to assist with installation. 

# Installation Instructions
## Downloading the Repository
To download the repository, either click on the "Clone or Download" button and click "Download ZIP" to download a zip of the repository or type into a terminal with git installed: 

`git clone https://github.com/LukasVannStruth/STEM-Skyprobe.git`

## Downloading Arduino Libraries
There are 5 individual Arduino libraries that need to be installed before the script can be compiled to an arduino. Additionally, the SensorCalibration library which contains the values that must be calibrated for accurate results from the sensors must be moved to your Arduino libraries folder. 

The libraries that need to be installed are:

* Adafruit Unified Sensor
* Sparkfun ADXL345
* Adafruit BMP085 Unified
* Sleep_n0m1
* SdFat

You can install all of these libraries with the exception of the Sparkfun ADXL345 library by going into your arduino IDE and clicking Sketch/Include Library/Manage Libraries. 

This will open up a window that will let you search and download all the needed libraries. 

To download the Sparkfun ADXL345 Library, go to this link: https://cdn.sparkfun.com/assets/learn_tutorials/5/4/8/SparkFun_ADXL345_Arduino_Library.zip

This will automatically download a zip file that contains the library, as well as few example sketches that'll let you test your wiring. 
In order for the arduino sketch to correctly compile, you must move this library to the location of your Arduino libraries, which depends on your operating system:


Windows:

`(Your user directory)/Arduino/libraries`

Linux: 

`/home/users/(Your username)/Arduino/libraries`

## Installing the Sensor Calibration Library

In order for the sketch to upload correctly, the SensorCalibration library's containing must be moved from it's location in STEM-Skyprobe/SensorCalibration to the same location that you moved the Sparkfun library in the previous step.


## Wiring
Information on how to wire the skyprobe for both testing and launch are available in the two fritzing sketches included in the repository. 

## Testing Procedure
In order to test the probe, first complete all the previous steps. Then, connect the arduino to your computer and from the arduino IDE and run the test sketch. You should get a successful upload to the arduino, if not check that you did all the previous steps correctly as well as the common issues section and try again. 

## Common Issues
The most common problem that I've encountered has been the accelerometer randomly failing, and stopping collecting data. The solution to this problem for me has been running the accelerometer test sketch, which you can find in the Arduino IDE in File/Examples/Adafruit ADXL345/sensortest. 

Uploading and running the sketch should output actual values from the accelerometer and fix it from both the test/launch sketch and the accelerometer sketch. 
