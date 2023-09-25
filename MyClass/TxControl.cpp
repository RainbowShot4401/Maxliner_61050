#include "..\stdafx.h"
#include "TxControl.h"

CTxControl::CTxControl(dutWrapper *dutApi) : _dutApi(dutApi) {}
//CDutControl::CDutControl(dutWrapper *dutApi){ _dutApi = dutApi; }
CTxControl::~CTxControl() {}

MT_RET CTxControl::Dut_EndTxPackets()
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_EndTxPackets.");
   if(!_dutApi->Dut_EndTxPackets())
   {
      ExportTestLog("Dut_EndTxPackets Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_GetTxAntennasMask(BYTE* hwTxAntennasMask)
{
   if(!_dutApi->Dut_GetTxAntennasMask(hwTxAntennasMask))
   {
      ExportTestLog("Dut_GetTxAntennasMask Fail.");
      return FAIL;
   }
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_GetTxAntennasMask => hwTxAntennasMask: %d.", *hwTxAntennasMask);
   return PASS;
}

MT_RET CTxControl::Dut_GetRxAntennasMask(BYTE* hwRxAntennasMask)
{
   if(!_dutApi->Dut_GetRxAntennasMask(hwRxAntennasMask))
   {
      ExportTestLog("Dut_GetRxAntennasMask Fail.");
      return FAIL;
   }
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_GetRxAntennasMask => hwRxAntennasMask: %d.", *hwRxAntennasMask);
   return PASS;
}

MT_RET CTxControl::Dut_SetRxAntennasMask(int rxAntsMask)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetRxAntennasMask => rxAntsMask: %0X.", rxAntsMask);
   if(!_dutApi->Dut_SetRxAntennasMask(rxAntsMask))
   {
      ExportTestLog("Dut_SetRxAntennasMask Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetTxAntennasMask(int txAntMask)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetTxAntennasMask => txAntMask: %0X.", txAntMask);
   if(!_dutApi->Dut_SetTxAntennasMask(txAntMask))
   {
      ExportTestLog("Dut_SetTxAntennasMask Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetTxAntenna(int mask)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetTxAntenna => mask: %d.", mask);
   if(!_dutApi->Dut_SetTxAntenna(mask))
   {
      ExportTestLog("Dut_SetTxAntenna Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetRxAntenna(int mask)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetRxAntenna => mask: %d.", mask);
   if(!_dutApi->Dut_SetRxAntenna(mask))
   {
      ExportTestLog("Dut_SetRxAntenna Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetCloseLoop(int isCloseLoop)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetCloseLoop => isCloseLoop: %d.", isCloseLoop);
   if(!_dutApi->Dut_SetCloseLoop(isCloseLoop))
   {
      ExportTestLog("Dut_SetCloseLoop Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetHdkConfigure(DWORD offlineCalMask, DWORD calMask)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetHdkConfigure => offlineCalMask: %lu, calMask: %lu.", offlineCalMask, calMask);
   if(!_dutApi->Dut_SetHdkConfigure(offlineCalMask, calMask))
   {
      ExportTestLog("Dut_SetHdkConfigure Fail.");
      return FAIL;
   }
   return PASS;
}


MT_RET CTxControl::Dut_SetChannel(int Mode, int band, int spectrumBW, int channel, int calibrationMask, int closedLoop)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetChannel => Mode: %d, band: %d, spectrumBW: %d, channel: %d, calibrationMask: %d, closedLoop: %d." \
      , Mode, band, spectrumBW, channel, calibrationMask, closedLoop);
   if(!_dutApi->Dut_SetChannel(Mode, band, spectrumBW, channel, calibrationMask, closedLoop))
   {
      ExportTestLog("Dut_SetChannel Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetTxRate(MT_UINT16 TxRate, int phyType, int SGI, int signalBW)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetTxRate => TxRate: %d, phyType: %d, SGI: %d, signalBW: %d.", TxRate, phyType, SGI, signalBW);
   if(!_dutApi->Dut_SetTxRate(TxRate, phyType, SGI, signalBW))
   {
      ExportTestLog("Dut_SetTxRate Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_SetIFS(DWORD IFS)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetIFS => IFS: %lu.", IFS);
   if(!_dutApi->Dut_SetIFS(IFS))
   {
      ExportTestLog("Dut_SetIFS Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CTxControl::Dut_TxPackets(int txRate, int length, int repetitions, int SGI, bool isDataLong)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_TxPackets => txRate: %d, length: %d, repetitions: %d, SGI: %d, isDataLong: %d.", txRate, length, repetitions, SGI, isDataLong);
   if(!_dutApi->Dut_TxPackets(txRate, length, repetitions, SGI, isDataLong))
   {
      ExportTestLog("Dut_TxPackets Fail.");
      return FAIL;
   }
   return PASS;
}

/* 
MT_RET CTxControl::Dut_SetPowerLimit(float powerLimit)
 {
    if(debug == "TRUE")
       ExportTestLog("API Flow: Dut_SetPowerLimit => powerLimit: %.3f.", powerLimit);
    if(!_dutApi->Dut_SetPowerLimit(powerLimit))
    {
       ExportTestLog("Dut_SetPowerLimit Fail.");
       return FAIL;
    }
    return PASS;
 }
 */


CTxControl::Status CTxControl::GetVoltage(vector<int>& voltageForAllAnts)
{
    voltageForAllAnts.clear();
    double voltage[MAX_DATA_STREAM];
    memset(voltage, '\0', sizeof(voltage));
    _error = (CTxControl::Status)_dutApi->Dut_ReadVoltageValue(voltage);
    if (CTxControl::Status::PASS != _error)
        return (CTxControl::Status::FAIL);
    voltageForAllAnts.insert(voltageForAllAnts.end(), &voltage[0], &voltage[MAX_DATA_STREAM]);
    return CTxControl::Status::PASS;
}

 MT_RET CTxControl::DUT_TxStart(DUT_PARAM DutParam, map<string, string> ActionParameter, int TxAnt, LPSTR lpszRunInfo)
{
    BYTE txMask, RxMask;
    int  phyType, band, bandwidth, channel, rate, SGI = 0, CP_MODE = 1, length, repetitions = 65535, calibrationMask = 65535, offlineCalMask = 65535;
    double freq, PowerLimit;
    BOOL isDataLong = 0;
    CTxControl::OpenClose isOpenClose = CTxControl::OPEN_LOOP;
    int IFS = 25;
    int txPacketRate = 0;

    freq = stod(ActionParameter["FREQ_MHZ"].c_str());

    phyType   = CCommFunc::ConvertStandardToPhyType(freq, ActionParameter["BAND"]);
    band      = CCommFunc::ConvertFreqToBand(freq);
    bandwidth = CCommFunc::ConvertBandwidth(ActionParameter["WIDE_BAND"]);
    channel   = CCommFunc::ConvertLoFreqtoLowChannel(ActionParameter["WIDE_BAND"], freq);
    rate      = CCommFunc::ConvertRate(1, ActionParameter["RATE"], ActionParameter["BAND"]);
    //SGI       = theApp.CalcValidGi(tmpAction["BAND"]);
    PowerLimit = stod(ActionParameter["OUTPUT_POWER"].c_str());

    if (ActionParameter["SGI"] != "")
        SGI = std::stoi(ActionParameter["SGI"].c_str());
    else
        SGI = 0;
   
    if (ActionParameter["CP_MODE"] != "")
        CP_MODE = std::stoi(ActionParameter["CP_MODE"].c_str());
    else if(ActionParameter["BAND"] == "11ax")
        CP_MODE = 2;
    else
        CP_MODE = 1;

    if (ActionParameter["IS_DATALONG"] != "")
    {
        if (!ActionParameter["IS_DATALONG"].compare("TRUE"))
            isDataLong = TRUE;
        else
            isDataLong = FALSE;
    }
    else
        isDataLong = FALSE;

    if (ActionParameter["DEBUG"] == "TRUE")
        debug = TRUE;

    if (ActionParameter["PACKET_LENGTH"] == "")
        length = CCommFunc::CalcPacketLength(ActionParameter["BAND"], ActionParameter["RATE"], ActionParameter["WIDE_BAND"]);
    else
        length = atoi(ActionParameter["PACKET_LENGTH"].c_str());

    if (ActionParameter["OPEN_CLOSE"] != "")
    {
        if (ActionParameter["OPEN_CLOSE"] == "0")
            isOpenClose = CTxControl::OPEN_LOOP;
        else
            isOpenClose = CTxControl::CLOSE_LOOP;  
    }
    if (ActionParameter["IFS"] != "")
        IFS = atoi(ActionParameter["IFS"].c_str());

     if (!Dut_GetTxAntennasMask(&txMask)) { strcpy(lpszRunInfo, "Dut_GetTxAntennasMask Fail."); }
     if (!Dut_GetRxAntennasMask(&RxMask)) { strcpy(lpszRunInfo, "Dut_GetRxAntennasMask Fail."); }
     if (!Dut_SetTxAntennasMask(txMask))  { strcpy(lpszRunInfo, "Dut_SetTxAntennasMask Fail."); }
     if (!Dut_SetRxAntennasMask(RxMask))  { strcpy(lpszRunInfo, "Dut_SetRxAntennasMask Fail."); }
     if (!Dut_SetTxAntenna(txMask))       { strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail."); }
     if (!Dut_SetRxAntenna(RxMask))       { strcpy(lpszRunInfo, "Dut_SetRxAntenna Fail."); }
     if (!Dut_SetCloseLoop(isOpenClose))  { strcpy(lpszRunInfo, "Dut_SetCloseLoop Fail."); }
     if (!Dut_SetHdkConfigure(offlineCalMask, 0)) { strcpy(lpszRunInfo, "Dut_SetHdkConfigure Fail."); }
     if (!Dut_SetChannel(phyType, band, bandwidth, channel, calibrationMask, isOpenClose)) { strcpy(lpszRunInfo, "Dut_SetChannel Fail."); }
     if (!Dut_SetTxAntenna(txMask)) { strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail."); }
     if (!Dut_SetRxAntenna(0))      { strcpy(lpszRunInfo, "Dut_SetRxAntenna Fail."); }
     if (!Dut_SetTxRate(rate, phyType, CP_MODE, bandwidth)) { strcpy(lpszRunInfo, "Dut_SetTxRate Fail."); }
     if (!Dut_SetIFS(IFS)) { strcpy(lpszRunInfo, "Dut_SetIFS Fail."); }
     if (!Dut_TxPackets(txPacketRate, length, repetitions, SGI, isDataLong)) { strcpy(lpszRunInfo, "Dut_TxPackets Fail."); }
     if (!Dut_SetTxAntenna(TxAnt)) { strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail."); }
     if (!SetPowerLimit(PowerLimit, isOpenClose)) { strcpy(lpszRunInfo, "SetPowerLimit Fail."); }
     return MT_RET_OK;
}

 CTxControl::Status CTxControl::ShiftPowerOutVector(const ShiftPowerOutTableParames& shiftPowerOutTableParames)
 {
    if (debug == TRUE)
         ExportTestLog("API Flow: ShiftPowerOutVector => antNumber: %d, BW: %d, pout_table_index: %d, pout_table_gain_or_offset: %d.", shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw, shiftPowerOutTableParames.pout_table_index, static_cast<unsigned char>(shiftPowerOutTableParames.pout_table_gain_or_offset));
  
    return (CTxControl::Status)_dutApi->Dut_ShiftPowerOutVector(shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw,
        shiftPowerOutTableParames.pout_table_index, static_cast<unsigned char>(shiftPowerOutTableParames.pout_table_gain_or_offset));
 }

 CTxControl::Status CTxControl::SetPowerLimit(const double& powerIndex, OpenClose LoopParameter)
 {
     _openLoopCloseLoop = LoopParameter;
     _powerIndex = powerIndex;
     _powerIndex = round((_openLoopCloseLoop == CLOSE_LOOP) ? powerIndex * 2 : powerIndex);

     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_SetPowerLimit => powerIndex:%.3lf, OpenClose:%d. ", _powerIndex, _openLoopCloseLoop);
     return (CTxControl::Status)_dutApi->Dut_SetPowerLimit(_powerIndex);
 }

 CTxControl::Status CTxControl::GetPoutTableOffset(ShiftPowerOutTableParames& shiftPowerOutTableParames)
 {
     CTxControl::Status m_Status = (CTxControl::Status)_dutApi->Dut_GetPoutTableOffset(shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw,
         &shiftPowerOutTableParames.pout_table_gain_or_offset);

     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_GetPoutTableOffset => antNumber: %d, BW: %d, pout_table_gain_or_offset: %d.", shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw, shiftPowerOutTableParames.pout_table_gain_or_offset);
     return m_Status;
 }

 CTxControl::Status CTxControl::SetPoutTableOffset(const ShiftPowerOutTableParames& shiftPowerOutTableParames)
 {
     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_SetPoutTableOffset => antNumber: %d, BW: %d, pout_table_gain_or_offset: %d.", shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw, shiftPowerOutTableParames.pout_table_gain_or_offset);

     return (CTxControl::Status)_dutApi->Dut_SetPoutTableOffset(shiftPowerOutTableParames.antNumber, shiftPowerOutTableParames.bw,
         shiftPowerOutTableParames.pout_table_gain_or_offset);
 }

 CTxControl::Status CTxControl::S2dSetInitials(const S2dSetInitialsParames& s2dSetInitialsParames)
 {
     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_S2dSetInitials => Power_vector: %d, S2D_offset: %d, tssiLow: %d, tssiHigh: %d, vectorLen: %d.", (int*)s2dSetInitialsParames.Power_vector, s2dSetInitialsParames.S2D_offset, s2dSetInitialsParames.tssiHigh, 2);

     return (CTxControl::Status)_dutApi->Dut_S2dSetInitials((int*)s2dSetInitialsParames.Power_vector, s2dSetInitialsParames.S2D_offset, s2dSetInitialsParames.tssiLow, s2dSetInitialsParames.tssiHigh, 2);
 }

 CTxControl::Status CTxControl::S2dCalibrate(S2dCalibrateParames& s2dCalibrateParames)
 {
     CTxControl::Status m_Status = (CTxControl::Status)_dutApi->Dut_S2dCalibrate(&s2dCalibrateParames.outS2dGain, &s2dCalibrateParames.outS2dOffset, s2dCalibrateParames.regionNumber, s2dCalibrateParames.pThreshold);
     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_S2dCalibrate => outS2dGain: %d, S2D_offset: %d, regionNumber: %d, pThreshold: %d.", s2dCalibrateParames.outS2dGain, s2dCalibrateParames.outS2dOffset, s2dCalibrateParames.regionNumber, s2dCalibrateParames.pThreshold);
     return m_Status;
 }

 CTxControl::Status CTxControl::SetS2dParams(const SetS2dParamsParames& setS2dParamsParames)
 {
     if (debug == TRUE)
         ExportTestLog("API Flow: Dut_SetS2dParams => antNumber: %d, signalBw: %d, region: %d, pThreshold: %d, gain: %d, offset: %d.", setS2dParamsParames.antNumber, setS2dParamsParames.signalBw, setS2dParamsParames.region, setS2dParamsParames.pThreshold, setS2dParamsParames.gain, setS2dParamsParames.offset);

     return (CTxControl::Status)_dutApi->Dut_SetS2dParams(setS2dParamsParames.antNumber, setS2dParamsParames.signalBw, setS2dParamsParames.region, setS2dParamsParames.pThreshold, setS2dParamsParames.gain, setS2dParamsParames.offset);
 }

 CTxControl::Status CTxControl::getABPoints(Dut_CalDataStructVer6* CalArray)
 {
     Dut_CalDataStructVer6 OutCalArray;
     _error = (CTxControl::Status)_dutApi->Dut_GetABPoints(CalArray, &OutCalArray);
     if (CTxControl::Status::PASS != _error)
         return (CTxControl::Status::FAIL);
     memcpy(CalArray, &OutCalArray, sizeof OutCalArray);
     return (CTxControl::Status::PASS);
 }

 CTxControl::Status CTxControl::txGetAbErrors(float abErrors[MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER])
 {
     _error = (CTxControl::Status)_dutApi->Dut_TxGetAbErrors(abErrors);
     if (CTxControl::Status::PASS != _error)
         return (CTxControl::Status::FAIL);
     return (CTxControl::Status::PASS);
 }

 CTxControl::Status CTxControl::writeTssiCalData(const Dut_CalDataStructVer6* CalArray, int ArraySize)
 {
     Dut_CalDataStructVer6* OutCalArray = new Dut_CalDataStructVer6[ArraySize];

     _error = (CTxControl::Status)_dutApi->Dut_writeTssiCalData(CalArray, OutCalArray, ArraySize);
     if (CTxControl::Status::PASS != _error)
         return (CTxControl::Status::FAIL);
     delete OutCalArray;
     return (CTxControl::Status::PASS);
 }

 CTxControl::Status CTxControl::WriteProdFlag(bool flag)
 {
     // dut 52
     return (CTxControl::Status)_dutApi->Dut_WriteProdFlag(false);
  //   return CTxControl::Status::PASS;
 }

 CTxControl::Status CTxControl::flushNvMemory()
 {
     _error = (CTxControl::Status)_dutApi->Dut_WriteCompleteEEPROM();
     if (CTxControl::Status::PASS != _error)
         return (CTxControl::Status::FAIL);

     _error = (CTxControl::Status)_dutApi->Dut_FlushNvMemory(1);
     if (CTxControl::Status::PASS != _error)
         return (CTxControl::Status::FAIL);

     return (CTxControl::Status::PASS);
 }