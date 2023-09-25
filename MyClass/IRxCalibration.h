#pragma once

#include <iterator>
#include "RxDutApi.h"
#include "IVSG.h"
#include <memory>
//#include "LogPrint.h"

//#define MAX_DATA_STREAM   4
#define MAX_SUB_BAND_CONT_WORDS 8 
//#define MAX_SUB_BAND_CROS_POINT   7
#define MAX_GAIN_STEPS   8
//#define MAX_FLATNESS_POINTS   7
#define MAX_TESTER_NUM    4
#define MAX_NUM_OF_INDEXES 64
#define NUMBER_OF_SAMPLES_FOR_MAX 3
#define MAX_NUMBER_OF_S2D_ATTEMPTS 5
#define S2D_OFFSET_MARGIN 30

//for Maxliner RF Rx MidGainCalibration.
#define NUM_OF_ANTENNA (4)
#define	NUM_OF_LNA_GAINS (6)

//Zack add for 610.50
#define RX_GAIN_FLATNESS_NUM_FREQS 16

struct VsgStartCWParams
{
	double loFreqMHz = 5500;
	double offsetFrequencyMHz = 1.25;
	double rfPowerLeveldBm = -10;
	double cableLoss = 20;
};

class IRxCalibration
{	
public:
	enum status { FAIL, PASS };
	enum GenType { GEN5, GEN6 };
	
	IRxCalibration();
	virtual ~IRxCalibration();

	struct RxRfSubBandParams {
		vector<double> subBandFreq;
		RxDutApi::RxSetChannelParams setChannelParams;
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
	};
	virtual status RxRfSubBandCal(RxRfSubBandParams rxRfSubBandParams) = 0;


	struct RxRfMidGainLimitsParams{
		double midLnaGainMin;
		double midLnaGainMax;
		double measGainTolerance;
	};
	struct RxRfMidGainParams {
		RxRfMidGainLimitsParams rxRfMidGainLimitsParams;
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		RxDutApi::MidGainCalibrateParams midGainCalibrateParams;
		VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
	};
	virtual status RxRfMidGainCal(RxRfMidGainParams rxRfMidGainParams) = 0;

	struct RxRfGainStepAccuracyParams {
		int numberOfLnaGains;
		int pgc1ArrayGains[NUM_LNA_STEPS];
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		RxDutApi::MeasureCwPowerParams measureCwPowerParams;
		RxDutApi::CalculateLnaGainStepsParams calculateLnaGainStepsParams;
		VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
	};
	virtual status RxRfGainStepAccuracyCal(RxRfGainStepAccuracyParams rxRfGainStepAccuracyParams) = 0;

	struct RxRfFlatnessParams {
		bool measureBypasFlatness;
		vector<double> rfFlatnessFreq;
		RxDutApi::RxSetChannelParams setChannelParams;
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParamsBypass;
		RxDutApi::MeasureCwPowerParams measureCwPowerParams;
		RxDutApi::CalculateRxRfFlatnessParams calculateRxRfFlatnessParams;
		RxDutApi::CalculateRxRfFlatnessParams calculateRxRfFlatnessParamsBypass;
		VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
	};
	virtual status RxRfFlatnessCal(RxRfFlatnessParams rxRfFlatnessParams) = 0;


	struct RxRssiS2dParams {
		int antennaMask;
		int numberOfRegions;
		double maxPower[DUT_NUM_REGION];
		double marginHigh[DUT_NUM_REGION];
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		RxDutApi::SetRssiOffsetGainParams setRssiOffsetGainParams;
		RxDutApi::RssiS2dSetInitialsParams rssiS2dSetInitialsParams;
		RxDutApi::MeasureRssiParams measureRssiParams;
		RxDutApi::CalculateOptimalIoffsParams calculateOptimalIoffsParams;
		RxDutApi::CalculateOptimalS2dGainParams calculateOptimalS2dGainParams;
		VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
		float gain[DUT_NUM_REGION];
		float offset[DUT_NUM_REGION];
	};
	virtual status RxRssiS2dCal(RxRssiS2dParams rxRssiS2dCalParams) = 0;


	struct RxRssiABParams {
		int antennaMask;
		int numberOfRegions;
		double maxPower[DUT_NUM_REGION];
		RxDutApi::SetRxGainBlocksParams setRxGainBlocksParams;
		RxDutApi::CalculatePVectorParams calculatePVectorParams;  //take powerMax[i] from double maxPower[MAX_REGION_NUM], gainStep from gain step calibration, 
		RxDutApi::MeasureRssiParams measureRssiParams;
		IVSG::VsgStartCWParams vsgStartCWParams;
		//IVSG *vsg;
		IVSG::PathLossParams pathLossParams;
	};
	virtual status RxRssiABCal(RxRssiABParams rxRssiABParams) = 0;



	struct RxRssiS2dFlatnessParams {
	};
	virtual status RxRssiS2dFlatnessCal(RxRssiS2dFlatnessParams rxRssiS2dFlatnessParams) = 0;


	struct MidGainResultsParams {
		vector<float> measGain;
		vector<int> calcTuneWord;
	};
	virtual MidGainResultsParams getMidGainResults() = 0;

	struct SubBandResultsParams {
		int subBandFreq[MAX_DATA_STREAM][NUM_LNA_CROSS_POINTS] = {};
	};
	virtual SubBandResultsParams getSubBandResults() = 0;

	struct GainStepResultsParams {
		GainPerAntPerStep_t gainSteps;
		int numLnaGainSteps;
	};
	virtual GainStepResultsParams getGainStepResults() = 0;

	struct FlatnessResultsParams {
		vector <double> flatnessFreq;
		int flatnessdB[MAX_DATA_STREAM][RX_GAIN_FLATNESS_NUM_FREQS] = {};
		int flatnessBypassDB[MAX_DATA_STREAM][RX_GAIN_FLATNESS_NUM_FREQS] = {};
		int midGainDelta[MAX_DATA_STREAM][RX_GAIN_FLATNESS_NUM_FREQS] = {};
	};
	virtual FlatnessResultsParams getFlatnessResults() = 0;


	struct S2dResultsParams {
		int numberOfRegions;
		int offset[MAX_DATA_STREAM][DUT_NUM_REGION] = {};
		int gain[MAX_DATA_STREAM][DUT_NUM_REGION] = {};
	};
	virtual S2dResultsParams getS2dResults() = 0;


	struct AbResultsParams {
		Dut_Rssi_AB ab[MAX_ANTENNA_NUMBER][DUT_NUM_REGION] = {};
		double maxError[MAX_ANTENNA_NUMBER][DUT_NUM_REGION] = {};
	};
	virtual AbResultsParams getAbResults() = 0;



	//set default values APIs
	virtual MidGainResultsParams setMidGainResults(MidGainResultsParams midGainResultsParams) = 0;

	virtual SubBandResultsParams setSubBandResults(SubBandResultsParams subBandResultsParams) = 0;

	virtual GainStepResultsParams setGainStepResults(GainStepResultsParams gainStepResultsParams) = 0;

	virtual FlatnessResultsParams setFlatnessResults(FlatnessResultsParams flatnessResultsParams) = 0;

	virtual S2dResultsParams setS2dResults(S2dResultsParams s2dResultsParams) = 0;

	virtual AbResultsParams setAbResults(AbResultsParams abResultsParams) = 0;
};

