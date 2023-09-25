#pragma once
#include "IRxCalibration.h"
#include "..\CommFunc.h"

class RxCalibration :
	public IRxCalibration
{
public:

	//struct PathLossParams {
	//	signed int test_id = 10;
	//	double freqMHz = 2442;
	//	double outPathLossPerAnt[MAX_DATA_STREAM] = {};
	//	double outPathLossAverage = 0;
	//	int antennaMask = 15;
	//};
	funcReadCableLoss m_funcReadCableLoss;
	funcStartRxPower m_funcStartRxPower;
	funcStopRxPower m_funcStopRxPower;
	void debugFlag(string flag);
	void debugSubBandFlag(string flag);
	vector<string>MidGain_Cableloss;
	IRxCalibration::status calcPathLoass(IVSG::PathLossParams& pathLossParams);

	RxCalibration(dutWrapper * dutApi, IRxCalibration::GenType genType);
	~RxCalibration();

	IRxCalibration::status RxRfSubBandCal(RxRfSubBandParams rxRfSubBandParams);
	IRxCalibration::status RxRfMidGainCal(RxRfMidGainParams rxRfMidGainParams);
	IRxCalibration::status RxRfGainStepAccuracyCal(RxRfGainStepAccuracyParams rxRfGainStepAccuracyParams);
	IRxCalibration::status RxRfFlatnessCal(RxRfFlatnessParams rxRfFlatnessParams);
	IRxCalibration::status RxRssiS2dCal(RxRssiS2dParams rxRssiS2dCalParams);
	IRxCalibration::status RxRssiABCal(RxRssiABParams rxRssiABParams);
	IRxCalibration::status RxRssiS2dFlatnessCal(RxRssiS2dFlatnessParams rxRssiS2dFlatnessParams);

	IRxCalibration::MidGainResultsParams RxCalibration::getMidGainResults();
	IRxCalibration::SubBandResultsParams RxCalibration::getSubBandResults();
	IRxCalibration::GainStepResultsParams RxCalibration::getGainStepResults();
	IRxCalibration::FlatnessResultsParams getFlatnessResults();
	IRxCalibration::S2dResultsParams getS2dResults();
	IRxCalibration::AbResultsParams getAbResults();

	IRxCalibration::MidGainResultsParams RxCalibration::setMidGainResults(IRxCalibration::MidGainResultsParams midGainResultsParams);
	IRxCalibration::SubBandResultsParams RxCalibration::setSubBandResults(IRxCalibration::SubBandResultsParams subBandResultsParams);
	IRxCalibration::GainStepResultsParams RxCalibration::setGainStepResults(IRxCalibration::GainStepResultsParams gainStepResultsParams);
	IRxCalibration::FlatnessResultsParams RxCalibration::setFlatnessResults(IRxCalibration::FlatnessResultsParams flatnessResultsParams);
	IRxCalibration::S2dResultsParams RxCalibration::setS2dResults(IRxCalibration::S2dResultsParams s2dResultsParams);
	IRxCalibration::AbResultsParams RxCalibration::setAbResults(IRxCalibration::AbResultsParams abResultsParams);

private:
	char   printChar[MAX_BUFFER_SIZE] = "";
	IRxCalibration::status ret = IRxCalibration::status::PASS;
	dutWrapper * _dutApiPonter;
	RxDutApi* _dutFunc;
	IRxCalibration::GenType _genType;
	//ILogPrint *_logPrint;

	//store inputs:
	RxRfSubBandParams _rxRfSubBandParams;
	RxRfMidGainParams _rxRfMidGainParams;
	RxRfGainStepAccuracyParams _rxRfGainStepAccuracyParams;
	RxRfFlatnessParams _rxRfFlatnessParams;
	RxRssiS2dParams _rxRssiS2dCalParams;
	RxRssiABParams _rxRssiABParams;

	//params for output:
	MidGainResultsParams _midGainResultsParams;
	SubBandResultsParams _subBandResultsParams;
	GainStepResultsParams _gainStepResultsParams;
	FlatnessResultsParams _flatnessResultsParams;
	S2dResultsParams _s2dResultsParams;
	AbResultsParams _abResultsParams;
};

