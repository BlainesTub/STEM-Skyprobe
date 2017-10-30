/* Find the the current SLP for Vancouver at https://weather.gc.ca/past_conditions/index_e.html?station=yvr
The value on the site is in kPa, while the value that we want is in hPa, so move the decimal point 1 digit to 
the right. */
float CURRENTSEALEVELPRESSURE = 1017;

/* These values are used to offset the erroneous values the accelerometer sometimes outputs,
and should be set during the calibration component of testing. */
float xoffset = 0;
float yoffset = 0;
float zoffset = 0;

/* This determines the time between SD card writes, and is also used as the measurement of time on the csv file. 
Setting this below 1000 is risky, and should only be done after extensive benchmarking and testing of the sd card. */
const uint32_t SAMPLE_INTERVAL_MS = 1000;