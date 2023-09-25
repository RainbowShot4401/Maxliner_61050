#pragma once
#include "dutWrapper.h"
#include "..\CommFunc.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

struct ShiftPowerOutTableParames {
	int pout_table_index = 0;
	short pout_table_gain_or_offset = 0;
	unsigned char antNumber = 0;
	unsigned char bw = 0;
};

//S2D 
struct S2dSetInitialsParames {
	int Power_vector[2];
	int S2D_offset;
	int tssiLow;
	int tssiHigh;
};

struct SetS2dParamsParames {
	int antNumber;
	int signalBw;
	int region;
	int pThreshold;
	int gain;
	int offset;
};

struct S2dCalibrateParames {
	int antNum;
	signed char outS2dGain;
	signed char outS2dOffset;
	int regionNumber;
	int pThreshold;
};
//////////////////////////////////////////////////

class CTxControl
{
public:
	enum Status { FAIL, PASS };
	enum OpenClose { OPEN_LOOP, CLOSE_LOOP };

	/*
	struct TxSetChannelParams {
		int boardAntennaMask;
		string standard;
		int  freq;
		string bandwidth;
		int offlineCalMask;
		OpenClose isCloseLoop;
		int txAntennaMask;
		int numOfStreams;
		string rate;
		int sleepAfterSetChannel = 0;
	};*/


   funcGetMessage m_funcGetMessage;
   BOOL debug;

	CTxControl(dutWrapper *dutApi);
	~CTxControl();
   
   MT_RET DUT_TxStart(DUT_PARAM DutParam, map<string, string> ActionParameter, int TxAnt, LPSTR lpszRunInfo);
   MT_RET Dut_EndTxPackets();
   MT_RET Dut_GetTxAntennasMask(BYTE* hwTxAntennasMask);
   MT_RET Dut_GetRxAntennasMask(BYTE* hwRxAntennasMask);
   MT_RET Dut_SetRxAntennasMask(int rxAntsMask);
   MT_RET Dut_SetTxAntennasMask(int txAntMask);
   MT_RET Dut_SetTxAntenna(int mask);
   MT_RET Dut_SetRxAntenna(int mask);
   MT_RET Dut_SetCloseLoop(int isCloseLoop);
   MT_RET Dut_SetHdkConfigure(DWORD offlineCalMask, DWORD calMask);
   MT_RET Dut_SetChannel(int Mode, int band, int spectrumBW, int channel, int calibrationMask, int closedLoop);
   MT_RET Dut_SetTxRate(MT_UINT16 TxRate, int phyType, int SGI, int signalBW);
   MT_RET Dut_SetIFS(DWORD IFS);
   MT_RET Dut_TxPackets(int txRate, int length, int repetitions, int SGI, bool isDataLong);
   //MT_RET Dut_SetPowerLimit(float powerLimit);
  
	//Tx calibration.
   Status ShiftPowerOutVector(const ShiftPowerOutTableParames& shiftPowerOutTableParames);
   Status SetPowerLimit(const double& powerIndex, OpenClose LoopParameter);
   Status GetPoutTableOffset(ShiftPowerOutTableParames& shiftPowerOutTableParames);
   Status SetPoutTableOffset(const ShiftPowerOutTableParames& shiftPowerOutTableParames);
   //S2D
   Status S2dSetInitials(const S2dSetInitialsParames& s2dSetInitialsParames);
   Status S2dCalibrate(S2dCalibrateParames& s2dCalibrateParames);
   Status SetS2dParams(const SetS2dParamsParames& setS2dParamsParames);
   //OpenLoop
   Status GetVoltage(vector<int>& voltageForAllAnts);
   //TxBurnCal
   Status getABPoints(Dut_CalDataStructVer6* CalArray);
   Status txGetAbErrors(float abErrors[MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER]);
   CTxControl::Status writeTssiCalData(const Dut_CalDataStructVer6* CalArray, int ArraySize);
   CTxControl::Status CTxControl::WriteProdFlag(bool flag);
   CTxControl::Status CTxControl::flushNvMemory();

private:
   dutWrapper *_dutApi;
   Status _error;
   double _powerIndex;
   OpenClose _openLoopCloseLoop;
};

