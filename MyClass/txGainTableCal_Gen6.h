#pragma once
#include "ITxGainTableCal.h"

#define GAIN_TABLE_CALC_ITERATIONS 4

class TxGainTableCal_Gen6 :
	public ITxGainTableCal
{
public:
	TxGainTableCal_Gen6(CTxControl *dutFunc, dutWrapper * dutApi, funcGetTxPower m_funcGetTxPower);
	~TxGainTableCal_Gen6();

	DUT_PARAM m_DUT_PARAM;
	ITxCalibration::Status txGainTableCal(const ITxCalibration::TxGainTableParams &txGainTableParams);
	ITxCalibration::TxGainTableParams getGainTableResultParams();
	ITxCalibration::GainTableResultsParams getTxGainTableParams();
	ITxCalibration::Status setTxGainTableParams();
	int sleep;
	funcGetMessage m_funcGetMessage;
	string debug;

private:
	CTxControl*_dutFunc;
	dutWrapper * _dutApiPonter;
	funcGetTxPower _m_funcGetTxPower;

	//IVSA::Status vsaError = IVSA::Status::PASS;
	CTxControl::Status dutError = CTxControl::Status::PASS;

	ITxCalibration::TxGainTableParams _txGainTableParams;
	ITxCalibration::GainTableResultsParams _gainTableResultsParams;
	int gainTableSelectTxIndex(int regions[DUT_NUM_REGION], const int &defaultCalibrationIndex);
	int calcNewGainTableValue(const int &targetIndex, const double &measPower, const int &initGainValue);

	//temp func:
	int _baseAddres9x[4] = { 0x00BE00A4, 0x00BE40A4, 0x00BE80A4, 0x00BEC0A4 };
	int Dut_ReadPoutTableOffset(int ant, int bw_id);
	bool Dut_WritePoutTableOffset(int ant, int bw_id, DWORD reg_value);
};

