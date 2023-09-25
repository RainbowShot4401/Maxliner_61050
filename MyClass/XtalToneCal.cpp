#include "..\stdafx.h"
#include "XtalToneCal.h"

IXTAL::IXTAL()
{
}

IXTAL::~IXTAL()
{
}

XtalToneCal::XtalToneCal(dutWrapper* dutApi, XTAL_PARAMS xtal_params)
{
	_xtal_params = xtal_params;
	_dutApi = dutApi;
	//_vsa = vsa;
	//_vsa->vsaInitialize();
}

XtalToneCal::~XtalToneCal()
{
}


IXTAL::Status XtalToneCal::startTx()
{
	return (IXTAL::Status)(_dutApi->Dut_TransmitCW(_xtal_params.powerIndex, 0));
}

IXTAL::Status XtalToneCal::stopTx()
{
	return (IXTAL::Status)(_dutApi->Dut_TransmitCW(255, 0));
}

/*IXTAL::Status XtalToneCal::measFreqError(double& outFreqError)
{
	if (IVSA::Status::PASS != (IVSA::Status)_vsa->vsaCapture())
		return IXTAL::Status::FAIL;
	if (IVSA::Status::PASS != (IVSA::Status)_vsa->vsaAnalyze())
		return IXTAL::Status::FAIL;
	outFreqError = _vsa->vsaReturnCwFreqErrorPpm();
	return IXTAL::Status::PASS;
}*/

IXTAL::Status XtalToneCal::setRate()
{
	return IXTAL::Status::PASS;
}

IXTAL::Status XtalToneCal::setGain()
{
	return IXTAL::Status::PASS;
}

IXTAL::Status XtalToneCal::writeXtalValue(int xtal_data, int xtal_bias)
{
	int err;
	err = stopTx();
	err = _dutApi->Dut_WriteXtal_full(xtal_data + (xtal_bias << 9));
	if (debug == TRUE)
		ExportTestLog("API Flow: Dut_WriteXtal_full => xtal_data: %d", xtal_data);
	return (IXTAL::Status)startTx();

}

IXTAL::Status XtalToneCal::writeXtalValueToNvMemorry(int xtal_data, int xtal_bias)
{
	IXTAL::Status retVal = (IXTAL::Status)_dutApi->Dut_WriteXtalCalDataToEEPROM_full(xtal_data + (xtal_bias << 9));
	if (debug == TRUE)
		ExportTestLog("API Flow: writeXtalValueToNvMemorry => xtal_data: %d", xtal_data);

	//if (nv_memmory > 0)
	//{
	//	if (IVSA::Status::PASS != (IXTAL::Status)_dutApi->Dut_SetNvMemoryType(nv_memmory));
	//	return IXTAL::Status::FAIL;
	//}
	return retVal;
}
