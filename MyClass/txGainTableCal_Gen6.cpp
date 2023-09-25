#include "..\stdafx.h"
#include "TxGainTableCal_Gen6.h"


TxGainTableCal_Gen6::TxGainTableCal_Gen6(CTxControl* dutFunc, dutWrapper * dutApi, funcGetTxPower m_funcGetTxPower)
{
	_dutFunc = dutFunc;
	_dutApiPonter = dutApi;
	_m_funcGetTxPower = m_funcGetTxPower;
	sleep = 1000;
}

TxGainTableCal_Gen6::~TxGainTableCal_Gen6()
{
}

ITxCalibration::Status TxGainTableCal_Gen6::txGainTableCal(const ITxCalibration::TxGainTableParams &txGainTableParams)
{
	char szReturnMessage[256];
	char tmpMessage[512];

	_txGainTableParams = txGainTableParams;
	//TxDutApi::ShiftPowerOutTableParames _initShiftPowerOutTableParames = txGainTableParams.shiftPowerOutTableParames;
	//_initShiftPowerOutTableParames.pout_table_gain_or_offset = 0;
	//_initShiftPowerOutTableParames.pout_table_index = 0;

	//dutError = (CTxControl::Status)_dutFunc->ShiftPowerOutVector(_txGainTableParams.shiftPowerOutTableParames);
	//	if (CTxControl::Status::PASS != dutError)
	//		return ITxCalibration::Status::FAIL;

	dutError = (CTxControl::Status)_dutFunc->SetPoutTableOffset(_txGainTableParams.shiftPowerOutTableParames);
	if (CTxControl::Status::PASS != dutError)
		return ITxCalibration::Status::FAIL;

	_txGainTableParams.shiftPowerOutTableParames.pout_table_index = gainTableSelectTxIndex(_txGainTableParams.regions, _txGainTableParams.defaultCalibrationIndex);

	dutError = _dutFunc->SetPowerLimit(_txGainTableParams.shiftPowerOutTableParames.pout_table_index, CTxControl::OpenClose::OPEN_LOOP);
	if (CTxControl::Status::PASS != dutError)
		return ITxCalibration::Status::FAIL;

	dutError = (CTxControl::Status)_dutFunc->GetPoutTableOffset(_txGainTableParams.shiftPowerOutTableParames);
	if (CTxControl::Status::PASS != dutError)
		return ITxCalibration::Status::FAIL;
	int initPoutOffset = _txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset;
	//int initPoutOffset = Dut_ReadPoutTableOffset(_txGainTableParams.shiftPowerOutTableParames.antNumber, _txGainTableParams.shiftPowerOutTableParames.BW);
	//to do read init poutOffset value in index: txGainTableParams.shiftPowerOutTableParames.pout_table_index

	//_txGainTableParams.setVsaParams.rfAmplDb += (_txGainTableParams.shiftPowerOutTableParames.pout_table_index / 2);
	//IVSA *_vsa = new VSA(_txGainTableParams.setVsaAmpTolParams, _txGainTableParams.setVsaParams, _txGainTableParams.captureVsaParams, _txGainTableParams.analyzeParams);

	//vsaError = (IVSA::Status)_vsa->vsaInitialize();
	//if (IVSA::Status::PASS != vsaError)
	//	return ITxCalibration::Status::FAIL;

	for (int i = 0; i < GAIN_TABLE_CALC_ITERATIONS; i++)
	{
		//vsaError = (IVSA::Status)_vsa->vsaCapture();
		//if (IVSA::Status::PASS != vsaError)
		//	return ITxCalibration::Status::FAIL;

		//vsaError = (IVSA::Status)_vsa->vsaAnalyzeOnlyPower();
		//if (IVSA::Status::PASS != vsaError)
		//	return ITxCalibration::Status::FAIL;

		//double measPower = _vsa->vsaReturnPower() + _txGainTableParams.cableLoss;
		//BOOL(CALLBACK* funcGetTxPower)(BOOL bRetry, int iChannelIdx, double dbExpectedPower, LPSTR szBand, LPSTR szAntenna, LPSTR szDataRate, LPSTR szBandWidth, double& dbActualPower, LPSTR lpszRunInfo);
		double measPower = -999;
		int retry_count = 3;
		for (int i_try = 0; i_try < retry_count; i_try++)
		{
			if (_m_funcGetTxPower(3, m_DUT_PARAM.CHANNEL, m_DUT_PARAM.TX_POWER_TARGET, m_DUT_PARAM.BAND, m_DUT_PARAM.TX_CHAIN, m_DUT_PARAM.RATE, m_DUT_PARAM.WIDE_BAND, measPower, szReturnMessage))
				break;
			else
			{
				if ((i_try + 1) == retry_count)
				{
					//_logPrint->print(string("SetPowerLimit"));
					sprintf(tmpMessage, "txGainTableCal => _m_funcGetTxPower Fail.");
					m_funcGetMessage(tmpMessage);
					return ITxCalibration::Status::FAIL;
				}
			}
		}
		if (measPower == -999)
			continue;
		_txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset = calcNewGainTableValue(_txGainTableParams.shiftPowerOutTableParames.pout_table_index, measPower, initPoutOffset);
		initPoutOffset = _txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset;

		//dutError = (TxDutApi::Status)Dut_WritePoutTableOffset(_txGainTableParams.shiftPowerOutTableParames.antNumber, _txGainTableParams.shiftPowerOutTableParames.BW, _txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset);
		dutError = (CTxControl::Status)_dutFunc->SetPoutTableOffset(_txGainTableParams.shiftPowerOutTableParames);
		if (CTxControl::Status::PASS != dutError)
			return ITxCalibration::Status::FAIL;
		Sleep(sleep);
	}
	//delete _vsa;
	return ITxCalibration::Status::PASS;
}

