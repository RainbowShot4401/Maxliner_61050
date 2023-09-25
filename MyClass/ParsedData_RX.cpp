#include "..\stdafx.h"
#include "ParsedData_RX.h"
#include <vector>

using namespace std;

ParsedData_RX::ParsedData_RX()
{
}

ParsedData_RX::~ParsedData_RX()
{
}

void ParsedData_RX::getRxCalibrationHelpParameters(const int &refFreq, int &sub_band_res, int &gain_flat_res)
{
	if (refFreq >= 4900)
	{
		sub_band_res = 20;
		gain_flat_res = 40;
	}
	else
	{
		sub_band_res = 10;
		gain_flat_res = 10;
	}
}

void convertChanneltoFreq(int &val)
{
	int base_freq = (val >= 36) ? 5000 : 2407;
	val = (val) * 5 + base_freq;
}

void ParsedData_RX::parseThisCIS(vector<BYTE> CIS_raw_data)
{
	int sub_band_res = 0;
	int gain_flat_res = 0;


	rx_cis.common_section.atennaMask = CIS_raw_data.at(ANTENA_MASK_LOCATION);
	rx_cis.common_section.startFreq = (CIS_raw_data.at(START_FREQ_LOCATION + 1) << 8) + CIS_raw_data.at(START_FREQ_LOCATION);   //channel units
	rx_cis.common_section.stopFreq = (CIS_raw_data.at(STOP_FREQ_LOCATION + 1) << 8) + CIS_raw_data.at(STOP_FREQ_LOCATION);      //channel units
	rx_cis.common_section.calibrationFreq = (CIS_raw_data.at(CAL_FREQ_LOCATION + 1) << 8) + CIS_raw_data.at(CAL_FREQ_LOCATION); //channel units
	rx_cis.common_section.chipTemperature = CIS_raw_data.at(TEMP_LOCATION);
	rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints = CIS_raw_data.at(NUMBER_OF_POINTS_LOCATION) & 0xF;
	rx_cis.common_section.rssiFlarnessPoints = (CIS_raw_data.at(NUMBER_OF_POINTS_LOCATION) >> 4) & 0xF;


	//convertChanneltoFreq(rx_cis.common_section.startFreq);			//freq units
	//convertChanneltoFreq(rx_cis.common_section.stopFreq);			//freq units
	//convertChanneltoFreq(rx_cis.common_section.calibrationFreq);	//freq units

	parseByHalfByte(CIS_raw_data, RF_FLATNESS_LOCATION, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.common_section.rfFlatness, locationInCIS);
	parseByHalfByte(CIS_raw_data, locationInCIS, rx_cis.common_section.rssiFlarnessPoints, rx_cis.common_section.rssiFltaness, locationInCIS);

	
	getRxCalibrationHelpParameters(rx_cis.common_section.calibrationFreq, sub_band_res, gain_flat_res);
	convertToReferanceValue(rx_cis.common_section.rfFlatness, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.common_section.startFreq, gain_flat_res);
	convertToReferanceValue(rx_cis.common_section.rssiFltaness, rx_cis.common_section.rssiFlarnessPoints, rx_cis.common_section.startFreq, gain_flat_res);

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (((rx_cis.common_section.atennaMask >> i_ant) & 0x1) == 0) continue;		
		parseByHalfByte(CIS_raw_data, locationInCIS, NUMBER_OF_LNA_SUB_BANDS, rx_cis.antenna_section[i_ant].RF_Path.lna_sub_bands, locationInCIS);
		parseByFullByte(CIS_raw_data, locationInCIS, 1, &rx_cis.antenna_section[i_ant].RF_Path.mid_gain, locationInCIS);
		parseByFullByte(CIS_raw_data, locationInCIS, NUMBER_OF_GAIN_STEPS, rx_cis.antenna_section[i_ant].RF_Path.gain_steps, locationInCIS);
		parseByHalfByte(CIS_raw_data, locationInCIS, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.antenna_section[i_ant].RF_Path.mid_gain_delta, locationInCIS);
		parseByHalfByte(CIS_raw_data, locationInCIS, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.antenna_section[i_ant].RF_Path.rf_flatness_high, locationInCIS);
		parseByHalfByte(CIS_raw_data, locationInCIS, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.antenna_section[i_ant].RF_Path.rf_flatness_bypass, locationInCIS);
		
		convertToReferanceValue(rx_cis.antenna_section[i_ant].RF_Path.lna_sub_bands, NUMBER_OF_LNA_SUB_BANDS, rx_cis.common_section.startFreq, sub_band_res);
		convertToReferanceValue(rx_cis.antenna_section[i_ant].RF_Path.mid_gain_delta, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.common_section.startFreq, gain_flat_res);
		convertToReferanceValue(rx_cis.antenna_section[i_ant].RF_Path.rf_flatness_high, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.common_section.startFreq, gain_flat_res);
		convertToReferanceValue(rx_cis.antenna_section[i_ant].RF_Path.rf_flatness_bypass, rx_cis.common_section.rfFlarnessAndMidGainDeltaPoints, rx_cis.common_section.startFreq, gain_flat_res);

		for (int i_reg = 0; i_reg < DUT_NUM_REGION; i_reg++)
		{
			parseByFullByte(CIS_raw_data, locationInCIS, 1, &(rx_cis.antenna_section[i_ant].RSSI_Path.offset[i_reg]), locationInCIS);
			parseByHalfByte(CIS_raw_data, locationInCIS, 1, &(rx_cis.antenna_section[i_ant].RSSI_Path.gain[i_reg]), locationInCIS);
		}

		for (int i_reg = 0; i_reg < DUT_NUM_REGION; i_reg++)
		{
			parseByFullByte(CIS_raw_data, locationInCIS, 1, &(rx_cis.antenna_section[i_ant].RSSI_Path.a[i_reg]), locationInCIS);
			parseByFullByte(CIS_raw_data, locationInCIS, 1, &(rx_cis.antenna_section[i_ant].RSSI_Path.b[i_reg]), locationInCIS);
		}
		parseByHalfByte(CIS_raw_data, locationInCIS, rx_cis.common_section.rssiFlarnessPoints, rx_cis.antenna_section[i_ant].RSSI_Path.rssi_flatness, locationInCIS);
			
	}



}

