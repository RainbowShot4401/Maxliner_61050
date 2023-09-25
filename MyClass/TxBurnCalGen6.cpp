#include "TxBurnCalGen6.h"
#include <algorithm> 

//vector<ITxBurnCal::FillTxStructParams> TxBurnCalGen6::calDataStructVer6;
//Dut_CalDataStructVer6 TxBurnCalGen6::dutCalDataStructVer6[MAX_BURN_STRUCT_SIZE] = { 0 };
//float TxBurnCalGen6::ABErrors[MAX_BURN_STRUCT_SIZE][MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER] = { 0 };

#if 1

TxBurnCalGen6::TxBurnCalGen6(CTxControl* dutFunc, dutWrapper* dutApi)
{
	_dutFunc = dutFunc;
	_dutApi = dutApi;
}

TxBurnCalGen6::~TxBurnCalGen6()
{
}

vector<Dut_XY> TxBurnCalGen6::calcPointsForRegion(const vector<int>& startStopRegionIndex, const ITxCalibration::OpenLoopSweepResultsParams& OpenLoopSweepResultsParams)
{
	vector<Dut_XY> regionPoints;
	Dut_XY xy;
	ITxCalibration::OpenLoopSweepResultsParams _openLoopSweepResultsParams = OpenLoopSweepResultsParams;
	int start = startStopRegionIndex[0];
	int end = startStopRegionIndex[1];

	int it_start = abs(distance(_openLoopSweepResultsParams.indexes.begin(), find_if(_openLoopSweepResultsParams.indexes.begin(), _openLoopSweepResultsParams.indexes.end(), [start](int i) {return (i >= start); })));
	int it_end = abs(distance(_openLoopSweepResultsParams.indexes.rend(), find_if(_openLoopSweepResultsParams.indexes.rbegin(), _openLoopSweepResultsParams.indexes.rend(), [end](int i) {return (i <= end); })));

	for (int i = it_start; i < it_end; i++)
	{
		xy.y = _openLoopSweepResultsParams.power[i];
		xy.x = _openLoopSweepResultsParams.voltage[i];
		regionPoints.push_back(xy);
	}

	return regionPoints;
}

ITxBurnCal::Status TxBurnCalGen6::convertToDutStruct()
{
	//int testNlog = 0; 

	int channelIndex = calDataStructVer6.size() - 1;
	ITxBurnCal::FillTxStructParams iqliteTxStruct = calDataStructVer6.at(channelIndex);
	dutCalDataStructVer6[channelIndex].antMask = iqliteTxStruct.txSetChannelParams.boardAntennaMask;
	dutCalDataStructVer6[channelIndex].band    = CCommFunc::ConvertFreqToBand(iqliteTxStruct.txSetChannelParams.freq);
	dutCalDataStructVer6[channelIndex].bw      = CCommFunc::ConvertBandwidthForBurn(iqliteTxStruct.txSetChannelParams.bandwidth, iqliteTxStruct.txSetChannelParams.standard);
	dutCalDataStructVer6[channelIndex].channel = CCommFunc::ConvertLoFreqtoLowChannel(iqliteTxStruct.txSetChannelParams.bandwidth, iqliteTxStruct.txSetChannelParams.freq);

	for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
	{
		if (((dutCalDataStructVer6[channelIndex].antMask >> i_ant) & 0x1) == 1)
		{
			dutCalDataStructVer6[channelIndex].numRegions = iqliteTxStruct.txS2dParams[i_ant].numberOfRegions;
			dutCalDataStructVer6[channelIndex].ants[i_ant].maxPower = iqliteTxStruct.maxPowerParams[i_ant].maxPower_Index;
			//		dutCalDataStructVer6[channelIndex].ants[i_ant].nLog = testNlog;//to do: read from API
			dutCalDataStructVer6[channelIndex].ants[i_ant].nLog = iqliteTxStruct.aBCoefficientsParams[i_ant].nlog;
			vector<int> regions = iqliteTxStruct.txS2dParams[i_ant].regionIndexes;
			vector<Dut_XY> regionPowerPoints;

			for (int i_reg = 0; i_reg < iqliteTxStruct.txS2dParams[i_ant].numberOfRegions; i_reg++)
			{
				int isFirst = (i_reg > 0) ? 1 : 0;
				regionPowerPoints = calcPointsForRegion({ isFirst * regions[isFirst * (i_reg - 1)], regions[i_reg] - 1 }, iqliteTxStruct.OpenLoopSweepResultsParams[i_ant]);

				copy(begin(regionPowerPoints), end(regionPowerPoints),
					begin(dutCalDataStructVer6[channelIndex].ants[i_ant].pointsPerRegion[i_reg]));

				dutCalDataStructVer6[channelIndex].ants[i_ant].numPointsVector[i_reg] = regionPowerPoints.size();

				regionPowerPoints.clear();
			}
			copy(begin(iqliteTxStruct.txS2dParams[i_ant].gain), end(iqliteTxStruct.txS2dParams[i_ant].gain),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].s2dGain));

			copy(begin(iqliteTxStruct.txS2dParams[i_ant].offset), end(iqliteTxStruct.txS2dParams[i_ant].offset),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].s2dOffset));

			dutCalDataStructVer6[channelIndex].ants[i_ant].uEvm = iqliteTxStruct.ultimateEvmParams[i_ant].ultimateEvmPower_Index;
			dutCalDataStructVer6[channelIndex].ants[i_ant].uEvmGain = iqliteTxStruct.txGainTableParams[i_ant].offsetGain;
		}
	}

	CTxControl::Status dutError = (CTxControl::Status)_dutFunc->getABPoints(&(dutCalDataStructVer6[channelIndex]));
	if (CTxControl::Status::PASS != dutError)
		return ITxBurnCal::Status::FAIL;

	float channelError[MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER] = { 0 };
	dutError = (CTxControl::Status)_dutFunc->txGetAbErrors(channelError);
	if (CTxControl::Status::PASS != dutError)
		return ITxBurnCal::Status::FAIL;

	memcpy(ABErrors[channelIndex], channelError, sizeof channelError);

	return ITxBurnCal::Status::PASS;
}

