#pragma once
#include "..\stdafx.h"
#include <memory>
#include "TxControl.h"
#include <iterator>

#define NUMBER_OF_BANDS 2
#define MAX_INDEX 63

class ITxCalibration
{
public:

	enum Status { FAIL, PASS };
	enum GenType { GEN5, GEN6 };
	enum CalOrLoad { CalibrateS2D, SetS2D};

	ITxCalibration();
	virtual ~ITxCalibration();

	struct TxSetChannelParams {
		int boardAntennaMask;
		string standard;
		int  freq;
		string bandwidth;
		int offlineCalMask;
		CTxControl::OpenClose isCloseLoop;
		int txAntennaMask;
		int numOfStreams;
		string rate;
		int sleepAfterSetChannel = 0;
	};

	//Calibrate:
	struct TxGainTableParams {
		int regions[DUT_NUM_REGION];
		int defaultCalibrationIndex;
		ShiftPowerOutTableParames shiftPowerOutTableParames;
		/*VSA::SetVsaAmpTolParams setVsaAmpTolParams;
		VSA::SetVsaParams setVsaParams;
		VSA::CaptureVsaParams captureVsaParams;
		VSA::AnalyzeParams analyzeParams;*/
		double cableLoss;
	};
	virtual Status txGainTableCal(const TxGainTableParams &txGainTableParams) = 0;
	
		struct ABCoefficientsParams {
		float nlog;
	};

	struct TxS2dParams {
		int antNum;
		int bw;
		int band;
		int regions[3];
		int tssiLow;
		int tssiHigh;
		int outS2dGain[DUT_NUM_REGION];
		int outS2dOffset[DUT_NUM_REGION];
	};
	virtual Status txS2dCal(const TxS2dParams &txS2dParams) = 0;

	struct ConstantResultsParams {
		bool enableConstValue;
		bool measureEvm;
		double const_max_power;
		double const_ultimat_evm;
	};

	struct TxOpenLoopSweepParams {
		int antennaNumber;
		vector<int> indexes;
		int analyzedPacketsPerIndex;
		int packetLength;
		int sifs;
		double cableLoss;
		double targetUltimateEvm;
		double targetMaxPowerEvm;
		bool vectorFirstCaptureEvmEnable;
		//VSA::SetVsaAmpTolParams setVsaAmpTolParams;
		//VSA::SetVsaParams setVsaParams;
		//VSA::CaptureVsaParams captureVsaParams;
		//VSA::AnalyzeParams analyzeParams;
		ConstantResultsParams constantResultsParams;
	};
	virtual Status txOpenLoopSweep(const TxOpenLoopSweepParams &txOpenLoopSweepParams) = 0;
	virtual Status txOpenLoopSweepVector(const TxOpenLoopSweepParams &txOpenLoopSweepParams) = 0;

	//Get:
	virtual int getTxGainTableIndex() = 0;
	virtual int getTxGainTableResult() = 0;
	
	struct OpenLoopSweepResultsParams {
		vector<int> indexes;
		vector<double> power;
		vector<int> voltage;
		vector<double> evm;
	};
	virtual OpenLoopSweepResultsParams getOpenLoopSweepResults() = 0;

	struct MaxPowerParams {
		double maxPower_dBm;
		int maxPower_Index;
		double maxPowerEvm_dB;
	};
	virtual  ITxCalibration::MaxPowerParams getMaxPower() = 0;

	struct UltimateEvmParams {
		double ultimateEvmPower_dBm;
		int ultimateEvmPower_Index;
		double ultimateEvmPowerEvm_dB;
	};
	virtual ITxCalibration::UltimateEvmParams getUltimateEvm() = 0;

	struct GainTableResultsParams {
		int calIndex;
		int offsetGain;
	};
	virtual ITxCalibration::GainTableResultsParams getTxGainTableParams() = 0;

	struct S2dResultsParams {
		int numberOfRegions;
		vector<int>regionIndexes = { 0, 0, 0 };
		vector<int> gain = { 0, 0, 0 };
		vector<int> offset = { 0, 0, 0 };
	};
	virtual ITxCalibration::S2dResultsParams getTxS2dParams() = 0;
};

