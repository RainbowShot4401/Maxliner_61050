#pragma once
#ifndef __AFXWIN_H__
#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號
#include "CommFunc.h"
//#include "MyClass/InsertDut.h"
//#include "MyClass/TxDutApi.h"
#include "MyClass/DutControl.h"
#include "MyClass/TxControl.h"
#include "MyClass/RxDutApi.h"
#include "MyClass/XtalCalibration.h"
#include "MyClass/txGainTableCal_Gen6.h"
#include "MyClass/txS2dCal.h"
#include "MyClass/TxOpenLoopSweep.h"
#include "MyClass/Rxcalibration.h"
#include "MyClass/CustomizeTest.h"
#include "MyClass/TxBurnCalGen6.h"
#include "MyClass/RxBurnCalGen6.h"
#include "MyClass/CalFileVerificationGen6.h"
#include "MyClass/ReadCalFile.h"
#include <exception>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <iostream>

using namespace std;

map<string, string> BAND_Table;
map<string, string> WIDE_BAND_Table;
map<string, int> Antenna;
map<string, int> antNum;
int Antenna_Index;

//RX Sub-Bands Calibration
char RX_CAL_SUB_BAND_FREQ[1024];
string  CH_FREQ_MHZ, STANDARD, BAND_START_FREQ, BAND_END_FREQ, RX_CAL_SUB_BAND_LNA_INDEX, RX_CAL_SUB_BAND_PGC1, 
RX_CAL_SUB_BAND_PGC2, RX_CAL_SUB_BAND_PGC3, RX_CAL_SUB_BAND_POWER_REF, RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL;

//RX Mid-Gain Calibration
string RX_CAL_CALC_LNA_INDEX_FOR_MID_LNAGAIN_CAL, RX_CAL_PGC1_FOR_MID_LNAGAIN_CAL, RX_CAL_PGC2_FOR_MID_LNAGAIN_CAL, RX_CAL_PGC3_FOR_MID_LNAGAIN_CAL,
RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_5G, RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_2G,
BSS_FREQ_MHZ_PRIMARY, RX_CAL_PRXIN_FOR_MID_LNAGAIN_CAL,
RX_CAL_MID_LNAGAIN_DEC_MIN, RX_CAL_MID_LNAGAIN_DEC_MAX, RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL;

//RX Gain Accuracy Calibration
int    RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN[NUM_LNA_STEPS];// 12;
double RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN[6];
double RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX[6];

vector<string>  RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN_RAW;
vector<string>  RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN_RAW;
vector<string>  RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX_RAW;

string  RX_CAL_PGC2_FOR_LNAGAIN_CAL, RX_CAL_PGC3_FOR_LNAGAIN_CAL, RX_CAL_NOS,
RX_CAL_NUM_OF_LNA_GAINS, RX_CAL_GAIN_STEP_POWER_REF;

//RX flatness Calibration
char RX_CAL_FLATNESS_FREQ[1024];

//RX S2D Gain Offset Calibration
int RX_CAL_IOFFSET_INIT[DUT_NUM_REGION];
int RX_CAL_S2DGAIN_INIT[DUT_NUM_REGION];
double RX_CAL_POWER_MAX[DUT_NUM_REGION];
double RX_CAL_MARGINHIGH[DUT_NUM_REGION];

//RX AB Calibration
int RX_CAL_LOWERP_BOUNDARY, RX_CAL_UPPERP_BOUNDARY;

vector<string>  RX_CAL_IOFFSET_INIT_RAW, RX_CAL_S2DGAIN_INIT_RAW, RX_CAL_POWER_MAX_RAW, RX_CAL_MARGINHIGH_RAW;
int RX_CAL_NO_OF_REGION, RX_CAL_LNA_INDEX, RX_CAL_PGC1_FOR_AB_CAL, RX_CAL_PGC2_FOR_AB_CAL, RX_CAL_PGC3_FOR_AB_CAL;
float RX_CAL_AUX_ADC_RES, RX_CAL_IOFFS_STEP;

class CIntel_WAV : public CWinApp
{
public:
   CIntel_WAV();
   ~CIntel_WAV();
public:
   virtual BOOL InitInstance();
   DECLARE_MESSAGE_MAP()
public:
   funcGetMessage m_funcGetMessage;
   dutWrapper *dutApi;
   ITxBurnCal::FillTxStructParams fillTxStructParams;
   IRxBurnCal::FillRxStructParams fillRxStructParams;
   TxBurnCalGen6 *m_TxBurnCalGen6;
   RxBurnCalGen6 *m_RxBurnCalGen6;
   CDutControl *insertDut;
   funcReadCableLoss m_funcReadCableLoss;
  // CDutControl m_CDutControl;
};