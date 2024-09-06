/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
  Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output the i/j/k/real parts of the rotation vector.
  https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 115200 baud to serial monitor.
*/

#include <Wire.h>

#include "SparkFun_BNO08x_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO08x myIMU1; //Open I2C ADR jumper - goes to address 0x4B
BNO08x myIMU2; //Closed I2C ADR jumper - goes to address 0x4A

const byte interruptPin = 5;
const byte ledPin = 10;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t lastIsrAt = 0;

void ARDUINO_ISR_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  
  portENTER_CRITICAL_ISR(&timerMux);
  lastIsrAt = micros();
  portEXIT_CRITICAL_ISR(&timerMux);
  
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}


void setup()
{
  Serial.begin(921600);
  while (!Serial) delay(20);    // wait until serial monitor opens
  Serial.println();

  Wire.begin();
  Wire.setClock(400000); //Increase I2C data rate to 400kHz
 
  
  //When a large amount of time has passed since we last polled the sensors
  //they can freeze up. To un-freeze it is easiest to power cycle the sensor.
  pinMode(interruptPin, INPUT_PULLDOWN);
   pinMode(ledPin, OUTPUT);
  //Start 2 sensors
  if (myIMU1.begin(0x4A) == false)
  {
    Serial.println("First BNO080 not detected with I2C ADR jumper open. Check your jumpers and the hookup guide. Freezing...");
    //while(1);
  }


  setReports();

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Set timer frequency to 1Mhz
  timer = timerBegin(1000000);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(timer, 8000, true, 0);
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (myIMU1.enableRotationVector(5) == true) {
    Serial.println(F("Rotation vector enabled"));
    Serial.println(F("Output in form roll, pitch, yaw"));
  } else {
    Serial.println("Could not enable rotation vector");
  }/*
  if (myIMU1.enableGyro() == true) {
    Serial.println(F("Gyro enabled"));
    Serial.println(F("Output in form x, y, z, in radians per second"));
  } else {
    Serial.println("Could not enable gyro");
  }*/
}

void loop()
{
 

  if (myIMU1.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }
   digitalWrite(ledPin, 1);
  // Has a new event come in on the Sensor Hub Bus?

  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    if (myIMU1.getSensorEvent() == true && (digitalRead(interruptPin))) {
      digitalWrite(ledPin, 0);

      // is it the correct sensor data we want?
      if (myIMU1.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
      //if (myIMU1.getSensorEventID() == SENSOR_REPORTID_GYROSCOPE_CALIBRATED) {
  
    
      float roll = (myIMU1.getRoll()) * 180.0 / PI; // Convert roll to degrees
      float pitch = (myIMU1.getPitch()) * 180.0 / PI; // Convert pitch to degrees
      float yaw = (myIMU1.getYaw()) * 180.0 / PI; // Convert yaw / heading to degrees
      float quatI = myIMU1.getQuatI();
      float quatJ = myIMU1.getQuatJ();
      float quatK = myIMU1.getQuatK();
      float quatReal = myIMU1.getQuatReal();
      //Serial.print("A");
      //Serial.print(",");
      Serial.print(lastIsrAt);
      Serial.print(",");
      Serial.print(quatI, 2);
      Serial.print(F(","));
      Serial.print(quatJ, 2);
      Serial.print(F(","));
      Serial.print(quatK, 2);
      Serial.print(F(","));
      Serial.print(quatReal, 2);
      Serial.print(F(","));
      Serial.print(roll, 2);
      Serial.print(F(","));
      Serial.print(pitch, 2);
      Serial.print(F(","));
      Serial.print(yaw, 2);
      //Serial.print("\n");
      Serial.println();
      /*
        float x = myIMU1.getGyroX()*57.32;
      float y = myIMU1.getGyroY()*57.32;
      float z = myIMU1.getGyroZ()*57.32;

      Serial.print(x, 2);
      Serial.print(F(","));
      Serial.print(y, 2);
      Serial.print(F(","));
      Serial.print(z, 2);
      Serial.println();*/
      }
    }
  }
}
