#include "..\stdafx.h"
#include "RxDutApi.h"
#include <algorithm>
#include <numeric>

RxDutApi::RxDutApi(dutWrapper *dutApi) : _dutApi(dutApi) {}


RxDutApi::~RxDutApi() {}

RxDutApi::status RxDutApi::startRxCalibration()
{
	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(0);
	
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	Sleep(10);

	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(1);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	Sleep(10);

	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(0);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	
	Sleep(10);

	ret = (RxDutApi::status)_dutApi->Dut_StartRSSI();
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::endRxCalibration()
{

	ret = (RxDutApi::status)_dutApi->Dut_EndRSSI();
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(1);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::setRxGainBlocks(const SetRxGainBlocksParams &setRxGainBlocksParams)
{
	ret = (RxDutApi::status)_dutApi->Dut_Set_Rx_Gain_Blocks(setRxGainBlocksParams.LnaIndex, setRxGainBlocksParams.pgc1, setRxGainBlocksParams.pgc2, setRxGainBlocksParams.pgc3);
	if (debug == "TRUE")
		ExportTestLog("API Flow: setRxGainBlocks => LnaIndex:%d., pgc1:%d,  pgc2:%d,  pgc3:%d", setRxGainBlocksParams.LnaIndex, setRxGainBlocksParams.pgc1, setRxGainBlocksParams.pgc2, setRxGainBlocksParams.pgc3);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	//Sleep(1000);
	//system("pause");
	Sleep(2); //delay for IIR stabilization
	return ret;
}

RxDutApi::status RxDutApi::setRssiOffsetGain(const SetRssiOffsetGainParams &setRssiOffsetGainParams)
{
	int reg = setRssiOffsetGainParams.regionIndex;
	return (RxDutApi::status)_dutApi->Dut_Set_RSSI_offset_gain(setRssiOffsetGainParams.iOffset[reg], setRssiOffsetGainParams.s2dGain[reg], setRssiOffsetGainParams.antMask, reg);
}


RxDutApi::status RxDutApi::rssiS2dSetInitials(RssiS2dSetInitialsParams &rssiS2dSetInitialsParams)
{
	return (RxDutApi::status)_dutApi->Dut_RssiS2dSetInitials(rssiS2dSetInitialsParams.numLnaGains, rssiS2dSetInitialsParams.numRegions, rssiS2dSetInitialsParams.lnaGain, rssiS2dSetInitialsParams.AuxADC_Res, rssiS2dSetInitialsParams.Ioffs_Step);
}

RxDutApi::status RxDutApi::measureRssi(const MeasureRssiParams &measureRssiParams, MT_INT16 rssi[MAX_ANTENNA_NUMBER])
{
	return (RxDutApi::status)_dutApi->Dut_Measure_RSSI(measureRssiParams.Method, measureRssiParams.NOS, rssi);
}

RxDutApi::status RxDutApi::calculateOptimalIoffs(CalculateOptimalIoffsParams calculateOptimalIoffs, int s2d_offset_opt_out[MAX_ANTENNA_NUMBER])
{
	return (RxDutApi::status)_dutApi->Dut_Calculate_optimal_Ioffs(calculateOptimalIoffs.region, calculateOptimalIoffs.V1, calculateOptimalIoffs.V2, calculateOptimalIoffs.s2d_gain_db, calculateOptimalIoffs.s2d_offset_db, s2d_offset_opt_out);
}

RxDutApi::status RxDutApi::calculateOptimalS2dGain(CalculateOptimalS2dGainParams calculateOptimalS2dGainParams, int s2d_gain_opt_out[MAX_ANTENNA_NUMBER])
{
	return (RxDutApi::status)_dutApi->Dut_Calculate_Optimal_S2Dgain(calculateOptimalS2dGainParams.region, calculateOptimalS2dGainParams.V1, calculateOptimalS2dGainParams.V2, calculateOptimalS2dGainParams.gainInitFloat, s2d_gain_opt_out);
}

RxDutApi::status RxDutApi::calculatePVector(CalculatePVectorParams calculatePVectorParams, float Pout[NUM_OF_POWER_POINTS])
{
	return (RxDutApi::status)_dutApi->Dut_Calculate_P_Vector(calculatePVectorParams.numPwrPoints, calculatePVectorParams.powerMax, calculatePVectorParams.LnaGains, calculatePVectorParams.lowerP_Boundary, calculatePVectorParams.upperP_Boundary, Pout);
}

RxDutApi::status RxDutApi::calculateLinearRegression(CalculateLinearRegressionParams calculateLinearRegressionParams, Dut_Rssi_AB ab[MAX_ANTENNA_NUMBER])
{
	return (RxDutApi::status)_dutApi->Dut_Calculate_Linear_Regression(calculateLinearRegressionParams.Pout, calculateLinearRegressionParams.V, calculateLinearRegressionParams.C, calculateLinearRegressionParams.LnaGains, ab);
}

RxDutApi::status RxDutApi::getRecivedGoodPackets(int &recivedGoodPackets)
{
	DWORD pPackets[10];

	ret = (RxDutApi::status)_dutApi->Dut_PhyPacketsCounter(pPackets);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	recivedGoodPackets = static_cast<int>(pPackets[0] - pPackets[1]);

	return RxDutApi::status::PASS;
}

double RxDutApi::CalcAverageOfVectorLin(const vector<double>& v)
{
	return (accumulate(v.begin(), v.end(), 0.0) / v.size());
}

int RxDutApi::CalcAverageOfVectorLin(const vector<int>& v)
{
	return static_cast<int>(accumulate(v.begin(), v.end(), 0.0) / v.size());
}

double RxDutApi::CalcAverageOfVectorLog(const vector<double>& v)
{
	vector<double> v_temp(v.begin(), v.end());
	std::transform(v_temp.begin(), v_temp.end(), v_temp.begin(), [](double x) -> double {return pow(10.0, x / 10.0); });
	double linAverage = CalcAverageOfVectorLin(v_temp);
	double average = 10.0 * log10(linAverage);
	return average;
}

RxDutApi::status RxDutApi::getEvmVector(vector<double> &evmVector)
{
	evmVector.resize(4);
	BYTE *rxEvmArry = new BYTE[4];
	vector<double> rxEvmVectorForAverage[4];

	for (int i_rssi = 0; i_rssi < EVM_NUM_SAMPLES; i_rssi++)
	{
		ret = (RxDutApi::status)_dutApi->Dut_ReadRxEvm(rxEvmArry);
		if (ret != RxDutApi::status::PASS)
			return RxDutApi::status::FAIL;

		for (int i_ant1 = 0; i_ant1 < 4; i_ant1++)
		{
			rxEvmVectorForAverage[i_ant1].push_back(static_cast<double>(rxEvmArry[i_ant1]));
		}
		Sleep(5);
	}

	for (int i_ant = 0; i_ant < 4; i_ant++)
	{
		evmVector[i_ant] = CalcAverageOfVectorLog(rxEvmVectorForAverage[i_ant]);
	}
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::getRssiVector(vector<double> &rssiVector)
{	
	double pRSSIVector_temp[4];
	double pRSSIVectorAverage[4];
	vector<double> rxRssiVectorForAverage[4];
	rssiVector.resize(4);

	for (int i_rssi = 0; i_rssi < RSSI_NUM_SAMPLES ; i_rssi++)
	{	
		ret = (RxDutApi::status)_dutApi->Dut_RSSIVector(pRSSIVector_temp);
			if (ret != RxDutApi::status::PASS)
				return RxDutApi::status::FAIL;

		Sleep(5);

		for (int i_ant1 = 0; i_ant1 < 4; i_ant1++)
		{
			rxRssiVectorForAverage[i_ant1].push_back(pRSSIVector_temp[i_ant1]);
		}
	}


	for (int i_ant = 0; i_ant < 4; i_ant++)
	{
		pRSSIVectorAverage[i_ant] = CalcAverageOfVectorLog(rxRssiVectorForAverage[i_ant]);
		vector<double> v(rxRssiVectorForAverage[i_ant].begin(), rxRssiVectorForAverage[i_ant].end());
		for (int i = 0; i < v.size(); i++)
		{
			v[i] = abs(rxRssiVectorForAverage[i_ant].at(i) - pRSSIVectorAverage[i_ant]);
		}

		for (int i = 0; i < round(RSSI_NUM_SAMPLES*0.2); i++)
		{
			auto maxIndex = std::max_element(std::begin(v), std::end(v));
			int eraseIndex = static_cast<int>(maxIndex - std::begin(v));
			v.erase(v.begin() + eraseIndex);
			rxRssiVectorForAverage[i_ant].erase(rxRssiVectorForAverage[i_ant].begin() + eraseIndex);		
		}

		rssiVector[i_ant] = CalcAverageOfVectorLog(rxRssiVectorForAverage[i_ant]);
	}

	return RxDutApi::status::PASS;
}


RxDutApi::status RxDutApi::resetPacketsCounter()
{
	return (RxDutApi::status)_dutApi->Dut_ResetPacketsCounter();
}



RxDutApi::status RxDutApi::TurnOnAllTxRxAntennas(int antennaMask)
{
	ret = (RxDutApi::status)_dutApi->Dut_SetTxAntennasMask(antennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetTxAntennasMask => %d.", antennaMask);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);

	ret = (RxDutApi::status)_dutApi->Dut_SetRxAntennasMask(antennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetRxAntennasMask => %d.", antennaMask);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);

	ret = (RxDutApi::status)_dutApi->Dut_SetTxAntenna(antennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetTxAntenna => %d.", antennaMask);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);

	ret = (RxDutApi::status)_dutApi->Dut_SetRxAntenna(antennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetRxAntenna => %d.", antennaMask);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::rxSetChannel(RxSetChannelParams setChannelParams)
{

	int lowChannel = CCommFunc::ConvertLoFreqtoLowChannel(setChannelParams.bandwidth, setChannelParams.freq);
	int bandwidth = CCommFunc::ConvertBandwidth(setChannelParams.bandwidth);
	int phyType = CCommFunc::ConvertStandardToPhyType(setChannelParams.freq, setChannelParams.standard);
	int band = CCommFunc::ConvertFreqToBand(setChannelParams.freq);
	int isCloseLoop = setChannelParams.isCloseLoop;
	int offlineCalMask = setChannelParams.offlineCalMask;

	ret = (RxDutApi::status)TurnOnAllTxRxAntennas(setChannelParams.boardAntennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow Interface: TurnOnAllTxRxAntennas");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	if (phyType == 1)
	{
		ret = (RxDutApi::status)_dutApi->Dut_EndTxPackets();
		if (debug == "TRUE")
			ExportTestLog("API Flow: Dut_EndTxPackets().");
		if (RxDutApi::status::PASS != ret)
			return (RxDutApi::status::FAIL);
	}
	//Dov
	ret = (RxDutApi::status)_dutApi->Dut_SetHdkConfigure(offlineCalMask, 0);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetHdkConfigure => offlineCalMask:%s, 0).", offlineCalMask);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetChannel(phyType, band, bandwidth, lowChannel, offlineCalMask, setChannelParams.isCloseLoop);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetChannel => phyType: %d, band: %d, bandwidth: %d, lowChannel: %d, offlineCalMask: %d, isCloseLoop: %d", phyType, band, bandwidth, lowChannel, offlineCalMask, setChannelParams.isCloseLoop);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	Sleep(setChannelParams.sleepAfterSetChannel);  

	ret = (RxDutApi::status)_dutApi->Dut_SetTxAntenna(0);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetTxAntenna => 0.");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetRxAntenna(setChannelParams.rxAntennaMask);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetRxAntenna => rxAntennaMask: %d", setChannelParams.rxAntennaMask);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	return RxDutApi::status::PASS;
}


RxDutApi::status RxDutApi::fastRxSetChannel(RxSetChannelParams setChannelParams)
{

	ret = (RxDutApi::status)_dutApi->Dut_EndRSSI();
	if(debug == "TRUE")
	   ExportTestLog("API Flow: Dut_EndRSSI().");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(1);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetRiscMode(1).");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetHdkConfigure(0, 0);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetHdkConfigure(0, 0).");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	
	ret = rxSetChannel(setChannelParams);
	if (debug == "TRUE")
		ExportTestLog("API Flow Interface : rxSetChannel");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_SetHdkConfigure(0xFFFF, 0);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetHdkConfigure(0xFFFF, 0)");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	RxCalregSetting.clear();
	regSetting_t m_regSetting = { 3, 2696282708, 1, 1 };
	RxCalregSetting.push_back(m_regSetting);
	regSetting_t m_regSetting2 = { 3, 2696299092, 1, 1 };
	RxCalregSetting.push_back(m_regSetting2);
	regSetting_t m_regSetting3 = { 3, 2696315476, 1, 1 };
	RxCalregSetting.push_back(m_regSetting3);
	regSetting_t m_regSetting4 = { 3, 2696331860, 1, 1 };
	RxCalregSetting.push_back(m_regSetting4);
	regSetting_t m_regSetting5 = { 3, 2696282712, 0, 128 };
	RxCalregSetting.push_back(m_regSetting5);
	regSetting_t m_regSetting6 = { 3, 2696299096, 0, 128 };
	RxCalregSetting.push_back(m_regSetting6);
	regSetting_t m_regSetting7 = { 3, 2696315480, 0, 128 };
	RxCalregSetting.push_back(m_regSetting7);
	regSetting_t m_regSetting8 = { 3, 2696331864, 0, 128 };
	RxCalregSetting.push_back(m_regSetting8);

	for (int i = 0; i < RxCalregSetting.size(); i++)
	{
	   ret = (RxDutApi::status)_dutApi->Dut_WriteReg(RxCalregSetting.at(i).module, RxCalregSetting.at(i).address, RxCalregSetting.at(i).pData, RxCalregSetting.at(i).mask);
	   if (debug == "TRUE")
		   ExportTestLog("API Flow: Dut_WriteReg => module: %d, address: %lu, pData: %lu, mask: %lu", RxCalregSetting.at(i).module, RxCalregSetting.at(i).address, RxCalregSetting.at(i).pData, RxCalregSetting.at(i).mask);
	   if (ret != RxDutApi::status::PASS)
	      return RxDutApi::status::FAIL;
	}

	ret = (RxDutApi::status)_dutApi->Dut_SetRiscMode(0);
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_SetRiscMode(0)");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	ret = (RxDutApi::status)_dutApi->Dut_StartRSSI();
	if (debug == "TRUE")
		ExportTestLog("API Flow: Dut_StartRSSI()");
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::MeasureLNASubBandGains(int referanceFreq)
{
	//DutApiCorrResults_t measureCorrResult[4][8]; //for 610.36
	DutApiCorrResults_t measureCorrResult[4][16];  //for 610.50
	//for Debug
	//for (;;)
	//{
	//	char tmp[1024];
	//	double tt = 5531.25 * 1E6, power = -28, offset=1.25 * 1E6;
	//
	//	m_funcStopRxPower(TRUE, tmp);
	//	m_funcStartRxPower(TRUE, tt, 0, power, tmp);
	//	ret = (RxDutApi::status)_dutApi->Dut_Measure_LNA_Sub_Band_Gains(measureCorrResult);
	//	if (ret != RxDutApi::status::PASS)
	//		return RxDutApi::status::FAIL;
	//	m_funcStopRxPower(TRUE, tmp);
	//}
	ret = (RxDutApi::status)_dutApi->Dut_Measure_LNA_Sub_Band_Gains(measureCorrResult);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	if (subBandDebug == "TRUE")
	{
		ExportTestLog("referanceFreq : %d", referanceFreq);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				//int II;
				//int QQ;
				//int IQ;
				ExportTestLog("Dut_Measure_LNA_Sub_Band_Gains => ANT: %d, II: %d, QQ: %d, IQ: %d", i, measureCorrResult[i][j].II, measureCorrResult[i][j].QQ, measureCorrResult[i][j].IQ);
			}
		}
	}
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::calculateLNASubBandGains(unsigned int rxBandFreq[MAX_ANTENNA_NUMBER][MAX_SUB_BAND_CROS_POINT])
{
	ret = (RxDutApi::status)_dutApi->Dut_calculate_LNA_Sub_Band_Gains(rxBandFreq);
		if (ret != RxDutApi::status::PASS)
			return RxDutApi::status::FAIL;
	return RxDutApi::status::PASS;
}


RxDutApi::status RxDutApi::setRxBandLUT(unsigned int rxBandFreq[MAX_ANTENNA_NUMBER][MAX_SUB_BAND_CROS_POINT_LUT])
{
	unsigned int sant[4] = {'\0'};
	ret = (RxDutApi::status)_dutApi->Dut_Set_Rx_Band_LUT(rxBandFreq, sant);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::midGainCalibrate(MidGainCalibrateParams midGainCalibrateParams, float targetMeasGain[MAX_ANTENNA_NUMBER], int targetMeasControl[MAX_ANTENNA_NUMBER])
{
	ret = (RxDutApi::status)_dutApi->Dut_MidGainCalibrate(midGainCalibrateParams.targetGain, midGainCalibrateParams.Prxin, midGainCalibrateParams.Pgc1, midGainCalibrateParams.Pgc2, targetMeasGain, targetMeasControl);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	return RxDutApi::status::PASS;
}

//Dut_Measure_CW_Power(int Method, int FreqOffset, int NOS, DutApiCorrResults_t *corrResultsOut);
RxDutApi::status RxDutApi::MeasureCwPower(MeasureCwPowerParams measureCwPowerParams, DutApiCorrResults_t * corrResultsOut)
{
	ret = (RxDutApi::status)_dutApi->Dut_Measure_CW_Power(measureCwPowerParams.Method, measureCwPowerParams.FreqOffset, measureCwPowerParams.NOS, corrResultsOut);
		if (ret != RxDutApi::status::PASS)
			return RxDutApi::status::FAIL;
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::CalculateLnaGainSteps(CalculateLnaGainStepsParams &calculateLnaGainStepsParams, GainPerAntPerStep_t * gainPerAntPerStep_p)
{
	ret = (RxDutApi::status)_dutApi->Dut_Calculate_LNA_Gain_Steps(
		&calculateLnaGainStepsParams.corrResultsPerAntPerStep_p, 
		calculateLnaGainStepsParams.lnaGainStep, calculateLnaGainStepsParams.Prxin, 
		calculateLnaGainStepsParams.Pgc1, calculateLnaGainStepsParams.Pgc2, calculateLnaGainStepsParams.Pgc3, 
		gainPerAntPerStep_p);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;
	return RxDutApi::status::PASS;
}

RxDutApi::status RxDutApi::CalculateRxRfFlatness(CalculateRxRfFlatnessParams & calculateRxRfFlatnessParams, float rf_flatness_delta_gain_results[MAX_ANTENNA_NUMBER][MAX_FLATNESS_POINTS])
{
	ret = (RxDutApi::status)_dutApi->Dut_Calculate_RX_RF_Flatness(calculateRxRfFlatnessParams.P_Adc_array, 
		calculateRxRfFlatnessParams.refGain, calculateRxRfFlatnessParams.PGC1, calculateRxRfFlatnessParams.PGC2, calculateRxRfFlatnessParams.PGC3, 
		calculateRxRfFlatnessParams.Pin, rf_flatness_delta_gain_results);
	if (ret != RxDutApi::status::PASS)
		return RxDutApi::status::FAIL;

	return RxDutApi::status::PASS;
}



RxDutApi::status RxDutApi::writeRssiCalData(const Dut_RssiCalDataStructVer6 * CalArray, int ArraySize)
{
	Dut_RssiCalDataStructVer6 *OutCalArray = new Dut_RssiCalDataStructVer6[ArraySize];

	ret = (RxDutApi::status)_dutApi->Dut_Burn_Rssi_Cal_Data(CalArray, OutCalArray, ArraySize);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);

	delete OutCalArray;

	return (RxDutApi::status::PASS);
}

RxDutApi::status RxDutApi::writeRssiCalData(const Dut_RssiCalDataStructVer5 * CalArray, int ArraySize)
{
	Dut_RssiCalDataStructVer5 *OutCalArray = new Dut_RssiCalDataStructVer5[ArraySize];

	ret = (RxDutApi::status)_dutApi->Dut_Burn_Rssi_Cal_Data(CalArray, OutCalArray, ArraySize);
	if (RxDutApi::status::PASS != ret)
		return (RxDutApi::status::FAIL);

	delete OutCalArray;

	return (RxDutApi::status::PASS);
}


RxDutApi::status RxDutApi::set11bRxAntennaMask(int rx1, int rx2, int rx3, int rx4, bool isGen6)
{
	if (isGen6 == false)
	{
		int err = 0;
		DWORD reg = 0, ant_reg_11b = 0;

		if (rx1 == 1)	   ant_reg_11b = 0x00;
		else if (rx2 == 1) ant_reg_11b = 0x10;
		else if (rx3 == 1) ant_reg_11b = 0x20;
		else if (rx4 == 1) ant_reg_11b = 0x30;

		ret = (RxDutApi::status)_dutApi->Dut_ReadReg(1, 0xAF310364, &reg, 0xFFFFFFFF);
		if (ret != RxDutApi::status::PASS)
			return RxDutApi::status::FAIL;

		reg = (reg & 0xFFFFFFCE) | ant_reg_11b | 0x01;

		ret = (RxDutApi::status)_dutApi->Dut_WriteReg(1, 0xAF310364, reg, 0xFFFFFFFF);
		if (ret != RxDutApi::status::PASS)
			return RxDutApi::status::FAIL;
	}
	else
	{

	}
	return RxDutApi::status::PASS;
}