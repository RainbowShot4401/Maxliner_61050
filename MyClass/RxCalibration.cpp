#include "..\stdafx.h"
#include "RxCalibration.h"
#include <algorithm>
#include <map>
#include <string>

using namespace std;

map<int, string> Chain_Table;
RxCalibration::RxCalibration(dutWrapper * dutApi, IRxCalibration::GenType genType)
{
	_genType = genType;
	_dutApiPonter = dutApi;
	_dutFunc = new RxDutApi(_dutApiPonter);
	_dutFunc->startRxCalibration();
	//_logPrint = new LogPrint(18, 5, 4);
	Chain_Table.insert(pair<int, string>(0, "TX_CHAIN_A"));
	Chain_Table.insert(pair<int, string>(1, "TX_CHAIN_B"));
	Chain_Table.insert(pair<int, string>(2, "TX_CHAIN_C"));
	Chain_Table.insert(pair<int, string>(3, "TX_CHAIN_D"));
}

RxCalibration::~RxCalibration()
{
	_dutFunc->endRxCalibration();
	delete _dutFunc;
}

void RxCalibration::debugFlag(string flag)
{
	_dutFunc->debug = flag;
}

void RxCalibration::debugSubBandFlag(string flag)
{
	_dutFunc->subBandDebug = flag;
}

double CalcCableLossDb(int ant1, int ant2, int ant3, int ant4, double cableLoss1, double cableLoss2, double cableLoss3, double cableLoss4)
{
	double resultDb = 0.0;

	int antNumber = ant1 + ant2 + ant3 + ant4;

	if (ant1 == 1)	// antenna 1 is ON
	{
		resultDb = resultDb + pow(10, (cableLoss1 / 10));
	}
	if (ant2 == 1)	// antenna 2 is ON
	{
		resultDb = resultDb + pow(10, (cableLoss2 / 10));
	}
	if (ant3 == 1)	// antenna 3 is ON
	{
		resultDb = resultDb + pow(10, (cableLoss3 / 10));
	}
	if (ant4 == 1)	// antenna 4 is ON
	{
		resultDb = resultDb + pow(10, (cableLoss4 / 10));
	}

	if ((antNumber == 0) || (resultDb == 0))
	{
		resultDb = 0.0;
	}
	else
	{
		resultDb = 10 * log10(resultDb / antNumber);
	}

	return resultDb;
}

IRxCalibration::status RxCalibration::calcPathLoass(IVSG::PathLossParams& pathLossParams)
{
	double cableLoss[MAX_DATA_STREAM];
	int antEnable[MAX_DATA_STREAM];
	char lpszRunInfo[1024];
	char szAntenna[64];

	for (int i = 0; i < MAX_DATA_STREAM; i++)
	{
		antEnable[i] = (pathLossParams.antennaMask >> i) & 0x1;
		if (antEnable[i])
		{	
			//string tmpANT = ;
			strcpy(szAntenna, Chain_Table[antEnable[i]].c_str());
			m_funcReadCableLoss(CABLELOSS_RX, szAntenna, pathLossParams.freqMHz, pathLossParams.outPathLossPerAnt[i], lpszRunInfo);
			//typedef BOOL(CALLBACK* funcReadCableLoss)(CABLELOSS_TYPE Type, LPSTR szAntenna, double dbFreqMHz, double& dbCableLoss, LPSTR lpszRunInfo);
		//	TM_RETURN vsg_ret = TM_GetPathLossAtFrequency(pathLossParams.test_id, pathLossParams.freqMHz, &pathLossParams.outPathLossPerAnt[i], i, 0);
		//	if (vsg_ret != TM_RETURN::TM_ERR_OK)
		//		return IVSG::status::FAIL;
		}
	}

	//for (int i = 0; i < MAX_DATA_STREAM; i++)
	//	pathLossParams.outPathLossPerAnt[i] = stoi(MidGain_Cableloss.at(i));

	pathLossParams.outPathLossAverage = CalcCableLossDb(
		antEnable[0], antEnable[1], antEnable[2], antEnable[3],
		pathLossParams.outPathLossPerAnt[0], pathLossParams.outPathLossPerAnt[1],
		pathLossParams.outPathLossPerAnt[2], pathLossParams.outPathLossPerAnt[3]);

	return status::PASS;
}

