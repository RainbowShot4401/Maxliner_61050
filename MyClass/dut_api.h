#ifndef DUT_DLL_API_H
#define DUT_DLL_API_H

#define PASCAL      __stdcall

// Defines unsigned 8-bit, 16-bit and 32-bit if windows.h is not defined:
typedef unsigned char BYTE;
typedef unsigned short WORD;
#ifdef WIN32
typedef unsigned long DWORD;
#else
typedef unsigned int DWORD;
#endif // WIN32

#include "mt_types.h"

#define DUT_USE_LOCAL_CAL_WLAN

#define HYP_MEMORY  0  /* access the hyperion memory */
#define HYP_PIF_CS0 1  /* access to Prometheus chip 0 */
#define HYP_PIF_CS1 2  /* access to Prometheus chip 1 */
#define HYP_SPI     3  /* access to Athena chip via SPI */

#define DUT_NEW_SET_CHANNEL

#define DUT_USE_EXIST_EEPROM_ADDR

#define DUT_NUM_REGION 3
#define MAC_ADDRESS_ARRAY_SIZE 6

// Sample point for calibration in the new TPC format:
typedef struct
{
	float x;
	float y;
	float backoff; // ignored when used as input to EEPROM V5 (max distance from Linear Regression result)
} Dut_XY;

// Return point for calibration in the new TPC (EEPROM Ver. 5) format:
typedef struct
{
	/*int*/short a;
	/*int*/short b;
} Dut_AB;

// Band types enumeration
typedef enum
{
	DUT_BAND_5000MHZ = 0,
	DUT_BAND_2400MHZ = 1,
	DUT_BAND_6000MHZ = 2,
	DUT_BAND_MAX = 3,
	DUT_BAND_2_AND_5 = 3,
	DUT_BAND_2_AND_6 = 4,
	DUT_BAND_5_AND_6 = 5,
	DUT_BAND_INVALID = 0xff
} dutBand_e;


typedef enum
{
	VER_CV = 0,
	VER_MAC = 1,
	VER_PSD = 2,
	VER_REGULATORY = 3,
	VER_TXSENDER = 4,
	VER_GENRISC_TXSENDER = 5,
	VER_GENRISC_HOST_INTERFACE = 6,
	VER_GENRISC_RXHANDLER = 7,
	VER_GENRISC_500B_PROGMODEL = 8,
	VER_GENRISC_600_PROGMODEL = 9,
	VER_GENRISC_600B_PROGMODEL = 10,
	VER_GENRISC_600D2_PROGMODEL = 11,
}dutComponentType_e;
#define DUT_CV_MAX_LENGTH  504
/***********************************************************************************/
/************************** TSSI ***************************************************/
/***********************************************************************************/

#define DUT_CALDATA3_MAXPOINTS 32
#define DUT_CALDATA_MAXPOINTS 64

// Ant. Calibration Structure for EEPROM Version 4 and 5 (Part of Dut_CalDataStruct below)
typedef struct
{
	int TPC; //Old Calibration
	::Dut_XY points[DUT_CALDATA3_MAXPOINTS]; //Old Calibration
	::Dut_XY pointsPerRegion[DUT_NUM_REGION][DUT_CALDATA_MAXPOINTS]; //New Calibration
	::Dut_AB pointsAB[DUT_NUM_REGION];
	int numRegions; //AR10 new cal = 1, else 1-3
	int s2dCalFlag; //0-no S2D cal, 1 - calibrate S2D
	int s2dGain[DUT_NUM_REGION]; //when s2dCalFlag != 0 dut calibrate per region S2D
	int s2dOffset[DUT_NUM_REGION]; //when s2dCalFlag != 0 dut calibrate per region S2D
	float nLog;
	/*unsigned char*/int maxPower;
	/*unsigned char*/int uEvm;
	/*unsigned char*/int uEvmGain;
	int numPointsVector[DUT_NUM_REGION];
	int numPoints; // # of points in 'points', must be last parameter in struct !
} Dut_AntCalDataStructVer5;

