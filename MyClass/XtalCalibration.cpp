#include "..\stdafx.h"
#include "XtalCalibration.h"


CXtalCalibration::CXtalCalibration(dutWrapper* dutApi, IXTAL::XTAL_PARAMS xtal_params) : XtalToneCal(dutApi, xtal_params), CTxControl(dutApi)
{
	//_dutApi = dutApi;
	//_xtal_params = xtal_params;
	//_vsa = vsa;
	phyType = 0;
	band = 0;
	bandwidth = 0;
	channel = 0;
	TxAnt = 0;
	calibrationMask = 65535;
	XTAL_DATA = 0.0;
	XTAL_BIAS = 0;
	targetPPM = 0;
	PPM_tolerance = 0;
	XTAL_MIN_LIMIT = 50;
	XTAL_MAX_LIMIT = 200;
	antMask = 15;
}

CXtalCalibration::~CXtalCalibration()
{
}

double  CXtalCalibration::LinearRegression_Index(int dec_arr[], vector<double> freq_eror, int targetPPM, int tolerance)
{
	CLinearRegression m_CLinearRegression;
	vector<LinearRegressionPoint> points;
	vector<PREDICTPPM> PredictPPM;
	PREDICTPPM m_PREDICTPPM;
	double index;

	points.push_back(LinearRegressionPoint(dec_arr[0], freq_eror.at(0)));
	points.push_back(LinearRegressionPoint(dec_arr[1], freq_eror.at(1)));
	m_CLinearRegression.Init(points);

	for (int i = dec_arr[0]; i < dec_arr[1]; i++)
	{
		m_PREDICTPPM.PPM = m_CLinearRegression.Run(i);
		m_PREDICTPPM.xtal_dec_val = i;
		PredictPPM.push_back(m_PREDICTPPM);
	}
	m_CLinearRegression.Run(targetPPM);
	index = m_CLinearRegression.sequentialSearch(PredictPPM, targetPPM, tolerance);
	return PredictPPM.at(index).xtal_dec_val;
}

TM_RETURN CXtalCalibration::xtal_dec_val_get(funcGetPPM cbGetPPM, DUT_PARAM* DutParam, int targetPPM, int tolerance, char* Msg)
{
	int temp_dec_arr[2] = { XTAL_MIN_LIMIT, XTAL_MAX_LIMIT };
	vector<double> xtal_dec_val(temp_dec_arr, temp_dec_arr + sizeof(temp_dec_arr) / sizeof(temp_dec_arr[0]));
	vector<double> xtal_freq_eror;
	double freq_error_ppm;

	for (int i = 0; i < (sizeof(temp_dec_arr) / sizeof(temp_dec_arr[0])); i++)
	{
		int msSleep = 1000;
		BOOL bRtn;
		double FreqOffsetHz=0, dbPPM;
		char szMessage[1024];
		//int bRetry = 3;
		BOOL bRetry = FALSE;

		IXTAL::Status stat = writeXtalValue(xtal_dec_val.at(i), XTAL_BIAS);
		if (IXTAL::Status::PASS != stat)
			ExportTestLog("xtal_dec_val_get => [WiFi] vDUT_Run(WRITE_XTAL) return error.\n");
		else
		{
			if (debug == TRUE)
				ExportTestLog("xtal_dec_val_get => [WiFi] vDUT_Run(WRITE_XTAL) return OK.");
		}
		//Sleep(msSleep);
		for (int nMeasRetry = 0; nMeasRetry <= 3; nMeasRetry++)
		{
			bRtn = cbGetPPM(bRetry, DutParam->CHANNEL, DutParam->TX_POWER_TARGET, DutParam->BAND, DutParam->TX_CHAIN, DutParam->RATE, DutParam->WIDE_BAND, FreqOffsetHz, freq_error_ppm, szMessage);
			if (!bRtn)
			{
				if (!stopTx()) { sprintf(Msg, "xtal_dec_val_get => stopTx() error"); ExportTestLog(Msg);}
				if (!startTx()) { sprintf(Msg, "xtal_dec_val_get => startTx() error");  ExportTestLog(Msg); }
				//Sleep(msSleep);
			}
			else
				break;
		}
		if (debug == TRUE)
			ExportTestLog("xtal_dec_val_get=> XTAL VALUE: %0.0f[dec]\tFREQ ERRO: %0.3f[ppm]", xtal_dec_val.at(i), freq_error_ppm);
		if (!bRtn)
			return TM_RETURN_ERROR;
		else
			xtal_freq_eror.push_back(freq_error_ppm);

	/*	i = 1;
		 stat = writeXtalValue(xtal_dec_val.at(i), XTAL_BIAS);
		if (IXTAL::Status::PASS != stat)
		{
			ExportTestLog("[WiFi] vDUT_Run(WRITE_XTAL) return error.\n");
		}
		else
		{
			ExportTestLog("[WiFi] vDUT_Run(WRITE_XTAL) return OK.");
		}
		Sleep(msSleep);
		for (int nMeasRetry = 0; nMeasRetry <= 3; nMeasRetry++)
		{
			bRtn = cbGetPPM(bRetry, DutParam->CHANNEL, DutParam->TX_POWER_TARGET, DutParam->BAND, DutParam->TX_CHAIN, DutParam->RATE, DutParam->WIDE_BAND, FreqOffsetHz, freq_error_ppm, szMessage);
			if (!bRtn)
			{
				if (!stopTx()) { sprintf(Msg, "xtal_dec_val_get => stopTx() error"); ExportTestLog(Msg); }
				if (!startTx()) { sprintf(Msg, "xtal_dec_val_get => startTx() error");  ExportTestLog(Msg); }
				Sleep(msSleep);
			}
			else
				break;
		}

			*/
	}
	XTAL_DATA = LinearRegression_Index(temp_dec_arr, xtal_freq_eror, targetPPM, tolerance);
	if (XTAL_DATA != -1)
		return TM_RETURN_PASS;
	else 
		return TM_RETURN_ERROR;
}