IRxCalibration::status RxCalibration::RxRfSubBandCal(RxRfSubBandParams rxRfSubBandParams)
{
	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   RX Sub-Bands Calibration"));
	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL]=>    RX Sub-Bands Calibration");
	int retry = 3;
	char lpszRunInfo[1024];

	_rxRfSubBandParams = rxRfSubBandParams;
	unsigned int outRxBandFreq[MAX_DATA_STREAM][MAX_SUB_BAND_CROS_POINT];

	int referanceFreq = rxRfSubBandParams.setChannelParams.freq;
	for (auto const& freq : rxRfSubBandParams.subBandFreq) {
		rxRfSubBandParams.setChannelParams.freq = freq;

		ret = (IRxCalibration::status)_dutFunc->fastRxSetChannel(rxRfSubBandParams.setChannelParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfSubBandParams.setRxGainBlocksParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		rxRfSubBandParams.pathLossParams.freqMHz = freq;
		calcPathLoass(rxRfSubBandParams.pathLossParams);
		//rxRfSubBandParams.vsgStartCWParams.cableLoss = rxRfSubBandParams.pathLossParams.outPathLossAverage;

		rxRfSubBandParams.vsgStartCWParams.loFreqMHz = freq;
		//rxRfSubBandParams.vsg->vsgStartCW(rxRfSubBandParams.vsgStartCWParams);
		
		for (int i = 0; i < retry; i++)
		{
			if (m_funcStartRxPower(TRUE, (rxRfSubBandParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRfSubBandParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRfSubBandParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
				break;
		}
		//for Debug.
		//_dutFunc->m_funcStartRxPower = m_funcStartRxPower;
		//_dutFunc->m_funcStopRxPower = m_funcStopRxPower;
		ret = (IRxCalibration::status)_dutFunc->MeasureLNASubBandGains(rxRfSubBandParams.setChannelParams.freq);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		for (int i = 0; i < retry; i++)
		{
			if (m_funcStopRxPower(TRUE, lpszRunInfo))
				break;
		}
		//IVSG::status vsg_ret = rxRfSubBandParams.vsg->vsgStopCW();
		//if (vsg_ret != IVSG::status::PASS)
		//	return IRxCalibration::status::FAIL;
	}
	ret = (IRxCalibration::status)_dutFunc->calculateLNASubBandGains(outRxBandFreq);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;
	
	//20230808, Zack modifis for 610.50
	unsigned int outRxBandFreqFLUT[MAX_DATA_STREAM][MAX_SUB_BAND_CROS_POINT_LUT];
	for (int i = 0; i < MAX_DATA_STREAM; i++)
	{
		for (int j = 0; j < MAX_SUB_BAND_CROS_POINT_LUT; j++)
			outRxBandFreqFLUT[i][j] = outRxBandFreq[i][j];
	}
	ret = (IRxCalibration::status)_dutFunc->setRxBandLUT(outRxBandFreqFLUT); 
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	rxRfSubBandParams.setChannelParams.freq = referanceFreq;
	ret = (IRxCalibration::status)_dutFunc->fastRxSetChannel(rxRfSubBandParams.setChannelParams);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	for (int i = 0; i < MAX_DATA_STREAM; i++)
	{
		transform(&outRxBandFreq[i][0], &outRxBandFreq[i][0] + MAX_SUB_BAND_CROS_POINT_LUT, begin(_subBandResultsParams.subBandFreq[i]), [](unsigned int x) -> int {return static_cast<int>(x); });
	}

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfSubBandParams.setChannelParams.boardAntennaMask & (1 << i_ant))
		{
			for (int j_BandFreq = 0; j_BandFreq < rxRfSubBandParams.subBandFreq.size(); j_BandFreq++)
			{
				for (int k_BandFreq = 0; k_BandFreq < MAX_SUB_BAND_CROS_POINT_LUT; k_BandFreq++)
				{
					sprintf(printChar, "ANT: %d CROSS POINT:%d Value:%d", i_ant + 1, j_BandFreq, _subBandResultsParams.subBandFreq[i_ant][k_BandFreq]);
					ExportTestLog("[DEBUG_CAL]=> %s", printChar);
				}
				//_logPrint->print(string(printChar));
			}
		}
	}
	return IRxCalibration::status::PASS;
}


//Mid-gain calibraiton sets Mid-gain (4 for WAV500 and 1 for WAV600) to fixed value
IRxCalibration::status RxCalibration::RxRfMidGainCal(RxRfMidGainParams rxRfMidGainParams)
{

	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   RX Mid Gain Calibration"));

	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL]=>    RX Mid Gain Calibration");
	_rxRfMidGainParams = rxRfMidGainParams;

	float outMeasuredGain[MAX_DATA_STREAM];
	int outControlWord[MAX_DATA_STREAM];

	ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfMidGainParams.setRxGainBlocksParams);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	//if (rxRfMidGainParams.vsg->isTransmitting == false)
	//{
	//	rxRfMidGainParams.vsg->calcPathLoass(rxRfMidGainParams.pathLossParams);
	//	rxRfMidGainParams.vsgStartCWParams.cableLoss = rxRfMidGainParams.pathLossParams.outPathLossAverage;
	//	rxRfMidGainParams.vsg->vsgStartCW(rxRfMidGainParams.vsgStartCWParams);
	//}

	//float MidGain_Prxin[MAX_DATA_STREAM] = {};

	calcPathLoass(rxRfMidGainParams.pathLossParams);
	for (int i_delta_pattLoss = 0; i_delta_pattLoss < MAX_DATA_STREAM; i_delta_pattLoss++)
	{
		rxRfMidGainParams.midGainCalibrateParams.Prxin[i_delta_pattLoss] = static_cast<float>(rxRfMidGainParams.vsgStartCWParams.rfPowerLeveldBm + rxRfMidGainParams.pathLossParams.outPathLossAverage - rxRfMidGainParams.pathLossParams.outPathLossPerAnt[i_delta_pattLoss]);
		//rxRfMidGainParams.midGainCalibrateParams.Prxin[i_delta_pattLoss] = stoi(MidGain_Cableloss.at(i_delta_pattLoss).c_str());
	}
	int retry = 3;
	char lpszRunInfo[1024];
	for (int i = 0; i < retry; i++)
	{
		if (m_funcStartRxPower(TRUE, (rxRfMidGainParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRfMidGainParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRfMidGainParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
			break;
	}

	ret = (IRxCalibration::status)_dutFunc->midGainCalibrate(rxRfMidGainParams.midGainCalibrateParams, outMeasuredGain, outControlWord);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	for (int i = 0; i < retry; i++)
	{
		if (m_funcStopRxPower(TRUE, lpszRunInfo))
			break;
	}
	//rxRfMidGainParams.vsg->vsgStopCW();

	_midGainResultsParams.calcTuneWord.clear();
	_midGainResultsParams.measGain.clear();
	_midGainResultsParams.calcTuneWord.resize(4);
	_midGainResultsParams.measGain.resize(4);
	copy(outControlWord, outControlWord + MAX_DATA_STREAM, _midGainResultsParams.calcTuneWord.begin());
	copy(outMeasuredGain, outMeasuredGain + MAX_DATA_STREAM, _midGainResultsParams.measGain.begin());

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfMidGainParams.pathLossParams.antennaMask & (1 << i_ant))
		{
			sprintf(printChar, "ANT: %d CALC_GAIN_CTRL:%d Min: %0.2f  Max %0.2f", i_ant + 1, _midGainResultsParams.calcTuneWord[i_ant], rxRfMidGainParams.rxRfMidGainLimitsParams.midLnaGainMin, rxRfMidGainParams.rxRfMidGainLimitsParams.midLnaGainMax);
			ExportTestLog("[DEBUG_CAL]=> %s", printChar);
	
			sprintf(printChar, "ANT: %d TARGET_GAIN:%0.2f Min: %0.2f Max %0.2f Measure-> :%0.2f", i_ant + 1
				, rxRfMidGainParams.midGainCalibrateParams.targetGain
				, rxRfMidGainParams.midGainCalibrateParams.targetGain - rxRfMidGainParams.rxRfMidGainLimitsParams.measGainTolerance
				, rxRfMidGainParams.midGainCalibrateParams.targetGain + rxRfMidGainParams.rxRfMidGainLimitsParams.measGainTolerance
				, _midGainResultsParams.measGain[i_ant]);
			ExportTestLog("[DEBUG_CAL]=> %s", printChar);
		}
	}
	return IRxCalibration::status::PASS;
}

