#pragma once
#include  "Arduino.h"

#include <HardwareSerial.h>

#define DE 3 // is Transferring ?
#define RE 2 // is Receiving ?

#define RS422_RX_PIN 0
#define RS422_TX_PIN 1

#define BUFFER_SIZE 1024

#define RS422_BAUD_RATE2 921600
#define RS422_BAUD_RATE 921600//38400

#define RS422_Serial Serial3

#define PNO_FRAME_BYTE_SIZE 60

#define HEADER_V 0x56
#define HEADER_P 0x50
#define HEADER_R 0x52
#define HEADER_C 0x43


typedef enum eCmdActions
{
	CMD_ACTION_SET = 0           /**< Used with a command and payload to change state. */
	, CMD_ACTION_GET             /**< Used with a command to request current state. */
	, CMD_ACTION_RESET           /**< Used with a command to restore default state. */
	, CMD_ACTION_ACK             /**< Command received and processed, possible payload included. */
	, CMD_ACTION_NAK             /**< Command received, error found: always includes NAK_INFO struct in payload. */
	, CMD_ACTION_NAK_WARNING     /**< Command received and processed, warning condition: payload includes NAK_INFO struct */

	, CMD_ACTION_MAX
} eCmdActions;


//! \anchor anchoreViperCmds
//! @enum eViperCmds
//! Every command frame has a command associated with it.
typedef enum eViperCmds
{							/** cmd  actions		scope	payload struct		persist		cmd type	arg1/scope?		arg2/qualifier?	*/
							/**-----------------------------------------------------------------------------------------------------*/
// sensor cfg - persistable
	CMD_HEMISPHERE		/**  0   set/get/reset,	sensor	HEMISPHERE_CONFIG		p		cfg			sensornum						*/
	, CMD_FILTER			/**  1   set/get/reset,	sensor	FILTER_CONFIG			p		cfg			sensornum		eFilterTargets	*/
	, CMD_TIP_OFFSET		/**  2   set/get/reset,	sensor	TIP_OFFSET_CONFIG		 		cfg			sensornum						*/
	, CMD_INCREMENT			/**  3   set/get/reset,	sensor	INCREMENT_CONFIG		p		cfg			sensornum						*/
							// sensor cfg - runtime
	, CMD_BORESIGHT			/**  4   set/get/reset,	sensor	BORESIGHT_CONFIG				cfg			sensornum		bool reset Origin*/
// sensor cmds - read-only
, CMD_SENSOR_WHOAMI		/**  5      /get/     ,	sensor	WHOAMI_STRUCT					read-only									*/
// seu cfg - persistable
, CMD_FRAMERATE			/**  6   set/get/reset,	SEU		FRAMERATE_CONFIG(ENUM)	p		cfg											*/
, CMD_UNITS				/**  7   set/get/reset,	SEU		UNITS_CONFIG			p		cfg											*/
, CMD_SRC_ROTATION		/**  8   set/get/reset,	SEU		SRCROT_CONFIG			p		cfg			sourcenum						*/
, CMD_SYNC_MODE			/**  9   set/get/reset,	SEU		BINARY_CONFIG			p		cfg-binary									*/
, CMD_STATION_MAP		/**  10     /get/     ,	SEU		STATION_MAP				p		read-only											*/
, CMD_STYLUS			/**  11  set/get/reset,	SEU		STYLUS_CONFIG(ENUM)		p		cfg											*/
, CMD_SEUID				/**  12  set/get/reset,	SEU		VIPER_SEUID				p		cfg											*/
, CMD_DUAL_OUTPUT		/**  13  set/get/reset,	SEU		BINARY_CONFIG			p		cfg-binary									*/
, CMD_SERIAL_CONFIG		/**  14  set/get/reset,	SEU		SERIAL_CONFIG			p		cfg											*/
, CMD_BLOCK_CFG			/**  15         /reset,	SEU		none					p		cfg											*/
// seu cfg - runtime
, CMD_FRAME_COUNT		/*  not implemented 16     /   /reset,	SEU		none							executive									*/
, CMD_BIT				/**  17     /get/     ,	SEU		BIT_STRUCT						executive									*/
// seu cmds - pno
, CMD_SINGLE_PNO		/**  18  get/   /     ,	SEU		SEUPNO+SENFRAMEDATA				pno							ePnoMode		*/
, CMD_CONTINUOUS_PNO	/**  19  set/   /reset,	SEU		CONT_FC_CONFIG					pno							ePnoMode		*/
// seu cmds - read-only
, CMD_WHOAMI			/**  20     /get/     ,	SEU		WHOAMI_STRUCT					read-only									*/
// executive cmds
, CMD_INITIALIZE		/**  21  set/   /     ,	SEU		none							executive									*/
, CMD_PERSIST			/**  22  set/   /     ,	SEU		none							executive									*/
, CMD_ENABLE_MAP		/**  23  set/   /reset,	SEU		none							executive									*/

, CMD_FTT_MODE			/**  24  set/get/reset,	sensor	FTTMODE_CONFIG(ENUM)	p		cfg			sensornum						*/
, CMD_MAP_STATUS		/**  25     /get/     ,	SEU																					*/
, CMD_SENSOR_BLOCKCFG   /*  not implemented 26         /reset,	sensor	none					p		cfg			sensornum						*/
, CMD_SOURCE_CFG		/**  27  set/get/reset,	SEU		SRC_CONFIG				p		cfg			sourcenum						*/
, CMD_PREDFILTER_CFG	/**  28  set/get/reset,	sensor	PF_CONFIG				p		cfg			sensornum						*/
, CMD_PREDFILTER_EXT	/**  29  set/get/reset,	sensor	PF_CONFIG_EXT			p		cfg			sensornum						*/
, CMD_SRC_SELECT		/**  30  set/get/reset,	sensor	SRC_SEL_CONFIG			p		cfg			sensornum						*/
, CMD_SNS_ORIGIN		/**  31  set/get/reset,	sensor	SNS_ORIG_CONFIG(ENUM)	p		cfg			sensornum						*/
, CMD_SNS_VIRTUAL		/**  32  set/get/reset,	sensor	VIRTUAL_SNS_CONFIG		p		cfg			sensornum						*/
, CMD_SRC_WHOAMI		/**  33     /get/     ,	source	WHOAMI_STRUCT    		p		read-only	sourcenum						*/
, CMD_MAX
} eViperCmds;

typedef struct STANDARD_FRAME_
{
	int id = 0;
	int frameNumber = 0;
	int sensorCount = 0;
	int sensorNumber = 0;
	int sensorButtonStates = 0;
	int distortionLevel = 0;
	int auxiliaryDigitalInputValue = 0;

	float x = 0;
	float y = 0;
	float z = 0;

	float az = 0;
	float el = 0;
	float ro = 0;

}STANDARD_FRAME;


class RS422
{
public:

	void init();

	uint32_t Viper_CalcCRC_Bytes(uint8_t *data, uint32_t count);
	void crc16(uint32_t * crc, uint32_t data);

	void startPNO();
	void stopPNO();
	void initContinuousPNO();
	void runDualoutput();
	void changeBaudrate();
	void changeViperBaudrate();

	void readPacket();

	STANDARD_FRAME getStandardFrame();


private:
	uint8_t rxBuff[BUFFER_SIZE];
	uint8_t txBuff[BUFFER_SIZE];
	uint8_t packetBuff[BUFFER_SIZE];

	int decodePos;

	STANDARD_FRAME standardFrame;


};


