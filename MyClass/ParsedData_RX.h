#pragma once
#include "DutControl.h"
#include <math.h>
#include <vector>

using namespace std;

#define MAX_RF_FLATNESS 11
#define MAX_RSSI_FLATNESS 11
#define NUMBER_OF_LNA_SUB_BANDS 7
#define NUMBER_OF_GAIN_STEPS 8


#define ANTENA_MASK_LOCATION 0
#define START_FREQ_LOCATION 1
#define STOP_FREQ_LOCATION 3
#define CAL_FREQ_LOCATION 5
#define TEMP_LOCATION 7
#define NUMBER_OF_POINTS_LOCATION 8
#define RF_FLATNESS_LOCATION 9

class ParsedData_RX
{
public:
	ParsedData_RX();
	~ParsedData_RX();
	
		struct RX_CIS {
				struct {
					int atennaMask;
					int startFreq;
					int stopFreq;
					int calibrationFreq;
					int chipTemperature;
					int rfFlarnessAndMidGainDeltaPoints;
					int rssiFlarnessPoints;
					int rfFlatness[MAX_RF_FLATNESS];
					int rssiFltaness[MAX_RSSI_FLATNESS];
				}common_section;

				struct {
					struct {
						int lna_sub_bands[NUMBER_OF_LNA_SUB_BANDS];
						int mid_gain;
						int gain_steps[NUMBER_OF_GAIN_STEPS];
						int mid_gain_delta[MAX_RF_FLATNESS];
						int rf_flatness_high[MAX_RF_FLATNESS];
						int rf_flatness_bypass[MAX_RF_FLATNESS];
					}RF_Path;

					struct {
						int offset[DUT_NUM_REGION];
						int gain[DUT_NUM_REGION];
						int a[DUT_NUM_REGION];
						int b[DUT_NUM_REGION];
						int rssi_flatness[MAX_RSSI_FLATNESS];
					}RSSI_Path;

				}antenna_section[MAX_DATA_STREAM];
		
	}rx_cis;

	void parseThisCIS(vector<BYTE> CIS_raw_data);


private:
	void parseByHalfByte(const vector<BYTE> data, const int & startPoint, const int &length, int return_array[], int &end_of_section);  //return the end pointer
	void parseByFullByte(const vector<BYTE> data, const int & startPoint, const int &length, int return_array[], int &end_of_section);  //return the end pointer
	void getRxCalibrationHelpParameters(const int & refFreq, int & sub_band_res, int & gain_flat_res);

	int locationInCIS = 0;
	void convertToReferanceValue(int input_array[], int array_length, int start_freq, int resolution);
};