//Gain-step or Gain accurecy calibration measurse the gain step between each LNA gain step.
//Each gain step controls external and internal LNA; The LNA index table located in PSD
IRxCalibration::status RxCalibration::RxRfGainStepAccuracyCal(RxRfGainStepAccuracyParams rxRfGainStepAccuracyParams)
{
	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   RX Gain Accuracy Calibration"));
	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL]=>    RX Gain Accuracy Calibration");

	const int lastGainBoost = 5;
	bool activateBoost = (_genType == IRxCalibration::GenType::GEN6);
	GainPerAntPerStep_t* gains = new GainPerAntPerStep_t;
	_rxRfGainStepAccuracyParams = rxRfGainStepAccuracyParams;

	calcPathLoass(rxRfGainStepAccuracyParams.pathLossParams);
	//if (rxRfGainStepAccuracyParams.vsg->isTransmitting == false)
	//{
	//	rxRfGainStepAccuracyParams.vsgStartCWParams.cableLoss = rxRfGainStepAccuracyParams.pathLossParams.outPathLossAverage;
	//	rxRfGainStepAccuracyParams.vsg->vsgStartCW(rxRfGainStepAccuracyParams.vsgStartCWParams);
	//}

	int retry = 3;
	char lpszRunInfo[1024];

	for (int i_lnaIndex = 0; i_lnaIndex < rxRfGainStepAccuracyParams.numberOfLnaGains; i_lnaIndex++)
	{
		//To avoid DC at the low gain, we increat the VSG signal (need to check if needed for first gain for Gen5)
		if (activateBoost)
		{
			if (i_lnaIndex >= (rxRfGainStepAccuracyParams.numberOfLnaGains - 2))
			{
				//rxRfGainStepAccuracyParams.vsg->vsgStopCW();
				rxRfGainStepAccuracyParams.vsgStartCWParams.rfPowerLeveldBm += lastGainBoost;
				//rxRfGainStepAccuracyParams.vsg->vsgStartCW(rxRfGainStepAccuracyParams.vsgStartCWParams);
			}
		}

		rxRfGainStepAccuracyParams.setRxGainBlocksParams.LnaIndex = i_lnaIndex;
		rxRfGainStepAccuracyParams.setRxGainBlocksParams.pgc1 = rxRfGainStepAccuracyParams.pgc1ArrayGains[i_lnaIndex];
		ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfGainStepAccuracyParams.setRxGainBlocksParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;
		for (int i = 0; i < retry; i++)
		{
			if (m_funcStartRxPower(TRUE, (rxRfGainStepAccuracyParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRfGainStepAccuracyParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRfGainStepAccuracyParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
				break;
		}
		DutApiCorrResults_t corrResultsOut[MAX_DATA_STREAM];
		ret = (IRxCalibration::status)_dutFunc->MeasureCwPower(rxRfGainStepAccuracyParams.measureCwPowerParams, corrResultsOut);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;
		for (int i = 0; i < retry; i++)
		{
			if (m_funcStopRxPower(TRUE, lpszRunInfo))
				break;
		}
		for (int i = 0; i < MAX_DATA_STREAM; i++)
		{
			rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.corrResultsPerAntPerStep_p.corrResults[i][i_lnaIndex].II = corrResultsOut[i].II;
		}

		for (int i_delta_pattLoss = 0; i_delta_pattLoss < MAX_DATA_STREAM; i_delta_pattLoss++)
		{
			rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.Prxin[i_delta_pattLoss] = rxRfGainStepAccuracyParams.vsgStartCWParams.rfPowerLeveldBm + rxRfGainStepAccuracyParams.pathLossParams.outPathLossAverage - rxRfGainStepAccuracyParams.pathLossParams.outPathLossPerAnt[i_delta_pattLoss];
		}

		rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.Pgc1 = _rxRfGainStepAccuracyParams.pgc1ArrayGains[i_lnaIndex];
		rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.lnaGainStep = i_lnaIndex;

		ret = (IRxCalibration::status)_dutFunc->CalculateLnaGainSteps(rxRfGainStepAccuracyParams.calculateLnaGainStepsParams, gains);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;
	}

	_gainStepResultsParams.gainSteps = *gains;
	delete gains;
	_gainStepResultsParams.numLnaGainSteps = rxRfGainStepAccuracyParams.numberOfLnaGains;

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfGainStepAccuracyParams.pathLossParams.antennaMask & (1 << i_ant))
		{
			for (int i_gain = 0; i_gain < _gainStepResultsParams.numLnaGainSteps; i_gain++)
			{
				sprintf(printChar, "ANT: %d LNA_GAIN:%d  Measure Gain -> :%0.2f", i_ant + 1, i_gain, _gainStepResultsParams.gainSteps.gain[i_ant][i_gain]);
				//_logPrint->print(string(printChar));
				ExportTestLog("[DEBUG_CAL]=> %s", printChar);
			}
		}
	}
	return IRxCalibration::status::PASS;
}

