#include "..\stdafx.h"
#include "DutControl.h"

CDutControl::CDutControl(dutWrapper *dutApi) : _dutApi(dutApi) 
{
    isConected = DISCONNECTED;
}
//CDutControl::CDutControl(dutWrapper *dutApi){ _dutApi = dutApi; }
CDutControl::~CDutControl() {}

/*********************** Inset DUT ***********************/
MT_RET CDutControl::DUT_SetPrintingLevel(int level)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: DUT_SetPrintingLevel => level: %d", level);
   if(!_dutApi->DUT_SetPrintingLevel(level))
   {
      ExportTestLog("DUT_SetPrintingLevel Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetNvMemoryType(int memoryType)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetNvMemoryType => memoryType: %d", memoryType);
   if(!_dutApi->Dut_SetNvMemoryType(memoryType))
   {
      ExportTestLog("Dut_SetNvMemoryType Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetNvMemorySize(int memorySize)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetNvMemorySize => memorySize: %d", memorySize);
   if(!_dutApi->Dut_SetNvMemorySize(memorySize))
   {
      ExportTestLog("Dut_SetNvMemorySize Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetResetSettings(int phyType, int spectrumBW, int signalBW, int rateIndex, int packetLength, int channel, int calibrationMask, int closedLoop, int txRate, float power)
{
   if(debug == TRUE)
   {
      ExportTestLog("API Flow: Dut_SetResetSettings => phyType: %d, spectrumBW: %d, signalBW: %d, rateIndex: %d, packetLength: %d, channel: %d, calibrationMask: %d, closedLoop: %d, txRate: %d, power: %f", \
         phyType, spectrumBW, signalBW, rateIndex, packetLength, channel, calibrationMask, closedLoop, txRate, power); 
   }
   if(!_dutApi->Dut_SetResetSettings(phyType, spectrumBW, signalBW, rateIndex, packetLength, channel, calibrationMask, closedLoop, txRate, power))
   {
      ExportTestLog("Dut_SetResetSettings Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetConnectionMode(int connectionType, const char* optIPaddr)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetConnectionMode => connectionType: %d, optIPaddr: %s", connectionType, optIPaddr);
   if(!_dutApi->Dut_SetConnectionMode(connectionType, optIPaddr))
   {
      ExportTestLog("Dut_SetConnectionMode Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetWLANindex(int wlanIndex)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetWLANindex => wlanIndex: %d", wlanIndex);
   if(!_dutApi->Dut_SetWLANindex(wlanIndex))
   {
      ExportTestLog("Dut_SetWLANindex Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_DriverInit()
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_DriverInit");
   if(!_dutApi->Dut_DriverInit())
   {
      ExportTestLog("Dut_DriverInit Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_SetChangeBand(unsigned int band)
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_SetChangeBand => band: %d", band);
   if(!_dutApi->Dut_SetChangeBand(band))
   {
      ExportTestLog("Dut_SetChangeBand Fail.");
      return FAIL;
   }
   return PASS;
}

MT_RET CDutControl::Dut_DriverVersion(double* version)
{
   if(!_dutApi->Dut_DriverVersion(version))
   {
      ExportTestLog("Dut_DriverVersion Fail.");
      return FAIL;
   }
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_DriverVersion => version: %lf", version);
   return PASS;
}

MT_RET CDutControl::Dut_ReadChipVersion(int* pRFChipVersion, int* pBBChipVersion)
{
   if(!_dutApi->Dut_ReadChipVersion(pRFChipVersion, pBBChipVersion))
   {
	  ExportTestLog("Dut_DriverVersion Fail.");
      return FAIL;
   }
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_ReadChipVersion=> pRFChipVersion: %d, pBBChipVersion: %d", pRFChipVersion, pBBChipVersion);
   return PASS;
}

MT_RET CDutControl::Dut_GetComponentVersion(int type, BYTE* version)
{
   if(!_dutApi->Dut_GetComponentVersion(type, version))
   {
	  ExportTestLog("Dut_GetComponentVersion Fail.");
      return FAIL;
   }
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_GetComponentVersion=> type: %d, version: %d", type, version);
   return PASS;
}

MT_RET CDutControl::Dut_GetBandSupport()
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_GetBandSupport");
   return _dutApi->Dut_GetBandSupport();
}

MT_RET CDutControl::Dut_DriverRelease()
{
   if(debug == TRUE)
      ExportTestLog("API Flow: Dut_DriverRelease");
   if(!_dutApi->Dut_DriverRelease())
   {
      ExportTestLog("Dut_DriverRelease Fail.");
      return FAIL;
   }
   return PASS;
}
/*********************************************************************/