// Calibration Structure for EEPROM Version 4 and 5
// This is the parameter for the EEPROM V4 and v5 burning function (Dut_Burn_Cal_Data)
//
// Note that maxError and maxLines can be different for 2.4 and 5 GHz, but must be
// the same in every 5GHz structure and in every 2.4GHz structure
typedef struct
{
  int channel;
  int band; // DUT_BAND_5000MHZ or DUT_BAND_2400MHZ or DUT_BAND_6000MHZ
  int isChannelBonding; // Old: 0 for nCB, 1 for CB;    New: 0 for 20, 1 for 40, 2 for 80, 
  int maxLines; // Max. # of lines allowed as the result of the algorithm
  float maxError; // Max. error allowed in the linear regression algorithm
  ::Dut_AntCalDataStructVer5 ants[4];
} Dut_CalDataStructVer5;

typedef struct
{
	::Dut_XY pointsPerRegion[DUT_NUM_REGION][DUT_CALDATA_MAXPOINTS]; //New Calibration
	::Dut_AB pointsAB[DUT_NUM_REGION];
	int s2dGain[DUT_NUM_REGION]; //when s2dCalFlag != 0 dut calibrate per region S2D
	int s2dOffset[DUT_NUM_REGION]; //when s2dCalFlag != 0 dut calibrate per region S2D
	float nLog;
	int maxPower;
	int uEvm;
	int uEvmGain;
	int numPointsVector[DUT_NUM_REGION];
} Dut_AntCalDataStructVer6;

typedef struct
{
	int channel;
	int band; // DUT_BAND_5000MHZ or DUT_BAND_2400MHZ or DUT_BAND_6000MHZ
	int bw; //0 for 20, 1 for 40, 2 for 80,
	unsigned int antMask;
	int numRegions;
	int maxLines; // Max. # of lines allowed as the result of the algorithm
	float maxError; // Max. error allowed in the linear regression algorithm
	::Dut_AntCalDataStructVer6 ants[4];
} Dut_CalDataStructVer6;
/***********************************************************************************/
/************************** RSSI ***************************************************/
/***********************************************************************************/

#define NUM_LNA_STEPS 8

// dovg
#define NUM_LNA_CROSS_POINTS 8
#define NUM_PW_POINTS 7
#define RX_GAIN_NUM_FREQS 7
#define RX_GAIN_FLATNESS_NUM_FREQS 11
#define RF_RSSI_FLATNESS_NUM_FREQS 7

typedef struct
{
	float a;
	float b;
} Dut_Rssi_AB;
typedef struct
{
	int gain;
	int offset;
} Dut_Rssi_S2D;

typedef struct
{
	int numRegions; // from PSD
	int numLnaGainSteps; // actual size, default= NUM_LNA_STEPS [?]
	int numPwPoints; // Tester pass this value [?]
	int tuneWord; // result of: LNA Mid Gain Tuning Calibration  ?????????????? Needed? Exist in EEPROM, missing from Metlab code [?]
	/*unsigned char*/int midGain; // result of: LNA Mid Gain Tuning Calibration
    /*int*/float gainLna[NUM_LNA_STEPS]; // result of: LNA Gain Steps calibration
    /*unsigned short*/int rxRfFlatnessDeltaPoint[RX_GAIN_FLATNESS_NUM_FREQS]; //out of: Calibrate RFIC RX Gain Flatness (rxRfFlatnessFreqPoint shared)
	//int s2dCalFlag; // 0-no S2D cal, 1 - calibrate S2D [?]
	int s2dGain[DUT_NUM_REGION]; // when s2dCalFlag != 0 dut calibrate per region S2D
	int s2dOffset[DUT_NUM_REGION]; // when s2dCalFlag != 0 dut calibrate per region S2D
	::Dut_Rssi_AB pointsAB[DUT_NUM_REGION]; // result of: RF RSSI Calibration at reference frequency
    /*unsigned short*/int rxRssiFlatnessDeltaPoint[RF_RSSI_FLATNESS_NUM_FREQS]; //out of: RSSI Flatness (rxRssiFlatnessFreqPoint shared)
	int calcPowerVector[NUM_PW_POINTS]; // result of: calc and used in RF RSSI Calibration at reference frequency [?]
} Dut_AntRssiCalDataStructVer5;

typedef struct
{
  int refFreq/*channel*/;
  int BW; // 0 for 20, 1 for 40, 2 for 80 [?]
  int AntMask; //holds active ant. Mask
  /*unsigned short*/int subBandsCrossPoints[4][8]; // result of: Calculate LNA Sub-Bands
  /*unsigned char*/int numRxRfFlatness; // used when write to NV Device
  /*unsigned short*/int rxRfFlatnessFreqPoint[RX_GAIN_FLATNESS_NUM_FREQS]; //out of: Calibrate RFIC RX Gain Flatness (rxRfFlatnessDeltaPoint per ant.)
  /*unsigned char*/int numRxRssiFlatness; // used when write to NV Device
  /*unsigned short*/int rxRssiFlatnessFreqPoint[RF_RSSI_FLATNESS_NUM_FREQS]; //out of: RSSI Flatness (rxRssiFlatnessDeltaPoint per ant.)
  Dut_AntRssiCalDataStructVer5 ants[4];
} Dut_RssiCalDataStructVer5;