//Flatness calibration built from 3 calibraitons: High gain Flatness, ByPass gain Flatness, mid-gain delta
//High gain Flatness  - with this calibraiton we corrent improfection of gain over spectrum, we store the delta to main mid-gain gain result(from gain-step calibration); 
//					   During this calibraion, external PA is in active mode
//ByPass gain Flatness - with this calibraiton we corrent improfection of gain over spectrum; 
//					   During this calibraion, external PA is in Bypass mode
//mid-gain delta      - with this calibraiton we corrent calibrate mid-gain value over spectrum, we store the delta to one above lowest gain from gain-step calibration;
//					   During this calibraion, external PA is in active mode
IRxCalibration::status RxCalibration::RxRfFlatnessCal(RxRfFlatnessParams rxRfFlatnessParams)
{
	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   RF flatness calibration"));

	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL]=>    RF flatness calibration");
	_rxRfFlatnessParams = rxRfFlatnessParams;

	vector<vector<int>> midGainVector;
	midGainVector.resize(MAX_DATA_STREAM);
	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		midGainVector.at(i_ant).resize(rxRfFlatnessParams.rfFlatnessFreq.size());
	}

	int freq = 0;
	int referanceFreq = rxRfFlatnessParams.setChannelParams.freq;
	for (int i_freq = 0; i_freq < rxRfFlatnessParams.rfFlatnessFreq.size(); i_freq++) 
	{
		freq = rxRfFlatnessParams.rfFlatnessFreq.at(i_freq);
		rxRfFlatnessParams.setChannelParams.freq = freq;

		ret = (IRxCalibration::status)_dutFunc->fastRxSetChannel(rxRfFlatnessParams.setChannelParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		rxRfFlatnessParams.pathLossParams.freqMHz = freq;
		//rxRfFlatnessParams.vsg->calcPathLoass(rxRfFlatnessParams.pathLossParams);
		calcPathLoass(rxRfFlatnessParams.pathLossParams);
		rxRfFlatnessParams.vsgStartCWParams.cableLoss = rxRfFlatnessParams.pathLossParams.outPathLossAverage;
		rxRfFlatnessParams.vsgStartCWParams.loFreqMHz = freq;
		//rxRfFlatnessParams.vsg->vsgStartCW(rxRfFlatnessParams.vsgStartCWParams);

		RxDutApi::MidGainCalibrateParams midGainCalibrateParams;
		midGainCalibrateParams.Pgc1 = rxRfFlatnessParams.setRxGainBlocksParams.pgc1;
		midGainCalibrateParams.Pgc2 = rxRfFlatnessParams.setRxGainBlocksParams.pgc2;
		midGainCalibrateParams.targetGain = _rxRfMidGainParams.midGainCalibrateParams.targetGain;

		int byPassGain = rxRfFlatnessParams.setRxGainBlocksParamsBypass.LnaIndex;
		int activeGain = rxRfFlatnessParams.setRxGainBlocksParams.LnaIndex;

		for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
		{
			if ((rxRfFlatnessParams.setChannelParams.boardAntennaMask >> i_ant) & 0x1 > 0)
			{
				midGainCalibrateParams.Prxin[i_ant] = rxRfFlatnessParams.vsgStartCWParams.rfPowerLeveldBm + rxRfFlatnessParams.pathLossParams.outPathLossAverage - rxRfFlatnessParams.pathLossParams.outPathLossPerAnt[i_ant];
				rxRfFlatnessParams.calculateRxRfFlatnessParams.refGain[i_ant] = _gainStepResultsParams.gainSteps.gain[i_ant][activeGain - 1];
				rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.refGain[i_ant] = _gainStepResultsParams.gainSteps.gain[i_ant][byPassGain];
				// Eran new activeGain-1  old activeGain
			}
		}
		rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.PGC1 = rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc1;
		rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.PGC2 = rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc2;
		rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.PGC3 = rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc3;

		rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC1 = rxRfFlatnessParams.setRxGainBlocksParams.pgc1;
		rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC2 = rxRfFlatnessParams.setRxGainBlocksParams.pgc2;
		rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC3 = rxRfFlatnessParams.setRxGainBlocksParams.pgc3;

		int retry = 3;
		char lpszRunInfo[1024];

		for (int i = 0; i < retry; i++)
		{
			if (m_funcStartRxPower(TRUE, (rxRfFlatnessParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRfFlatnessParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRfFlatnessParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
				break;
		}
		//Active mode
		ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfFlatnessParams.setRxGainBlocksParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		//Active mode Mid-gain delta calibration
		float targetMeasGain[MAX_ANTENNA_NUMBER];
		int targetMeasControl[MAX_ANTENNA_NUMBER];
		ret = (IRxCalibration::status)_dutFunc->midGainCalibrate(midGainCalibrateParams, targetMeasGain, targetMeasControl);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		//Active mode flatness calibration
		//Active mode

		// Eran New Call 
		rxRfFlatnessParams.setRxGainBlocksParams.LnaIndex = 0;
		ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfFlatnessParams.setRxGainBlocksParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		//////
		DutApiCorrResults_t corrResultsOut[MAX_DATA_STREAM];
		ret = (IRxCalibration::status)_dutFunc->MeasureCwPower(rxRfFlatnessParams.measureCwPowerParams, corrResultsOut);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;
		//
		rxRfFlatnessParams.setRxGainBlocksParams.LnaIndex = 1;
		//
		//byPass mode flatness calibration
		ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRfFlatnessParams.setRxGainBlocksParamsBypass);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		DutApiCorrResults_t corrResultsOutByPass[MAX_DATA_STREAM];
		ret = (IRxCalibration::status)_dutFunc->MeasureCwPower(rxRfFlatnessParams.measureCwPowerParams, corrResultsOutByPass);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;


		for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
		{
			if ((rxRfFlatnessParams.setChannelParams.boardAntennaMask >> i_ant) & 0x1 > 0)
			{
				rxRfFlatnessParams.calculateRxRfFlatnessParams.Pin[i_ant][i_freq] = rxRfFlatnessParams.vsgStartCWParams.rfPowerLeveldBm + rxRfFlatnessParams.pathLossParams.outPathLossAverage - rxRfFlatnessParams.pathLossParams.outPathLossPerAnt[i_ant];
				rxRfFlatnessParams.calculateRxRfFlatnessParams.P_Adc_array[i_ant][i_freq] = corrResultsOut[i_ant].II;

				rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.Pin[i_ant][i_freq] = rxRfFlatnessParams.vsgStartCWParams.rfPowerLeveldBm + rxRfFlatnessParams.pathLossParams.outPathLossAverage - rxRfFlatnessParams.pathLossParams.outPathLossPerAnt[i_ant];
				rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass.P_Adc_array[i_ant][i_freq] = corrResultsOutByPass[i_ant].II;
			}
		}

		for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
		{
			if ((rxRfFlatnessParams.setChannelParams.boardAntennaMask >> i_ant) & 0x1 > 0)
			{
				midGainVector.at(i_ant).at(i_freq) = targetMeasControl[i_ant];
			}
		}

		for (int i = 0; i < retry; i++)
		{
			if (m_funcStopRxPower(TRUE, lpszRunInfo))
				break;
		}
		//IVSG::status vsg_ret = rxRfFlatnessParams.vsg->vsgStopCW();
		//if (vsg_ret != IVSG::status::PASS)
		//	return IRxCalibration::status::FAIL;
	}

	float rf_flatness_delta_gain_results[MAX_ANTENNA_NUMBER][MAX_FLATNESS_POINTS];
	ret = (IRxCalibration::status)_dutFunc->CalculateRxRfFlatness(rxRfFlatnessParams.calculateRxRfFlatnessParams, rf_flatness_delta_gain_results);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	float rf_flatness_delta_gain_results_byPass[MAX_ANTENNA_NUMBER][MAX_FLATNESS_POINTS];
	ret = (IRxCalibration::status)_dutFunc->CalculateRxRfFlatness(rxRfFlatnessParams.calculateRxRfFlatnessParamsBypass, rf_flatness_delta_gain_results_byPass);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;


	_rxRfFlatnessParams = rxRfFlatnessParams;

	copy(begin(rxRfFlatnessParams.rfFlatnessFreq), end(rxRfFlatnessParams.rfFlatnessFreq), back_inserter(_flatnessResultsParams.flatnessFreq));
	

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if ((rxRfFlatnessParams.setChannelParams.boardAntennaMask >> i_ant) & 0x1 > 0)
		{
			transform(&rf_flatness_delta_gain_results_byPass[i_ant][0], &rf_flatness_delta_gain_results_byPass[i_ant][0] + MAX_FLATNESS_POINTS, begin(_flatnessResultsParams.flatnessBypassDB[i_ant]), [](float x) -> int {return std::round(2 * x); });
			transform(&rf_flatness_delta_gain_results[i_ant][0], &rf_flatness_delta_gain_results[i_ant][0] + MAX_FLATNESS_POINTS, begin(_flatnessResultsParams.flatnessdB[i_ant]), [](float x) -> int {return std::round(2 * x); });
			transform(midGainVector.at(i_ant).begin(), midGainVector.at(i_ant).end(), begin(_flatnessResultsParams.midGainDelta[i_ant]), [=](int x) -> int {return (x - _midGainResultsParams.calcTuneWord.at(i_ant)); });
			//printf("111");
			//ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][0] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][0]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][1] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][1]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][2] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][2]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][3] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][3]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][4] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][4]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][5] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][5]);
            //ExportTestLog("[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][6] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][6]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][0] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][0]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][1] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][1]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][2] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][2]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][3] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][3]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][4] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][4]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][5] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][5]);
            //ExportTestLog("[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][6] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][6]);	
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][0] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][0]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][1] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][1]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][2] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][2]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][3] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][3]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][4] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][4]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][5] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][5]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> rf_flatness_delta_gain_results_byPass[%d][6] is %f \n", i_ant, rf_flatness_delta_gain_results_byPass[i_ant][6]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][0] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][0]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][1] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][1]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][2] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][2]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][3] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][3]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][4] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][4]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][5] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][5]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> _flatnessResultsParams.flatnessBypassDB[%d][6] is %f \n", i_ant, _flatnessResultsParams.flatnessBypassDB[i_ant][6]);
		}

	}

	rxRfFlatnessParams.setChannelParams.freq = referanceFreq;
	ret = (IRxCalibration::status)_dutFunc->fastRxSetChannel(rxRfFlatnessParams.setChannelParams);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;


	//_logPrint->print();
	//_logPrint->print(string("[DEBUG_CAL_RX]=> RF mid-gain delta calibration"));
	ExportTestLog("");
	ExportTestLog("[DEBUG_CAL_RX]=> RF mid-gain delta calibration");
	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfFlatnessParams.setChannelParams.boardAntennaMask & (1 << i_ant))
		{
			for (int j_fletness_freq = 0; j_fletness_freq < _flatnessResultsParams.flatnessFreq.size(); j_fletness_freq++)
			{
				sprintf(printChar, "[DEBUG_CAL]=> ANT: %d FREQ: %0.0f   MID_GAIN_DELTA: %d", i_ant + 1, _flatnessResultsParams.flatnessFreq[j_fletness_freq], _flatnessResultsParams.midGainDelta[i_ant][j_fletness_freq]);
				ExportTestLog(printChar);
				//_logPrint->print(string(printChar));
			}
		}
	}

	//_logPrint->print();
	//_logPrint->print(string("RF high gain flatness calibration"));
	ExportTestLog("");
	ExportTestLog("[DEBUG_CAL_RX]=> RF high gain flatness calibration");
	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfFlatnessParams.setChannelParams.boardAntennaMask & (1 << i_ant))
		{

			for (int j_fletness_freq = 0; j_fletness_freq < _flatnessResultsParams.flatnessFreq.size(); j_fletness_freq++)
			{
				if (_flatnessResultsParams.flatnessdB[i_ant][j_fletness_freq] > 7) _flatnessResultsParams.flatnessdB[i_ant][j_fletness_freq] = 7;
				sprintf(printChar, "[DEBUG_CAL]=> ANT: %d FREQ: %0.0f   DELTA_GAIN: %d", i_ant + 1, _flatnessResultsParams.flatnessFreq[j_fletness_freq], _flatnessResultsParams.flatnessdB[i_ant][j_fletness_freq]);
				ExportTestLog(printChar);
				//_logPrint->print(string(printChar));
			}
		}
	}

	//_logPrint->print();
	//_logPrint->print(string("RF byPass gain flatness calibration"));
	ExportTestLog("");
	ExportTestLog("[DEBUG_CAL_RX]=> RF byPass gain flatness calibration");
	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRfFlatnessParams.setChannelParams.boardAntennaMask & (1 << i_ant))
		{
			for (int j_fletness_freq = 0; j_fletness_freq < _flatnessResultsParams.flatnessFreq.size(); j_fletness_freq++)
			{
				sprintf(printChar, "[DEBUG_CAL]=> ANT: %d FREQ: %0.0f   DELTA_GAIN: %d", i_ant + 1, _flatnessResultsParams.flatnessFreq[j_fletness_freq], _flatnessResultsParams.flatnessBypassDB[i_ant][j_fletness_freq]);
				ExportTestLog(printChar);
				//_logPrint->print(string(printChar));
			}
		}
	}
	return IRxCalibration::status::PASS;
}

