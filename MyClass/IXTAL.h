#pragma once
#include "dutWrapper.h"
#include <string>
//#include "DUT_IQlite_HELPER.h"

using namespace std;

class IXTAL
{
public:
	IXTAL();
	enum Status { FAIL, PASS };

	virtual ~IXTAL();

	struct XTAL_PARAMS {
		double centralFreqMHz = 5180;
		double cableLoss = 20;
		int powerIndex = 40;
		string standard = "802.11n";
		string bandwidth = "BW - 40";
		string rate = "MCS0";
		int num_of_streams = 1;
		int ant = 1;
	};

	virtual Status startTx() = 0;
	virtual Status stopTx() = 0;
	//virtual Status measFreqError(double &outFreqError) = 0;
	virtual Status setRate() = 0;
	virtual Status setGain() = 0;
	virtual Status writeXtalValue(int xtal_data, int xtal_bias) = 0;
	virtual Status writeXtalValueToNvMemorry(/*int nv_memmory, */int xtal_data, int xtal_bias) = 0;

protected:
	XTAL_PARAMS _xtal_params;

};

