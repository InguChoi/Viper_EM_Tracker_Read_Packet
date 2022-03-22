// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       TeensyRS422Test.ino
	Created:	2020-07-01 오후 5:36:13
	Author:     DESKTOP-CSULS70\user
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

#include "RS422.h"
#include <IntervalTimer.h>

#define CONTROL_PERIOD 10000	// 10ms

RS422 viper;
STANDARD_FRAME standardFrame;

IntervalTimer mainControlTimer;
IntervalTimer AccSensorTimer;

IntervalTimer streamTimer;

//unsigned long time = 0;
//unsigned long timeCheck = 0;

extern unsigned long time;
extern unsigned long timeCheck;


// ACC Sensor
const int xPin = 14;
const int yPin = 15;
const int zPin = 16;

const int LaserPin = 19;


int xRead = 0;
int yRead = 0;
int zRead = 0;

int LaserRead = 0;

// The setup() function runs once each time the micro-controller starts
void setup()
{

	viper.init();

	Serial.begin(921600);


	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
//		Serial.println("Serial Connecting...");
	}

	Serial.println("Serial Connect!");

	pinMode(DE, OUTPUT);
	digitalWrite(DE, HIGH);

	pinMode(RE, OUTPUT);
	digitalWrite(RE, LOW);

	AccSensorTimer.begin(readAccSensorLoop, CONTROL_PERIOD);
}

void serialEvent()
{
	char dataIn = 0;

	if (Serial.available())
	{
		Serial.readBytes(&dataIn, 1);

		// Run P&O
		if (dataIn == 'R')
		{
			Serial.println("Run");

			viper.startPNO();

			mainControlTimer.begin(mainControlLoop, CONTROL_PERIOD);
		}

		// Init P&O
		else if (dataIn == 'I')
		{
			Serial.println("Init");

			viper.initContinuousPNO();

		}

		// Dual Output
		else if (dataIn == 'D')
		{
			Serial.println("Dual Output");

			viper.runDualoutput();

		}

		// Dual Output
		else if (dataIn == 'B')
		{
			Serial.println("Baud Rate");

			viper.changeViperBaudrate();
			viper.changeBaudrate();
		}

		// Stop P&O
		else if (dataIn == 'K')
		{
			Serial.println("Kill");

			viper.stopPNO();

			mainControlTimer.end();
		}

	}

}


// Viper Packet
void serialEvent3()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////

	time = micros();

	viper.readPacket();

//	timeCheck = micros() - time;

	/////////////////////////////////////////////////////////////////////////////////////////////////

//	uint8_t rxBuff[BUFFER_SIZE];
//	memset(rxBuff, 0, BUFFER_SIZE);
//
////	Serial.println("123123");
//
//	int dataSize = Serial1.available();
//
//	if (dataSize > 0)
//	{
//
//		Serial1.readBytes(rxBuff, dataSize);
//
//		Serial.println("rxBuff = ");
//
//		for (int i = 0; i < dataSize; i++)
//			Serial.println(rxBuff[i]);
//	}

}

//
//void streamLoop()
//{
//
//
//}
//

void readAccSensorLoop()
{
	xRead = analogRead(xPin);
	yRead = analogRead(yPin);
	zRead = analogRead(zPin);

	LaserRead = analogRead(LaserPin);

}

void mainControlLoop()
{
	standardFrame = viper.getStandardFrame();

	Serial.print("frameNumber = ");
	Serial.println(standardFrame.frameNumber);

	Serial.print("Vx = ");
	Serial.println(standardFrame.x);
	Serial.print("Vy = ");
	Serial.println(standardFrame.y);
	Serial.print("Vz = ");
	Serial.println(standardFrame.z);

	Serial.print("Vaz = ");
	Serial.println(standardFrame.az);
	Serial.print("Vel = ");
	Serial.println(standardFrame.el);
	Serial.print("Vro = ");
	Serial.println(standardFrame.ro);

	Serial.print("timeCheck = ");
	Serial.println(timeCheck);

	//Serial.print("Ax = ");
	//Serial.println(xRead);
	//Serial.print("Ay = ");
	//Serial.println(yRead);
	//Serial.print("Az = ");
	//Serial.println(zRead);

	Serial.print("Laser = ");
	Serial.println(LaserRead);


	Serial.println("======================");
}

// Add the main program code into the continuous loop() function
void loop()
{


}