// RX S2D calibrates the optimal current offset and gain in order to reach optimal dynamic range of A2D.
// On each region we measure A2D noise level afterwards we injected signal measure A2D value.
// From is procedure we can calculate S2D offset and gain.
IRxCalibration::status RxCalibration::RxRssiS2dCal(RxRssiS2dParams rxRssiS2dCalParams)
{
	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   S2D Calibration"));
	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL_RX]=>   S2D Calibration");
	_rxRssiS2dCalParams = rxRssiS2dCalParams;

	//rxRssiS2dCalParams.vsg->calcPathLoass(rxRssiS2dCalParams.pathLossParams);
	calcPathLoass(rxRssiS2dCalParams.pathLossParams);
	ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRssiS2dCalParams.setRxGainBlocksParams);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;


	for (int i_reg = 0; i_reg < rxRssiS2dCalParams.numberOfRegions; i_reg++)
	{
		MT_INT16 offset_rssi_v1_signalOff[MAX_ANTENNA_NUMBER] = {};
		MT_INT16 offset_rssi_v2_signalOn[MAX_ANTENNA_NUMBER] = {};
		MT_INT16 gain_rssi_v1_signalOff[MAX_ANTENNA_NUMBER] = {};
		MT_INT16 gain_rssi_v2_signalOn[MAX_ANTENNA_NUMBER] = {};

		MT_INT16 gainTemp[MAX_ANTENNA_NUMBER] = {};
		MT_INT16 onTemp[MAX_ANTENNA_NUMBER] = {};
		MT_INT16 offTemp[MAX_ANTENNA_NUMBER] = {};

		rxRssiS2dCalParams.setRssiOffsetGainParams.antMask = rxRssiS2dCalParams.antennaMask;
		rxRssiS2dCalParams.setRssiOffsetGainParams.regionIndex = i_reg;
		ret = (IRxCalibration::status)_dutFunc->setRssiOffsetGain(rxRssiS2dCalParams.setRssiOffsetGainParams);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		//if (rxRssiS2dCalParams.vsg->isTransmitting == true)
		//{
		//	IVSG::status vsgError = rxRssiS2dCalParams.vsg->vsgStopCW();
		//	if (vsgError != IVSG::status::PASS)
		//		return IRxCalibration::status::FAIL;
		//}
		// 
		////start temp testing
		//DutApiCorrResults_t corrResultsOut[MAX_DATA_STREAM];
		//ret = (IRxCalibration::status)_dutFunc->MeasureCwPower(_rxRfGainStepAccuracyParams.measureCwPowerParams, corrResultsOut);
		////end temp testing

		ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, offset_rssi_v1_signalOff);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v1_signalOff[1] is %d \n", offset_rssi_v1_signalOff[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v1_signalOff[3] is %d \n", offset_rssi_v1_signalOff[3]);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		rxRssiS2dCalParams.rssiS2dSetInitialsParams.numLnaGains = _gainStepResultsParams.numLnaGainSteps;

		int retry = 3;
		char lpszRunInfo[1024];

		for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
		{
			if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
			{
				copy(begin(_gainStepResultsParams.gainSteps.gain[i_ant]), end(_gainStepResultsParams.gainSteps.gain[i_ant]),
					begin(rxRssiS2dCalParams.rssiS2dSetInitialsParams.lnaGain));

				ret = (IRxCalibration::status)_dutFunc->rssiS2dSetInitials(rxRssiS2dCalParams.rssiS2dSetInitialsParams);
				if (ret != IRxCalibration::status::PASS)
					return IRxCalibration::status::FAIL;

				//if (rxRssiS2dCalParams.vsg->isTransmitting == true)
				//{
				//	IVSG::status vsgError = rxRssiS2dCalParams.vsg->vsgStopCW();
				//	if (vsgError != IVSG::status::PASS)
				//		return IRxCalibration::status::FAIL;
				//}

				rxRssiS2dCalParams.vsgStartCWParams.cableLoss = rxRssiS2dCalParams.pathLossParams.outPathLossPerAnt[i_ant];
				rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm = rxRssiS2dCalParams.maxPower[i_reg] + rxRssiS2dCalParams.marginHigh[i_reg] - _gainStepResultsParams.gainSteps.gain[i_ant][rxRssiS2dCalParams.setRxGainBlocksParams.LnaIndex];
				//rxRssiS2dCalParams.vsg->vsgStartCW(rxRssiS2dCalParams.vsgStartCWParams);
				for (int i = 0; i < retry; i++)
				{
					if (m_funcStartRxPower(TRUE, (rxRssiS2dCalParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRssiS2dCalParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
						break;
				}
				////start temp testing?BN
				//DutApiCorrResults_t corrResultsOut[MAX_DATA_STREAM];
				//ret = (IRxCalibration::status)_dutFunc->MeasureCwPower(_rxRfGainStepAccuracyParams.measureCwPowerParams, corrResultsOut);
				////end temp testing


				ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, gainTemp);
				if (ret != IRxCalibration::status::PASS)
					return IRxCalibration::status::FAIL;

				offset_rssi_v2_signalOn[i_ant] = gainTemp[i_ant];
				//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> 1_gainTemp[%d] is %d \n", i_ant, gainTemp[i_ant]);
				for (int i = 0; i < retry; i++)
				{
					if (m_funcStopRxPower(TRUE, lpszRunInfo))
						break;
				}
			}
		}

		rxRssiS2dCalParams.calculateOptimalIoffsParams.region = i_reg;
		//char i_reg_print[MAX_BUFFER_SIZE] = "";
		//sprintf_s(i_reg_print, MAX_BUFFER_SIZE, "i_reg: %d", i_reg);
		//_logPrint->print(string(i_reg_print));
		copy(begin(offset_rssi_v1_signalOff), end(offset_rssi_v1_signalOff), begin(rxRssiS2dCalParams.calculateOptimalIoffsParams.V1));
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v1_signalOff[1] is %d \n", offset_rssi_v1_signalOff[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v1_signalOff[3] is %d \n", offset_rssi_v1_signalOff[3]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v1[0] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V1[0]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v1[1] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V1[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v1[2] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V1[2]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v1[3] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V1[3]);
		copy(begin(offset_rssi_v2_signalOn), end(offset_rssi_v2_signalOn), begin(rxRssiS2dCalParams.calculateOptimalIoffsParams.V2));
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v2_signalOn[1] is %d \n", offset_rssi_v2_signalOn[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> offset_rssi_v2_signalOn[3] is %d \n", offset_rssi_v2_signalOn[3]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v2[0] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V2[0]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v2[1] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V2[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v2[2] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V2[2]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> v2[3] :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.V2[3]);

		int s2d_offset_opt_out[MAX_ANTENNA_NUMBER];
		int s2d_gain_opt_out[MAX_ANTENNA_NUMBER];

		int iOffsetCorse = (int)(rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[i_reg]) & 0x60;
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> ioffsetcorse is :%d \n", iOffsetCorse);
		rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[i_reg] = (int)(rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[i_reg]) & 0x1F;
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> s2d gain reg0 is :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_gain_db[0]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> s2d gain reg1 is :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_gain_db[1]);

		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> s2d offset reg0 is :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[0]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> s2d offset reg1 is :%f \n", rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[1]);


		ret = (IRxCalibration::status)_dutFunc->calculateOptimalIoffs(rxRssiS2dCalParams.calculateOptimalIoffsParams, s2d_offset_opt_out);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=>s2d_offset_opt_out[0] is :%d \n", s2d_offset_opt_out[0]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=>s2d_offset_opt_out[1] is :%d \n", s2d_offset_opt_out[1]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=>s2d_offset_opt_out[2] is :%d \n", s2d_offset_opt_out[2]);
		//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=>s2d_offset_opt_out[3] is :%d \n", s2d_offset_opt_out[3]);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[i_reg] = (int)(rxRssiS2dCalParams.calculateOptimalIoffsParams.s2d_offset_db[i_reg]) | iOffsetCorse;

		bool continueLoop;
		for (int i_attempt = 0; i_attempt < MAX_NUMBER_OF_S2D_ATTEMPTS; i_attempt++) 
		{
			continueLoop = false;

			for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
			{
				if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
				{
					_s2dResultsParams.offset[i_ant][i_reg] = (s2d_offset_opt_out[i_ant] & 0x1F) | iOffsetCorse;



					rxRssiS2dCalParams.setRssiOffsetGainParams.antMask = 1 << i_ant;
					rxRssiS2dCalParams.setRssiOffsetGainParams.regionIndex = i_reg;
					rxRssiS2dCalParams.setRssiOffsetGainParams.iOffset[i_reg] = (s2d_offset_opt_out[i_ant] & 0x1F) | iOffsetCorse;
					ret = (IRxCalibration::status)_dutFunc->setRssiOffsetGain(rxRssiS2dCalParams.setRssiOffsetGainParams);
					if (ret != IRxCalibration::status::PASS)
						return IRxCalibration::status::FAIL;
				}
			}

			for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
			{
				if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
				{
					//if (rxRssiS2dCalParams.vsg->isTransmitting == true)
					//{
					//	IVSG::status vsgError = rxRssiS2dCalParams.vsg->vsgStopCW();
					//	if (vsgError != IVSG::status::PASS)
					//		return IRxCalibration::status::FAIL;
					//}

					rxRssiS2dCalParams.vsgStartCWParams.cableLoss = rxRssiS2dCalParams.pathLossParams.outPathLossPerAnt[i_ant];
					rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm = rxRssiS2dCalParams.maxPower[i_reg] + rxRssiS2dCalParams.marginHigh[i_reg] - _gainStepResultsParams.gainSteps.gain[i_ant][rxRssiS2dCalParams.setRxGainBlocksParams.LnaIndex];
					//rxRssiS2dCalParams.vsg->vsgStartCW(rxRssiS2dCalParams.vsgStartCWParams);

					for (int i = 0; i < retry; i++)
					{
						if (m_funcStartRxPower(TRUE, (rxRssiS2dCalParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRssiS2dCalParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
							break;
					}
					ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, gainTemp);
					if (ret != IRxCalibration::status::PASS)
						return IRxCalibration::status::FAIL;

					gain_rssi_v2_signalOn[i_ant] = gainTemp[i_ant];
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> 2_gainTemp[%d] is %d \n", i_ant, gainTemp[i_ant]);
					for (int i = 0; i < retry; i++)
					{
						if (m_funcStopRxPower(TRUE, lpszRunInfo))
							break;
					}
				}
			}

			//if (rxRssiS2dCalParams.vsg->isTransmitting == true)
			//{
			//	IVSG::status vsgError = rxRssiS2dCalParams.vsg->vsgStopCW();
			//	if (vsgError != IVSG::status::PASS)
			//		return IRxCalibration::status::FAIL;
			//}
			for (int i = 0; i < retry; i++)
			{
				if (m_funcStopRxPower(TRUE, lpszRunInfo))
					break;
			}
			ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, gain_rssi_v1_signalOff);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> gain_rssi_v1_signalOff[1] is %d \n", gain_rssi_v1_signalOff[1]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> gain_rssi_v1_signalOff[3] is %d \n", gain_rssi_v1_signalOff[3]);
			if (ret != IRxCalibration::status::PASS)
				return IRxCalibration::status::FAIL;

			rxRssiS2dCalParams.calculateOptimalS2dGainParams.region = i_reg;
			rxRssiS2dCalParams.calculateOptimalS2dGainParams.gainInitFloat[i_reg] = (float)(rxRssiS2dCalParams.setRssiOffsetGainParams.s2dGain[i_reg]);
			copy(begin(gain_rssi_v1_signalOff), end(gain_rssi_v1_signalOff), begin(rxRssiS2dCalParams.calculateOptimalS2dGainParams.V1));
			copy(begin(gain_rssi_v2_signalOn), end(gain_rssi_v2_signalOn), begin(rxRssiS2dCalParams.calculateOptimalS2dGainParams.V2));
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> gain_rssi_v2_signalOn[1] is %d \n", gain_rssi_v2_signalOn[1]);
			//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> gain_rssi_v2_signalOn[3] is %d \n", gain_rssi_v2_signalOn[3]);



			ret = (IRxCalibration::status)_dutFunc->calculateOptimalS2dGain(rxRssiS2dCalParams.calculateOptimalS2dGainParams, s2d_gain_opt_out);
			if (ret != IRxCalibration::status::PASS)
				return IRxCalibration::status::FAIL;

			//STATRT VERIFICATION STAGE
			for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
			{
				if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
				{
					rxRssiS2dCalParams.setRssiOffsetGainParams.antMask = 1 << i_ant;
					rxRssiS2dCalParams.setRssiOffsetGainParams.regionIndex = i_reg;
					rxRssiS2dCalParams.setRssiOffsetGainParams.iOffset[i_reg] = s2d_offset_opt_out[i_ant];
					rxRssiS2dCalParams.setRssiOffsetGainParams.s2dGain[i_reg] = s2d_gain_opt_out[i_ant];
					ret = (IRxCalibration::status)_dutFunc->setRssiOffsetGain(rxRssiS2dCalParams.setRssiOffsetGainParams);
					if (ret != IRxCalibration::status::PASS)
						return IRxCalibration::status::FAIL;
				}
			}

			ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, offTemp);
			if (ret != IRxCalibration::status::PASS)
				return IRxCalibration::status::FAIL;

			for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
			{
				if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
				{
					//if (rxRssiS2dCalParams.vsg->isTransmitting == true)
					//{
					//	IVSG::status vsgError = rxRssiS2dCalParams.vsg->vsgStopCW();
					//	if (vsgError != IVSG::status::PASS)
					//		return IRxCalibration::status::FAIL;
					//}
					rxRssiS2dCalParams.vsgStartCWParams.cableLoss = rxRssiS2dCalParams.pathLossParams.outPathLossPerAnt[i_ant];
					rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm = rxRssiS2dCalParams.maxPower[i_reg] + rxRssiS2dCalParams.marginHigh[i_reg] - _gainStepResultsParams.gainSteps.gain[i_ant][rxRssiS2dCalParams.setRxGainBlocksParams.LnaIndex];
					//rxRssiS2dCalParams.vsg->vsgStartCW(rxRssiS2dCalParams.vsgStartCWParams);
					for (int i = 0; i < retry; i++)
					{
						if (m_funcStartRxPower(TRUE, (rxRssiS2dCalParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRssiS2dCalParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRssiS2dCalParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
							break;
					}
					ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiS2dCalParams.measureRssiParams, gainTemp);
					if (ret != IRxCalibration::status::PASS)
						return IRxCalibration::status::FAIL;

					onTemp[i_ant] = gainTemp[i_ant];
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> 3_gainTemp[%d] is %d \n", i_ant, gainTemp[i_ant]);
					for (int i = 0; i < retry; i++)
					{
						if (m_funcStopRxPower(TRUE, lpszRunInfo))
							break;
					}
				}
			}

			for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
			{
				if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
				{
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "[ming_CAL]=> 4_gainTemp[%d] is %d \n", i_ant, gainTemp[i_ant]);
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "offTemp[%d] is :%d \n", i_ant, offTemp[i_ant]);
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "onTemp[%d] is :%d \n", i_ant, onTemp[i_ant]);
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res is :%f \n", rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res);
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "S2D_OFFSET_MARGIN is :%f \n", S2D_OFFSET_MARGIN);
					double check_case2 = pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) / 2;
					double check_case3 = pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) - S2D_OFFSET_MARGIN;
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "check_case2 :%f \n", check_case2);
					//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "check_case3 :%f \n", check_case3);

					if (offTemp[i_ant] < (0 + S2D_OFFSET_MARGIN))
					{
						//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "failed at case 1 offTemp[%d] is :%d \n", i_ant, offTemp[i_ant]);
						s2d_offset_opt_out[i_ant]--;

						continueLoop = true;
					}
					else if (offTemp[i_ant] > pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) / 2)
					{
						s2d_offset_opt_out[i_ant]++;
						//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "failed at case 2 offTemp[%d] is :%d, pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) / 2 is %f \n", i_ant, offTemp[i_ant], pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) / 2);
						continueLoop = true;
					}


					else if (onTemp[i_ant] > (pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) - S2D_OFFSET_MARGIN))
					{
						s2d_offset_opt_out[i_ant]++;
						//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "failed at case 3 onTemp[%d] is :%d, pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) - S2D_OFFSET_MARGIN is %f \n", i_ant, onTemp[i_ant], pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) - S2D_OFFSET_MARGIN);
						continueLoop = true;
					}
					else if (onTemp[i_ant] < pow(2, rxRssiS2dCalParams.rssiS2dSetInitialsParams.AuxADC_Res) / 2)
					{
						s2d_offset_opt_out[i_ant]--;
						//::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, 5, LOGGER_INFORMATION, "failed at case 4 onTemp[%d] is :%d \n", i_ant, onTemp[i_ant]);
						continueLoop = true;
					}

				}
			}

			if (continueLoop == false)
				break;
			//END VERIFICATION STAGE
		}

		for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
		{
			if ((rxRssiS2dCalParams.antennaMask >> i_ant) & 0x1 > 0)
			{
				_s2dResultsParams.gain[i_ant][i_reg] = s2d_gain_opt_out[i_ant];
			}
		}

		if (continueLoop)
			return IRxCalibration::status::FAIL;

	} //reg loop

	_s2dResultsParams.numberOfRegions = rxRssiS2dCalParams.numberOfRegions;


	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRssiS2dCalParams.antennaMask & (1 << i_ant))
		{
			for (int i_reg = 0; i_reg < _s2dResultsParams.numberOfRegions; i_reg++)
			{
				sprintf(printChar, "ANT: %d REGION:%d  Measure S2D Gain -> :%d", i_ant + 1, i_reg, _s2dResultsParams.gain[i_ant][i_reg]);
				ExportTestLog("[DEBUG_CAL]=> %s", printChar);
				//_logPrint->print(string(printChar));
				sprintf(printChar, "ANT: %d REGION:%d  Measure S2D Offset -> :%d", i_ant + 1, i_reg, _s2dResultsParams.offset[i_ant][i_reg]);
				ExportTestLog("[DEBUG_CAL]=> %s", printChar);
				//_logPrint->print(string(printChar));
			}
		}
	}


	return IRxCalibration::status::PASS;
}


