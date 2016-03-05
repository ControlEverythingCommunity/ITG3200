// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ITG-3200
// This code is designed to work with the ITG-3200_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Gyro?sku=ITG-3200_I2CS#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class ITG3200
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus Bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, ITG3200 I2C address is 0x68(104)
		I2CDevice device = Bus.getDevice(0x68);
		
		// Power Up, Set xGyro Reference
		device.write(0x3E, (byte)0x01);
		// Set Full scale range of +/- 2000 deg/sec
		device.write(0x16, (byte)0x18);
		Thread.sleep(300);
		
		// Read 6 bytes of data from address 0x1D(29)
		// X msb, X lsb, Y msb, Y lsb, Z msb, Z lsb
		byte[] data = new byte[6];
		device.read(0x1D, data, 0, 6);
		
		// Convert the values
		int xGyro = ((data[0] & 0xFF) * 256) + (data[1] & 0xFF);
		if (xGyro > 32767)
		{
			xGyro -= 65536;
		}
		
		int yGyro = ((data[2] & 0xFF) * 256) + (data[3] & 0xFF);
		if (yGyro > 32767)
		{
			yGyro -= 65536;
		}
		
		int zGyro = ((data[4] & 0xFF) * 256) + (data[5] & 0xFF);
		if (zGyro > 32767)
		{
			zGyro -= 65536;
		}
		
		// Output data to screen
		System.out.printf("X-Axis of Rotation : %d %n", xGyro);
		System.out.printf("Y-Axis of Rotation : %d %n", yGyro);
		System.out.printf("Z-Axis of Rotation : %d %n", zGyro);
	}
}