TM_RETURN CXtalCalibration::Xtal_Verify(funcGetPPM cbGetPPM, DUT_PARAM* DutParam, int targetPPM, int tolerance, char* Msg)
{
	int msSleep = 1000;
	BOOL bRtn;
	double FreqOffsetHz = 0, dbPPM;
	char szMessage[1024];
	BOOL bRetry = FALSE;
	double freq_error_ppm;

	IXTAL::Status stat = writeXtalValue(XTAL_DATA, XTAL_BIAS);
	if (IXTAL::Status::PASS != stat)
		ExportTestLog("Xtal_Verify => [WiFi] vDUT_Run(WRITE_XTAL) return error.\n");
	else
	{
		if(debug == TRUE)
			ExportTestLog("Xtal_Verify => [WiFi] vDUT_Run(WRITE_XTAL) return OK.");
	}
	//Sleep(msSleep);
	for (int nMeasRetry = 0; nMeasRetry < 3; nMeasRetry++)
	{
		bRtn = cbGetPPM(bRetry, DutParam->CHANNEL, DutParam->TX_POWER_TARGET, DutParam->BAND, DutParam->TX_CHAIN, DutParam->RATE, DutParam->WIDE_BAND, FreqOffsetHz, freq_error_ppm, szMessage);
		if (!bRtn)
		{
			if (!stopTx()) { sprintf(Msg, "Xtal_Verify => stopTx() error"); ExportTestLog(Msg); }
			if (!startTx()) { sprintf(Msg, "Xtal_Verify => startTx() error");  ExportTestLog(Msg); }
			//Sleep(msSleep);
			if (nMeasRetry == (nMeasRetry - 1))
				break;
			else
				bRetry = TRUE;
		}
		else
			break;
	}
	if (!bRtn)
	{
		if (!stopTx()) { sprintf(Msg, "Xtal_Verify =>cbGetPPM stopTx() error"); ExportTestLog(Msg); }
		ExportTestLog("Xtal_Verify => cbGetPPM error");
		return TM_RETURN_ERROR;
	}
	char tmpMessage[256];
	sprintf(tmpMessage, "[DEBUG_CAL]=> XTAL VALUE : % 0.0f[dec]\tFREQ ERRO : % 0.3f[ppm]",XTAL_DATA, freq_error_ppm);
	ExportTestLog(tmpMessage);
	m_cbListMessage(tmpMessage);
	PPM_DATA = freq_error_ppm;

	if ((freq_error_ppm > (targetPPM - tolerance)) && (freq_error_ppm < (targetPPM + tolerance)))
		return TM_RETURN_PASS;
	else
		return TM_RETURN_FAIL;
}