typedef struct
{
	int numRegions; // from PSD
	int numLnaGainSteps; // actual size, default= NUM_LNA_STEPS [?]
	int midGain; // result of: LNA Mid Gain Tuning Calibration
	float gainLna[NUM_LNA_STEPS]; // result of: LNA Gain Steps calibration
	int midGainDelta[RX_GAIN_FLATNESS_NUM_FREQS];
	int rxRfFlatnessDeltaPoint[RX_GAIN_FLATNESS_NUM_FREQS];
	int rxRfFlatnessDeltaPointBypass[RX_GAIN_FLATNESS_NUM_FREQS];
	::Dut_Rssi_S2D s2dGainOffset[DUT_NUM_REGION];
	::Dut_Rssi_AB pointsAB[DUT_NUM_REGION]; // result of: RF RSSI Calibration at reference frequency
	int rxRssiFlatnessDeltaPoint[RF_RSSI_FLATNESS_NUM_FREQS]; //out of: RSSI Flatness (rxRssiFlatnessFreqPoint shared)
	int subBandsCrossPoints[NUM_LNA_CROSS_POINTS]; // result of: Calculate LNA Sub-Bands
	//int subBandsCrossPoints7]; // result of: Calculate LNA Sub-Bands
	//int start_sb;
} Dut_AntRssiCalDataStructVer6;
typedef struct
{
	int antMask;
	int startFreqChannel;
	int stopFreqChannel;
	int calFreqChannel;
	int chipTemprature;
	int numRxRfFlatness; // used when write to NV Device
	int numRxRssiFlatness; // used when write to NV Device
	int rxRfFlatnessFreqPoint[RX_GAIN_FLATNESS_NUM_FREQS]; //out of: Calibrate RFIC RX Gain Flatness (rxRfFlatnessDeltaPoint per ant.)
	int rxRssiFlatnessFreqPoint[RF_RSSI_FLATNESS_NUM_FREQS]; //out of: RSSI Flatness (rxRssiFlatnessDeltaPoint per ant.)
	Dut_AntRssiCalDataStructVer6 ants[4];
} Dut_RssiCalDataStructVer6;

typedef struct GainPerAntPerStep_tag
{
	float gain[4][NUM_LNA_STEPS];
}GainPerAntPerStep_t;

typedef struct DutApiCorrResults_tag
{
	int II;
	int QQ;
	int IQ;
}DutApiCorrResults_t;

typedef struct CorrResultsPerAntPerStep_tag
{
	DutApiCorrResults_t corrResults[4][NUM_LNA_STEPS];// acoifman fix to CorrResults_t
}CorrResultsPerAntPerStep_t;

typedef struct DUTSetTransmitPowerVector_tag
{
	short lengthOfPowerVector;
	short startingPacketLength;
	short packetLengthIncrementPerIndex;
	wchar_t powerIndexArray[96];		   //C# char is 2byte,therefore, we need to use wchar_t in wrapper
	wchar_t numOfTransmitionPerPower[96];  //C# char is 2byte,therefore, we need to use wchar_t in wrapper
	short measuredVoltagePerPower[4][96];
} DUT_SetTransmitPowerVector_t;



/***********************************************************************************/


// HW types enumeration
typedef enum
{
  DUT_KNOWN_HW_MIN = 0xBEEF, //Magic to avoid coincidences
  DUT_HW_GEN2_PCI,
  DUT_HW_GEN3_PCI,
  DUT_HW_GEN3_PCIE,
  DUT_HW_AR10,
  DUT_HW_GEN5,
  DUT_KNOWN_HW_MAX,
  DUT_HW_UNKNOWN
} Dut_HwTypes;

// Work modes enumeration
typedef enum
{
  DUT_KNOWN_MODE_MIN = 0xFEED, //Magic to avoid coincidences
  DUT_MODE_2X3,                //Two transmitting antennas
  DUT_MODE_3X3,                //Three transmitting antennas
  DUT_MODE_4X4,                //Four transmitting antennas
  DUT_MODE_DEFAULT             //2X3 on Gen2, 3X3 on Gen3 and 4X4 on Gen5
} Dut_WorkModes;


