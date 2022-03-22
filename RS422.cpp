// 
// 
// 

#include "RS422.h"

unsigned long time = 0;
unsigned long timeCheck = 0;


void RS422::init()
{
	memset(rxBuff, 0, BUFFER_SIZE);
	memset(txBuff, 0, BUFFER_SIZE);
	memset(packetBuff, 0, BUFFER_SIZE);

	decodePos = 0;

	RS422_Serial.begin(RS422_BAUD_RATE);


}

uint32_t RS422::Viper_CalcCRC_Bytes(uint8_t *data, uint32_t count)
{
	uint32_t crc;
	uint32_t n;

	crc = 0;
	for (n = 0; n < count; n++)
		crc16(&crc, data[n]);

	return crc;
}

void RS422::crc16(uint32_t * crc, uint32_t data)
{
	static const char op[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
	data = (data ^ (*crc)) & 0xff;
	*crc >>= 8;

	if (op[data & 0xf] ^ op[data >> 4])
		*crc ^= 0xc001;

	data <<= 6;
	*crc ^= data;
	data <<= 1;
	*crc ^= data;

	return;
}

void RS422::initContinuousPNO()
{
	// PREAMBLE
	txBuff[0] = 0x56;
	txBuff[1] = 0x50;
	txBuff[2] = 0x52;
	txBuff[3] = 0x43;

	txBuff[4] = 0x18;
	txBuff[5] = 0x00;
	txBuff[6] = 0x00;
	txBuff[7] = 0x00;

	//////////////////////////////////////////////////

	txBuff[8] = 0x00;
	txBuff[9] = 0x00;
	txBuff[10] = 0x00;
	txBuff[11] = 0x00;

	txBuff[12] = CMD_CONTINUOUS_PNO;
	txBuff[13] = 0x00;
	txBuff[14] = 0x00;
	txBuff[15] = 0x00;

	txBuff[16] = CMD_ACTION_RESET;
	txBuff[17] = 0x00;
	txBuff[18] = 0x00;
	txBuff[19] = 0x00;

	txBuff[20] = 0x00;
	txBuff[21] = 0x00;
	txBuff[22] = 0x00;
	txBuff[23] = 0x00;

	txBuff[24] = 0x00;
	txBuff[25] = 0x00;
	txBuff[26] = 0x00;
	txBuff[27] = 0x00;

	//////////////////////////////////////////////////

	uint32_t CRC_Data = 0;
	CRC_Data = Viper_CalcCRC_Bytes(txBuff, 28);

	uint32_t CRCDataCalc[4] = { 0, };

	CRCDataCalc[0] = (CRC_Data & 0x000000FF) >> 0;
	CRCDataCalc[1] = (CRC_Data & 0x0000FF00) >> 8;
	CRCDataCalc[2] = (CRC_Data & 0x00FF0000) >> 16;
	CRCDataCalc[3] = (CRC_Data & 0xFF000000) >> 24;

	txBuff[28] = CRCDataCalc[0];
	txBuff[29] = CRCDataCalc[1];
	txBuff[30] = CRCDataCalc[2];
	txBuff[31] = CRCDataCalc[3];


	RS422_Serial.write(txBuff, 32);
}

void RS422::runDualoutput()
{
	// PREAMBLE
	txBuff[0] = 0x56;
	txBuff[1] = 0x50;
	txBuff[2] = 0x52;
	txBuff[3] = 0x43;

	txBuff[4] = 0x1C;	// Size
	txBuff[5] = 0x00;
	txBuff[6] = 0x00;
	txBuff[7] = 0x00;

	//////////////////////////////////////////////////

	txBuff[8] = 0x00;
	txBuff[9] = 0x00;
	txBuff[10] = 0x00;
	txBuff[11] = 0x00;

	txBuff[12] = 0x0D;
	txBuff[13] = 0x00;
	txBuff[14] = 0x00;
	txBuff[15] = 0x00;

	txBuff[16] = 0x00;
	txBuff[17] = 0x00;
	txBuff[18] = 0x00;
	txBuff[19] = 0x00;

	txBuff[20] = 0x00;
	txBuff[21] = 0x00;
	txBuff[22] = 0x00;
	txBuff[23] = 0x00;

	txBuff[24] = 0x00;
	txBuff[25] = 0x00;
	txBuff[26] = 0x00;
	txBuff[27] = 0x00;

	//////////////////////////////////////////////////
	// Payload struct
	txBuff[28] = 1;
	txBuff[29] = 0;
	txBuff[30] = 0;
	txBuff[31] = 0;

	//////////////////////////////////////////////////

	uint32_t CRC_Data = 0;
	CRC_Data = Viper_CalcCRC_Bytes(txBuff, 32);

	uint32_t CRCDataCalc[4] = { 0, };

	CRCDataCalc[0] = (CRC_Data & 0x000000FF) >> 0;
	CRCDataCalc[1] = (CRC_Data & 0x0000FF00) >> 8;
	CRCDataCalc[2] = (CRC_Data & 0x00FF0000) >> 16;
	CRCDataCalc[3] = (CRC_Data & 0xFF000000) >> 24;

	txBuff[32] = CRCDataCalc[0];
	txBuff[33] = CRCDataCalc[1];
	txBuff[34] = CRCDataCalc[2];
	txBuff[35] = CRCDataCalc[3];


	RS422_Serial.write(txBuff, 36);
}


void RS422::startPNO()
{
	// PREAMBLE
	txBuff[0] = HEADER_V;
	txBuff[1] = HEADER_P;
	txBuff[2] = HEADER_R;
	txBuff[3] = HEADER_C;

	// SIZE
	txBuff[4] = 0x1C;
	txBuff[5] = 0x00;
	txBuff[6] = 0x00;
	txBuff[7] = 0x00;

	// SEUID
	txBuff[8] = 0x00;
	txBuff[9] = 0x00;
	txBuff[10] = 0x00;
	txBuff[11] = 0x00;

	// CMD
	txBuff[12] = CMD_CONTINUOUS_PNO;
	txBuff[13] = 0x00;
	txBuff[14] = 0x00;
	txBuff[15] = 0x00;

	// ACTION
	txBuff[16] = CMD_ACTION_SET;
	txBuff[17] = 0x00;
	txBuff[18] = 0x00;
	txBuff[19] = 0x00;

	// ARG1
	txBuff[20] = 0x00;
	txBuff[21] = 0x00;
	txBuff[22] = 0x00;
	txBuff[23] = 0x00;

	// ARG2
	txBuff[24] = 0x00;
	txBuff[25] = 0x00;
	txBuff[26] = 0x00;
	txBuff[27] = 0x00;

	// Command Payload
	txBuff[28] = 0x00;
	txBuff[29] = 0x00;
	txBuff[30] = 0x00;
	txBuff[31] = 0x00;

	uint32_t CRC_Data = 0;
	CRC_Data = Viper_CalcCRC_Bytes(txBuff, 32);

	uint32_t CRCDataCalc[4] = { 0, };

	CRCDataCalc[0] = (CRC_Data & 0x000000FF) >> 0;
	CRCDataCalc[1] = (CRC_Data & 0x0000FF00) >> 8;
	CRCDataCalc[2] = (CRC_Data & 0x00FF0000) >> 16;
	CRCDataCalc[3] = (CRC_Data & 0xFF000000) >> 24;

	txBuff[32] = CRCDataCalc[0];
	txBuff[33] = CRCDataCalc[1];
	txBuff[34] = CRCDataCalc[2];
	txBuff[35] = CRCDataCalc[3];


	RS422_Serial.write(txBuff, 36);
}

void RS422::stopPNO()
{
	// PREAMBLE
	txBuff[0] = HEADER_V;
	txBuff[1] = HEADER_P;
	txBuff[2] = HEADER_R;
	txBuff[3] = HEADER_C;

	// SIZE
	txBuff[4] = 0x18;
	txBuff[5] = 0x00;
	txBuff[6] = 0x00;
	txBuff[7] = 0x00;

	// SEUID
	txBuff[8] = 0x00;
	txBuff[9] = 0x00;
	txBuff[10] = 0x00;
	txBuff[11] = 0x00;

	// CMD
	txBuff[12] = CMD_CONTINUOUS_PNO;
	txBuff[13] = 0x00;
	txBuff[14] = 0x00;
	txBuff[15] = 0x00;

	// ACTION
	txBuff[16] = CMD_ACTION_RESET;
	txBuff[17] = 0x00;
	txBuff[18] = 0x00;
	txBuff[19] = 0x00;

	// ARG1
	txBuff[20] = 0x00;
	txBuff[21] = 0x00;
	txBuff[22] = 0x00;
	txBuff[23] = 0x00;

	// ARG2
	txBuff[24] = 0x00;
	txBuff[25] = 0x00;
	txBuff[26] = 0x00;
	txBuff[27] = 0x00;

	uint32_t CRC_Data = 0;
	CRC_Data = Viper_CalcCRC_Bytes(txBuff, 28);

	uint32_t CRCDataCalc[4] = { 0, };

	CRCDataCalc[0] = (CRC_Data & 0x000000FF) >> 0;
	CRCDataCalc[1] = (CRC_Data & 0x0000FF00) >> 8;
	CRCDataCalc[2] = (CRC_Data & 0x00FF0000) >> 16;
	CRCDataCalc[3] = (CRC_Data & 0xFF000000) >> 24;

	// CRC
	txBuff[28] = CRCDataCalc[0];
	txBuff[29] = CRCDataCalc[1];
	txBuff[30] = CRCDataCalc[2];
	txBuff[31] = CRCDataCalc[3];


	RS422_Serial.write(txBuff, 32);
}

void RS422::changeViperBaudrate()
{
	// PREAMBLE
	txBuff[0] = 0x56;
	txBuff[1] = 0x50;
	txBuff[2] = 0x52;
	txBuff[3] = 0x43;

	txBuff[4] = 0x18;
	txBuff[5] = 0x00;
	txBuff[6] = 0x00;
	txBuff[7] = 0x00;

	txBuff[8] = 0x00;
	txBuff[9] = 0x00;
	txBuff[10] = 0x00;
	txBuff[11] = 0x00;

	txBuff[12] = 0x0E;
	txBuff[13] = 0x00;
	txBuff[14] = 0x00;
	txBuff[15] = 0x00;

	txBuff[16] = 0x00;
	txBuff[17] = 0x00;
	txBuff[18] = 0x00;
	txBuff[19] = 0x00;

	txBuff[20] = 0x00;
	txBuff[21] = 0x00;
	txBuff[22] = 0x00;
	txBuff[23] = 0x00;

	txBuff[24] = 0x00;
	txBuff[25] = 0x00;
	txBuff[26] = 0x00;
	txBuff[27] = 0x00;

	//////////////////////////////////////////////////
	// Payload struct
	txBuff[28] = 0;
	txBuff[29] = 10;
	txBuff[30] = 0;
	txBuff[31] = 0;

	//////////////////////////////////////////////////

	uint32_t CRC_Data = 0;
	CRC_Data = Viper_CalcCRC_Bytes(txBuff, 32);

	uint32_t CRCDataCalc[4] = { 0, };

	CRCDataCalc[0] = (CRC_Data & 0x000000FF) >> 0;
	CRCDataCalc[1] = (CRC_Data & 0x0000FF00) >> 8;
	CRCDataCalc[2] = (CRC_Data & 0x00FF0000) >> 16;
	CRCDataCalc[3] = (CRC_Data & 0xFF000000) >> 24;

	txBuff[32] = CRCDataCalc[0];
	txBuff[33] = CRCDataCalc[1];
	txBuff[34] = CRCDataCalc[2];
	txBuff[35] = CRCDataCalc[3];


	RS422_Serial.write(txBuff, 36);
}

void RS422::changeBaudrate()
{
//	RS422_Serial.begin(RS422_BAUD_RATE2);
}

void RS422::readPacket()
{
	int dataSize = RS422_Serial.available();

	if (dataSize > 0)
	{
		time = micros();

		RS422_Serial.readBytes(rxBuff, dataSize);

		// Exception...
		if (decodePos >= BUFFER_SIZE)
		{
			decodePos = 0;
		}

		memcpy(&packetBuff[decodePos], rxBuff, dataSize);

		decodePos = decodePos + dataSize;

		///////////////////////////////////////////////////


		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			if (packetBuff[i] == HEADER_V && packetBuff[i + 1] == HEADER_P && packetBuff[i + 2] == HEADER_R && packetBuff[i + 3] == HEADER_P &&
				packetBuff[i + 60] == HEADER_V && packetBuff[i + 61] == HEADER_P && packetBuff[i + 62] == HEADER_R && packetBuff[i + 63] == HEADER_P)	// Header			
			{

				uint8_t dataPacket[PNO_FRAME_BYTE_SIZE] = { 0, };

				for (int j = 0; j < PNO_FRAME_BYTE_SIZE; j++)
				{
					dataPacket[j] = packetBuff[i + j];
				}

				memcpy(&standardFrame.frameNumber, &dataPacket[12], 4);

				memcpy(&standardFrame.x, &dataPacket[28], 4);
				memcpy(&standardFrame.y, &dataPacket[32], 4);
				memcpy(&standardFrame.z, &dataPacket[36], 4);
				memcpy(&standardFrame.az, &dataPacket[40], 4);
				memcpy(&standardFrame.el, &dataPacket[44], 4);
				memcpy(&standardFrame.ro, &dataPacket[48], 4);

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////

				memset(&packetBuff[i], 0, 60);
				memcpy(&packetBuff[i], &packetBuff[i + 60], 100);
				memcpy(&packetBuff[0], &packetBuff[i], 100);

				decodePos = 6;

				timeCheck = micros() - time;

				break;
			}
		}

	}

}

STANDARD_FRAME RS422::getStandardFrame()
{
	return standardFrame;
}