#pragma once
#include "ICalFileVerification.h"
#include "ParsedData_XTAL.h"
#include "ParsedData_RX.h"

class CalFileVerificationGen6 : public ICalFileVerification
{
public:
	//CalFileVerificationGen6(const ReadCalFile::CAL_FILE_DATA& cal_file_data_and_size, const DWORD& xtal_data, const Dut_CalDataStructVer6* txDutInputStruct, const int& number_of_channels, const int& antMask);
	CalFileVerificationGen6(const ReadCalFile::CAL_FILE_DATA& cal_file_data_and_size);
	~CalFileVerificationGen6();
	return_status rxVerification();
	return_status txVerification();


//private:
public:
	const static int MAX_ARRAY_SIZE = 4096;

	BYTE binFileData[MAX_ARRAY_SIZE];
	int binFileLength;

	struct {
		Dut_CalDataStructVer6 txParsed_struct[DUT_CAL_MAX_FREQS] = { 0 };
		DWORD xtal_data;
		int dutInputStructSize;
		Dut_CalDataStructVer6 txDutInputStruct[DUT_CAL_MAX_FREQS];
		int antennaMask;
	} txVer;


	struct {

	} rxVer;


	struct {
		const BYTE CARD_ID_CIS = 0x60;
		const BYTE TX_TSSI_CIS = 0x61;
		const BYTE TX_SUB_BAND_CIS = 0x62;
		const BYTE RX_CIS = 0x63;
		const BYTE XTAL_CIS = 0x64;
		const BYTE CIS_TOTAL_SIZE_LENGTH = 2;
		const BYTE CIS_HEADER_LENGTH = 1;
		const BYTE EEPROM_VERSION = 6;
		const BYTE EEPROM_VERSION_LOCATION = 40;  //0x28
	}CIS_CONST;


	struct CIS_GENERAL_INFO {
		int CIS_location;
		int CIS_size;
		vector<BYTE> CIS_raw_data;
	};

	struct {
		struct {
			vector<ParsedData_XTAL::XTAL_CIS> parsedData_XTAL;
			vector<ParsedData_RX::RX_CIS> parsedData_RX;
		}paresed_Data;

		struct {
			vector<CIS_GENERAL_INFO> CARD_ID_LOCATION;
			vector<CIS_GENERAL_INFO> TX_TSSI_LOCATION;
			vector<CIS_GENERAL_INFO> TX_SUB_BAND_LOCATION;
			vector<CIS_GENERAL_INFO> RX_LOCATION;
			vector<CIS_GENERAL_INFO> XTAL_LOCATION;
		}raw_data;

	}CIS_Parsed_info;

	CIS_GENERAL_INFO parseCalibrationFile(int CIS_Header_location);
};