int TxGainTableCal_Gen6::gainTableSelectTxIndex(int regions[DUT_NUM_REGION], const int &defaultCalibrationIndex)
{
	if ((regions[0] == regions[1]) && (regions[1] == regions[2]))
	{
		return defaultCalibrationIndex;
	}
	else
	{
		return regions[1];
	}
}



int TxGainTableCal_Gen6::Dut_ReadPoutTableOffset(int ant, int bw_id)
{
	DWORD reg;
	enum BW {
		bw_20MHz, bw_40MHz, bw_80MHz, bw_160MHz_or_11b
	} bw;

	bw = (BW)bw_id;

	_dutApiPonter->Dut_ReadReg(1, _baseAddres9x[ant], &reg, 0xFFFFFFFF);
	switch (bw)
	{
	case bw_20MHz:
		return static_cast<int>(reg & 0x7F);
		break;
	case bw_40MHz:
		return static_cast<int>((reg & 0x7F00) >> 8);
		break;
	case bw_80MHz:
		return static_cast<int>((reg & 0x7F0000) >> 16);
		break;
	case bw_160MHz_or_11b:
		return static_cast<int>((reg & 0x7F000000) >> 24);
		break;
	}
}


bool TxGainTableCal_Gen6::Dut_WritePoutTableOffset(int ant, int bw_id, DWORD reg_value)
{

	DWORD reg_to_read, reg_to_write;
	enum BW {
		bw_20MHz, bw_40MHz, bw_80MHz, bw_160MHz_or_11b
	} bw;

	bw = (BW)bw_id;

	_dutApiPonter->Dut_ReadReg(1, _baseAddres9x[ant], &reg_to_read, 0xFFFFFFFF);
	switch (bw)
	{
	case bw_20MHz:
		reg_to_write = ((reg_to_read & 0xFFFFFF80) | (reg_value & 0x7F));
		break;
	case bw_40MHz:
		reg_to_write = ((reg_to_read & 0xFFFF80FF) | ((reg_value & 0x7F) << 8));
		break;
	case bw_80MHz:
		reg_to_write = ((reg_to_read & 0xFF80FFFF) | ((reg_value & 0x7F) << 16));
		break;
	case bw_160MHz_or_11b:
		reg_to_write = ((reg_to_read & 0x80FFFFFF) | ((reg_value & 0x7F) << 24));
		break;
	}

	_dutApiPonter->Dut_WriteReg(1, _baseAddres9x[ant], reg_to_write, 0xFFFFFFFF);
	reg_to_read = Dut_ReadPoutTableOffset(ant, bw_id);

	if (reg_to_read == reg_value)
		return true;
	else
		return false;
}

int TxGainTableCal_Gen6::calcNewGainTableValue(const int &targetIndex, const double &measPower, const int &initPoutOffset)
{
	int retValue = initPoutOffset + targetIndex - round(measPower * 2);
	return (retValue < 0) ? 0 : retValue;
}


ITxCalibration::TxGainTableParams TxGainTableCal_Gen6::getGainTableResultParams()
{
	return _txGainTableParams;
}

ITxCalibration::GainTableResultsParams TxGainTableCal_Gen6::getTxGainTableParams()
{
	return _gainTableResultsParams;
}

ITxCalibration::Status TxGainTableCal_Gen6::setTxGainTableParams()
{
	char tmpMessage[512];

	if (debug == "TRUE")
		sprintf(tmpMessage, "txGainTableCal => Antenna %d:\n[DEBUG_CAL]=> GAIN TABLE CALIBRATION RESULT: INDEX:%d GAIN:%d", _txGainTableParams.shiftPowerOutTableParames.antNumber, _txGainTableParams.shiftPowerOutTableParames.pout_table_index, _txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset);
	_gainTableResultsParams.calIndex = _txGainTableParams.shiftPowerOutTableParames.pout_table_index;
	_gainTableResultsParams.offsetGain = _txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset;
	sprintf(tmpMessage, "[CAL]=>  Freq: %d Antenna %d:",m_DUT_PARAM.CHANNEL, (_txGainTableParams.shiftPowerOutTableParames.antNumber)+1, _gainTableResultsParams.calIndex, _gainTableResultsParams.offsetGain);
	m_funcGetMessage(tmpMessage);
	ExportTestLog(tmpMessage);
	sprintf(tmpMessage, "[DEBUG_CAL]=> GAIN TABLE CALIBRATION RESULT: INDEX:%d GAIN:%d", _gainTableResultsParams.calIndex, _gainTableResultsParams.offsetGain);
	m_funcGetMessage(tmpMessage);
	ExportTestLog(tmpMessage);

	return ITxCalibration::Status::PASS;
}

