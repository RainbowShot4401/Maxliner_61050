// version: 1.0.0.2
//change some parameter of header define, refer v1.0.0.1

typedef enum CABLELOSS_TYPE
{
	CABLELOSS_TX = 0,
	CABLELOSS_RX = 1,
	CABLELOSS_GU = 2
};

#ifndef _IQFACT_DEFINE_H__
#define _IQFACT_DEFINE_H__
//#define	DLLEXPORT	__declspec(dllexport)

typedef BOOL(CALLBACK *funcSetPowerSupply)(BOOL TurnOn, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK *funcGetMessage)(LPSTR lpszRunInfo);
typedef BOOL(CALLBACK *funcBtGetPPM)(BOOL bRetry, int iChannelIdx, double dbExpectedPower, LPSTR szAntenna,double FreqShiftHz, double &dbPPM, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK *funcGetPPM)(BOOL bRetry, int iChannelIdx, double dbExpectedPower, LPSTR szBand, LPSTR szAntenna, LPSTR szDataRate, LPSTR szBandWidth, double FreqShiftHz, double& dbPPM, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK *funcGetTxPower)(BOOL bRetry, int iChannelIdx, double dbExpectedPower, LPSTR szBand, LPSTR szAntenna, LPSTR szDataRate, LPSTR szBandWidth, double& dbActualPower, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK* funcGetTxPowerEvm)(BOOL bRetry, int iChannelIdx, double dbExpectedPower, LPSTR szBand, LPSTR szAntenna, LPSTR szDataRate, LPSTR szBandWidth, double& dbActualPower, double& dbEvm, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK* funcReadCableLoss)(CABLELOSS_TYPE Type, LPSTR szAntenna, double dbFreqMHz, double& dbCableLoss, LPSTR lpszRunInfo);

extern "C" BOOL PASCAL EXPORT CableLossController(funcReadCableLoss cbReadCableLoss, LPSTR lpszRunInfo);
//Wifi Rx Calibration
typedef BOOL(CALLBACK* funcStartRxPower)(BOOL bRetry, double dbFrequencyHz, double dbFreqOffset, double dbAmplitude, LPSTR lpszRunInfo);
typedef BOOL(CALLBACK* funcStopRxPower)(BOOL bRetry, LPSTR lpszRunInfo);

extern "C" BOOL PASCAL EXPORT PowerSupplyCtrl(funcSetPowerSupply cbPSCtrl, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT SfcsResponds(int MESSAGE_TYPE, LPSTR PREPOSING_CODE, LPSTR RESPONS_MSG, LPSTR lpszRunInfo);
//extern "C" BOOL PASCAL EXPORT GetRdModeLicense();

extern "C" BOOL PASCAL EXPORT APPLICATION_CLOSE_TO_DUT();
extern "C" BOOL PASCAL EXPORT LOAD_GUI_INFORMATION(LPSTR lpszOP, LPSTR lpszBarcode, LPSTR lpszPN, SFCS_MODE eSfcsMode, BOOL bStopOnFailed, HWND hParentWnd, ...);
extern "C" BOOL PASCAL EXPORT RestartDutTest();

extern "C" BOOL PASCAL EXPORT SetToDutParameter(LPSTR ActionName, LPSTR ParameterName,LPSTR ParameterValue);
extern "C" BOOL PASCAL EXPORT CleanToDutParameter(LPSTR ActionName);
extern "C" BOOL PASCAL EXPORT PreShootToDutParameter(LPSTR ActionName, LPSTR lpszRunInfo);
extern "C" TM_RETURN PASCAL EXPORT CustomizeTest(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, int nRetryCnt, BOOL bRetry, ...);
extern "C" BOOL PASCAL EXPORT WiFiInitialize(BOOL bRetry, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT WiFiTerminate(BOOL bRetry, LPSTR lpszRunInfo);

//Wifi Tx stack
extern "C" BOOL PASCAL EXPORT GetTargetPower(DUT_PARAM &DutParam,LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT GetPowerIndex(DUT_PARAM &DutParam,LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT StartFrameModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT StopFrameModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo);
//Wifi Calibration
extern "C" TM_RETURN PASCAL EXPORT StartForXtalCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, funcGetPPM cbGetPPM,...);
//extern "C" TM_RETURN PASCAL EXPORT StartForTxCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcGetTxPower cbGetTxPower, ...);
extern "C" TM_RETURN PASCAL EXPORT StartForTxCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcGetTxPower cbGetTxPower, funcGetTxPowerEvm cbGetTxEvm, ...);
//Wifi Rx Calibration
extern "C" TM_RETURN PASCAL EXPORT StartForRxIQCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcStartRxPower cbStartRxPower, funcStopRxPower cbStopRxPower, ...);

extern "C" BOOL PASCAL EXPORT LoadDutRequired(BOOL bRetry, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT ParserWiFiCableLossToDut(double *dbCableLoss, LPSTR lpszRunInfo);

extern "C" TM_RETURN PASCAL EXPORT ReturnOemAppMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt,BOOL bRetry);
extern "C" TM_RETURN PASCAL EXPORT ReturnPreloadMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport);
extern "C" TM_RETURN PASCAL EXPORT WriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
extern "C" TM_RETURN PASCAL EXPORT EepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);

//bt
extern "C" BOOL PASCAL EXPORT BtInitialize(BOOL bRetry, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStartFrameModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStopFrameModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStartContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStopContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtGetTargetPower(BT_DUT_PARAM &DutParam,LPSTR lpszRunInfo);

//Jerry Add
extern "C" BOOL PASCAL EXPORT BtResetRxPacketCount(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStartRxPacketReceive(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtGetRxPacketCount(BT_DUT_PARAM DutParam, double &dbRxPacketCount, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT BtStopRxPacketReceive(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);

extern "C" TM_RETURN PASCAL EXPORT BtEepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
extern "C" TM_RETURN PASCAL EXPORT BtWriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
extern "C" TM_RETURN PASCAL EXPORT BtForXtalCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, funcBtGetPPM cbGetPPM);

extern "C" BOOL PASCAL EXPORT StartContModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo);
extern "C" BOOL PASCAL EXPORT StopContModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo);

#endif