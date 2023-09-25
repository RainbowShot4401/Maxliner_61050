#pragma once
#include "XtalToneCal.h"
#include "TxControl.h"
#include "CLinearRegression.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

class CXtalCalibration : public XtalToneCal, CTxControl
{
public:
	CXtalCalibration(dutWrapper* dutApi, IXTAL::XTAL_PARAMS xtal_params);
	~CXtalCalibration();
	int phyType;
	int band;
	int bandwidth;
	int channel;
	int TxAnt;
	int calibrationMask = 65535;
	double XTAL_DATA;
	double PPM_DATA;
	int XTAL_BIAS;
	int targetPPM;
	int PPM_tolerance;
	int XTAL_MIN_LIMIT;
	int XTAL_MAX_LIMIT;
	int antMask;
	funcGetMessage m_cbListMessage;
	BOOL debug;

	CLinearRegression m_CLinearRegression;
	TM_RETURN Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT& ErrorReport, funcGetPPM cbGetPPM, DUT_PARAM *DutParam);
	double LinearRegression_Index(int dec_arr[], vector<double> freq_eror, int targetPPM, int tolerance);
	TM_RETURN xtal_dec_val_get(funcGetPPM cbGetPPM, DUT_PARAM* DutParam, int targetPPM, int tolerance, char* Msg);
	TM_RETURN Xtal_Verify(funcGetPPM cbGetPPM, DUT_PARAM* DutParam, int targetPPM, int tolerance, char* Msg);

//private:
	//dutWrapper* _dutApi;
	//IXTAL::XTAL_PARAMS _xtal_params;
};

