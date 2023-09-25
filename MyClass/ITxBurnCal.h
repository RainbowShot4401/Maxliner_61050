#pragma once
#include "ITxCalibration.h"

#define MAX_BURN_STRUCT_SIZE 128

#if 1
class ITxBurnCal
{
public:
	ITxBurnCal();
	virtual ~ITxBurnCal();

	enum Status { FAIL, PASS };

	struct FillTxStructParams
	{
		ITxCalibration::TxSetChannelParams txSetChannelParams;
		ITxCalibration::OpenLoopSweepResultsParams OpenLoopSweepResultsParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::GainTableResultsParams txGainTableParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::S2dResultsParams txS2dParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::MaxPowerParams maxPowerParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::UltimateEvmParams ultimateEvmParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::ABCoefficientsParams aBCoefficientsParams[MAX_ANTENNA_NUMBER];

	};
	virtual Status fillTxStruct(const FillTxStructParams& fillTxStructParams) = 0;
	virtual Status clearTxStruct() = 0;
	//virtual Status writeTssiCalData() = 0;
	//virtual Status writeToNvMemory() = 0;
	//virtual void printResults() = 0;
	//virtual Status WriteProdFlag(bool flag) = 0;
	virtual int getNumberOfChannels() = 0;
	virtual float*** getABError() = 0;
private:
	virtual Status convertToDutStruct() = 0;
};
#endif