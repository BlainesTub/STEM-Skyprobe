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
* Adafruit ADXL345
* Adafruit BMP085 Unified
* Sleep_n0m1
* SdFat

To install these libraries, go into your arduino library and click Sketch/Include Library/Manage Libraries. 

[add screenshot of arduino library window]

This will open up a window that will let you search and download all 