#define MT_NO_MASK 0xFFFFFFFF // for use in Dut_WriteReg and Dut_ReadReg if no mask is needed
#define NO_XTAL_SECTION_FOUND 0xffffffff // returned in case read function does not find
                                         // the Xtal section in EEPROM

/*******************************************************************************/
/***************** RSSI ********************************************************/
/*******************************************************************************/

//2D array Example:
//P_Adc - 2D array size 4/5 (4 ant, 5 values each)
//Use example:
//  int** P_Adc_new;
//  P_Adc_new = new int*[4];
//  for (int i = 0; i < 4; ++i) {
//      P_Adc_new[i] = new int[5];
//  }
//  P_Adc_new[0][0]=5; ...etc.
//  Dut_xxx(x,..., P_Adc_new);
//
//Use example: param 'V' is 2D array in Dut_Calculate_Linear_Regression

//Not use abVal_t:
typedef struct
{
    unsigned char a;
    unsigned char b;
} abVal_t;



/*******************************************************************************/
/***************** Other ********************************************/
/*******************************************************************************/

// Maximum version length for Dut_ReadRiscVersionString
#define MAX_RISK_VER_LENGTH (256)

// Define for the 'band' parameter in Dut_SetChannel:
#define DUT_BAND_2400MHZ_FREQ	2400
#define DUT_BAND_5000MHZ_FREQ	5000
#define DUT_BAND_6000MHZ_FREQ	6000

// Defines for the 'mode' parameter in Dut_SetChannel:
#define DUT_MODE_A		0x00
#define DUT_MODE_B		0x01
#define DUT_MODE_G		0x02
#define DUT_MODE_N_5	0x04
#define DUT_MODE_N_2_4	0x05
#define DUT_MODE_AC		0x06
#define DUT_MODE_AX		0x07

// Defines for the 'BW' parameter in Dut_SetChannel:
#ifdef DUT_USE_NEW_BW
#define DUT_BW_20M 0
#define DUT_BW_40M 1
#define DUT_BW_80M 2
#define	DUT_BW_160M 3
#else
#define DUT_BW_20M 20
#define DUT_BW_40M 40
#define DUT_BW_80M 80
#define	DUT_BW_160M 160
#endif
#define DUT_BW_LOW_40_UPPER 40  // Upper freq of lower 40 in CB mode                        
#define DUT_BW_LOW_40_LOWER 41  // Lower freq of lower 40 in CB mode                        
#define DUT_BW_HIGH_40_UPPER 80 // Upper freq of upper 40 in CB mode
#define DUT_BW_HIGH_40_LOWER 81 // Lower freq of upper 40 in CB mode                

#define DUT_BW_LOW_40_UPPER_FW 0  // Upper freq of lower 40 in CB mode                        
#define DUT_BW_LOW_40_LOWER_FW 1  // Lower freq of lower 40 in CB mode                        
#define DUT_BW_HIGH_40_UPPER_FW 2 // Upper freq of upper 40 in CB mode
#define DUT_BW_HIGH_40_LOWER_FW 3 // Lower freq of upper 40 in CB mode                

// Defines for the 'mode' parameter in Dut_ChangeRFChipMode:
#define DUT_RF_MODE_LISTEN		0xF77 // Tx turned off
#define DUT_RF_MODE_STANDBY	0x800 // Only LO is kept on
#define DUT_RF_MODE_POWERDOWN	0

// Defines for Dut_ReadRFGain/Dut_ReadBBGain
#define DUT_LNA_HIGH    0 //Gen2/Gen3/Gen5 
#define DUT_LNA_MEDIUM  1 //Gen3/Gen5
#define DUT_LNA_LOW     2 //Gen2/Gen3/Gen5

#define DUT_MIXER_MINUS_6 8  //Gen2
#define DUT_MIXER_N       9  //Gen2

#define DUT_INFINITE_PACKETS 0xFFFF

#define DUT_DEF_PARAM_VALUE 0x55AA55AA // Use this value in Dut_SetResetSettings to ignore the parameter