IRxCalibration::status RxCalibration::RxRssiABCal(RxRssiABParams rxRssiABParams)
{
	//_logPrint->print();
	//_logPrint->print();
	//_logPrint->print(string("   AB Calibration"));

	_rxRssiABParams = rxRssiABParams;
	ExportTestLog(" ");
	ExportTestLog("[DEBUG_CAL_RX]=>   AB Calibration");

	//rxRssiABParams.vsg->calcPathLoass(rxRssiABParams.pathLossParams);
	calcPathLoass(rxRssiABParams.pathLossParams);
	ret = (IRxCalibration::status)_dutFunc->setRxGainBlocks(rxRssiABParams.setRxGainBlocksParams);
	if (ret != IRxCalibration::status::PASS)
		return IRxCalibration::status::FAIL;

	for (int i_reg = 0; i_reg < rxRssiABParams.numberOfRegions; i_reg++)
	{
		float Pout[NUM_OF_POWER_POINTS];

		for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
		{
			if ((rxRssiABParams.antennaMask >> i_ant) & 0x1 > 0)
				rxRssiABParams.calculatePVectorParams.LnaGains[i_ant] = _gainStepResultsParams.gainSteps.gain[i_ant][rxRssiABParams.setRxGainBlocksParams.LnaIndex];
		}

		rxRssiABParams.calculatePVectorParams.powerMax = rxRssiABParams.maxPower[i_reg];

		ret = (IRxCalibration::status)_dutFunc->calculatePVector(rxRssiABParams.calculatePVectorParams, Pout);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		RxDutApi::SetRssiOffsetGainParams setRssiOffsetGainParams;
		for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
		{
			if ((rxRssiABParams.antennaMask >> i_ant) & 0x1 > 0)
			{
				setRssiOffsetGainParams.antMask = 1 << i_ant;
				setRssiOffsetGainParams.regionIndex = i_reg;
				copy(begin(_s2dResultsParams.offset[i_ant]), end(_s2dResultsParams.offset[i_ant]), begin(setRssiOffsetGainParams.iOffset));
				copy(begin(_s2dResultsParams.gain[i_ant]), end(_s2dResultsParams.gain[i_ant]), begin(setRssiOffsetGainParams.s2dGain));

				ret = (IRxCalibration::status)_dutFunc->setRssiOffsetGain(setRssiOffsetGainParams);
				if (ret != IRxCalibration::status::PASS)
					return IRxCalibration::status::FAIL;
			}
		}

		//if (rxRssiABParams.vsg->isTransmitting == true)
		//{
		//	IVSG::status vsgError = rxRssiABParams.vsg->vsgStopCW();
		//	if (vsgError != IVSG::status::PASS)
		//		return IRxCalibration::status::FAIL;
		//}
		int retry = 3;
		char lpszRunInfo[512];
		for (int i = 0; i < retry; i++)
		{
			if (m_funcStopRxPower(TRUE, lpszRunInfo))
				break;
		}

		signed short rssi_c_signalOff[MAX_ANTENNA_NUMBER];
		signed short rssi_v_signalOn[MAX_ANTENNA_NUMBER];

		ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiABParams.measureRssiParams, rssi_c_signalOff);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;


		RxDutApi::CalculateLinearRegressionParams calculateLinearRegressionParams;

		for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
		{
			if ((rxRssiABParams.antennaMask >> i_ant) & 0x1 > 0)
			{
				for (int i_power = 0; i_power < rxRssiABParams.calculatePVectorParams.numPwrPoints; i_power++)
				{
					//if (rxRssiABParams.vsg->isTransmitting == true)
					//{
					//	IVSG::status vsgError = rxRssiABParams.vsg->vsgStopCW();
					//	if (vsgError != IVSG::status::PASS)
					//		return IRxCalibration::status::FAIL;
					//}

					//rxRssiABParams.vsgStartCWParams.cableLoss = rxRssiABParams.pathLossParams.outPathLossPerAnt[i_ant];
					rxRssiABParams.vsgStartCWParams.rfPowerLeveldBm = Pout[i_power];
					//rxRssiABParams.vsg->vsgStartCW(rxRssiABParams.vsgStartCWParams);

					for (int i = 0; i < retry; i++)
					{
						if (m_funcStartRxPower(TRUE, (rxRssiABParams.vsgStartCWParams.loFreqMHz * 1E6) + (rxRssiABParams.vsgStartCWParams.offsetFrequencyMHz * 1E6), 0, rxRssiABParams.vsgStartCWParams.rfPowerLeveldBm, lpszRunInfo))
							break;
					}

					ret = (IRxCalibration::status)_dutFunc->measureRssi(rxRssiABParams.measureRssiParams, rssi_v_signalOn);
					if (ret != IRxCalibration::status::PASS)
						return IRxCalibration::status::FAIL;

					for (int i = 0; i < retry; i++)
					{
						if (m_funcStopRxPower(TRUE, lpszRunInfo))
							break;
					}
					calculateLinearRegressionParams.V[i_ant][i_power] = static_cast<float>(rssi_v_signalOn[i_ant]);
				}
			}
		}

		Dut_Rssi_AB ab[MAX_ANTENNA_NUMBER];

		copy(begin(Pout), end(Pout), begin(calculateLinearRegressionParams.Pout));
		copy(begin(rxRssiABParams.calculatePVectorParams.LnaGains), end(rxRssiABParams.calculatePVectorParams.LnaGains), begin(calculateLinearRegressionParams.LnaGains));
		copy(begin(rssi_c_signalOff), end(rssi_c_signalOff), begin(calculateLinearRegressionParams.C));

		ret = (IRxCalibration::status)_dutFunc->calculateLinearRegression(calculateLinearRegressionParams, ab);
		if (ret != IRxCalibration::status::PASS)
			return IRxCalibration::status::FAIL;

		//Dut_RssiGetMaxAbError
		for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
		{
			_abResultsParams.ab[i_ant][i_reg] = ab[i_ant];
		}
	}  //reg loop

	//if (rxRssiABParams.vsg->isTransmitting == true)
	//{
	//	IVSG::status vsgError = rxRssiABParams.vsg->vsgStopCW();
	//	if (vsgError != IVSG::status::PASS)
	//		return IRxCalibration::status::FAIL;
	//}

	for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
	{
		if (rxRssiABParams.antennaMask & (1 << i_ant))
		{
			for (int i_reg = 0; i_reg < rxRssiABParams.numberOfRegions; i_reg++)
			{
				sprintf(printChar, "[DEBUG_CAL]=> ANT: %d REGION:%d  Measure A -> :%0.2f", i_ant + 1, i_reg, _abResultsParams.ab[i_ant][i_reg].a);
				ExportTestLog(printChar);
				//_logPrint->print(string(printChar));
				sprintf(printChar, "[DEBUG_CAL]=> ANT: %d REGION:%d  Measure B -> :%0.2f", i_ant + 1, i_reg, _abResultsParams.ab[i_ant][i_reg].b);
				ExportTestLog(printChar);
				//_logPrint->print(string(printChar));
			}
		}
	}
	return IRxCalibration::status::PASS;
}

