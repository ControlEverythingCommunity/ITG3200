// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ITG-3200
// This code is designed to work with the ITG-3200_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Gyro?sku=ITG-3200_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// ITG-3200 I2C address is 0x68(104)
#define Addr 0x68

int xGyro = 0, yGyro = 0, zGyro = 0;
void setup() 
{
  // Set variable
  Particle.variable("i2cdevice", "ITG-3200");
  Particle.variable("xGyro", xGyro);
  Particle.variable("yGyro", yGyro);
  Particle.variable("zGyro", zGyro);
  
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select Power Management register
  Wire.write(0x3E);
  // Set power-up enable
  Wire.write(0x01);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select range register
  Wire.write(0x16);
  // Set full scale, 2000 dps
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
  
  // Request 6 bytes of data
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
  xGyro = data[0] * 256.0 + data[1];
  if(xGyro > 32767)
  {
    xGyro -= 65536;
  }
  
  yGyro = data[2] * 256.0 + data[3];
  if(yGyro > 32767)
  {
    yGyro -= 65536;
  }
  
  zGyro = data[4] * 256.0 + data[5];
  if(zGyro > 32767)
  {
    zGyro -= 65536;
  } 
   
  // Output data to dashboard
  Particle.publish("X-Axis of Rotation : ", String(xGyro));
  Particle.publish("Y-Axis of Rotation : ", String(yGyro));
  Particle.publish("Z-Axis of Rotation : ", String(zGyro));
  delay(500); 
}

