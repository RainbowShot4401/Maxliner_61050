#include "ITxCalibration.h"

class TxS2dCal
{
public:
	TxS2dCal(CTxControl* dutFunc, dutWrapper* dutApi);
	~TxS2dCal();

	bool s2dDone[2][MAX_ANTENNA_NUMBER];
	int offsetStatic[MAX_ANTENNA_NUMBER][DUT_NUM_REGION];
	int gainStatic[MAX_ANTENNA_NUMBER][DUT_NUM_REGION];
	funcGetMessage m_funcGetMessage;

	ITxCalibration::Status Run(const ITxCalibration::TxS2dParams &txS2dParams);
	ITxCalibration::S2dResultsParams TxS2dCal::getTxS2dParams();

private:
	CTxControl* _dutFunc;
	dutWrapper* _dutApiPonter;
	ITxCalibration::TxS2dParams _txS2dParams;
	ITxCalibration::S2dResultsParams _s2dResultsParams;
};