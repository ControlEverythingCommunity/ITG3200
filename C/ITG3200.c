// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ITG-3200
// This code is designed to work with the ITG-3200_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Gyro?sku=ITG-3200_I2CS#tabs-0-product_tabset-2v

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, ITG3200 I2C address is 0x68(104)
	ioctl(file, I2C_SLAVE, 0x68);

	// Power Up, Set xGyro Reference(0x01)
	char config[2] = {0};
	config[0] = 0x3E;
	config[1] = 0x01;
	write(file, config, 2);

	// Set Full scale range of +/- 2000 deg/sec(0x18)
	config[0] = 0x16;
	config[1] = 0x18;
	write(file, config, 2);
	sleep(1);

	// Read 6 bytes of data from register(0x1D)
	// X msb, X lsb, Y msb, Y lsb, Z msb, Z lsb
	char reg[1] = {0x1D};
	write(file, reg, 1);
	char data[6] = {0};
	if(read(file, data, 6) != 6)
	{
		printf("Error : Input/output Error \n");
	}
	else
	{
		// Convert the values
		int xGyro = (data[0] * 256 + data[1]);
		if (xGyro > 32767)
		{
			xGyro -= 65536;
		}
		
		int yGyro = (data[2] * 256 + data[3]);
		if (yGyro > 32767)
		{
			yGyro -= 65536;
		}
		
		int zGyro = (data[4] * 256 + data[5]);
		if (zGyro > 32767)
		{
			zGyro -= 65536;
		}

		// Output data to screen
		printf("X-Axis of Rotation : %d \n", xGyro);
		printf("Y-Axis of Rotation : %d \n", yGyro);
		printf("Z-Axis of Rotation : %d \n", zGyro);
	}
}
