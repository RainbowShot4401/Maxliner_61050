/**
*** Copyright (C) 2017 Intel Corporation. All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
*** File: dutWrapper.h
*** Created on: 04 August 2017
*** Implementation of function declartion of dutWrapper
*** Original author: Merav Reuveni
**/

#pragma once

#include "dut_api.h"

class dutWrapperPrivate; //hold the pointer

class __declspec(dllexport) dutWrapper
{
private: dutWrapperPrivate* _private;

public: dutWrapper();
public: ~dutWrapper();

public: MT_RET Dut_SetConnectionMode(int connectionType, const char* optIPaddr);
public: MT_RET Dut_Burn_File(const char* filename);
public: MT_RET Dut_BurnInfo(const char* barcode, const BYTE* macAddr, const int barcodeMode);
public: MT_RET Dut_setPowerVector(::DUT_SetTransmitPowerVector_t *transmitPowerVector);
public: MT_RET Dut_SetTPCVector(int* TPCVector, int TPCVectorLength);
public: MT_RET Dut_GetGainTableIndexVector(int* GainTableIndexVector);
public: MT_RET Dut_GetVoltageVector(int* VoltageVector);
public: MT_RET Dut_ReadVoltageValue(double* pVoltage);
public: MT_RET Dut_writeTssiCalData(const Dut_CalDataStructVer5* inCalArray, Dut_CalDataStructVer5* outCalArray, int numStructures);
public: MT_RET Dut_writeTssiCalData(const Dut_CalDataStructVer6* inCalArray, Dut_CalDataStructVer6* outCalArray, int numStructures);
public: MT_RET Dut_GetABPoints(const Dut_CalDataStructVer5* inCalArray, Dut_CalDataStructVer5* outCalArray);
public: MT_RET Dut_GetABPoints(const Dut_CalDataStructVer6* inCalArray, Dut_CalDataStructVer6* outCalArray);
public: MT_RET Dut_PhyPacketsCounter(DWORD* Counters);
public: MT_RET Dut_RssiS2dSetInitials(int numLnaGains, int numRegions, float* lnaGain, float AuxADC_Res, float Ioffs_Step);
public: MT_RET Dut_RssiGetMaxAbError(float maxAbError[4]);
public: MT_RET Dut_Measure_RSSI(MT_INT16 Method, MT_INT16 NOS, MT_INT16* RSSI);
public: MT_RET Dut_RSSIVector(double* RSSIVector);
public: MT_RET Dut_ReadRxEvm(BYTE* rxEvmArry);
public: MT_RET Dut_Measure_CW_Power(int Method, int FreqOffset, int NOS, DutApiCorrResults_t *corrResultsOut);
public: MT_RET Dut_MidGainCalibrate(float targetGain, float Prxin[4], int Pgc1, int Pgc2, float* calcGain, int* calcGainCtrl);
public: MT_RET Dut_Measure_LNA_Sub_Band_Gains(DutApiCorrResults_t measureCorrResult[4][16]);
public: MT_RET Dut_calculate_LNA_Sub_Band_Gains(unsigned int rxBandFreq[4][16]);
public: MT_RET Dut_Set_Rx_Band_LUT(unsigned int rxBandFreq[4][7] ,unsigned int sant[4]);
public: MT_RET Dut_Calculate_RX_RF_Flatness(int P_Adc_array[4][16], float refGain[4], unsigned int PGC1, unsigned int PGC2, unsigned int PGC3, float Pin[4][16], float results[4][16]);
public: MT_RET Dut_Calculate_Linear_Regression(float* P, float V[4][16], float* C, float* LnaGains, ::Dut_Rssi_AB* ab);
public: MT_RET Dut_Calculate_P_Vector(int numPwrPoints, int powerMax, float *LnaGains, int lowerP_Boundary, int upperP_Boundary, float* Pout);
public: MT_RET Dut_Calculate_LNA_Gain_Steps(::CorrResultsPerAntPerStep_t *corrResultsPerAntPerStep_p, int lnaGainStep, float Prxin[4], int Pgc1, int Pgc2, int Pgc3, ::GainPerAntPerStep_t *gainPerAntPerStep_p);
public: MT_RET Dut_Calculate_optimal_Ioffs(int region, float* V1/*rssiVoltage1*/, float* V2/*rssiVoltage2*/, float* s2d_gain_db, float* s2d_offset_db, int* s2d_offset_opt_out);
public: MT_RET Dut_Calculate_Optimal_S2Dgain(int region, float* V1/*rssiVoltage1*/, float* V2/*rssiVoltage2*/, float* s2d_gain_db, int* s2d_gain_opt_out);
public: MT_RET Dut_Burn_Rssi_Cal_Data(const Dut_RssiCalDataStructVer5* inCalArray, Dut_RssiCalDataStructVer5* outCalArray, int numStructures);
public: MT_RET Dut_Burn_Rssi_Cal_Data(const Dut_RssiCalDataStructVer6* inCalArray, Dut_RssiCalDataStructVer6* outCalArray, int numStructures);
public: MT_RET Dut_S2dSetInitials(int* Power_vector, int S2D_offset, int tssiLow, int tssiHigh, int vectorLen);
public: MT_RET Dut_ReadEEPROM(DWORD address, DWORD length, BYTE* data, bool useCache = true);
public: MT_RET Dut_WritePowerLimitsToEfuse(BYTE* powerLimits, int numOfChannels2_4G, int numOfChannels5G, int version);
public: MT_RET Dut_ReadPowerLimitsFromEfuse(BYTE* powerLimits, int* numOfChannels2_4G, int* numOfChannels5G, int* version);
public: MT_RET Dut_SetRiscMode(int setOrReset);
public: MT_RET Dut_WriteCompleteEEPROM(void);
public:	MT_RET Dut_GetEEPROMSize(int* EEPROMSize);
public: MT_RET Dut_WriteReg(int module, DWORD address, DWORD pData, DWORD mask);
public: MT_RET Dut_ReadReg(int module, DWORD address, DWORD* pData, DWORD mask);
public: MT_RET Dut_StartRSSI(void);
public:	MT_RET Dut_Set_Rx_Gain_Blocks(int LnaIndex, int pgc1, int pgc2, int pgc3);
public: MT_RET Dut_Set_RSSI_offset_gain(int iOffset, int s2dGain, int ant, BYTE regionIndex);
public: MT_RET Dut_EndRSSI(void);
public: MT_RET Dut_SetCloseLoop(int isCloseLoop);
public: MT_RET Dut_SetChannel(int Mode, int band, int spectrumBW, int channel, int calibrationMask, int closedLoop);
public: MT_RET Dut_SetTxAntenna(int mask);
public: MT_RET Dut_SetRxAntenna(int mask);
public: MT_RET Dut_S2dCalibrate(signed char* s2dGain, signed char* s2dOffset, int region, int pThreshold);
public: MT_RET Dut_ShiftPowerOutVector(MT_UBYTE antenna, MT_UBYTE BW, MT_UBYTE powerVectorIndex, MT_UBYTE powerVectorWord);
public: MT_RET Dut_SetRxAntennasMask(int rxAntsMask);
public: MT_RET Dut_SetTxAntennasMask(int txAntMask);
public: MT_RET Dut_SetTxAntennasNumber(int txAntNum);
public: MT_RET Dut_SetRxAntennasNumber(int rxAntNum);
public: MT_RET Dut_TransmitCW(int amp, int binNum);
public: MT_RET Dut_SetRfBand(int band);
public: MT_RET Dut_EndTxPackets();
public: MT_RET Dut_SetWLANindex(int wlanIndex);
public: MT_RET Dut_SetResetSettings(int phyType, int spectrumBW, int signalBW, int rateIndex, int packetLength, int channel, int calibrationMask, int closedLoop, int txRate, float power);
public: MT_RET Dut_ReadChipVersion(int* pRFChipVersion, int* pBBChipVersion);
public: MT_RET Dut_IsConnected();
public: MT_RET Dut_DriverRelease();
public: MT_RET Dut_SetNvMemoryType(int memoryType);
public: MT_RET Dut_SetNvMemorySize(int memorySize);
public: MT_RET Dut_DriverInit();
public: MT_RET Dut_DllVersion(char* dllVersion);
public: MT_RET Dut_DriverVersion(double* version);
public: MT_RET Dut_SetNvRequiredInfo(int cisCode);
public: MT_RET Dut_ReadLnaData(signed char* pLnaBypass, signed char* pLnaHigh);
public: MT_RET Dut_GetTxAntennasMask(BYTE* hwTxAntennasMask);
public: MT_RET Dut_GetRxAntennasMask(BYTE* hwRxAntennasMask);
public: MT_RET Dut_SetPowerLimit(float powerLimit);
public: MT_RET Dut_SetTxRate(MT_UINT16 TxRate, int phyType, int SGI, int signalBW);
public: MT_RET Dut_FlushNvMemory(DWORD verifyNeeded);
public: MT_RET DUT_SetCdd(int CddValue);
public:	MT_RET Dut_SetIFS(DWORD IFS);
public: MT_RET Dut_TxPackets(int txRate, int length, int repetitions, int SGI, bool isDataLong);
public: MT_RET Dut_ResetPacketsCounter();
public: MT_RET Dut_SetTPC(int TPC);
public: MT_RET Dut_WriteXtal_full(DWORD data);
public: MT_RET Dut_ReadXtal(int* data);
public: MT_RET Dut_WriteXtalCalDataToEEPROM_full(DWORD data);
public: MT_RET Dut_SetStopTxOnSetChannel(int stop);
public:	MT_RET Dut_ReadXtalCalDataFromEEPROM(int* data);
public: MT_RET DUT_SetPrintingLevel(int level);
public: MT_RET Dut_SetHdkConfigure(DWORD offlineCalMask, DWORD calMask);
public: MT_RET Dut_SetSnifferMode(bool snifferOn);
public: MT_RET Dut_TxGetAbErrors(float abErrors[4][3]);
public: MT_RET Dut_GetPoutTableOffset(BYTE antNum, BYTE bw, short* pTableOffset);
public: MT_RET Dut_SetPoutTableOffset(BYTE antNum, BYTE bw, short pTableOffset);
public: MT_RET Dut_SetChangeBand(unsigned int band); /*band=0 for 5GHz, band=1 for 2.4GHz*/
public: MT_RET Dut_SetS2dParams(int antNum, int signalBw, int region, int pThreshold, int gain, int offset);
public: MT_RET Dut_GetBandSupport();
public: MT_RET Dut_GetBandCurrent();
public: MT_RET Dut_ReadProdFlag(bool *prodFlag);
public: MT_RET Dut_WriteProdFlag(bool prodFlag);
public: MT_RET Dut_EnableRxAggregation(bool enable); /*true means enable,false means disable*/
public: MT_RET Dut_setFCCRegulatory(bool fcc_enable);
public: MT_RET Dut_getFccEnabled(bool* fcc_enable);
public: MT_RET Dut_GetComponentVersion(int type, BYTE* version); //type:dutComponentType_e; Max_length(version) = DUT_CV_MAX_LENGTH
public: MT_RET Dut_BurnCombinedVersion(BYTE* version); //Max_length(version) = DUT_CV_MAX_LENGTH
public: MT_RET Dut_GetBbicCddValues(BYTE antNum, unsigned int* offset1_rtrn, unsigned int* offset2_rtrn, unsigned int* offset3_rtrn);
public: MT_RET Dut_SetBbicCddValues(BYTE antNum, unsigned int offset1_rtrn, unsigned int offset2_rtrn, unsigned int offset3_rtrn);
public: MT_RET Dut_BurnCalFileSignature();
public: MT_RET Dut_ReadCalFileSignature(const char* savePath);
};
