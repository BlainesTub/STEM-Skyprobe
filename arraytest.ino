float xaccel[3] = {0, 0, 0};

float xvalues[10] = {1.2, 3.4, 6.5, 5.4, 3.4, 6.4, 0.04, -0,04, 0};
int counter = 8;
 
bool inMotion = true;
void isStopped() {
    float xmean = (xaccel[0] + xaccel[1] + xaccel[2]) / 3;
    nSerial.println(xmean);
    if (xmean <= 0.1 && xmean >= -0.1) {
        inMotion = false; 
    }
}
void setup() {
    Serial.begin(9600);
}
void loop() {
    while(inMotion)
    {
        xaccel[2] = xaccel[1];
        Serial.println(xaccel[3]);
        xaccel[1] = xaccel[0];
        Serial.println(xaccel[2]);
        xaccel[0] = xvalues[counter];
        Serial.println(xaccel[0]);
        isStopped();
        counter -= 1;
        delay(500);
    }
    
    
}