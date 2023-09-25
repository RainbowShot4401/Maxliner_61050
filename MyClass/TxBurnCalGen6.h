#pragma once

#include "ITxBurnCal.h"

#if 1
class TxBurnCalGen6 : public ITxBurnCal
{
public:
	TxBurnCalGen6(CTxControl* dutFunc, dutWrapper* dutApi);
	~TxBurnCalGen6();

	void dutFuncSet(CTxControl* var);
	vector<ITxBurnCal::FillTxStructParams> calDataStructVer6;
	Dut_CalDataStructVer6 dutCalDataStructVer6[MAX_BURN_STRUCT_SIZE];

	float ABErrors[MAX_BURN_STRUCT_SIZE][MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER];
	vector<Dut_XY> calcPointsForRegion(const vector<int>& startStopRegionIndex, const ITxCalibration::OpenLoopSweepResultsParams& OpenLoopSweepResultsParams);
	int getNumberOfChannels();
	float*** getABError();
	void printResults();

	ITxBurnCal::Status fillTxStruct(const ITxBurnCal::FillTxStructParams& fillTxStructParams);
	ITxBurnCal::Status clearTxStruct();
	ITxBurnCal::Status writeTssiCalData();
	ITxBurnCal::Status writeToNvMemory();
	ITxBurnCal::Status WriteProdFlag(bool flag);

private:
	CTxControl* _dutFunc;
	dutWrapper* _dutApi;
	Dut_CalDataStructVer6 _dutCalDataStructVer6[MAX_BURN_STRUCT_SIZE] = {};
	Status convertToDutStruct();
};

#endif