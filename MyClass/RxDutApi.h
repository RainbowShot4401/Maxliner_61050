#pragma once

//#include "DUT_IQlite_HELPER.h"
#include "DutControl.h"
#include <vector>

using namespace std;

#define RSSI_NUM_SAMPLES 15
#define EVM_NUM_SAMPLES 5

typedef struct regSetting
{
	int module;
	DWORD address;
	DWORD pData;
	DWORD mask;
}regSetting_t;

class RxDutApi
{
public:
	//funcStartRxPower m_funcStartRxPower;
	//funcStopRxPower m_funcStopRxPower;

	enum status {FAIL, PASS};
	RxDutApi(dutWrapper *dutApi);
	~RxDutApi();

	string debug;
	string subBandDebug;
	vector<regSetting> RxCalregSetting;

	status startRxCalibration();
	status endRxCalibration();

	struct SetRxGainBlocksParams{
		int LnaIndex;
		int pgc1;
		int pgc2;
		int pgc3;
	};
	status setRxGainBlocks(const SetRxGainBlocksParams &setRxGainBlocksParams);

	struct SetRssiOffsetGainParams {
		int iOffset[DUT_NUM_REGION];
		int s2dGain[DUT_NUM_REGION];
		int antMask;
		int regionIndex;
	};
	status setRssiOffsetGain(const SetRssiOffsetGainParams &setRssiOffsetGainParams);
	double CalcAverageOfVectorLin(const vector<double>& v);
	int CalcAverageOfVectorLin(const vector<int>& v);
	double CalcAverageOfVectorLog(const vector<double>& v);
	status getEvmVector(vector<double> &evmVector);
	status getRssiVector(vector<double> &rssiVector);
	status getRecivedGoodPackets(int &recivedGoodPackets);
	status resetPacketsCounter();
	status WriteProdFlag(bool flag);

	status TurnOnAllTxRxAntennas(int antennaMask);

	struct RxSetChannelParams {
		int boardAntennaMask;
		string standard;
		int  freq;
		string bandwidth;
		int offlineCalMask = 0;
		int isCloseLoop;
		int rxAntennaMask;
		int sleepAfterSetChannel = 0;
		int startFreq;
		int endFrq;
	};
	status rxSetChannel(RxSetChannelParams setChannelParams);
	status fastRxSetChannel(RxSetChannelParams setChannelParams);
	

	struct RssiS2dSetInitialsParams {
		int numLnaGains;
		int numRegions; 
		float lnaGain[NUM_LNA_STEPS];
		float AuxADC_Res;
		float Ioffs_Step;
	};
	status rssiS2dSetInitials(RssiS2dSetInitialsParams &rssiS2dSetInitialsParams);

	enum RssiMeasMethod : MT_INT16 { accumulators = 0, Goertzel = 1, Capture = 2};
	struct MeasureRssiParams {
		RssiMeasMethod Method;
		MT_INT16 NOS;
	};
	status measureRssi(const MeasureRssiParams &measureRssiParams, MT_INT16 rssi[MAX_ANTENNA_NUMBER]);


	struct CalculateOptimalIoffsParams {
		int region;
		float V1[MAX_ANTENNA_NUMBER];
		float V2[MAX_ANTENNA_NUMBER];
		float s2d_gain_db[DUT_NUM_REGION];
		float s2d_offset_db[DUT_NUM_REGION];
	};
	status calculateOptimalIoffs(CalculateOptimalIoffsParams calculateOptimalIoffs, int s2d_offset_opt_out[MAX_ANTENNA_NUMBER]);


	struct CalculateOptimalS2dGainParams {
		int region;
		float V1[MAX_ANTENNA_NUMBER];
		float V2[MAX_ANTENNA_NUMBER];
		float gainInitFloat[DUT_NUM_REGION];
	};
	status calculateOptimalS2dGain(CalculateOptimalS2dGainParams calculateOptimalS2dGainParams, int s2d_gain_opt_out[MAX_ANTENNA_NUMBER]);

	struct CalculatePVectorParams {
		int numPwrPoints;
		int powerMax;
		float LnaGains[MAX_ANTENNA_NUMBER];
		int lowerP_Boundary;
		int upperP_Boundary;
	};
	status calculatePVector(CalculatePVectorParams calculatePVectorParams, float Pout[NUM_OF_POWER_POINTS]);


	struct CalculateLinearRegressionParams {
		float Pout[NUM_OF_POWER_POINTS];
		//float V[MAX_ANTENNA_NUMBER][7]; for 610.36
		float V[MAX_ANTENNA_NUMBER][16];
		float C[MAX_ANTENNA_NUMBER];
		float LnaGains[MAX_ANTENNA_NUMBER];
	};
	status calculateLinearRegression(CalculateLinearRegressionParams calculateLinearRegressionParams, ::Dut_Rssi_AB ab[MAX_ANTENNA_NUMBER]);

	status MeasureLNASubBandGains(int referanceFreq);
	status calculateLNASubBandGains(unsigned int rxBandFreq[MAX_ANTENNA_NUMBER][MAX_SUB_BAND_CROS_POINT]);
	
	//unsigned int sant[4];
	status setRxBandLUT(unsigned int rxBandFreq[MAX_ANTENNA_NUMBER][MAX_SUB_BAND_CROS_POINT_LUT]);

	struct MidGainCalibrateParams {
		float targetGain;
		float Prxin[MAX_ANTENNA_NUMBER];
		int Pgc1;
		int Pgc2;
	};
	status midGainCalibrate(MidGainCalibrateParams midGainCalibrateParams, float targetMeasGain[MAX_ANTENNA_NUMBER], int targetMeasControl[MAX_ANTENNA_NUMBER]);


	struct MeasureCwPowerParams {
		int Method = 0;
		int FreqOffset = 0;
		int NOS = 4096;
	};
	status MeasureCwPower(MeasureCwPowerParams measureCwPowerParams, DutApiCorrResults_t *corrResultsOut);


	struct CalculateLnaGainStepsParams {
		CorrResultsPerAntPerStep_t corrResultsPerAntPerStep_p;
		int lnaGainStep;
		float Prxin[MAX_DATA_STREAM];
		int Pgc1;
		int Pgc2;
		int Pgc3;
	};
	status CalculateLnaGainSteps(CalculateLnaGainStepsParams &calculateLnaGainStepsParams, GainPerAntPerStep_t *gainPerAntPerStep_p);


	struct CalculateRxRfFlatnessParams {
		int P_Adc_array[MAX_DATA_STREAM][16];
		float refGain[MAX_DATA_STREAM];
		unsigned int PGC1;
		unsigned int PGC2;
		unsigned int PGC3;
		float Pin[MAX_DATA_STREAM][16];
	};
	status CalculateRxRfFlatness(CalculateRxRfFlatnessParams &calculateRxRfFlatnessParams, float rf_flatness_delta_gain_results[MAX_ANTENNA_NUMBER][MAX_FLATNESS_POINTS]);

	status WriteProdFlag();

	status writeRssiCalData(const Dut_RssiCalDataStructVer6 * CalArray, int ArraySize);
	status writeRssiCalData(const Dut_RssiCalDataStructVer5 * CalArray, int ArraySize);


	status set11bRxAntennaMask(int rx1, int rx2, int rx3, int rx4, bool isGen6);


private:
	dutWrapper *_dutApi;
	RxDutApi::status ret;
};

