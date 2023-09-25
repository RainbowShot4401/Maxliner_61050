#include "..\stdafx.h"
#include "CalFileVerificationGen6.h"

//CalFileVerificationGen6::CalFileVerificationGen6(const ReadCalFile::CAL_FILE_DATA& cal_file_data_and_size, const DWORD& xtal_data, const Dut_CalDataStructVer6* txDutInputStruct, /*const Dut_RssiCalDataStruct* rxDutInputStruct,*/ const int& number_of_channels, const int& antMask)
CalFileVerificationGen6::CalFileVerificationGen6(const ReadCalFile::CAL_FILE_DATA& cal_file_data_and_size)
{
	memcpy(binFileData, cal_file_data_and_size.raw_data_cal_file, sizeof(cal_file_data_and_size.raw_data_cal_file));
	binFileLength = cal_file_data_and_size.nv_memmory_size;
	//txVer.dutInputStructSize = number_of_channels;
	//txVer.xtal_data = xtal_data;
	//memcpy(&txVer.txDutInputStruct, txDutInputStruct, sizeof(txVer.txDutInputStruct));
	//txVer.antennaMask = antMask;
	//memcpy(&rxVer.rxDutInputStruct, rxDutInputStruct, sizeof(rxVer.rxDutInputStruct));
}

CalFileVerificationGen6::~CalFileVerificationGen6()
{
}

return_status CalFileVerificationGen6::txVerification()
{
	return return_status();
}


CalFileVerificationGen6::CIS_GENERAL_INFO CalFileVerificationGen6::parseCalibrationFile(int CIS_Header_location)
{
	CIS_GENERAL_INFO general_info;
	general_info.CIS_location = CIS_Header_location;
	general_info.CIS_size = ((binFileData[CIS_Header_location + 2] & 0xFF) << 8) + binFileData[CIS_Header_location + 1] & 0xFF;
	general_info.CIS_raw_data.reserve(general_info.CIS_size);
	int stastAdressOffset = general_info.CIS_location + CIS_CONST.CIS_HEADER_LENGTH + CIS_CONST.CIS_TOTAL_SIZE_LENGTH;
	general_info.CIS_raw_data.insert(general_info.CIS_raw_data.end(), binFileData + stastAdressOffset, binFileData + stastAdressOffset + general_info.CIS_size);

	if (binFileData[CIS_Header_location] == CIS_CONST.CARD_ID_CIS)
	{
		CIS_Parsed_info.raw_data.CARD_ID_LOCATION.push_back(general_info);
	}
	else if (binFileData[CIS_Header_location] == CIS_CONST.XTAL_CIS)
	{
		ParsedData_XTAL* parsedData_XTAL = new ParsedData_XTAL();
		parsedData_XTAL->parseThisCIS(general_info.CIS_raw_data);

		CIS_Parsed_info.paresed_Data.parsedData_XTAL.push_back(parsedData_XTAL->xtal_cis);
		CIS_Parsed_info.raw_data.XTAL_LOCATION.push_back(general_info);

	}
	else if (binFileData[CIS_Header_location] == CIS_CONST.RX_CIS)
	{
		ParsedData_RX* parsedData_RX = new ParsedData_RX();
		parsedData_RX->parseThisCIS(general_info.CIS_raw_data);

		CIS_Parsed_info.paresed_Data.parsedData_RX.push_back(parsedData_RX->rx_cis);

		CIS_Parsed_info.raw_data.RX_LOCATION.push_back(general_info);
	}
	else if (binFileData[CIS_Header_location] == CIS_CONST.TX_TSSI_CIS)
	{
		CIS_Parsed_info.raw_data.TX_TSSI_LOCATION.push_back(general_info);
	}
	else if (binFileData[CIS_Header_location] == CIS_CONST.TX_SUB_BAND_CIS)
	{
		CIS_Parsed_info.raw_data.TX_SUB_BAND_LOCATION.push_back(general_info);
	}
	return general_info;
}

//all the paresed data stored at CIS_Parsed_info
return_status CalFileVerificationGen6::rxVerification()
{
	CalFileVerificationGen6::CIS_GENERAL_INFO current_cis_general_info;
	int currentLocation = CIS_CONST.EEPROM_VERSION_LOCATION;

	if (binFileData[currentLocation] == CIS_CONST.EEPROM_VERSION)
	{
		currentLocation++;
		while (currentLocation < binFileLength)
		{
			if (binFileData[currentLocation] == 0xFF)
				break;

			current_cis_general_info = parseCalibrationFile(currentLocation);
			currentLocation += current_cis_general_info.CIS_size + CIS_CONST.CIS_TOTAL_SIZE_LENGTH + 1;
		}
	}

	return return_status::Pass;
}