IRxCalibration::status RxCalibration::RxRssiS2dFlatnessCal(RxRssiS2dFlatnessParams rxRssiS2dFlatnessParams)
{
	return IRxCalibration::status::PASS;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//*****************  Get functions  ***********************//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

IRxCalibration::MidGainResultsParams RxCalibration::getMidGainResults()
{
	return _midGainResultsParams;
}

IRxCalibration::SubBandResultsParams RxCalibration::getSubBandResults()
{
	return _subBandResultsParams;
}

IRxCalibration::GainStepResultsParams RxCalibration::getGainStepResults()
{
	return _gainStepResultsParams;
}

IRxCalibration::FlatnessResultsParams RxCalibration::getFlatnessResults()
{
	return _flatnessResultsParams;
}

IRxCalibration::S2dResultsParams RxCalibration::getS2dResults()
{
	return _s2dResultsParams;
}

IRxCalibration::AbResultsParams RxCalibration::getAbResults()
{
	return _abResultsParams;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//* Set functions (For default values, important for GEN5) *//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

IRxCalibration::MidGainResultsParams RxCalibration::setMidGainResults(IRxCalibration::MidGainResultsParams midGainResultsParams)
{
	_midGainResultsParams = midGainResultsParams;
	return _midGainResultsParams;
}

IRxCalibration::SubBandResultsParams RxCalibration::setSubBandResults(IRxCalibration::SubBandResultsParams subBandResultsParams)
{
	_subBandResultsParams = subBandResultsParams;
	return _subBandResultsParams;
}

IRxCalibration::GainStepResultsParams RxCalibration::setGainStepResults(IRxCalibration::GainStepResultsParams gainStepResultsParams)
{
	_gainStepResultsParams = gainStepResultsParams;
	return _gainStepResultsParams;
}

IRxCalibration::FlatnessResultsParams RxCalibration::setFlatnessResults(IRxCalibration::FlatnessResultsParams flatnessResultsParams)
{
	_flatnessResultsParams = flatnessResultsParams;
	return _flatnessResultsParams;
}

IRxCalibration::S2dResultsParams RxCalibration::setS2dResults(IRxCalibration::S2dResultsParams s2dResultsParams)
{
	_s2dResultsParams = s2dResultsParams;
	return _s2dResultsParams;
}

IRxCalibration::AbResultsParams RxCalibration::setAbResults(IRxCalibration::AbResultsParams abResultsParams)
{
	_abResultsParams = abResultsParams;
	return _abResultsParams;
}




























































































































































































