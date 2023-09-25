#include "ITxCalibration.h"

class OpenLoopSweep
{
public:
	OpenLoopSweep(CTxControl* dutFunc, dutWrapper* dutApi, funcGetTxPower m_funcGetTxPower, funcGetTxPowerEvm m_funcGetTxPowerEvm);
	~OpenLoopSweep();

public:
	DUT_PARAM m_DUT_PARAM;
	funcGetMessage m_funcGetMessage;
	ITxCalibration::OpenLoopSweepResultsParams getOpenLoopSweepResults();
	int measSaturIndex(const ITxCalibration::TxOpenLoopSweepParams& txOpenLoopSweepParams);
	double CalcAverageOfVectorLog(const vector<double>& v);
	ITxCalibration::Status setConstMaxPower(const double& constPower);
	ITxCalibration::Status setConstUltimateEvm(const double& constPower);
	ITxCalibration::Status txOpenLoopSweep(const ITxCalibration::TxOpenLoopSweepParams& txOpenLoopSweepParams);
	ITxCalibration::MaxPowerParams getMaxPower();
	ITxCalibration::UltimateEvmParams getUltimateEvm();
	BOOL MEASURE_EVM;
	BOOL debug;

private:
	CTxControl* _dutFunc;
	dutWrapper* _dutApiPonter;
	funcGetTxPower _m_funcGetTxPower;
	funcGetTxPowerEvm _m_funcGetTxPowerEvm;
	//store inputs:
	ITxCalibration::TxOpenLoopSweepParams _txOpenLoopSweepParams;
	//params for output:
	ITxCalibration::OpenLoopSweepResultsParams _OpenLoopSweepResultsParams;
	ITxCalibration::MaxPowerParams _maxPowerParams;
	ITxCalibration::UltimateEvmParams _ultimateEvmParams;
};