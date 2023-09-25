#pragma once
#include "dutWrapper.h"
#include "..\CommFunc.h"
#include <string>

using namespace std;

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 4096
#endif

class CDutControl
{
public:
   enum Status { FAIL, PASS };
   enum OpenClose {OPEN_LOOP, CLOSE_LOOP};
   enum ConnectionEnum { DISCONNECTED, CONNECTED };
   ConnectionEnum isConected;

   funcGetMessage m_funcGetMessage;
   BOOL debug;

	CDutControl(dutWrapper *dutApi);
	~CDutControl();
//Insert DUT
   MT_RET DUT_SetPrintingLevel(int level);
   MT_RET Dut_SetNvMemoryType(int memoryType);
   MT_RET Dut_SetNvMemorySize(int memorySize);
   MT_RET Dut_SetResetSettings(int phyType, int spectrumBW, int signalBW, int rateIndex, int packetLength, int channel, int calibrationMask, int closedLoop, int txRate, float power);
   MT_RET Dut_SetConnectionMode(int connectionType,const char* optIPaddr);
   MT_RET Dut_SetWLANindex(int wlanIndex);
   MT_RET Dut_DriverInit();
   MT_RET Dut_SetChangeBand(unsigned int band); /*band=0 for 5GHz, band=1 for 2.4GHz*/
   MT_RET Dut_DriverVersion(double* version);
   MT_RET Dut_ReadChipVersion(int* pRFChipVersion, int* pBBChipVersion);
   MT_RET Dut_GetComponentVersion(int type, BYTE* version); //type:dutComponentType_e; Max_length(version) = DUT_CV_MAX_LENGTH
   MT_RET Dut_GetBandSupport();

