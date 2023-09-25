#include "..\stdafx.h"
#include "txS2dCal.h"

TxS2dCal::TxS2dCal(CTxControl* dutFunc, dutWrapper* dutApi)
{
	_dutFunc      = dutFunc;
	_dutApiPonter = dutApi;
}

TxS2dCal::~TxS2dCal()
{
}

int CalcNumberOfRegions(const int regions[DUT_NUM_REGION])
{
	int numOfRegions = 1;
	for (int i_region = 0; i_region < (DUT_NUM_REGION - 1); i_region++)
	{
		if (regions[i_region] < regions[i_region + 1])
		{
			numOfRegions++;
		}
		else
			break;
	}
	return numOfRegions;
}

void getInITOffsetArray(signed char offsetArray[3])
{
	offsetArray[0] = 15;
	offsetArray[1] = 18;
	offsetArray[2] = 25;
}

ITxCalibration::Status TxS2dCal::Run(const ITxCalibration::TxS2dParams& txS2dParams)
{
	_txS2dParams = txS2dParams;
	int numberOfRegions = CalcNumberOfRegions(_txS2dParams.regions);
	_s2dResultsParams.numberOfRegions = numberOfRegions;
	copy(begin(_txS2dParams.regions), end(_txS2dParams.regions), begin(_s2dResultsParams.regionIndexes));
	s2dDone[_txS2dParams.band][_txS2dParams.antNum] = true;

	signed char s2dInitOffsets[3];
	getInITOffsetArray(s2dInitOffsets);

	int Power_vector[DUT_NUM_REGION][2] =
	{
		{ 0, _txS2dParams.regions[0] - 1 },
		{ _txS2dParams.regions[0], _txS2dParams.regions[1] - 1 },
		{ _txS2dParams.regions[1], _txS2dParams.regions[2] }
	};

	S2dSetInitialsParames s2dSetInitialsParames;
	s2dSetInitialsParames.tssiLow = _txS2dParams.tssiLow;
	s2dSetInitialsParames.tssiHigh = _txS2dParams.tssiHigh;

	SetS2dParamsParames setS2dParamsParames = { 0 };
	S2dCalibrateParames s2dCalibrateParames = { 0 };

	CTxControl::Status dutError;
	char tmpMessage[512];

	for (int i_reg = 0; i_reg < numberOfRegions; i_reg++)
	{
		memcpy_s(s2dSetInitialsParames.Power_vector, sizeof s2dSetInitialsParames.Power_vector, Power_vector[i_reg], sizeof Power_vector[i_reg]);
		s2dSetInitialsParames.S2D_offset = s2dInitOffsets[i_reg];

		dutError = _dutFunc->S2dSetInitials(s2dSetInitialsParames);
		if (CTxControl::Status::PASS != dutError)
		{
			sprintf(tmpMessage, "TxS2dCal => S2dSetInitials error.");
			m_funcGetMessage(tmpMessage);
			return ITxCalibration::Status::FAIL;
		}

		s2dCalibrateParames.pThreshold = _txS2dParams.regions[i_reg];
		s2dCalibrateParames.regionNumber = i_reg + 1;

		dutError = _dutFunc->S2dCalibrate(s2dCalibrateParames);
		if (CTxControl::Status::PASS != dutError)
		{
			sprintf(tmpMessage, "TxS2dCal => S2dCalibrate error.");
			m_funcGetMessage(tmpMessage);
			return ITxCalibration::Status::FAIL;
		}

		_txS2dParams.outS2dGain[i_reg] = (int)s2dCalibrateParames.outS2dGain;
		_txS2dParams.outS2dOffset[i_reg] = (int)s2dCalibrateParames.outS2dOffset;

		_s2dResultsParams.gain[i_reg] = 5;											// _txS2dParams.outS2dGain[i_reg]; AMIT
		_s2dResultsParams.offset[i_reg] = s2dSetInitialsParames.S2D_offset;			//_txS2dParams.outS2dOffset[i_reg]; AMIT

		offsetStatic[_txS2dParams.antNum][i_reg] = _s2dResultsParams.offset[i_reg];
		gainStatic[_txS2dParams.antNum][i_reg] = _s2dResultsParams.gain[i_reg];

		setS2dParamsParames.offset = offsetStatic[_txS2dParams.antNum][i_reg];
		setS2dParamsParames.gain = gainStatic[_txS2dParams.antNum][i_reg];

		setS2dParamsParames.antNumber = _txS2dParams.antNum;
		setS2dParamsParames.signalBw = _txS2dParams.bw;
		setS2dParamsParames.region = i_reg + 1;
		setS2dParamsParames.pThreshold = txS2dParams.regions[i_reg];
		setS2dParamsParames.gain = gainStatic[_txS2dParams.antNum][i_reg];
		setS2dParamsParames.offset = offsetStatic[_txS2dParams.antNum][i_reg];

		dutError = _dutFunc->SetS2dParams(setS2dParamsParames);
		if (CTxControl::Status::PASS != dutError)
		{
			sprintf(tmpMessage, "TxS2dCal => SetS2dParams error.");
			m_funcGetMessage(tmpMessage);
			return ITxCalibration::Status::FAIL;
		}
		sprintf_s(tmpMessage, 512, "[DEBUG_CAL]=> Region: %d  Gain: %u  Offset: %d", i_reg, _s2dResultsParams.gain[i_reg], _s2dResultsParams.offset[i_reg]);
		m_funcGetMessage(tmpMessage);
		ExportTestLog(tmpMessage);
	}
	return ITxCalibration::Status::PASS;
}

ITxCalibration::S2dResultsParams TxS2dCal::getTxS2dParams()
{
	return _s2dResultsParams;
}