TM_RETURN CXtalCalibration::Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT& ErrorReport, funcGetPPM cbGetPPM, DUT_PARAM *DutParam)
{
	IXTAL::Status xtal_error;
	char returnMessage[512] = {'\0'};
	m_cbListMessage = cbListMessage;
	XtalToneCal::debug = debug;
	
	if (Dut_SetTxAntennasMask(antMask) != MT_RET_OK)
	{
		cbListMessage("Dut_SetTxAntennasMask Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetRxAntennasMask(antMask) != MT_RET_OK)
	{
		cbListMessage("Dut_SetRxAntennasMask Fail.");
		return TM_RETURN_ERROR;
	}
	
	if (Dut_SetTxAntenna(antMask) != MT_RET_OK)
	{
		cbListMessage("Dut_SetTxAntenna Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetRxAntenna(antMask) != MT_RET_OK)
	{
		cbListMessage("Dut_SetRxAntenna Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetCloseLoop(OPEN_LOOP) != MT_RET_OK)
	{
		cbListMessage("Dut_SetCloseLoop Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetHdkConfigure(65535, 0) != MT_RET_OK)
	{
		cbListMessage("Dut_SetHdkConfigure Fail.");
		return TM_RETURN_ERROR;
	}
	
	if (Dut_SetChannel(phyType, band, bandwidth, channel, calibrationMask, OPEN_LOOP) != MT_RET_OK)
	{
		cbListMessage("Dut_SetChannel Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetTxAntenna(TxAnt) != MT_RET_OK)
	{
		cbListMessage("Dut_SetTxAntenna Fail.");
		return TM_RETURN_ERROR;
	}
	if (Dut_SetRxAntenna(0) != MT_RET_OK)
	{
		cbListMessage("Dut_SetRxAntenna Fail.");
		return TM_RETURN_ERROR;
	}

	xtal_error = setRate();
	if (xtal_error != IXTAL::Status::PASS)
		throw "setGain Fail";
	xtal_error = setGain();
	if (xtal_error != IXTAL::Status::PASS)
		throw "setGain Fail";
	//xtal_error = startTx();
	//f (xtal_error != IXTAL::Status::PASS)
		//throw "startTx Fail";
	TM_RETURN testFlag = TM_RETURN_FAIL;
	int XTAL_MAX_ITRATION = 0;
	do
	{
		if (xtal_dec_val_get(cbGetPPM, DutParam, targetPPM, PPM_tolerance, returnMessage) == TM_RETURN_ERROR)
			return TM_RETURN_ERROR;

		testFlag = Xtal_Verify(cbGetPPM, DutParam, targetPPM, PPM_tolerance, returnMessage);
		if (testFlag == TM_RETURN_ERROR)
			return TM_RETURN_ERROR;
		else if (testFlag == TM_RETURN_FAIL)
		{
			if (PPM_DATA < targetPPM)
				XTAL_MAX_LIMIT = XTAL_DATA;
			else if (PPM_DATA > targetPPM)
				XTAL_MIN_LIMIT = XTAL_DATA;
		}
		else if (testFlag == TM_RETURN_PASS)
			break;
		XTAL_MAX_ITRATION++;
	} while ((testFlag == TM_RETURN_FAIL) && (XTAL_MAX_ITRATION < 5));

	if (!stopTx()) { ExportTestLog("Run => stopTx() error"); }
	if (testFlag == TM_RETURN_FAIL)
	{
		ExportTestLog("Run => Xtal_Verify return Fail.\n");
		return TM_RETURN_FAIL;
	}

	IXTAL::Status stat = writeXtalValueToNvMemorry(XTAL_DATA, XTAL_BIAS);
	if (IXTAL::Status::PASS != stat)
	{
		ExportTestLog("Run => writeXtalValueToEEPROM return error.\n");
		return TM_RETURN_ERROR;
	}
	else
	{
		if(debug == TRUE)
			ExportTestLog("Run => writeXtalValueToEEPROM return OK.");
	}
	return testFlag;

#if 0
	for (int i = 0; i < XTAL_MAX_ITRATION; i++)
	{
		IXTAL::Status stat = writeXtalValue(xtal_dec_val.at(i), XTAL_BIAS);
		if (IXTAL::Status::PASS != stat)
		{
			ExportTestLog("[WiFi] vDUT_Run(WRITE_XTAL) return error.\n");
		}
		else
		{
			ExportTestLog("[WiFi] vDUT_Run(WRITE_XTAL) return OK.");
		}

		int msSleep = 1000;
		BOOL bRtn;
		double FreqOffsetHz, dbPPM;
		char szMessage[1024];
		int bRetry = 3;

		for (int nMeasRetry = 0; nMeasRetry <= 3; nMeasRetry++)
		{
			bRtn = cbGetPPM(bRetry, DutParam->CHANNEL, DutParam->TX_POWER_TARGET, DutParam->BAND, DutParam->TX_CHAIN, DutParam->RATE, DutParam->WIDE_BAND, FreqOffsetHz, freq_error_ppm, szMessage);
			if (!bRtn)
			{	
				if (!stopTx())	goto ERRORED;
				if (!startTx()) goto ERRORED;
					Sleep(msSleep);
			}
		}
		ExportTestLog("XTAL VALUE: %0.0f[dec]\tFREQ ERRO: %0.3f[ppm]", xtal_dec_val.at(i), freq_error_ppm);
		xtal_freq_eror.push_back(freq_error_ppm);
		if (abs(freq_error_ppm) < XTAL_ACCURACY)
		{
			//FREQ_ERROR_AVG = freq_error_ppm;
			XTAL_DATA = xtal_dec_val.at(i);
			break;
		}

	/*	
		if (abs(l_11ACXtalCalibrationParam.MIN_XTAL_ACCURACY) < l_11ACXtalCalibrationParam.XTAL_ACCURACY)
		{
			if ((freq_error_ppm <= l_11ACXtalCalibrationParam.XTAL_ACCURACY) && (freq_error_ppm >= l_11ACXtalCalibrationParam.MIN_XTAL_ACCURACY))
			{
				l_XtalCalibrationReturn.FREQ_ERROR_AVG = freq_error_ppm;
				l_XtalCalibrationReturn.XTAL_DATA = xtal_dec_val.at(i);
				break;
			}
		}
		else if (abs(freq_error_ppm) < l_11ACXtalCalibrationParam.XTAL_ACCURACY)
		{
			l_XtalCalibrationReturn.FREQ_ERROR_AVG = freq_error_ppm;
			l_XtalCalibrationReturn.XTAL_DATA = xtal_dec_val.at(i);
			break;
		}
		 */
		/*if (i == (xtal_dec_val.size() - 1))
		{
			if (xtal_dec_val.size() == 2)
			{
				oCoeff = polyfit(xtal_freq_eror, xtal_dec_val, 1);
			}
			else if (xtal_dec_val.size() == 3)
			{
				oCoeff = polyfit(xtal_freq_eror, xtal_dec_val, 2);
			}
			else
			{
				oCoeff = polyfit(xtal_freq_eror, xtal_dec_val, 3);
			}
			new_dec_val = polyval(oCoeff, target_ppm);
			new_dec_val.at(0) = round(new_dec_val.at(0));
			if (new_dec_val.at(0) <= 0)  // need to be tested
			{
				if (std::find(xtal_dec_val.begin(), xtal_dec_val.end(), 0) != xtal_dec_val.end())  //check if zero was in previous interactions 0. if yes, stop the search
				{
					stopTx(); //stop TX before stopping the test
				//	LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] Can't calibrate XTAL, negative result.\n");
				//	throw logMessage;
				}
				else
				{
					new_dec_val.at(0) = 0;
				}
			}
			xtal_dec_val.insert(xtal_dec_val.end(), new_dec_val.begin(), new_dec_val.end());
		}*/

	/*	if (i == (l_11ACXtalCalibrationParam.XTAL_MAX_ITRATION - 1))
		{
			LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR,
				"[WiFi] Fail to find target PPM.\n");
			err = xtalCla_obj->stopTx();
			throw logMessage;
		} */
	}
#endif
}