   MT_RET Dut_Burn_File(const char* filename);
   MT_RET Dut_BurnInfo(const char* barcode, const BYTE* macAddr, const int barcodeMode);
   MT_RET Dut_setPowerVector(::DUT_SetTransmitPowerVector_t *transmitPowerVector);
   MT_RET Dut_SetTPCVector(int* TPCVector, int TPCVectorLength);
   MT_RET Dut_GetGainTableIndexVector(int* GainTableIndexVector);
   MT_RET Dut_GetVoltageVector(int* VoltageVector);
   MT_RET Dut_ReadVoltageValue(double* pVoltage);
   MT_RET Dut_writeTssiCalData(const Dut_CalDataStructVer5* inCalArray, Dut_CalDataStructVer5* outCalArray, int numStructures);
   MT_RET Dut_writeTssiCalData(const Dut_CalDataStructVer6* inCalArray, Dut_CalDataStructVer6* outCalArray, int numStructures);
   MT_RET Dut_GetABPoints(const Dut_CalDataStructVer5* inCalArray, Dut_CalDataStructVer5* outCalArray);
   MT_RET Dut_GetABPoints(const Dut_CalDataStructVer6* inCalArray, Dut_CalDataStructVer6* outCalArray);
   MT_RET Dut_PhyPacketsCounter(DWORD* Counters);
   MT_RET Dut_RssiS2dSetInitials(int numLnaGains, int numRegions, float* lnaGain, float AuxADC_Res, float Ioffs_Step);
   MT_RET Dut_RssiGetMaxAbError(float maxAbError[4]);
   MT_RET Dut_Measure_RSSI(MT_INT16 Method, MT_INT16 NOS, MT_INT16* RSSI);
   MT_RET Dut_RSSIVector(double* RSSIVector);
   MT_RET Dut_ReadRxEvm(BYTE* rxEvmArry);
   MT_RET Dut_Measure_CW_Power(int Method, int FreqOffset, int NOS, DutApiCorrResults_t *corrResultsOut);
   MT_RET Dut_MidGainCalibrate(float targetGain, float Prxin[4], int Pgc1, int Pgc2, float* calcGain, int* calcGainCtrl);
   //MT_RET Dut_Measure_LNA_Sub_Band_Gains(DutApiCorrResults_t measureCorrResult[4][8]);
   //MT_RET Dut_calculate_LNA_Sub_Band_Gains(unsigned int rxBandFreq[4][7]);
   //MT_RET Dut_Set_Rx_Band_LUT(unsigned int rxBandFreq[4][7]);
   //MT_RET Dut_Calculate_RX_RF_Flatness(int P_Adc_array[4][7], float refGain[4], unsigned int PGC1, unsigned int PGC2, unsigned int PGC3, float Pin[4][7], float results[4][7]);
   //MT_RET Dut_Calculate_Linear_Regression(float* P, float V[4][7], float* C, float* LnaGains, ::Dut_Rssi_AB* ab);
   //MT_RET Dut_Calculate_P_Vector(int numPwrPoints, int powerMax, float *LnaGains, int lowerP_Boundary, int upperP_Boundary, float* Pout);
   //MT_RET Dut_Calculate_LNA_Gain_Steps(::CorrResultsPerAntPerStep_t *corrResultsPerAntPerStep_p, int lnaGainStep, float Prxin[4], int Pgc1, int Pgc2, int Pgc3, ::GainPerAntPerStep_t *gainPerAntPerStep_p);
   //MT_RET Dut_Calculate_optimal_Ioffs(int region, float* V1/*rssiVoltage1*/, float* V2/*rssiVoltage2*/, float* s2d_gain_db, float* s2d_offset_db, int* s2d_offset_opt_out);
   //MT_RET Dut_Calculate_Optimal_S2Dgain(int region, float* V1/*rssiVoltage1*/, float* V2/*rssiVoltage2*/, float* s2d_gain_db, int* s2d_gain_opt_out);
   //MT_RET Dut_Burn_Rssi_Cal_Data(const Dut_RssiCalDataStructVer5* inCalArray, Dut_RssiCalDataStructVer5* outCalArray, int numStructures);
   //MT_RET Dut_Burn_Rssi_Cal_Data(const Dut_RssiCalDataStructVer6* inCalArray, Dut_RssiCalDataStructVer6* outCalArray, int numStructures);
   MT_RET Dut_S2dSetInitials(int* Power_vector, int S2D_offset, int tssiLow, int tssiHigh, int vectorLen);
   MT_RET Dut_ReadEEPROM(DWORD address, DWORD length, BYTE* data);
   MT_RET Dut_WritePowerLimitsToEfuse(BYTE* powerLimits, int numOfChannels2_4G, int numOfChannels5G, int version);
   MT_RET Dut_ReadPowerLimitsFromEfuse(BYTE* powerLimits, int* numOfChannels2_4G, int* numOfChannels5G, int* version);
   MT_RET Dut_SetRiscMode(int setOrReset);
   MT_RET Dut_WriteCompleteEEPROM(void);
   MT_RET Dut_GetEEPROMSize(int* EEPROMSize);
   MT_RET Dut_WriteReg(int module, DWORD address, DWORD pData, DWORD mask);
   MT_RET Dut_ReadReg(int module, DWORD address, DWORD* pData, DWORD mask);
   MT_RET Dut_StartRSSI(void);
   MT_RET Dut_Set_Rx_Gain_Blocks(int LnaIndex, int pgc1, int pgc2, int pgc3);
   MT_RET Dut_Set_RSSI_offset_gain(int iOffset, int s2dGain, int ant, BYTE regionIndex);
   MT_RET Dut_EndRSSI(void);
   MT_RET Dut_SetCloseLoop(int isCloseLoop);
   MT_RET Dut_SetChannel(int Mode, int band, int spectrumBW, int channel, int calibrationMask, int closedLoop);
   MT_RET Dut_SetTxAntenna(int mask);
   MT_RET Dut_SetRxAntenna(int mask);
   MT_RET Dut_S2dCalibrate(signed char* s2dGain, signed char* s2dOffset, int region, int pThreshold);
   MT_RET Dut_ShiftPowerOutVector(MT_UBYTE antenna, MT_UBYTE BW, MT_UBYTE powerVectorIndex, MT_UBYTE powerVectorWord);
   MT_RET Dut_SetRxAntennasMask(int rxAntsMask);
   MT_RET Dut_SetTxAntennasMask(int txAntMask);
   MT_RET Dut_SetTxAntennasNumber(int txAntNum);
   MT_RET Dut_SetRxAntennasNumber(int rxAntNum);
   MT_RET Dut_TransmitCW(int amp, int binNum);
   MT_RET Dut_SetRfBand(int band);
   MT_RET Dut_EndTxPackets(); 
   MT_RET Dut_IsConnected();
   MT_RET Dut_DriverRelease();
   MT_RET Dut_SetNvRequiredInfo(int cisCode);
   MT_RET Dut_ReadLnaData(signed char* pLnaBypass, signed char* pLnaHigh);
   MT_RET Dut_GetTxAntennasMask(BYTE* hwTxAntennasMask);
   MT_RET Dut_GetRxAntennasMask(BYTE* hwRxAntennasMask);
   MT_RET Dut_SetPowerLimit(float powerLimit);
   MT_RET Dut_SetTxRate(MT_UINT16 TxRate, int phyType, int SGI, int signalBW);
   MT_RET Dut_FlushNvMemory(DWORD verifyNeeded);
   MT_RET DUT_SetCdd(int CddValue);
   MT_RET Dut_SetIFS(DWORD IFS);
   MT_RET Dut_TxPackets(int txRate, int length, int repetitions, int SGI, bool isDataLong);
   MT_RET Dut_ResetPacketsCounter();
   MT_RET Dut_SetTPC(int TPC);
   MT_RET Dut_WriteXtal_full(DWORD data);
   MT_RET Dut_ReadXtal(int* data);
   MT_RET Dut_WriteXtalCalDataToEEPROM_full(DWORD data);
   MT_RET Dut_SetStopTxOnSetChannel(int stop);
   MT_RET Dut_ReadXtalCalDataFromEEPROM(int* data);
   
   
   MT_RET Dut_SetSnifferMode(bool snifferOn);
   MT_RET Dut_TxGetAbErrors(float abErrors[4][3]);
   MT_RET Dut_GetPoutTableOffset(BYTE antNum, BYTE bw, short* pTableOffset);
   MT_RET Dut_SetPoutTableOffset(BYTE antNum, BYTE bw, short pTableOffset);

   MT_RET Dut_SetS2dParams(int antNum, int signalBw, int region, int pThreshold, int gain, int offset);

   MT_RET Dut_GetBandCurrent();
   MT_RET Dut_ReadProdFlag(bool *prodFlag);
   MT_RET Dut_WriteProdFlag(bool prodFlag);
   MT_RET Dut_EnableRxAggregation(bool enable); /*true means enable,false means disable*/
   MT_RET Dut_setFCCRegulatory(bool fcc_enable);
   MT_RET Dut_getFccEnabled(bool* fcc_enable);

private:
   dutWrapper *_dutApi;
   Status _error;
};

