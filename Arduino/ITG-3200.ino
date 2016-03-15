// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ITG-3200
// This code is designed to work with the ITG-3200_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Gyro?sku=ITG-3200_I2CS#tabs-0-product_tabset-2

#include <Wire.h>

// ITG-3200 I2C address is 0x68(104)
#define Addr 0x68

void setup() 
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select Power Management register
  Wire.write(0x3E);
  // Power-up enabled
  Wire.write(0x01);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select range register
  Wire.write(0x16);
  // Full scale, 2000 dps
  Wire.write(0x18);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(300);
}

void loop() 
{
  unsigned data[6];
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x1D);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(500);
  
  // Request 6 byte of data
  Wire.requestFrom(Addr, 6);
  
  // Read 6 bytes of data 
  // xGyro msb, xGyro lsb, yGyro msb, yGyro lsb, zGyro msb, zGyro lsb 
  if(Wire.available() == 6) 
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  } 
   
  // Convert the data
  int xGyro = data[0] * 256 + data[1];
  int yGyro = data[2] * 256 + data[3];
  int zGyro = data[4] * 256 + data[5];
    
  // Output data to serial monitor
  Serial.print("X-Axis of Rotation :  ");
  Serial.println(xGyro);
  Serial.print("Y-Axis of Rotation :  ");
  Serial.println(yGyro);
  Serial.print("Z-Axis of Rotation :  ");
  Serial.println(zGyro);
  delay(500);
}