// EEPROM addresses constants - cannot be changed (linked also in the DLL)
//
#define DUT_START_CIS_AREA_AR10 0x5A
#define DUT_START_CIS_AREA 0x29
#define MAC_address_configuration_input				0x4F
#define Country_address_configuration_input			0x4C
#define SN_address_configuration_input				0x55 // 24-bit in little endian
#define DATE_address_configuration_input			0x58
#define CIS_code_address_configuration_input	    0x48
#define CIS_TPC_tpl_code_address_configuration_input	0x5A
//#define CIS_TPC_size_address_configuration_input	0x5B
#define ChipVersions_BB_address_configuration_input	0x4D
//#define ChipVersions_RF_address_configuration_input	0x4E

//EEPROM V5:
#define MAC_address_configuration_input_V5_old				0x2A
#define Country_address_configuration_input_V5_old			0x22
#define SN_address_configuration_input_V5_old				0x2F
#define DATE_address_configuration_input_V5_old			0x30
//number of bytes = CIS_TPC_size_address_configuration_input_V5_old * CIS_TPC_size_address_single_struct_V5
//number of bytes per channel = number of bytes / numAnt
#define CIS_TPC_tpl_code_address_configuration_input_V5_old	0x34
//#define CIS_TPC_size_address_configuration_input_V5_old	0x35 //Size = number of structs
//#define CIS_TPC_size_address_single_struct_V5_old	0x36 //size of each struct
#define ChipVersions_BB_address_configuration_input_V5_old	0x23
//#define ChipVersions_RF_address_configuration_input_V5_old	0x24

//New:
#define MAC_address_configuration_input_V5				0x30//0x48
#define Country_address_configuration_input_V5			0x2D//0x45
#define SN_address_configuration_input_V5				0x36//0x4E
#define DATE_address_configuration_input_V5			    0x39//0x51
//number of bytes = CIS_TPC_size_address_configuration_input_V5 * CIS_TPC_size_address_single_struct_V5
//number of bytes per channel = number of bytes / numAnt
#define CIS_TPC_tpl_code_address_configuration_input_V5	0x29/*0x57*/ //Arad: Start search at new address - this is the first CIS in device
//#define CIS_TPC_size_address_configuration_input_V5	0x58 //Size = number of structs
//#define CIS_TPC_size_address_single_struct_V5	0x36 //size of each struct
#define ChipVersions_BB_address_configuration_input_V5	0x2E//0x46
#define ChipVersions_RF_address_configuration_input_V5	0x2F//0x47

//CISs related offsets:
#define DUT_CIS_TOTAL_LENGTH_OFFSET 1
#define DUT_CIS_DATA_OFFSET 2
//TPC header is different than non TPC CISs
#define DUT_CIS_LENGTH_STRUCT_OFFSET 2
#define DUT_CIS_TPC_DATA_OFFSET 3

//CIS related sizes:
#define DUT_CIS_HW_SIZE 0x12
#define  DUT_CIS_XTAL_SIZE 0x04
#define DUT_CIS_RSSI_SIZE 0x75


// Connection modes between the DUT DLL and the Lantiq device:
// Used in Dut_SetConnectionMode
#define DUT_CONNECTION_PCI		0	// Windows PCI test driver (WDMT)
#define DUT_CONNECTION_UART		1	// Serial connection to EVM boards (using Broker app)
#define DUT_CONNECTION_SPI		2	// SPI connection (currently under development)
#define DUT_CONNECTION_LINUX	3	// TCP/IP connection to DUT server inside a dongle

#define DUT_MODULATION_

/*
modulation=(param&0xF0)>>4
coding_rate=param&0x0F
where:
05  = 1/2
075 = 3/4
083 = 5/6
and
BPSK   = 1
QPSK   = 2
16QAM  = 4
64QAM  = 6
256QAM = 8
*/
#define DUT_MODULATION_BPSK_05 0x11
#define DUT_MODULATION_QPSK_05 0x21
#define DUT_MODULATION_QPSK_075 0x22
#define DUT_MODULATION_16QAM_05 0x41
#define DUT_MODULATION_16QAM_075 0x42
#define DUT_MODULATION_64QAM_05 0x61
#define DUT_MODULATION_64QAM_075 0x62
#define DUT_MODULATION_64QAM_083 0x64
#define DUT_MODULATION_256QAM_075 0x82
#define DUT_MODULATION_256QAM_083 0x84

#define DUT_SS_1 1
#define DUT_SS_2 2
#define DUT_SS_3 3
#define DUT_SS_4 4

#define DUT_GI_LONG 0
#define DUT_GI_SHORT 1


#endif // DUT_DLL_API_H
