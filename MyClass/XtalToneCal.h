#pragma once
#include "IXTAL.h"
#include "..\CommFunc.h"
//#include "polyfit.hpp"
#include <string>
#include <map>

using namespace std;

class XtalToneCal : public IXTAL
{
public:
	//XtalToneCal(dutWrapper* dutApi, XTAL_PARAMS xtal_params, IVSA* _vsa);
	XtalToneCal(dutWrapper* dutApi, XTAL_PARAMS xtal_params);
	~XtalToneCal();
	BOOL debug;

	Status startTx();
	Status stopTx();
	//Status measFreqError(double& outFreqError);
	Status setRate();
	Status setGain();
	Status writeXtalValue(int xtal_data, int xtal_bias);
	Status writeXtalValueToNvMemorry(/*int nv_memmory, */int xtal_data, int xtal_bias);
	dutWrapper* _dutApi;
	XTAL_PARAMS _xtal_params;
	//IVSA* _vsa;
};