ITxBurnCal::Status TxBurnCalGen6::fillTxStruct(const ITxBurnCal::FillTxStructParams& fillTxStructParams)
{
	//ITxBurnCal::FillTxStructParams tempfillTxStructParams = fillTxStructParams;
	TxBurnCalGen6::calDataStructVer6.emplace_back(fillTxStructParams);
	convertToDutStruct();
	return ITxBurnCal::Status::PASS;
}

ITxBurnCal::Status TxBurnCalGen6::clearTxStruct()
{
	TxBurnCalGen6::calDataStructVer6.clear();
	std::copy(begin(_dutCalDataStructVer6), end(_dutCalDataStructVer6), begin(dutCalDataStructVer6));
	return ITxBurnCal::Status::PASS;
}

void TxBurnCalGen6::dutFuncSet(CTxControl* var)
{
	_dutFunc = var;
}

int TxBurnCalGen6::getNumberOfChannels()
{
	return calDataStructVer6.size();
}

float*** TxBurnCalGen6::getABError()
{
	return (float***)ABErrors;
}

void TxBurnCalGen6::printResults()
{
	for (int i_channel = 0; i_channel < getNumberOfChannels(); i_channel++)
	{
		string s = "Channel: " + to_string(dutCalDataStructVer6[i_channel].channel) + " BW: " + to_string(dutCalDataStructVer6[i_channel].bw);
		ExportTestLog("%s", s);
		//_logPrint->print(s);
		for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
		{
			if (((dutCalDataStructVer6[i_channel].antMask >> i_ant) & 1) > 0)
			{
				for (int i_reg = 0; i_reg < dutCalDataStructVer6[i_channel].numRegions; i_reg++)
				{
					s = "ANT: " + to_string(i_ant) + " REG: " + to_string(i_reg) +
						" ->A: " + to_string(dutCalDataStructVer6[i_channel].ants[i_ant].pointsAB[i_reg].a) +
						" B: " + to_string(dutCalDataStructVer6[i_channel].ants[i_ant].pointsAB[i_reg].b) +
						" Gain: " + to_string(dutCalDataStructVer6[i_channel].ants[i_ant].s2dGain[i_reg]) +
						" Offset: " + to_string(dutCalDataStructVer6[i_channel].ants[i_ant].s2dOffset[i_reg]);
					ExportTestLog("%s", s);
				}
			}
		}
	}
}

ITxBurnCal::Status TxBurnCalGen6::writeTssiCalData()
{
	CTxControl::Status dutError = (CTxControl::Status)_dutFunc->writeTssiCalData(dutCalDataStructVer6, calDataStructVer6.size());
	if (CTxControl::Status::PASS != dutError)
		return ITxBurnCal::Status::FAIL;

	return ITxBurnCal::Status::PASS;
}

ITxBurnCal::Status TxBurnCalGen6::WriteProdFlag(bool flag)
{
	CTxControl::Status dutError = (CTxControl::Status)_dutFunc->WriteProdFlag(flag);
	if (CTxControl::Status::PASS != dutError)
		return ITxBurnCal::Status::FAIL;
	return ITxBurnCal::Status::PASS;
}

ITxBurnCal::Status TxBurnCalGen6::writeToNvMemory()
{
	CTxControl::Status dutError = (CTxControl::Status)_dutFunc->flushNvMemory();
	if (CTxControl::Status::PASS != dutError)
		return ITxBurnCal::Status::FAIL;

	return ITxBurnCal::Status::PASS;
}

#endif