void ParsedData_RX::parseByHalfByte(const vector<BYTE> data, const int & startPoint, const int & length, int return_array[], int &end_of_section)
{
	for (int i = 0, j = 0; i < length; i++)
	{
		if (i % 2)
		{
			return_array[i] = (data.at(startPoint + j) >> 4) & 0xF;
			j++;
		}
		else
		{
			return_array[i] = data.at(startPoint + j) & 0xF;
		}
	}
	end_of_section = startPoint + ceil(length / 2.0);
}

void ParsedData_RX::parseByFullByte(const vector<BYTE> data, const int & startPoint, const int & length, int return_array[], int & end_of_section)
{
	for (int i = 0; i < length; i++)
	{
		return_array[i] = data.at(startPoint + i) & 0xFF;

	}
	end_of_section = startPoint + length;
}




signed char convertInttoSignedChar(int input_val)
{
	/*
	signed char return_val;

	if (input_val > 7)
		input_val -= 16;
*/
	return input_val;
}


void ParsedData_RX::convertToReferanceValue(int input_array[], int array_length, int start_freq, int resolution)
{

	for (int i = 0; i < array_length; i++)
	{
		input_array[i] = convertInttoSignedChar(input_array[i]);
	}

	for (int i = 0; i < (array_length - 1); i++)
	{
		input_array[i + 1] += convertInttoSignedChar(input_array[i]);
	}

	for (int i = 0; i < array_length; i++)
	{
		input_array[i] *= resolution;
		input_array[i] += start_freq;
	}
}
/*
void ParsedData_RX::convertToReferanceValue(ParsedData_RX::RX_CIS & parsedData_RX)
{
	int freq_offset_ref = 0;
	int sub_band_res = 0;
	int gain_flat_res = 0;
	int temp_val = 0;
	getRxCalibrationHelpParameters(parsedData_RX.common_section.calibrationFreq, freq_offset_ref, sub_band_res, gain_flat_res);

	for (int i = 0; i < parsedData_RX.common_section.rfFlarnessAndMidGainDeltaPoints; i++)
	{
		parsedData_RX.common_section.rfFlatness[i] = convertInttoSignedChar(parsedData_RX.common_section.rfFlatness[i]);
	}

	for (int i = 0; i < (parsedData_RX.common_section.rfFlarnessAndMidGainDeltaPoints - 1); i++)
	{
		parsedData_RX.common_section.rfFlatness[i+1] += convertInttoSignedChar(parsedData_RX.common_section.rfFlatness[i]);
	}
	
	for (int i = 0; i < parsedData_RX.common_section.rfFlarnessAndMidGainDeltaPoints; i++)
	{
		parsedData_RX.common_section.rfFlatness[i] *= gain_flat_res;
		parsedData_RX.common_section.rfFlatness[i] += (parsedData_RX.common_section.startFreq * 5 + freq_offset_ref);
	}

}
*/