#pragma once
#include "IRxBurnCal.h"
class RxBurnCalGen6 :
	public IRxBurnCal
{
public:
	RxBurnCalGen6(RxDutApi *dutFunc, dutWrapper * dutApi);
	~RxBurnCalGen6();

	vector<IRxBurnCal::FillRxStructParams> RxBurnCalGen6::calDataStructVer6;
	Dut_RssiCalDataStructVer6 RxBurnCalGen6::dutCalDataStructVer6[MAX_BURN_STRUCT_SIZE] = { 0 };
	float RxBurnCalGen6::ABErrors[MAX_BURN_STRUCT_SIZE][MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER] = { 0 };


//	IRxBurnCal::Status WriteProdFlag(bool flag);
	IRxBurnCal::Status writeRssiCalData();
	IRxBurnCal::Status fillRxStruct(const IRxBurnCal::FillRxStructParams &fillRxStructParams);
	IRxBurnCal::Status clearRxStruct();
	int getNumberOfChannels();
	void dutFuncSet(RxDutApi* var);

private:
	//ILogPrint *_logPrint;
	RxDutApi *_dutFunc;
	dutWrapper * _dutApi;
	Dut_RssiCalDataStructVer6 _dutCalDataStructVer6[MAX_BURN_STRUCT_SIZE] = {};
	Status convertToDutStruct();
};

