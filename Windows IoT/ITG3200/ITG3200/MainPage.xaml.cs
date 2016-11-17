// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Threading;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace ITG3200
{
	struct Gyroscope
	{
		public double X;
		public double Y;
		public double Z;
	};

	// App that reads data over I2C from a ITG3200, 3-Axis MEMS Gyro Angular Rate Sensor
	public sealed partial class MainPage : Page
	{
		private const byte GYRO_I2C_ADDR = 0x68;	// I2C address of the ITG3200
		private const byte GYRO_REG_DLPF = 0x16;	// DLPF, Full Scale register
		private const byte GYRO_REG_POWER = 0x3E;	// Power Management register
		private const byte GYRO_REG_X = 0x1D;		// X Axis High data register
		private const byte GYRO_REG_Y = 0x1F;		// Y Axis High data register
		private const byte GYRO_REG_Z = 0x21;		// Z Axis High data register

		private I2cDevice I2CGyro;
		private Timer periodicTimer;

		public MainPage()
		{
			this.InitializeComponent();

			// Register for the unloaded event so we can clean up upon exit
			Unloaded += MainPage_Unloaded;

			// Initialize the I2C bus, 3-Axis MEMS Gyro Angular Rate Sensor, and timer
			InitI2CGyro();
		}

		private async void InitI2CGyro()
		{
			string aqs = I2cDevice.GetDeviceSelector();		// Get a selector string that will return all I2C controllers on the system
			var dis = await DeviceInformation.FindAllAsync(aqs);	// Find the I2C bus controller device with our selector string
			if (dis.Count == 0)
			{
				Text_Status.Text = "No I2C controllers were found on the system";
				return;
			}

			var settings = new I2cConnectionSettings(GYRO_I2C_ADDR);
			settings.BusSpeed = I2cBusSpeed.FastMode;
			I2CGyro = await I2cDevice.FromIdAsync(dis[0].Id, settings);	// Create an I2C Device with our selected bus controller and I2C settings
			if (I2CGyro == null)
			{
				Text_Status.Text = string.Format(
					"Slave address {0} on I2C Controller {1} is currently in use by " +
					"another application. Please ensure that no other applications are using I2C.",
				settings.SlaveAddress,
				dis[0].Id);
				return;
			}

			/*
				Initialize the 3-Axis MEMS Gyro Angular Rate Sensor
				For this device, we create 2-byte write buffers
				The first byte is the register address we want to write to
				The second byte is the contents that we want to write to the register
			*/
			byte[] WriteBuf_Power = new byte[] { GYRO_REG_POWER, 0x01 };	// 0x01 Power ON's the sensor and clock source is set to PLL with X Gyro reference
			byte[] WriteBuf_Dlpf = new byte[] { GYRO_REG_DLPF, 0x18 };	// 0x18 sets gyro full-scale range to ±2000°/sec, low pass filter bandwidth to 256 Hz and internal sample rate to 8 Hz

			// Write the register settings
			try
			{
				I2CGyro.Write(WriteBuf_Power);
				I2CGyro.Write(WriteBuf_Dlpf);
			}
			// If the write fails display the error and stop running
			catch (Exception ex)
			{
				Text_Status.Text = "Failed to communicate with device: " + ex.Message;
				return;
			}

			// Create a timer to read data every 500ms
			periodicTimer = new Timer(this.TimerCallback, null, 0, 500);
		}

		private void MainPage_Unloaded(object sender, object args)
		{
			// Cleanup
			I2CGyro.Dispose();
		}

		private void TimerCallback(object state)
		{
			string xText, yText, zText;
			string addressText, statusText;

			// Read and format 3-Axis MEMS Gyro Angular Rate Sensor data
			try
			{
				Gyroscope GYRO = ReadI2CGyro();
				addressText = "I2C Address of the 3-Axis MEMS Gyro Angular Rate Sensor ITG3200: 0x68";
				xText = String.Format("X Axis: {0:F0}", GYRO.X);
				yText = String.Format("Y Axis: {0:F0}", GYRO.Y);
				zText = String.Format("Z Axis: {0:F0}", GYRO.Z);
				statusText = "Status: Running";
			}
			catch (Exception ex)
			{
				xText = "X Axis: Error";
				yText = "Y Axis: Error";
				zText = "Z Axis: Error";
				statusText = "Failed to read from 3-Axis MEMS Gyro Angular Rate Sensor: " + ex.Message;
			}

			// UI updates must be invoked on the UI thread
			var task = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
			{
				Text_X_Axis.Text = xText;
				Text_Y_Axis.Text = yText;
				Text_Z_Axis.Text = zText;
				Text_Status.Text = statusText;
			});
		}

		private Gyroscope ReadI2CGyro()
		{
			byte[] RegAddrBuf = new byte[] { GYRO_REG_X };	// Read data from the register address
			byte[] ReadBuf = new byte[6];			// We read 6 bytes sequentially to get X-Axis and all 3 two-byte axes registers in one read

			/*
				Read from the 3-Axis MEMS Gyro Angular Rate Sensor 
				We call WriteRead() so we first write the address of the X-Axis I2C register, then read all 3 axes
			*/
			I2CGyro.WriteRead(RegAddrBuf, ReadBuf);
			
			/*
				In order to get the raw 14-bit data values, we need to concatenate two 8-bit bytes from the I2C read for each axis
			*/
			int GYRORawX = (int)((ReadBuf[0] & 0xFF) * 256);
			GYRORawX |= (int)(ReadBuf[1] & 0xFF);
			if (GYRORawX > 32767 )
			{
				GYRORawX -= 65536;
			}
			
			int GYRORawY = (int)((ReadBuf[2] & 0xFF) * 256);
			GYRORawY |= (int)(ReadBuf[3] & 0xFF);
			if (GYRORawY > 32767 )
			{
				GYRORawY -= 65536;
			}
			
			int GYRORawZ = (int)((ReadBuf[4] & 0xFF) * 256);
			GYRORawZ |= (int)(ReadBuf[5] & 0xFF);
			if (GYRORawZ > 32767 )
			{
				GYRORawZ -= 65536;
			}

			Gyroscope Gyro;
			Gyro.X = GYRORawX;
			Gyro.Y = GYRORawY;
			Gyro.Z = GYRORawZ;

			return Gyro;
		}
	}
}
