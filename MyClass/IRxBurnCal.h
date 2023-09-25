#pragma once

//#include "DUT_IQlite_HELPER.h"
#include "IRxCalibration.h"

#define MAX_BURN_STRUCT_SIZE 128

class IRxBurnCal
{
public:
	IRxBurnCal();
	virtual ~IRxBurnCal();


	enum Status { FAIL, PASS };

	struct FillRxStructParams
	{
		RxDutApi::RxSetChannelParams rxSetChannelParams;
		IRxCalibration::MidGainResultsParams midGainResultsParams;
		IRxCalibration::SubBandResultsParams subBandResultsParams;
		IRxCalibration::GainStepResultsParams gainStepResultsParams;
		IRxCalibration::FlatnessResultsParams flatnessResultsParams;
		IRxCalibration::S2dResultsParams s2dResultsParams;
		IRxCalibration::AbResultsParams abResultsParams;
		/*
		ITxCalibration::OpenLoopSweepResultsParams OpenLoopSweepResultsParams[MAX_ANTENNA_NUMBER];
		ITxCalibration::GainTableResultsParams txGainTableParams[MAX_ANTENNA_NUMBER];
		*/
	};
	virtual Status fillRxStruct(const FillRxStructParams &fillTxStructParams) = 0;
	virtual Status clearRxStruct() = 0;
	virtual Status writeRssiCalData() = 0;
	//virtual Status WriteProdFlag(bool flag) = 0;
	virtual int  getNumberOfChannels() = 0;
	//virtual Status writeToNvMemory() = 0;

	//virtual float*** getABError() = 0;
	//virtual void printResults() = 0;

private:
	virtual Status convertToDutStruct() = 0;
};

