#include "..\stdafx.h"
#include "RxBurnCalGen6.h"

RxBurnCalGen6::RxBurnCalGen6(RxDutApi *dutFunc, dutWrapper * dutApi)
{
	_dutFunc = dutFunc;
	_dutApi = dutApi;
	//_logPrint = new LogPrint(18, 5, 4);

}


RxBurnCalGen6::~RxBurnCalGen6()
{
}


IRxBurnCal::Status RxBurnCalGen6::writeRssiCalData()
{
	RxDutApi::status dutError = (RxDutApi::status)_dutFunc->writeRssiCalData(dutCalDataStructVer6, calDataStructVer6.size());
	if (RxDutApi::status::PASS != dutError)
		return IRxBurnCal::Status::FAIL;
	return IRxBurnCal::Status::PASS;
}


IRxBurnCal::Status RxBurnCalGen6::fillRxStruct(const IRxBurnCal::FillRxStructParams &fillRxStructParams)
{
	IRxBurnCal::FillRxStructParams tempfillRxStructParams = fillRxStructParams;
	RxBurnCalGen6::calDataStructVer6.emplace_back(tempfillRxStructParams);
	convertToDutStruct();
	return IRxBurnCal::Status::PASS;
}

IRxBurnCal::Status RxBurnCalGen6::clearRxStruct()
{
	RxBurnCalGen6::calDataStructVer6.clear();
	std::copy(begin(_dutCalDataStructVer6), end(_dutCalDataStructVer6), begin(dutCalDataStructVer6));
	return IRxBurnCal::Status::PASS;
}


IRxBurnCal::Status RxBurnCalGen6::convertToDutStruct()
{
	int channelIndex = calDataStructVer6.size() - 1;

	IRxBurnCal::FillRxStructParams iqliteRxStruct = calDataStructVer6.at(channelIndex);

	dutCalDataStructVer6[channelIndex].antMask = iqliteRxStruct.rxSetChannelParams.rxAntennaMask;
	dutCalDataStructVer6[channelIndex].calFreqChannel = iqliteRxStruct.rxSetChannelParams.freq;
	dutCalDataStructVer6[channelIndex].chipTemprature = 38; //TEMP value
	dutCalDataStructVer6[channelIndex].numRxRfFlatness = iqliteRxStruct.flatnessResultsParams.flatnessFreq.size();
	dutCalDataStructVer6[channelIndex].numRxRssiFlatness = 0; //it's not calibrated now
	copy(begin(iqliteRxStruct.flatnessResultsParams.flatnessFreq),
		end(iqliteRxStruct.flatnessResultsParams.flatnessFreq),
		begin(dutCalDataStructVer6[channelIndex].rxRfFlatnessFreqPoint));

	std::fill(begin(dutCalDataStructVer6[channelIndex].rxRssiFlatnessFreqPoint),
		begin(dutCalDataStructVer6[channelIndex].rxRssiFlatnessFreqPoint), 0);

	dutCalDataStructVer6[channelIndex].startFreqChannel = iqliteRxStruct.rxSetChannelParams.startFreq;
	dutCalDataStructVer6[channelIndex].stopFreqChannel = iqliteRxStruct.rxSetChannelParams.endFrq;

	for (int i_ant = 0; i_ant < MAX_ANTENNA_NUMBER; i_ant++)
	{
		if((iqliteRxStruct.rxSetChannelParams.rxAntennaMask >> i_ant) & 0x1)
		{ 
			copy(begin(iqliteRxStruct.gainStepResultsParams.gainSteps.gain[i_ant]),
				end(iqliteRxStruct.gainStepResultsParams.gainSteps.gain[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].gainLna));
			
			dutCalDataStructVer6[channelIndex].ants[i_ant].midGain = iqliteRxStruct.midGainResultsParams.calcTuneWord[i_ant];
			
			dutCalDataStructVer6[channelIndex].ants[i_ant].numLnaGainSteps = iqliteRxStruct.gainStepResultsParams.numLnaGainSteps;

			dutCalDataStructVer6[channelIndex].ants[i_ant].numRegions = iqliteRxStruct.s2dResultsParams.numberOfRegions;
			
			copy(begin(iqliteRxStruct.abResultsParams.ab[i_ant]),
				end(iqliteRxStruct.abResultsParams.ab[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].pointsAB));

			copy(begin(iqliteRxStruct.flatnessResultsParams.midGainDelta[i_ant]),
				end(iqliteRxStruct.flatnessResultsParams.midGainDelta[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].midGainDelta));

			copy(begin(iqliteRxStruct.flatnessResultsParams.flatnessdB[i_ant]),
				end(iqliteRxStruct.flatnessResultsParams.flatnessdB[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].rxRfFlatnessDeltaPoint));


			copy(begin(iqliteRxStruct.flatnessResultsParams.flatnessBypassDB[i_ant]),
				end(iqliteRxStruct.flatnessResultsParams.flatnessBypassDB[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].rxRfFlatnessDeltaPointBypass));

			fill(begin(dutCalDataStructVer6[channelIndex].ants[i_ant].rxRssiFlatnessDeltaPoint),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].rxRssiFlatnessDeltaPoint), 0);

			for (int i_go = 0; i_go < iqliteRxStruct.s2dResultsParams.numberOfRegions; i_go++)
			{
				dutCalDataStructVer6[channelIndex].ants[i_ant].s2dGainOffset[i_go].gain = iqliteRxStruct.s2dResultsParams.gain[i_ant][i_go];
				dutCalDataStructVer6[channelIndex].ants[i_ant].s2dGainOffset[i_go].offset = iqliteRxStruct.s2dResultsParams.offset[i_ant][i_go];
			}
	
			copy(begin(iqliteRxStruct.subBandResultsParams.subBandFreq[i_ant]),
				end(iqliteRxStruct.subBandResultsParams.subBandFreq[i_ant]),
				begin(dutCalDataStructVer6[channelIndex].ants[i_ant].subBandsCrossPoints));
						
		}
	}
	

	return IRxBurnCal::Status::PASS;
}



int RxBurnCalGen6::getNumberOfChannels()
{
	return calDataStructVer6.size();
}

void RxBurnCalGen6::dutFuncSet(RxDutApi* var)
{
	_dutFunc = var;
}