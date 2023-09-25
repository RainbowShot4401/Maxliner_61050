// IQfact_QCA99xx.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include "Intel_WAV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果這個 DLL 是動態地對 MFC DLL 連結，
//		那麼從這個 DLL 匯出的任何會呼叫
//		MFC 內部的函式，都必須在函式一開頭加上 AFX_MANAGE_STATE
//		巨集。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此處為正常函式主體
//		}
//
//		這個巨集一定要出現在每一個
//		函式中，才能夠呼叫 MFC 的內部。這意味著
//		它必須是函式內的第一個陳述式
//		，甚至必須在任何物件變數宣告前面
//		，因為它們的建構函式可能會產生對 MFC
//		DLL 內部的呼叫。
//
//		請參閱 MFC 技術提示 33 和 58 中的
//		詳細資料。
//

// CIQfact_QCA99xxApp

BEGIN_MESSAGE_MAP(CIntel_WAV, CWinApp)
END_MESSAGE_MAP()

GLOBAL_INFO		g_GlobalInfo;
CABLELOSS		g_CableLoss;
CALLBACKFN		CallBackFn;
NET_INFO			Net_Info;
TEST_INFO		Test_Info;

map<string, string> tmpAction;
string templateAction;

CIntel_WAV theApp;

CIntel_WAV::CIntel_WAV()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
    WIDE_BAND_Table.insert(pair<string, string>("WB_20MHZ", "BW-20"));
    WIDE_BAND_Table.insert(pair<string, string>("WB_40MHZ", "BW-40"));
    WIDE_BAND_Table.insert(pair<string, string>("WB_80MHZ", "BW-80"));
    WIDE_BAND_Table.insert(pair<string, string>("WB_160MHZ", "BW-160"));

    BAND_Table.insert(pair<string, string>("11a", "802.11ag"));
    BAND_Table.insert(pair<string, string>("11b", "802.11b"));
    BAND_Table.insert(pair<string, string>("11g", "802.11ag"));
    BAND_Table.insert(pair<string, string>("11n", "802.11n"));
    BAND_Table.insert(pair<string, string>("11ac", "802.11ac"));
    BAND_Table.insert(pair<string, string>("11ax", "802.11ax"));
    theApp.fillTxStructParams = {};
}

CIntel_WAV::~CIntel_WAV()
{
   // TODO: 在此加入建構程式碼，
   // 將所有重要的初始設定加入 InitInstance 中
    WIDE_BAND_Table.clear();
    BAND_Table.clear();
    theApp.fillTxStructParams = {};
}

BOOL CIntel_WAV::InitInstance()
{
	CWinApp::InitInstance();
	//Open the Pipe ID
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	if(!PipeLineOpen(&g_GlobalInfo.Pipe_SYS_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);//arp -d & write Mac or License Key tool
	}

	if(!PipeLineOpen(&g_GlobalInfo.Pipe_OEM_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);//plink & /etc/init.d/.. & write dut env & verify dut env
	}
	if(!PipeLineOpen(&g_GlobalInfo.Pipe_PROG_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);// IQrun_Console -run
	}

	GetLocalDirectory(g_GlobalInfo.szLocalPath,szMessage);
	return TRUE;
}

/**********************************************************
* Templeate Action initial Call stack
**********************************************************/
extern "C" BOOL PASCAL EXPORT LOAD_GUI_INFORMATION(LPSTR lpszOP, LPSTR lpszBarcode, LPSTR lpszPN, SFCS_MODE eSfcsMode, BOOL bStopOnFailed, HWND hParentWnd, ...)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());//Load GUI Info.
   strCopy(g_GlobalInfo.szOP,lpszOP);
   strCopy(g_GlobalInfo.szBarcode,lpszBarcode);
   strCopy(g_GlobalInfo.szPN,lpszPN);
   g_GlobalInfo.Main_Handle=hParentWnd;
   theApp.dutApi = new dutWrapper;
  
   if(theApp.dutApi == NULL)
   {
      ExportTestLog("dutWrapper create fail.");
      return FALSE;
   }
   else 
      return TRUE;
}
extern "C" BOOL PASCAL EXPORT CleanToDutParameter(LPSTR ActionName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   tmpAction.clear();
   templateAction = "";
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT SetToDutParameter(LPSTR ActionName, LPSTR ParameterName,LPSTR ParameterValue)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   templateAction = ActionName;
   tmpAction.insert(pair<string, string>(ParameterName, ParameterValue));
   return TRUE;
}

extern "C" BOOL PASCAL EXPORT PreShootToDutParameter(LPSTR ActionName, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}

extern "C" TM_RETURN PASCAL EXPORT CustomizeTest(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, int nRetryCnt, BOOL bRetry, ...)
{    
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   theApp.m_funcGetMessage  = cbListMessage;
   // CCustomizeTest m_CCustomizeTest;
   // m_CCustomizeTest.test();
   return TM_RETURN_PASS;
}
extern "C" BOOL PASCAL EXPORT WiFiInitialize(BOOL bRetry, LPSTR lpszRunInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (theApp.dutApi == NULL)
    {
        ExportTestLog("dutWrapper create fail.");
        return FALSE;
    }

    if (theApp.insertDut == NULL)
    {
        theApp.insertDut = new CDutControl(theApp.dutApi);
        if (theApp.insertDut == NULL)
        {
            strcpy(lpszRunInfo, "Class create FAIL.");
            return FALSE;
        }
    }

    char strMsg[DLL_INFO_SIZE];
    MT_RET status;
    int printingLevel = atoi(tmpAction["PRINTINGLEVEL"].c_str());
    int memoryType = atoi(tmpAction["NVMEMORYTYPE"].c_str());
    int memorySize = atoi(tmpAction["NVMEMORYSIZE"].c_str());
    int phyType, spectrumBW, signalBW, rateIndex, packetLength, channel, calibrationMask, closedLoop, txRate;
    float power;

    phyType = 0;
    spectrumBW = 0;
    signalBW = 0;
    rateIndex = 2;
    packetLength = 1200;
    channel = 7;
    calibrationMask = 65535;
    closedLoop = 0;
    txRate = 7;
    power = 13;
    int connectionType = 3;
    string ipAddr = tmpAction["IPADDRESS"];
    int WLANindex = atoi(tmpAction["WLANINDEX"].c_str());
    unsigned int band = atoi(tmpAction["BAND"].c_str());
    double DUT_Version;
    int pRFChipVersion, pBBChipVersion;
    int type;
    BYTE CVversion[64], PSDversion[64];

    if (theApp.insertDut->isConected == CDutControl::ConnectionEnum::DISCONNECTED)
    {
        if (theApp.insertDut->DUT_SetPrintingLevel(printingLevel) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_SetNvMemoryType(memoryType) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_SetNvMemorySize(memorySize) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_SetResetSettings(phyType, spectrumBW, signalBW, rateIndex, packetLength, channel, calibrationMask, closedLoop, txRate, power) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_SetConnectionMode(connectionType, ipAddr.c_str()) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_SetWLANindex(WLANindex) != MT_RET_OK)
            return FALSE;
        if (theApp.insertDut->Dut_DriverInit() != MT_RET_OK)
            return FALSE;

        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "DUT_SetPrintingLevel  => printingLevel: %d", printingLevel);
        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_SetNvMemoryType   => memoryType   : %d", memoryType);
        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_SetNvMemorySize   => memorySize   : %d", memorySize);
        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, \
            "Dut_SetResetSettings => phyType: %d, spectrumBW: %d, signalBW:%d, rateIndex: %d, packetLength: %d, channel: %d, calibrationMask: %d, closedLoop: %d, txRate: %d, power: %f", \
            phyType, spectrumBW, signalBW, rateIndex, packetLength, channel, calibrationMask, closedLoop, txRate, power);
        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_SetConnectionMode   => connectionType: %d, ipAddr: %s", connectionType, ipAddr.c_str());
        CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_SetWLANindex        => WLANindex     : %d", WLANindex);
    }
    if (theApp.insertDut->Dut_SetChangeBand(band) != MT_RET_OK)
        return FALSE;
    if (theApp.insertDut->Dut_DriverVersion(&DUT_Version) != MT_RET_OK)
        return FALSE;
    if (theApp.insertDut->Dut_ReadChipVersion(&pRFChipVersion, &pBBChipVersion) != MT_RET_OK)
        return FALSE;
    if (theApp.insertDut->Dut_GetComponentVersion(0, CVversion) != MT_RET_OK)
        return FALSE;

    int bandSupport = theApp.insertDut->Dut_GetBandSupport();
    CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_SetChangeBand       => band          : %d", band);
    CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_DriverVersion       => DUT Version   : %.3f", DUT_Version);
    CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_ReadChipVersion     => ChipVersion   : %0X, BBChipVersion: %0X", pRFChipVersion, pBBChipVersion);
    CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_GetComponentVersion => CVversion     : %s", CVversion);
    CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_GetComponentVersion => PSDversion    : %s", PSDversion);

    if (theApp.insertDut->isConected == CDutControl::ConnectionEnum::DISCONNECTED)
    {
        theApp.m_TxBurnCalGen6 = new TxBurnCalGen6(NULL, theApp.dutApi);
        if (theApp.m_TxBurnCalGen6 == NULL)
        {
            sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "WiFiInitialize => Class create FAIL.");
            return FALSE;
        }

        theApp.m_RxBurnCalGen6 = new RxBurnCalGen6(NULL, theApp.dutApi);
        if (theApp.m_RxBurnCalGen6 == NULL)
        {
            sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "WiFiInitialize => Class create FAIL.");
            return FALSE;
        }

        if (theApp.m_TxBurnCalGen6->clearTxStruct() != ITxBurnCal::Status::PASS)
        {
            sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "WiFiInitialize => clearTxStruct() FAIL.");
            return FALSE;
        }

        if (theApp.m_RxBurnCalGen6->clearRxStruct() != ITxBurnCal::Status::PASS)
        {
            sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "WiFiInitialize => clearRxStruct() FAIL.");
            return FALSE;
        }
    }
    Antenna_Index = WLANindex;
    theApp.insertDut->isConected = CDutControl::CONNECTED;
    return TRUE;
}

extern "C" BOOL PASCAL EXPORT WiFiTerminate(BOOL bRetry, LPSTR lpszRunInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   MT_RET status;
   CDutControl *removeDut = new CDutControl(theApp.dutApi);
   if(removeDut == NULL)
   {
      sprintf_s(lpszRunInfo, sizeof(lpszRunInfo),"Class create FAIL.");
      return FALSE;
   }
   status = removeDut->Dut_DriverRelease();
   CCommFunc::OutputMsgToBoth(theApp.m_funcGetMessage, "Dut_DriverRelease()");
   delete removeDut;
   delete theApp.m_TxBurnCalGen6;
   delete theApp.m_RxBurnCalGen6;
   delete theApp.insertDut;
   if(!status)
   {
      //sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "WiFiTerminate Fail.");
      return FALSE;
   }
   return TRUE;
}

int AntIndexMapping(double freq)
{
    if (Antenna_Index == 4)
    {
        Antenna.clear();
        Antenna.insert(pair<string, int>("TX_CHAIN_A", 1));
        Antenna.insert(pair<string, int>("TX_CHAIN_B", 2));
        Antenna.insert(pair<string, int>("TX_CHAIN_C", 4));
        Antenna.insert(pair<string, int>("TX_CHAIN_D", 8));
        antNum.clear();
        antNum.insert(pair<string, int>("TX_CHAIN_A", 0));
        antNum.insert(pair<string, int>("TX_CHAIN_B", 1));
        antNum.insert(pair<string, int>("TX_CHAIN_C", 2));
        antNum.insert(pair<string, int>("TX_CHAIN_D", 3));
    }
    else
    {
        Antenna.clear();
        antNum.clear();
        if ((freq >= 2412) && (freq <= 2484))
        {
            Antenna.insert(pair<string, int>("TX_CHAIN_A", 1));
            Antenna.insert(pair<string, int>("TX_CHAIN_C", 4));
            antNum.insert(pair<string, int>("TX_CHAIN_A", 0));
            antNum.insert(pair<string, int>("TX_CHAIN_B", 2));
        }
        else
        {
            Antenna.insert(pair<string, int>("TX_CHAIN_B", 2));
            Antenna.insert(pair<string, int>("TX_CHAIN_D", 8));
            antNum.insert(pair<string, int>("TX_CHAIN_A", 1));
            antNum.insert(pair<string, int>("TX_CHAIN_B", 3));
        }
    }
    return TRUE;
}
//Wifi Tx stack
extern "C" BOOL PASCAL EXPORT GetTargetPower(DUT_PARAM &DutParam,LPSTR lpszRunInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());	
   DutParam.TX_POWER_TARGET = std::stod(tmpAction["OUTPUT_POWER"].c_str());
   return TRUE;
}

extern "C" BOOL PASCAL EXPORT GetPowerIndex(DUT_PARAM &DutParam,LPSTR lpszRunInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return TRUE;
}

extern "C" BOOL PASCAL EXPORT StartFrameModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    double freq;
    int TxAnt;

    freq = stod(tmpAction["FREQ_MHZ"].c_str());
    AntIndexMapping(freq);
    TxAnt = Antenna[tmpAction["TX_CHAIN"]];
    CTxControl* m_CTxControl = new CTxControl(theApp.dutApi);
    if (tmpAction["DEBUG"] == "TRUE")
        m_CTxControl->debug = TRUE;
    m_CTxControl->DUT_TxStart(DutParam, tmpAction, TxAnt, lpszRunInfo);
    delete m_CTxControl;
    return TRUE;
}

extern "C" BOOL PASCAL EXPORT StopFrameModulatedTx(DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    MT_RET status;
    CTxControl* m_CTxControl = new CTxControl(theApp.dutApi);
    if (tmpAction["DEBUG"] == "TRUE")
        m_CTxControl->debug = TRUE;
    status = m_CTxControl->Dut_EndTxPackets();
    delete m_CTxControl;
    if (!status)
    {
        sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "StopFrameModulatedTx Fail.");
        return FALSE;
    }
    return TRUE;
}

extern "C" TM_RETURN PASCAL EXPORT StartForXtalCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, funcGetPPM cbGetPPM, ...)
{
   CTxControl *m_CTxControl = new CTxControl(theApp.dutApi);
   if (m_CTxControl == NULL)
   {
       ErrorReport.DisableSfcsLink = FALSE;
       strCopy(ErrorReport.ErrorCode, _T("T1109"));
       strCopy(ErrorReport.ErrorDetail, _T("NULL"));
       sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForXtalCalibration =>Class(m_CTxControl) create Fail.");
       strCopy(ErrorReport.FailedTest, _T("StartForXtalCalibration"));
       ErrorReport.TestResult = FALSE;
       return TM_RETURN_ERROR;
   }

   CXtalCalibration::XTAL_PARAMS m_XTAL_PARAMS;
   DUT_PARAM DutParam;
   double freq;
   int TxAnt;
   /*
   typedef struct _DUT_PARAM
   {
       char TX_CHAIN[32];
       char RX_CHAIN[32];
       char BAND[32];
       char RATE[32];
       char WIDE_BAND[32];
       char PREAMBLE[32];
       int CHANNEL;
       double TX_POWER_TARGET;
       int TX_POWER_INDEX[4];
   }DUT_PARAM, * PDUT_PARAM;
   */
   /*
   	struct XTAL_PARAMS {
		double centralFreqMHz = 5180;
		double cableLoss = 20;
		int powerIndex = 40;
		string standard = "802.11n";
		string bandwidth = "BW - 40";
		string rate = "MCS0";
		int num_of_streams = 1;
	};
   */
   DutParam.TX_POWER_TARGET = stoi(tmpAction["TARGET_POWER"].c_str());
   strcpy(DutParam.TX_CHAIN, tmpAction["TX_CHAIN"].c_str());
   strcpy(DutParam.RX_CHAIN, tmpAction["RX_CHAIN"].c_str());
   strcpy(DutParam.BAND, tmpAction["BAND"].c_str());
   strcpy(DutParam.RATE, tmpAction["RATE"].c_str());
   strcpy(DutParam.WIDE_BAND, tmpAction["WIDE_BAND"].c_str());
   strcpy(DutParam.PREAMBLE, tmpAction["PREAMBLE"].c_str());
   DutParam.TX_POWER_INDEX[0] = 0;
   freq = stod(tmpAction["FREQ_MHZ"].c_str());
   DutParam.CHANNEL = CCommFunc::WIFI_FrequencyMHzToChannel(freq);

   m_XTAL_PARAMS.centralFreqMHz = freq;
   m_XTAL_PARAMS.powerIndex = stoi(tmpAction["POWERINDEX"].c_str());
   CXtalCalibration* m_CXtalCalibration = new CXtalCalibration(theApp.dutApi, m_XTAL_PARAMS);
   if (m_CXtalCalibration == NULL)
   {
       ErrorReport.DisableSfcsLink = FALSE;
       strCopy(ErrorReport.ErrorCode, _T("T1109"));
       strCopy(ErrorReport.ErrorDetail, _T("NULL"));
       sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForXtalCalibration =>Class(m_CXtalCalibration) create Fail.");
       strCopy(ErrorReport.FailedTest, _T("StartForXtalCalibration"));
       ErrorReport.TestResult = FALSE;
       return TM_RETURN_ERROR;
   }
   AntIndexMapping(freq);
   TxAnt = Antenna[tmpAction["TX_CHAIN"]];
   m_CXtalCalibration->antMask       = stoi(tmpAction["ANTMASK"]);
   m_CXtalCalibration->TxAnt         = TxAnt;
   m_CXtalCalibration->phyType       = CCommFunc::ConvertStandardToPhyType(freq, tmpAction["BAND"]);
   m_CXtalCalibration->band          = CCommFunc::ConvertFreqToBand(freq);
   m_CXtalCalibration->bandwidth     = CCommFunc::ConvertBandwidth(tmpAction["WIDE_BAND"]);
   m_CXtalCalibration->channel       = CCommFunc::ConvertLoFreqtoLowChannel(tmpAction["WIDE_BAND"], freq);
   m_CXtalCalibration->PPM_tolerance = stoi(tmpAction["TOLERANCE"]);
   m_CXtalCalibration->targetPPM     = stoi(tmpAction["TARGET_PPM"]);
   m_CXtalCalibration->debug         = (tmpAction["DEBUG"] == "TRUE" ? TRUE : FALSE);

   TM_RETURN XtalTest = m_CXtalCalibration->Run(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport, cbGetPPM, &DutParam);
   if (XtalTest == TM_RETURN_ERROR)
       return TM_RETURN_ERROR;
   else if(XtalTest == TM_RETURN_FAIL)
       return TM_RETURN_FAIL;

   return TM_RETURN_PASS;
}

const std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    std::vector<std::string> result;
    std::string::size_type begin, end;

    end = str.find(pattern);
    begin = 0;

    while (end != std::string::npos) {
        if (end - begin != 0) {
            result.push_back(str.substr(begin, end - begin));
        }
        begin = end + pattern.size();
        end = str.find(pattern, begin);
    }

    if (begin != str.length()) {
        result.push_back(str.substr(begin));
    }
    return result;
}

#define MAX_NUM_OF_INDEXES 127
struct
{
    int index_array[MAX_NUM_OF_INDEXES];
    int TPC_value_arr[MAX_NUM_OF_INDEXES];
    double power_arr[MAX_NUM_OF_INDEXES];
    int voltage_arr[MAX_NUM_OF_INDEXES];
    double evm_arr[MAX_NUM_OF_INDEXES];
} txCalIndexesData;

extern "C" TM_RETURN PASCAL EXPORT StartForTxCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcGetTxPower cbGetTxPower, funcGetTxPowerEvm cbGetTxEvm, ...)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DUT_PARAM DutParam;
    double freq;
    char lpszRunInfo[512];
    int TxAnt, ant_Num;
    ITxCalibration::Status Test_Status = ITxCalibration::Status::FAIL;
    BOOL Measure_EVM;

    /* struct TxSetChannelParams {
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
     
    DutParam.TX_POWER_TARGET = stoi(tmpAction["TARGET_POWER"].c_str());
    strcpy(DutParam.TX_CHAIN, tmpAction["TX_CHAIN"].c_str());
    strcpy(DutParam.RX_CHAIN, tmpAction["RX_CHAIN"].c_str());
    strcpy(DutParam.BAND, tmpAction["BAND"].c_str());
    strcpy(DutParam.RATE, tmpAction["RATE"].c_str());
    strcpy(DutParam.WIDE_BAND, tmpAction["WIDE_BAND"].c_str());
    strcpy(DutParam.PREAMBLE, tmpAction["PREAMBLE"].c_str());
    DutParam.TX_POWER_INDEX[0] = 0;
    freq = stod(tmpAction["FREQ_MHZ"].c_str());
    AntIndexMapping(freq);
    //DutParam.CHANNEL = CCommFunc::WIFI_FrequencyMHzToChannel(freq);
    DutParam.CHANNEL = freq;
    TxAnt = Antenna[tmpAction["TX_CHAIN"]];
    ant_Num = antNum[tmpAction["TX_CHAIN"]];

    ITxCalibration::TxSetChannelParams txSetChannelParams;
    txSetChannelParams.boardAntennaMask = stoi(tmpAction["ANTMASK"]);
    txSetChannelParams.bandwidth = WIDE_BAND_Table[tmpAction["WIDE_BAND"].c_str()];
    txSetChannelParams.freq = freq;
    txSetChannelParams.isCloseLoop = CTxControl::OPEN_LOOP;
    txSetChannelParams.offlineCalMask = 0xFFFF;
    txSetChannelParams.standard = BAND_Table[tmpAction["BAND"].c_str()];
    txSetChannelParams.txAntennaMask = stoi(tmpAction["ANTMASK"]);
    txSetChannelParams.numOfStreams =  1;
    txSetChannelParams.rate = tmpAction["CAL_RATE"].c_str();
    if (tmpAction["DUT_SLEEP_AFTER_SET_CHANNEL"] != "")
        txSetChannelParams.sleepAfterSetChannel = stoi(tmpAction["DUT_SLEEP_AFTER_SET_CHANNEL"].c_str());
 
    CTxControl* m_CTxControl = new CTxControl(theApp.dutApi);
    if (m_CTxControl == NULL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForTxCalibration =>Class(m_CTxControl) create Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }

    theApp.m_TxBurnCalGen6->dutFuncSet(m_CTxControl);
    if (tmpAction["DEBUG"] == "TRUE")
        m_CTxControl->debug = TRUE;

    if (m_CTxControl->Dut_EndTxPackets() == MT_RET_FAIL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "Dut_EndTxPackets Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_FAIL;
    }
    if (m_CTxControl->DUT_TxStart(DutParam, tmpAction, TxAnt, lpszRunInfo) == MT_RET_FAIL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "DUT_TxStart Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_FAIL;
    }

    ////////////// Start shift gain table//////////////////////
    int	regions[3];
    regions[0] = 62;
    regions[1] = 62;
    regions[2] = 62;

    ITxCalibration::TxGainTableParams txGainTableParams;
    memcpy_s(txGainTableParams.regions, sizeof txGainTableParams.regions, regions, sizeof regions);
    txGainTableParams.defaultCalibrationIndex = stoi(tmpAction["DEFAULTCALIBRATIONINDEX"].c_str());
    txGainTableParams.shiftPowerOutTableParames.antNumber = ant_Num;
    txGainTableParams.shiftPowerOutTableParames.bw = CCommFunc::ConvertBandwidthForBurn(tmpAction["WIDE_BAND"], tmpAction["BAND"]);
    txGainTableParams.shiftPowerOutTableParames.pout_table_index = 0;
    txGainTableParams.shiftPowerOutTableParames.pout_table_gain_or_offset = 0;

    TxGainTableCal_Gen6* m_TxGainTableCal_Gen6 = new TxGainTableCal_Gen6(m_CTxControl, theApp.dutApi, cbGetTxPower);
    if (m_TxGainTableCal_Gen6 == NULL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForTxCalibration =>Class(m_TxGainTableCal_Gen6) create Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    m_TxGainTableCal_Gen6->m_DUT_PARAM = DutParam;
    m_TxGainTableCal_Gen6->m_funcGetMessage = cbListMessage;
    if (tmpAction["SLEEP"] != "")
        m_TxGainTableCal_Gen6->sleep = stoi(tmpAction["SLEEP"].c_str());
    m_TxGainTableCal_Gen6->m_DUT_PARAM = DutParam;
    Test_Status = m_TxGainTableCal_Gen6->txGainTableCal(txGainTableParams);
    if (Test_Status == ITxCalibration::Status::PASS)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            m_TxGainTableCal_Gen6->debug = "TRUE";
        m_TxGainTableCal_Gen6->setTxGainTableParams();
    }
    else
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "txGainTableCal Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_FAIL;
    }
    ////////////// END shift gain table//////////////////////
    
    ////////////// Start S2D//////////////////////
    ITxCalibration::TxS2dParams txS2dParams;
    txS2dParams.antNum = ant_Num;
    txS2dParams.bw = CCommFunc::ConvertBandwidthForBurn(tmpAction["WIDE_BAND"], tmpAction["BAND"]);
    txS2dParams.band = CCommFunc::ConvertFreqToBand(freq);
    memcpy_s(txS2dParams.regions, sizeof txS2dParams.regions, regions, sizeof regions);
    txS2dParams.tssiHigh = stoi(tmpAction["TX_CAL_S2D_CAL_TSSI_HIGH"].c_str());
    txS2dParams.tssiLow = stoi(tmpAction["TX_CAL_S2D_CAL_TSSI_LOW"].c_str());

    TxS2dCal *m_TxS2dCal = new TxS2dCal(m_CTxControl, theApp.dutApi);
    if (m_TxS2dCal == NULL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForTxCalibration =>Class(m_TxS2dCal) create Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    m_TxS2dCal->m_funcGetMessage = cbListMessage;
    Test_Status = m_TxS2dCal->Run(txS2dParams);
    if (Test_Status != ITxCalibration::Status::PASS)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "txGainTableCal Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_FAIL;
    }
    ////////////// END S2D//////////////////////

    //------------------------------------------------------------------
    // EVM starting loop												
    //------------------------------------------------------------------
    Measure_EVM = (tmpAction["MEASURE_EVM"] == "TRUE") ? TRUE : FALSE;
    ITxCalibration::TxOpenLoopSweepParams txOpenLoopSweepParams = {};
    txOpenLoopSweepParams.antennaNumber = ant_Num;
    txOpenLoopSweepParams.analyzedPacketsPerIndex = 1;
    
    vector<string> index_Calibration = split(tmpAction["INDEXES_FOR_CALIBRATION"].c_str(), ",");
    txOpenLoopSweepParams.indexes.clear();
    for (int i = 0; i < index_Calibration.size(); i++)
        txOpenLoopSweepParams.indexes.push_back(stoi(index_Calibration.at(i)));
    //txOpenLoopSweepParams.indexes.insert(txOpenLoopSweepParams.indexes.begin(), index_Calibration_int.size(), index_Calibration_int);
    txOpenLoopSweepParams.packetLength = CCommFunc::CalcPacketLength(tmpAction["BAND"], tmpAction["RATE"], tmpAction["WIDE_BAND"]);
    txOpenLoopSweepParams.sifs = 25;

    //txOpenLoopSweepParams.setVsaAmpTolParams.ampTolerance = g_WiFi11ACDefaultGlobalSettingParam.VSA_AMPLITUDE_TOLERANCE_DB + 3;
    //txOpenLoopSweepParams.setVsaParams.rfFreqMHz = l_11ACtxCalParam.BSS_FREQ_MHZ_PRIMARY;
    //txOpenLoopSweepParams.setVsaParams.rfAmplDb = peakToAvgRatio - l_11ACtxCalParam.CABLE_LOSS_DB[antenna]; //need to add target power in class itself
    //txOpenLoopSweepParams.setVsaParams.port = g_WiFi11ACGlobalSettingParam.VSA_PORT;
    //txOpenLoopSweepParams.setVsaParams.triggerLevelDb = g_WiFi11ACDefaultGlobalSettingParam.VSA_TRIGGER_LEVEL_DB;
    //txOpenLoopSweepParams.setVsaParams.triggerPreTime = g_WiFi11ACDefaultGlobalSettingParam.VSA_PRE_TRIGGER_TIME_US / 1000000;
    //txOpenLoopSweepParams.captureVsaParams.samplingTimeSecs = DUT_IQlite_HELPER::CalcPacketTime(string(l_11ACtxCalParam.STANDARD), string(l_11ACtxCalParam.BSS_BANDWIDTH), string(l_11ACtxCalParam.DATA_RATE)) + g_WiFi11ACDefaultGlobalSettingParam.SIFS_TIME;
    //txOpenLoopSweepParams.cableLoss = l_11ACtxCalParam.CABLE_LOSS_DB[antenna];
    //txOpenLoopSweepParams.analyzeParams.standard = string(l_11ACtxCalParam.STANDARD);

    txOpenLoopSweepParams.constantResultsParams.enableConstValue  = TRUE;
    txOpenLoopSweepParams.constantResultsParams.measureEvm        = TRUE;
    txOpenLoopSweepParams.constantResultsParams.const_max_power   = stod(tmpAction["CONST_MAX_POWER"].c_str());
    txOpenLoopSweepParams.constantResultsParams.const_ultimat_evm = stod(tmpAction["CONST_UEVM_POWER"].c_str());

    txOpenLoopSweepParams.targetUltimateEvm = -36;
    txOpenLoopSweepParams.targetMaxPowerEvm = -25;

    OpenLoopSweep *m_OpenLoopSweep = new OpenLoopSweep(m_CTxControl, theApp.dutApi, cbGetTxPower, cbGetTxEvm);
    if (m_OpenLoopSweep == NULL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForTxCalibration =>Class(m_OpenLoopSweep) create Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    if (tmpAction["DEBUG"] == "TRUE")
        m_OpenLoopSweep->debug = TRUE;
    else 
        m_OpenLoopSweep->debug = FALSE;
    m_OpenLoopSweep->m_funcGetMessage = cbListMessage;
    m_OpenLoopSweep->m_DUT_PARAM = DutParam;
    Test_Status = m_OpenLoopSweep->txOpenLoopSweep(txOpenLoopSweepParams);
    if (Test_Status == ITxCalibration::Status::FAIL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForTxCalibration => txOpenLoopSweep Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    printf("11111");
    ITxCalibration::OpenLoopSweepResultsParams openLoopSweepResults = m_OpenLoopSweep->getOpenLoopSweepResults();
    copy(openLoopSweepResults.indexes.begin(), openLoopSweepResults.indexes.end(), txCalIndexesData.TPC_value_arr);
    copy(openLoopSweepResults.power.begin(), openLoopSweepResults.power.end(), txCalIndexesData.power_arr);
    copy(openLoopSweepResults.voltage.begin(), openLoopSweepResults.voltage.end(), txCalIndexesData.voltage_arr);
    copy(openLoopSweepResults.evm.begin(), openLoopSweepResults.evm.end(), txCalIndexesData.evm_arr);

    theApp.fillTxStructParams.txSetChannelParams                  = txSetChannelParams;
    theApp.fillTxStructParams.txGainTableParams[ant_Num]          = m_TxGainTableCal_Gen6->getTxGainTableParams();
    theApp.fillTxStructParams.OpenLoopSweepResultsParams[ant_Num] = m_OpenLoopSweep->getOpenLoopSweepResults();
    theApp.fillTxStructParams.maxPowerParams[ant_Num]             = m_OpenLoopSweep->getMaxPower();
    theApp.fillTxStructParams.ultimateEvmParams[ant_Num]          = m_OpenLoopSweep->getUltimateEvm();
    theApp.fillTxStructParams.txS2dParams[ant_Num]                = m_TxS2dCal->getTxS2dParams();
    //nlog = (g_WiFi11ACGlobalSettingParam.GEN5_OR_GEN6) ? (float)(round((g_WiFi11ACDefaultGlobalSettingParam.NLOG / pow(2,7)) * 10)) : g_WiFi11ACDefaultGlobalSettingParam.NLOG;
    theApp.fillTxStructParams.aBCoefficientsParams[ant_Num].nlog = 0;
    // end of loop   end of antenna loop

    if (theApp.fillTxStructParams.maxPowerParams[ant_Num].maxPower_Index < theApp.fillTxStructParams.ultimateEvmParams[ant_Num].ultimateEvmPower_Index)
    {
        //IF you stop, it's an issue, need to investigate!!!
        int init_value = theApp.fillTxStructParams.maxPowerParams[ant_Num].maxPower_Index;
        int fixedMaxPowerIndex = theApp.fillTxStructParams.ultimateEvmParams[ant_Num].ultimateEvmPower_Index + 6;

        if (fixedMaxPowerIndex > 63)
        {
            fixedMaxPowerIndex = theApp.fillTxStructParams.ultimateEvmParams[ant_Num].ultimateEvmPower_Index + 1;
            if (fixedMaxPowerIndex > 63)
            {
                fixedMaxPowerIndex = theApp.fillTxStructParams.ultimateEvmParams[ant_Num].ultimateEvmPower_Index;
            }
        }
        theApp.fillTxStructParams.maxPowerParams[ant_Num].maxPower_Index = fixedMaxPowerIndex;
        string erorMsg = "Had to fix max power index value to: " + to_string(fixedMaxPowerIndex) + " Was: " + to_string(init_value) + "!!!";
        //log.print(erorMsg, LOGGER_LEVEL::LOGGER_ERROR);
        //MessageBox(nullptr, TEXT(erorMsg.c_str()), TEXT("Warning Message!!!"), MB_OK);
    }
    if (m_CTxControl->Dut_EndTxPackets() == MT_RET_FAIL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T189"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "Dut_EndTxPackets Fail.");
        strCopy(ErrorReport.FailedTest, _T("StartForTxCalibration"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_FAIL;
    }
    if (tmpAction["TX_CAL_SAVE"] == "TRUE")
    {
        theApp.m_TxBurnCalGen6->fillTxStruct(theApp.fillTxStructParams);
        theApp.fillTxStructParams = {};
    }
    //  if ((l_11ACtxCalParam.ENABLE_S2D_CAL == 1) && (g_WiFi11ACDefaultGlobalSettingParam.TX_CAL_GAIN_TABLE_CALIBRATION_ENABLE == 1))
  //  {
  //      unique_ptr<TxBurnCalFactory> txBurnCalFactory(new TxBurnCalFactory(dutApi, (ITxCalibration::GenType)g_WiFi11ACGlobalSettingParam.GEN5_OR_GEN6));
  //      ITxBurnCal* tx_burn_obj = txBurnCalFactory->getTxBurnCalObj();

  //      delete tx_burn_obj;
  //  }
    delete m_CTxControl;
    delete m_TxGainTableCal_Gen6;
    delete m_TxS2dCal;
    delete m_OpenLoopSweep;

    return TM_RETURN_PASS;
}


int subBandConvertCharToInt(char* sub_band_freq_char, int rf_sub_band_freq[], int* subBandNumberOfFreq)
{
    try
    {
        vector<string> splits;
        splits =split(sub_band_freq_char, ",");
        for (int i = 0; i < splits.size(); i++)
        {
            rf_sub_band_freq[i] = atoi(splits[i].c_str());
        }
        *subBandNumberOfFreq = splits.size();
        return 1;
    }
    catch (...)
    {
        return 0;
    }
}

int fltanessConvertCharToInt(char* flteness_freq_char, int rf_flatness_freq[], int* flatnessNumberOfFreq)
{
    try
    {
        vector<string> splits;
        int new_freq = 0;
        int values_burned_in_cal = 0;

       // SplitString(flteness_freq_char, splits, ",");
        splits = split(flteness_freq_char, ",");
        for (int i = 0; i < splits.size(); i++)
        {
            rf_flatness_freq[i] = atoi(splits[i].c_str());

            if (stoi(CH_FREQ_MHZ) > 4000)
            {
                // round freq to 40MHz as in calibration file resolution

                values_burned_in_cal = ((rf_flatness_freq[i] - 4900) / 40);
                new_freq = values_burned_in_cal * 40 + 4900;

                if ((new_freq > 5320) && (new_freq < 5490))  // freq region without practical channels
                {
                    if (rf_flatness_freq[i] < 5320)
                    {
                        values_burned_in_cal -= 1;
                    }
                    else
                    {
                        values_burned_in_cal += 1;
                    }

                }
                new_freq = values_burned_in_cal * 40 + 4900;
                new_freq = (new_freq > 5735) ? new_freq + 5 : new_freq;
                rf_flatness_freq[i] = new_freq;
            }
        }
        *flatnessNumberOfFreq = splits.size();
        return 1;
    }
    catch (...)
    {
        return 0;
    }
}

//typedef BOOL(CALLBACK* funcStartRxPower)(BOOL bRetry, double dbFrequencyHz, double dbFreqOffset, double dbAmplitude, LPSTR lpszRunInfo);
//typedef BOOL(CALLBACK* funcStopRxPower)(BOOL bRetry, LPSTR lpszRunInfo);
#if 0
extern "C" TM_RETURN PASCAL EXPORT StartForRxIQCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcStartRxPower cbStartRxPower, funcStopRxPower cbStopRxPower, ...)
{
    char lpszRunInfo[1024];
    printf("1111");
    cbStartRxPower(FALSE, (5220 * 1E6), (1 * 1E6), -50, lpszRunInfo);
    //cbStartRxPower(TRUE, (5220 * 1E6) + (1.25 * 1E6), 0, -50, lpszRunInfo);
    printf("1111");
    return TM_RETURN_PASS;
}
#endif

#if 1
extern "C" TM_RETURN PASCAL EXPORT StartForRxIQCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT & ErrorReport, funcStartRxPower cbStartRxPower, funcStopRxPower cbStopRxPower, ...)
{
    int RxAnt, ant_Num;
    string APICallback, errorCode;
    TM_RETURN TestFlag = TM_RETURN_ERROR;

    RxAnt = Antenna[tmpAction["RX_CHAIN"]];
    ant_Num = antNum[tmpAction["RX_CHAIN"]];
   
    //IQLite script common.
    CH_FREQ_MHZ = tmpAction["CH_FREQ_MHZ"];
    BSS_FREQ_MHZ_PRIMARY = tmpAction["BSS_FREQ_MHZ_PRIMARY"];
    RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL = tmpAction["RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL"];
    STANDARD = tmpAction["STANDARD"];
    BAND_START_FREQ = tmpAction["BAND_START_FREQ"];
    BAND_END_FREQ = tmpAction["BAND_END_FREQ"];
    RX_CAL_PGC2_FOR_LNAGAIN_CAL = tmpAction["RX_CAL_PGC2_FOR_LNAGAIN_CAL"];
    RX_CAL_PGC3_FOR_LNAGAIN_CAL = tmpAction["RX_CAL_PGC3_FOR_LNAGAIN_CAL"];
    RX_CAL_NOS = tmpAction["RX_CAL_NOS"];
    RX_CAL_GAIN_STEP_POWER_REF = tmpAction["RX_CAL_GAIN_STEP_POWER_REF"];

    RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN_RAW = split(tmpAction["RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN"], ",");
    for (int i = 0; i < RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN_RAW.size(); i++)
        RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN[i] = stoi(RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN_RAW.at(i));
    strcpy(RX_CAL_SUB_BAND_FREQ, tmpAction["RX_CAL_SUB_BAND_FREQ"].c_str());

    RX_CAL_POWER_MAX_RAW = split(tmpAction["RX_CAL_POWER_MAX"], ",");
    for (int i = 0; i < RX_CAL_POWER_MAX_RAW.size(); i++)
        RX_CAL_POWER_MAX[i] = stoi(RX_CAL_POWER_MAX_RAW.at(i));
    
    RX_CAL_NO_OF_REGION = stoi(tmpAction["RX_CAL_NO_OF_REGION"]);
    RX_CAL_LNA_INDEX = stoi(tmpAction["RX_CAL_LNA_INDEX"]);
    RX_CAL_PGC1_FOR_AB_CAL = stoi(tmpAction["RX_CAL_PGC1_FOR_AB_CAL"]);
    RX_CAL_PGC2_FOR_AB_CAL = stoi(tmpAction["RX_CAL_PGC2_FOR_AB_CAL"]);
    RX_CAL_PGC3_FOR_AB_CAL = stoi(tmpAction["RX_CAL_PGC3_FOR_AB_CAL"]);

    //Rx fillstruct rxSetChannelParams.
    RxDutApi::RxSetChannelParams rxSetChannelParams;
    rxSetChannelParams.bandwidth = tmpAction["BSS_BANDWIDTH"];
    rxSetChannelParams.boardAntennaMask = stoi(tmpAction["ANTMASK"].c_str());
    rxSetChannelParams.startFreq = stoi(BAND_START_FREQ.c_str());
    rxSetChannelParams.endFrq = stoi(BAND_END_FREQ.c_str());
    rxSetChannelParams.freq = stoi(CH_FREQ_MHZ.c_str());
    rxSetChannelParams.isCloseLoop = 0;
    rxSetChannelParams.offlineCalMask = 0xFFFF;
    rxSetChannelParams.rxAntennaMask = stoi(tmpAction["ANTMASK"].c_str());
    if (tmpAction["DUT_SLEEP_AFTER_SET_CHANNEL"] != "")
        rxSetChannelParams.sleepAfterSetChannel = stoi(tmpAction["DUT_SLEEP_AFTER_SET_CHANNEL"].c_str());
    rxSetChannelParams.standard = STANDARD;
    theApp.fillRxStructParams.rxSetChannelParams = rxSetChannelParams;

//Rx Calbration init.
    if (theApp.dutApi->Dut_EndTxPackets() != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_EndTxPackets()");

        APICallback = "Dut_EndTxPackets()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetCloseLoop(0) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetCloseLoop()");

        APICallback = "Dut_SetCloseLoop()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    BYTE TxAntennasMask;
    if (theApp.dutApi->Dut_GetTxAntennasMask(&TxAntennasMask) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_GetTxAntennasMask()");

        APICallback = "Dut_GetTxAntennasMask()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    BYTE RxAntennasMask;
    if (theApp.dutApi->Dut_GetRxAntennasMask(&RxAntennasMask) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_GetRxAntennasMask()");

        APICallback = "Dut_GetRxAntennasMask()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetTxAntenna(stoi(tmpAction["ANTMASK"].c_str())) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetTxAntenna()");

        APICallback = "Dut_SetTxAntenna()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetRxAntenna(stoi(tmpAction["ANTMASK"].c_str())) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetRxAntenna()");

        APICallback = "Dut_SetRxAntenna()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetTxAntenna(0) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetTxAntenna()");

        APICallback = "Dut_SetTxAntenna()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    //if (theApp.dutApi->Dut_SetTxAntenna(stoi(tmpAction["ANTMASK"].c_str())) != MT_RET_OK)
    //{
    //    if (tmpAction["DEBUG"] == "TRUE")
    //        ExportTestLog("API Flow: Dut_SetTxAntenna()");
    //
    //    APICallback = "Dut_SetTxAntenna()";
    //    errorCode = "T189";
    //    goto ERRORREPORT;
    //}

    if (theApp.dutApi->Dut_SetRxAntenna(stoi(tmpAction["ANTMASK"].c_str())) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetRxAntenna()");

        APICallback = "Dut_SetRxAntenna()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

  /*  if (theApp.dutApi->Dut_SetRiscMode(0) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetRiscMode()");

        APICallback = "Dut_SetRiscMode()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetRiscMode(1) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetRiscMode()");

        APICallback = "Dut_SetRiscMode()";
        errorCode = "T189";
        goto ERRORREPORT;
    }

    if (theApp.dutApi->Dut_SetRiscMode(0) != MT_RET_OK)
    {
        if (tmpAction["DEBUG"] == "TRUE")
            ExportTestLog("API Flow: Dut_SetRiscMode()");

        APICallback = "Dut_SetRiscMode()";
        errorCode = "T189";
        goto ERRORREPORT;
    }*/
   
///////////////////////////////////////////////////////
 
    //IRxCalibration::GenType board_type = (IRxCalibration::GenType)g_WiFi11ACGlobalSettingParam.GEN5_OR_GEN6;
    RxCalibration *rxCal_obj = new RxCalibration(theApp.dutApi, IRxCalibration::GEN6);
    rxCal_obj->m_funcStartRxPower = cbStartRxPower;
    rxCal_obj->m_funcStopRxPower = cbStopRxPower;
    if (tmpAction["DEBUG"] == "TRUE")
        rxCal_obj->debugFlag(tmpAction["DEBUG"]);
    if (tmpAction["SUB_BAND_DEBUG"] == "TRUE")
        rxCal_obj->debugSubBandFlag(tmpAction["SUB_BAND_DEBUG"]);

    if (theApp.m_funcReadCableLoss == NULL)
    {
        APICallback = "funcReadCableLoss Test";
        errorCode = "T189";
        goto ERRORREPORT;
    }
    else
        rxCal_obj->m_funcReadCableLoss = theApp.m_funcReadCableLoss;
    //rxCal_obj->MidGain_Cableloss = split(tmpAction["MID_GAIN_CABLELOSS"].c_str(), ",");
    ///////////////////////////////////////////////////////
    //*********** RX Sub-Bands Calibration **************//
    ///////////////////////////////////////////////////////
    int RX_CAL_SUB_BAND_ENABLE = 1;
    if (RX_CAL_SUB_BAND_ENABLE == 1)
    {
       // printf(".");
        int Sub_band_freq[11] = {};
        int subBandNumberOfFreq = 0;
        subBandConvertCharToInt(RX_CAL_SUB_BAND_FREQ, Sub_band_freq, &subBandNumberOfFreq);

        IRxCalibration::RxRfSubBandParams rxRfSubBandParams;
        rxRfSubBandParams.subBandFreq.assign(Sub_band_freq, Sub_band_freq + subBandNumberOfFreq);
        rxRfSubBandParams.setChannelParams.bandwidth          = string("BW-20");
        rxRfSubBandParams.setChannelParams.boardAntennaMask   = stoi(tmpAction["ANTMASK"].c_str());
        rxRfSubBandParams.setChannelParams.startFreq          = stoi(BAND_START_FREQ);
        rxRfSubBandParams.setChannelParams.endFrq             = stoi(BAND_END_FREQ);
        rxRfSubBandParams.setChannelParams.freq               = stoi(CH_FREQ_MHZ);
        rxRfSubBandParams.setChannelParams.isCloseLoop        = 0;
        rxRfSubBandParams.setChannelParams.standard           = STANDARD;
        rxRfSubBandParams.setChannelParams.offlineCalMask     = 0;
        rxRfSubBandParams.setChannelParams.rxAntennaMask      = stoi(tmpAction["ANTMASK"].c_str());
        rxRfSubBandParams.setRxGainBlocksParams.LnaIndex      = stoi(RX_CAL_SUB_BAND_LNA_INDEX);
        rxRfSubBandParams.setRxGainBlocksParams.pgc1          = stoi(RX_CAL_SUB_BAND_PGC1);
        rxRfSubBandParams.setRxGainBlocksParams.pgc2          = stoi(RX_CAL_SUB_BAND_PGC2);
        rxRfSubBandParams.setRxGainBlocksParams.pgc3          = stoi(RX_CAL_SUB_BAND_PGC3);
        rxRfSubBandParams.vsgStartCWParams.rfPowerLeveldBm    = stod(RX_CAL_SUB_BAND_POWER_REF);
        rxRfSubBandParams.vsgStartCWParams.offsetFrequencyMHz = stod(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL);
        //rxRfSubBandParams.vsg = vsg;

        IVSG::PathLossParams _pathLossParams;
        //_pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        _pathLossParams.antennaMask = rxRfSubBandParams.setChannelParams.rxAntennaMask;
        rxRfSubBandParams.pathLossParams = _pathLossParams;

        IRxCalibration::status ret = rxCal_obj->RxRfSubBandCal(rxRfSubBandParams);
        if (ret != IRxCalibration::status::PASS)
        {
            APICallback = "RxRfSubBandCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
        }
        theApp.fillRxStructParams.subBandResultsParams = rxCal_obj->getSubBandResults();
        //tagInputPara.CH_FREQ_MHZ = tagInputPara.BSS_FREQ_MHZ_PRIMARY;
    }
    /*else
    {
        IRxCalibration::SubBandResultsParams subBandResultsParams;
        if (g_WiFi11ACGlobalSettingParam.GEN5_OR_GEN6 == 1)
        {
            for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
            {
                for (int j_band = 0; j_band < NUM_LNA_CROSS_POINTS; j_band++)
                {
                    subBandResultsParams.subBandFreq[i_ant][j_band] = 0;
                }
            }
        }
        else
        {
            for (int i_ant = 0; i_ant << MAX_DATA_STREAM; i_ant++)
            {
                for (int j_band = 0; j_band << NUM_LNA_CROSS_POINTS; j_band++)
                {
                    subBandResultsParams.subBandFreq[i_ant][j_band] = 0; // g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_SUB_BANDS_CROSS_POINTS_DEFAULT_VALUE[j_band];
                }
            }
        }
        fillRxStructParams.subBandResultsParams = rxCal_obj->setSubBandResults(subBandResultsParams);
    } */

    ///////////////////////////////////////////////////////
    //***********  RX Mid-Gain Calibration **************//
    ///////////////////////////////////////////////////////
    int RX_CAL_MID_GAIN_ENABLE = 1;
    if (RX_CAL_MID_GAIN_ENABLE == 1)
    {
        IRxCalibration::RxRfMidGainParams rxRfMidGainParams;
        rxRfMidGainParams.setRxGainBlocksParams.LnaIndex = stoi(RX_CAL_CALC_LNA_INDEX_FOR_MID_LNAGAIN_CAL);
        rxRfMidGainParams.setRxGainBlocksParams.pgc1 = stoi(RX_CAL_PGC1_FOR_MID_LNAGAIN_CAL);
        rxRfMidGainParams.setRxGainBlocksParams.pgc2 = stoi(RX_CAL_PGC2_FOR_MID_LNAGAIN_CAL);
        rxRfMidGainParams.setRxGainBlocksParams.pgc3 = stoi(RX_CAL_PGC3_FOR_MID_LNAGAIN_CAL);

        rxRfMidGainParams.midGainCalibrateParams.Pgc1 = stoi(RX_CAL_PGC1_FOR_MID_LNAGAIN_CAL);
        rxRfMidGainParams.midGainCalibrateParams.Pgc2 = stoi(RX_CAL_PGC2_FOR_MID_LNAGAIN_CAL);

        if (stoi(BSS_FREQ_MHZ_PRIMARY) > 3000)
        {
            rxRfMidGainParams.midGainCalibrateParams.targetGain = stoi(RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_5G);
        }
        else
        {
            rxRfMidGainParams.midGainCalibrateParams.targetGain = stoi(RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_2G);
        }
        rxRfMidGainParams.vsgStartCWParams.loFreqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRfMidGainParams.vsgStartCWParams.rfPowerLeveldBm = stoi(RX_CAL_PRXIN_FOR_MID_LNAGAIN_CAL);
        rxRfMidGainParams.vsgStartCWParams.offsetFrequencyMHz = stoi(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL);
        // rxRfMidGainParams.vsg = vsg;
        rxRfMidGainParams.rxRfMidGainLimitsParams.midLnaGainMin = stoi(RX_CAL_MID_LNAGAIN_DEC_MIN);
        rxRfMidGainParams.rxRfMidGainLimitsParams.midLnaGainMax = stoi(RX_CAL_MID_LNAGAIN_DEC_MAX);
        rxRfMidGainParams.rxRfMidGainLimitsParams.measGainTolerance = stoi(RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL);

        IVSG::PathLossParams _pathLossParams;
        //_pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        //_pathLossParams.antennaMask = DUT_IQlite_HELPER::CalcAntennaMask(l_11ACrxCalParam.RX1, l_11ACrxCalParam.RX2, l_11ACrxCalParam.RX3, l_11ACrxCalParam.RX4);
        _pathLossParams.antennaMask = stoi(tmpAction["ANTMASK"].c_str());
        _pathLossParams.freqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRfMidGainParams.pathLossParams = _pathLossParams;

        IRxCalibration::status ret = rxCal_obj->RxRfMidGainCal(rxRfMidGainParams);
        if (ret != IRxCalibration::status::PASS)
        {
            APICallback = "RxRfMidGainCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
        }
        theApp.fillRxStructParams.midGainResultsParams = rxCal_obj->getMidGainResults();

        for (int i_mid = 0; i_mid < NUM_OF_ANTENNA; i_mid++)
        {
            if (((stoi(tmpAction["ANTMASK"].c_str()) >> i_mid) & 0x1) == 0)
                continue;

            if ((theApp.fillRxStructParams.midGainResultsParams.calcTuneWord[i_mid] < stoi(RX_CAL_MID_LNAGAIN_DEC_MIN))
                || (theApp.fillRxStructParams.midGainResultsParams.calcTuneWord[i_mid] > stoi(RX_CAL_MID_LNAGAIN_DEC_MAX)))
            {
                ExportTestLog(" FAIL on Limit");
                //LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR,
                //    "[ERROR]ANT: %d CALC_GAIN_CTRL:%d Min: %0.2f  Max %0.2f \n", i_mid + 1,
                //    fillRxStructParams.midGainResultsParams.calcTuneWord[i_mid],
                //    g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_MID_LNAGAIN_DEC_MIN,
                //    g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_MID_LNAGAIN_DEC_MAX);
                //
                //if (l_11ACrxCalParam.STOP_ON_ERROR_RX_CAL == 1)
                //{
                //    throw logMessage;
                //}
                //else
                //{
                //    SetConsoleMode(CM_RED);
                //    printf(" FAIL on Limit  \n");
                //    SetConsoleMode(CM_WHITE);
                //}
            }
            double rx_cal;
            if (stoi(CH_FREQ_MHZ) > 3000)
            {
                rx_cal = stoi(RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_5G);
            }
            else
            {
                rx_cal = stoi(RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_2G);
            }
            if (theApp.fillRxStructParams.midGainResultsParams.measGain[i_mid] < (rx_cal - stoi(RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL))
                || (theApp.fillRxStructParams.midGainResultsParams.measGain[i_mid] > (rx_cal + stoi(RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL))))
            {
                ExportTestLog(" FAIL on Limit");
                //LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR,
                //    "[ERROR]ANT: %d TARGET_GAIN:%d   Min: %0.2f  Max %0.2f  Measure  -> :%0.2f\n", i_mid + 1, rx_cal
                //    , rx_cal - g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL
                //    , rx_cal + g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL
                //    , fillRxStructParams.midGainResultsParams.measGain[i_mid]);
                //
                //if (l_11ACrxCalParam.STOP_ON_ERROR_RX_CAL == 1)
                //{
                //    throw logMessage;
                //}
                //else
                //{
                //    SetConsoleMode(CM_RED);
                //    printf(" FAIL on Limit  \n");
                //    SetConsoleMode(CM_WHITE);
                //}
            }
        }
    }
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //********  RX Gain Accuracy Calibration  ***********//
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    int RX_CAL_GAIN_ACCURACY_ENABLE = 1;
    if (RX_CAL_GAIN_ACCURACY_ENABLE == 1)
    {
        double lna_gain_step_accuracy_min[NUM_OF_LNA_GAINS];
        double lna_gain_step_accuracy_max[NUM_OF_LNA_GAINS];

        IRxCalibration::RxRfGainStepAccuracyParams rxRfGainStepAccuracyParams;
        rxRfGainStepAccuracyParams.setRxGainBlocksParams.LnaIndex = 0;

        copy(begin(RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN),
            end(RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN),
            begin(rxRfGainStepAccuracyParams.pgc1ArrayGains));

        rxRfGainStepAccuracyParams.setRxGainBlocksParams.pgc2 = stoi(RX_CAL_PGC2_FOR_LNAGAIN_CAL);
        rxRfGainStepAccuracyParams.setRxGainBlocksParams.pgc3 = stoi(RX_CAL_PGC3_FOR_LNAGAIN_CAL);

        rxRfGainStepAccuracyParams.measureCwPowerParams.Method = 0;
        rxRfGainStepAccuracyParams.measureCwPowerParams.FreqOffset = 0;
        rxRfGainStepAccuracyParams.measureCwPowerParams.NOS = stoi(RX_CAL_NOS);

        rxRfGainStepAccuracyParams.numberOfLnaGains = stoi(RX_CAL_NUM_OF_LNA_GAINS);
        rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.Pgc2 = stoi(RX_CAL_PGC2_FOR_LNAGAIN_CAL);
        rxRfGainStepAccuracyParams.calculateLnaGainStepsParams.Pgc3 = stoi(RX_CAL_PGC3_FOR_LNAGAIN_CAL);

        rxRfGainStepAccuracyParams.vsgStartCWParams.loFreqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRfGainStepAccuracyParams.vsgStartCWParams.rfPowerLeveldBm = stod(RX_CAL_GAIN_STEP_POWER_REF);
        rxRfGainStepAccuracyParams.vsgStartCWParams.offsetFrequencyMHz = stoi(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL);

        //rxRfGainStepAccuracyParams.vsg = vsg;

        IVSG::PathLossParams _pathLossParams;
        //_pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        //_pathLossParams.antennaMask = DUT_IQlite_HELPER::CalcAntennaMask(l_11ACrxCalParam.RX1, l_11ACrxCalParam.RX2, l_11ACrxCalParam.RX3, l_11ACrxCalParam.RX4);
        _pathLossParams.antennaMask = stoi(tmpAction["ANTMASK"]);
        _pathLossParams.freqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRfGainStepAccuracyParams.pathLossParams = _pathLossParams;

        IRxCalibration::status ret = rxCal_obj->RxRfGainStepAccuracyCal(rxRfGainStepAccuracyParams);
        if (ret != IRxCalibration::status::PASS)
        {
            APICallback = "RxRfGainStepAccuracyCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
        }

        theApp.fillRxStructParams.gainStepResultsParams = rxCal_obj->getGainStepResults();

        copy(begin(RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN),
            end(RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN),
            begin(lna_gain_step_accuracy_min));

        copy(begin(RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX),
            end(RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX),
            begin(lna_gain_step_accuracy_max));

        printf(".");
        for (int k = 0; k < stoi(RX_CAL_NUM_OF_LNA_GAINS); k++)
        {
            //sprintf(burn_print, "\n");
            //::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, g_Logger_ID_CAL, LOGGER_INFORMATION, "[DEBUG_CAL_RX]=> %s \n", burn_print);

            for (int iant = 0; iant < MAX_DATA_STREAM; iant++)
            {
                if (((stoi(tmpAction["ANTMASK"]) >> iant) & 0x1) == 0)
                    continue;

                //if (g_WiFi11ACGlobalSettingParam.GEN5_OR_GEN6 == 1)
                //{
                    //sprintf(burn_print, "ANT: %d LNA_GAIN:%d  Measure Gain -> :%0.2f", iant + 1, k, fillRxStructParams.gainStepResultsParams.gainSteps.gain[iant][k]);
                    //::LOGGER_Write_Ext(LOG_IQLITE_DEBUG_CAL_RX, g_Logger_ID_CAL, LOGGER_INFORMATION, "[DEBUG_CAL_RX]=> %s \n", burn_print);
                    if ((theApp.fillRxStructParams.gainStepResultsParams.gainSteps.gain[iant][k - 1] - theApp.fillRxStructParams.gainStepResultsParams.gainSteps.gain[iant][k] < 2) && (k > 1))
                    {
                        ExportTestLog(" FAIL on Limit");
                       /* LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR,
                            "[ERROR] Delta between two gain is less than 2dB");
                        if (l_11ACrxCalParam.STOP_ON_ERROR_RX_CAL == 1)
                        {
                            throw logMessage;
                        }
                        else
                        {
                            SetConsoleMode(CM_RED);
                            printf(" FAIL on Limit  \n");
                            SetConsoleMode(CM_WHITE);
                        }*/
                    }
                //}
            }
        }
    }
    ///////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //*********** RX flatness Calibration  **************//
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    int RX_CAL_FLATNESS_ENABLE = 1;
    if (RX_CAL_FLATNESS_ENABLE == 1)
    {
        int P_Adc_array[MAX_DATA_STREAM][MAX_FLATNESS_POINTS] = {};
        float gain_step_prxin[MAX_DATA_STREAM][MAX_FLATNESS_POINTS] = {};
        int flatness_channel = 0;

        int Sub_band_freq[11] = {};
        int subBandNumberOfFreq = 0;
        subBandConvertCharToInt(RX_CAL_SUB_BAND_FREQ, Sub_band_freq, &subBandNumberOfFreq);

        int rf_flatness_freq[50] = { 0 };
        int flatnessNumberOfFreq;
        fltanessConvertCharToInt(RX_CAL_FLATNESS_FREQ, rf_flatness_freq, &flatnessNumberOfFreq);

        IRxCalibration::RxRfFlatnessParams rxRfFlatnessParams;
        rxRfFlatnessParams.measureBypasFlatness = IRxCalibration::GEN6;
        rxRfFlatnessParams.rfFlatnessFreq.assign(rf_flatness_freq, rf_flatness_freq + flatnessNumberOfFreq);
        rxRfFlatnessParams.setChannelParams.bandwidth = string("BW-20");
        rxRfFlatnessParams.setChannelParams.boardAntennaMask = stoi(tmpAction["ANTMASK"].c_str());
        rxRfFlatnessParams.setChannelParams.startFreq = stoi(BAND_START_FREQ);
        rxRfFlatnessParams.setChannelParams.endFrq = stoi(BAND_END_FREQ);
        rxRfFlatnessParams.setChannelParams.freq = stoi(CH_FREQ_MHZ);
        rxRfFlatnessParams.setChannelParams.isCloseLoop = 0;
        rxRfFlatnessParams.setChannelParams.standard = string(STANDARD);
        rxRfFlatnessParams.setChannelParams.offlineCalMask = 0;
        rxRfFlatnessParams.setChannelParams.rxAntennaMask = stoi(tmpAction["ANTMASK"].c_str());

        int activeGain = stoi(RX_CAL_CALC_LNA_INDEX_FOR_MID_LNAGAIN_CAL);
        rxRfFlatnessParams.setRxGainBlocksParams.LnaIndex = activeGain;
        rxRfFlatnessParams.setRxGainBlocksParams.pgc1 = RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN[activeGain];
        rxRfFlatnessParams.setRxGainBlocksParams.pgc2 = stoi(RX_CAL_PGC2_FOR_LNAGAIN_CAL);
        rxRfFlatnessParams.setRxGainBlocksParams.pgc3 = stoi(RX_CAL_PGC3_FOR_LNAGAIN_CAL);

        rxRfFlatnessParams.measureCwPowerParams.Method = 0;
        rxRfFlatnessParams.measureCwPowerParams.FreqOffset = 0;
        rxRfFlatnessParams.measureCwPowerParams.NOS = stoi(RX_CAL_NOS);

        rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC1 = RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN[activeGain];
        rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC2 = stoi(RX_CAL_PGC2_FOR_LNAGAIN_CAL);
        rxRfFlatnessParams.calculateRxRfFlatnessParams.PGC3 = stoi(RX_CAL_PGC3_FOR_LNAGAIN_CAL);


        int byPassGain = theApp.fillRxStructParams.gainStepResultsParams.numLnaGainSteps - 2;
        rxRfFlatnessParams.setRxGainBlocksParamsBypass.LnaIndex = byPassGain;
        rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc1 = RX_CAL_PGC1_FOR_LNAGAIN_CAL_GAIN[byPassGain];
        rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc2 = stoi(RX_CAL_PGC2_FOR_LNAGAIN_CAL);
        rxRfFlatnessParams.setRxGainBlocksParamsBypass.pgc3 = stoi(RX_CAL_PGC3_FOR_LNAGAIN_CAL);

        rxRfFlatnessParams.vsgStartCWParams.rfPowerLeveldBm = stoi(RX_CAL_GAIN_STEP_POWER_REF);
        rxRfFlatnessParams.vsgStartCWParams.offsetFrequencyMHz = stoi(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL);
        //rxRfFlatnessParams.vsg = vsg;

        IVSG::PathLossParams _pathLossParams;
       // _pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        //_pathLossParams.antennaMask = rxRfFlatnessParams.setChannelParams.rxAntennaMask;
        _pathLossParams.antennaMask = stoi(tmpAction["ANTMASK"].c_str());;
        rxRfFlatnessParams.pathLossParams = _pathLossParams;
        IRxCalibration::status ret = rxCal_obj->RxRfFlatnessCal(rxRfFlatnessParams);
        if (ret != IRxCalibration::status::PASS)
        {
           // LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] RxRfSubBandCal failed.\n");
           // throw logMessage;
            APICallback = "RxRfFlatnessCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
        }
        theApp.fillRxStructParams.flatnessResultsParams = rxCal_obj->getFlatnessResults();
    }
    //else
    //{
    //    IRxCalibration::FlatnessResultsParams flatnessResultsParams;
    //    flatnessResultsParams.flatnessFreq.resize(0/*flatnessNumberOfFreq*/);
    //    //for (int i_ant = 0; i_ant < MAX_DATA_STREAM; i_ant++)
    //    //{
    //    //	for (int i_freq = 0; i_freq < flatnessNumberOfFreq; i_freq++)
    //    //	{
    //    //		flatnessResultsParams.flatnessdB[i_ant][i_freq] = 0;//g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_RF_FLATNESSS_DELTA_POINT_DEFAULT_VALUE[i_freq];
    //    //		flatnessResultsParams.flatnessBypassDB[i_ant][i_freq] = 0;// g_WiFi11ACDefaultGlobalSettingParam.RX_CAL_RF_FLATNESSS_DELTA_POINT_DEFAULT_VALUE[i_freq];
    //    //		flatnessResultsParams.midGainDelta[i_ant][i_freq] = 0;
    //    //		flatnessResultsParams.flatnessFreq[i_freq] = 0;//  rf_flatness_freq[i_freq];
    //    //	}
    //    //}
    //    //fillRxStructParams.flatnessResultsParams = rxCal_obj->setFlatnessResults(flatnessResultsParams);
    //}
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //******** RX S2D Gain Offset Calibration  **********//
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    int RX_CAL_S2D_ENABLE = 1;
    if (RX_CAL_S2D_ENABLE == 1)
    {
        IRxCalibration::RxRssiS2dParams rxRssiS2dParams;
        //rxRssiS2dParams.antennaMask = DUT_IQlite_HELPER::CalcAntennaMask(l_11ACrxCalParam.RX1, l_11ACrxCalParam.RX2, l_11ACrxCalParam.RX3, l_11ACrxCalParam.RX4);
        rxRssiS2dParams.antennaMask = stoi(tmpAction["ANTMASK"].c_str());
        rxRssiS2dParams.numberOfRegions = RX_CAL_NO_OF_REGION;

        rxRssiS2dParams.setRxGainBlocksParams.LnaIndex = RX_CAL_LNA_INDEX;
        rxRssiS2dParams.setRxGainBlocksParams.pgc1 = RX_CAL_PGC1_FOR_AB_CAL;
        rxRssiS2dParams.setRxGainBlocksParams.pgc2 = RX_CAL_PGC2_FOR_AB_CAL;
        rxRssiS2dParams.setRxGainBlocksParams.pgc3 = RX_CAL_PGC3_FOR_AB_CAL;

        copy(begin(RX_CAL_IOFFSET_INIT), end(RX_CAL_IOFFSET_INIT), begin(rxRssiS2dParams.setRssiOffsetGainParams.iOffset));
        copy(begin(RX_CAL_S2DGAIN_INIT), end(RX_CAL_S2DGAIN_INIT), begin(rxRssiS2dParams.setRssiOffsetGainParams.s2dGain));
        //rxRssiS2dParams.setRssiOffsetGainParams.antMask = DUT_IQlite_HELPER::CalcAntennaMask(l_11ACrxCalParam.RX1, l_11ACrxCalParam.RX2, l_11ACrxCalParam.RX3, l_11ACrxCalParam.RX4);
        rxRssiS2dParams.setRssiOffsetGainParams.antMask = stoi(tmpAction["ANTMASK"].c_str());

        //rxRssiS2dParams.vsg = vsg;

        IVSG::PathLossParams _pathLossParams;
        //_pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        _pathLossParams.antennaMask = rxRssiS2dParams.antennaMask;
        _pathLossParams.freqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);

        rxRssiS2dParams.vsgStartCWParams.loFreqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRssiS2dParams.vsgStartCWParams.offsetFrequencyMHz = stoi(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL); //AMIT.a
        rxRssiS2dParams.pathLossParams = _pathLossParams;
        rxRssiS2dParams.rssiS2dSetInitialsParams.numRegions = RX_CAL_NO_OF_REGION;
        rxRssiS2dParams.rssiS2dSetInitialsParams.AuxADC_Res = RX_CAL_AUX_ADC_RES;
        rxRssiS2dParams.rssiS2dSetInitialsParams.Ioffs_Step = RX_CAL_IOFFS_STEP;

        //"CW measure Method: 0 – accumulators, 1 – Goertzel, 2 –TD Capture";
        rxRssiS2dParams.measureRssiParams.Method = RxDutApi::RssiMeasMethod::accumulators;
        rxRssiS2dParams.measureRssiParams.NOS = stoi(RX_CAL_NOS);

        copy(begin(RX_CAL_POWER_MAX), end(RX_CAL_POWER_MAX), begin(rxRssiS2dParams.maxPower));
        copy(begin(RX_CAL_MARGINHIGH), end(RX_CAL_MARGINHIGH), begin(rxRssiS2dParams.marginHigh));

        copy(begin(RX_CAL_S2DGAIN_INIT), end(RX_CAL_S2DGAIN_INIT), begin(rxRssiS2dParams.calculateOptimalIoffsParams.s2d_gain_db));
        copy(begin(RX_CAL_IOFFSET_INIT), end(RX_CAL_IOFFSET_INIT), begin(rxRssiS2dParams.calculateOptimalIoffsParams.s2d_offset_db));

        IRxCalibration::status ret = rxCal_obj->RxRssiS2dCal(rxRssiS2dParams);
        if (ret != IRxCalibration::status::PASS)
        {
            APICallback = "RxRssiS2dCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
           // LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] RxRssiS2dCal failed.\n");
           // throw logMessage;
        }
        theApp.fillRxStructParams.s2dResultsParams = rxCal_obj->getS2dResults();
    }
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //*************** RX AB Calibration *****************//
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    int RX_CAL_AB_ENABLE = 1;
    if (RX_CAL_AB_ENABLE == 1)
    {
        IRxCalibration::RxRssiABParams rxRssiABParams;
        rxRssiABParams.antennaMask = stoi(tmpAction["ANTMASK"].c_str());
        rxRssiABParams.numberOfRegions = RX_CAL_NO_OF_REGION;

        IVSG::PathLossParams _pathLossParams;
        //_pathLossParams.test_id = g_WiFi_11ac_Test_ID;
        _pathLossParams.antennaMask = rxRssiABParams.antennaMask;
        _pathLossParams.freqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);

        rxRssiABParams.vsgStartCWParams.loFreqMHz = stoi(BSS_FREQ_MHZ_PRIMARY);
        rxRssiABParams.vsgStartCWParams.offsetFrequencyMHz = stoi(RX_CAL_CW_OFFSET_FOR_MID_LNAGAIN_CAL);   //AMIT.a
        rxRssiABParams.pathLossParams = _pathLossParams;

        //rxRssiABParams.vsg = vsg;

        rxRssiABParams.setRxGainBlocksParams.LnaIndex = RX_CAL_LNA_INDEX;
        rxRssiABParams.setRxGainBlocksParams.pgc1 = RX_CAL_PGC1_FOR_AB_CAL;
        rxRssiABParams.setRxGainBlocksParams.pgc2 = RX_CAL_PGC2_FOR_AB_CAL;
        rxRssiABParams.setRxGainBlocksParams.pgc3 = RX_CAL_PGC3_FOR_AB_CAL;

        rxRssiABParams.calculatePVectorParams.numPwrPoints = NUM_OF_POWER_POINTS;
        rxRssiABParams.calculatePVectorParams.lowerP_Boundary = RX_CAL_LOWERP_BOUNDARY;
        rxRssiABParams.calculatePVectorParams.upperP_Boundary = RX_CAL_UPPERP_BOUNDARY;

        //"CW measure Method: 0 – accumulators, 1 – Goertzel, 2 –TD Capture";
        rxRssiABParams.measureRssiParams.Method = (RxDutApi::RssiMeasMethod)RxDutApi::RssiMeasMethod::accumulators;
        rxRssiABParams.measureRssiParams.NOS = stoi(RX_CAL_NOS);
        copy(begin(RX_CAL_POWER_MAX), end(RX_CAL_POWER_MAX), begin(rxRssiABParams.maxPower));

        IRxCalibration::status ret = rxCal_obj->RxRssiABCal(rxRssiABParams);
        if (ret != IRxCalibration::status::PASS)
        {
            APICallback = "RxRssiABCal Test";
            errorCode = "T189";
            goto ERRORREPORT;
           // LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] RxRssiABCal failed.\n");
           // throw logMessage;
        }
        //else
        //{
        //    LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_INFORMATION, "[WiFi] RxRssiABCal return OK.\n");
        //}
        theApp.fillRxStructParams.abResultsParams = rxCal_obj->getAbResults();
    }

    theApp.m_RxBurnCalGen6->fillRxStruct(theApp.fillRxStructParams);
    theApp.fillRxStructParams = {};

    TestFlag = TM_RETURN_PASS;
    return TestFlag;

ERRORREPORT:
    ErrorReport.DisableSfcsLink = FALSE;
    strCopy(ErrorReport.ErrorCode, _T(errorCode.c_str()));
    strCopy(ErrorReport.ErrorDetail, _T("NULL"));
    sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "StartForRxIQCalibration => %s Fail.", APICallback);
    strCopy(ErrorReport.FailedTest, _T("StartForRxIQCalibration"));
    ErrorReport.TestResult = FALSE;
    return TM_RETURN_ERROR;
}
#endif

extern "C" BOOL PASCAL EXPORT LoadDutRequired(BOOL bRetry, LPSTR lpszRunInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if (tmpAction["INPUT_PARAMETERS"] != "")
   {
       if (tmpAction["INPUT_PARAMETERS"] == "SUB_BANDS_CALIBRATION")
       {   
           RX_CAL_SUB_BAND_LNA_INDEX            = tmpAction["RX_CAL_SUB_BAND_LNA_INDEX"];
           RX_CAL_SUB_BAND_PGC1                 = tmpAction["RX_CAL_SUB_BAND_PGC1"];
           RX_CAL_SUB_BAND_PGC2                 = tmpAction["RX_CAL_SUB_BAND_PGC2"];
           RX_CAL_SUB_BAND_PGC3                 = tmpAction["RX_CAL_SUB_BAND_PGC3"];
           RX_CAL_SUB_BAND_POWER_REF            = tmpAction["RX_CAL_SUB_BAND_POWER_REF"];
       }
       if (tmpAction["INPUT_PARAMETERS"] == "MID_GAIN_CALIBRATION")
       {
           RX_CAL_CALC_LNA_INDEX_FOR_MID_LNAGAIN_CAL = tmpAction["RX_CAL_CALC_LNA_INDEX_FOR_MID_LNAGAIN_CAL"];
           RX_CAL_PGC1_FOR_MID_LNAGAIN_CAL           = tmpAction["RX_CAL_PGC1_FOR_MID_LNAGAIN_CAL"];
           RX_CAL_PGC2_FOR_MID_LNAGAIN_CAL           = tmpAction["RX_CAL_PGC2_FOR_MID_LNAGAIN_CAL"];
           RX_CAL_PGC3_FOR_MID_LNAGAIN_CAL           = tmpAction["RX_CAL_PGC3_FOR_MID_LNAGAIN_CAL"];
           RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_5G      = tmpAction["RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_5G"];
           RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_2G      = tmpAction["RX_CAL_TARGET_FOR_MID_LNAGAIN_CAL_2G"];
           RX_CAL_PRXIN_FOR_MID_LNAGAIN_CAL          = tmpAction["RX_CAL_PRXIN_FOR_MID_LNAGAIN_CAL"];
           RX_CAL_MID_LNAGAIN_DEC_MIN                = tmpAction["RX_CAL_MID_LNAGAIN_DEC_MIN"];
           RX_CAL_MID_LNAGAIN_DEC_MAX                = tmpAction["RX_CAL_MID_LNAGAIN_DEC_MAX"];
           RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL = tmpAction["RX_CAL_CALC_GAIN_TOLERANCE_FOR_MID_LNAGAIN_CAL"];
       }
 
       if (tmpAction["INPUT_PARAMETERS"] == "GAIN_ACCURACY_CALIBRATION")
       {
           RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN_RAW = split(tmpAction["RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN"], ",");
           for (int i = 0; i < RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN_RAW.size(); i++)
               RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN[i] = stod(RX_CAL_LNA_GAIN_STEP_ACCURACY_MIN_RAW.at(i));

           RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX_RAW = split(tmpAction["RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX"], ",");
           for (int i = 0; i < RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX_RAW.size(); i++)
               RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX[i] = stod(RX_CAL_LNA_GAIN_STEP_ACCURACY_MAX_RAW.at(i));

           RX_CAL_NUM_OF_LNA_GAINS = tmpAction["RX_CAL_NUM_OF_LNA_GAINS"];
       }

       if (tmpAction["INPUT_PARAMETERS"] == "FLATNESS_CALIBRATION")
       {
           memset(RX_CAL_FLATNESS_FREQ, 0, 1024);
           strcpy(RX_CAL_FLATNESS_FREQ, tmpAction["RX_CAL_FLATNESS_FREQ"].c_str());
       }

       if (tmpAction["INPUT_PARAMETERS"] == "S2D_GAIN_OFFSET_CALIBRATION")
       {
           RX_CAL_IOFFSET_INIT_RAW = split(tmpAction["RX_CAL_IOFFSET_INIT"], ",");
           for (int i = 0; i < RX_CAL_IOFFSET_INIT_RAW.size(); i++)
               RX_CAL_IOFFSET_INIT[i] = stoi(RX_CAL_IOFFSET_INIT_RAW.at(i));

           RX_CAL_S2DGAIN_INIT_RAW = split(tmpAction["RX_CAL_S2DGAIN_INIT"], ",");
           for (int i = 0; i < RX_CAL_S2DGAIN_INIT_RAW.size(); i++)
               RX_CAL_S2DGAIN_INIT[i] = stoi(RX_CAL_S2DGAIN_INIT_RAW.at(i));

           RX_CAL_AUX_ADC_RES = stof(tmpAction["RX_CAL_AUX_ADC_RES"]);
           RX_CAL_IOFFS_STEP  = stof(tmpAction["RX_CAL_IOFFS_STEP"]);

           RX_CAL_MARGINHIGH_RAW = split(tmpAction["RX_CAL_MARGINHIGH"], ",");
           for (int i = 0; i < RX_CAL_MARGINHIGH_RAW.size(); i++)
               RX_CAL_MARGINHIGH[i] = stoi(RX_CAL_MARGINHIGH_RAW.at(i));
       }
       if (tmpAction["INPUT_PARAMETERS"] == "AB_CALIBRATION")
       {
           RX_CAL_LOWERP_BOUNDARY = stoi(tmpAction["RX_CAL_LOWERP_BOUNDARY"]);
           RX_CAL_UPPERP_BOUNDARY = stoi(tmpAction["RX_CAL_UPPERP_BOUNDARY"]);
       }
       return TRUE;
   }
   return FALSE;
}

extern "C" BOOL PASCAL EXPORT ParserWiFiCableLossToDut(double *dbCableLoss, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //strCopy(lpszRunInfo, "Intel_WAV.dll do not suport ACTION WIFI_CABLELOSS_PARSER.");
	return TRUE;
}

extern "C" TM_RETURN PASCAL EXPORT WriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   int NUMBER_OF_TX_CHANNELS;
   float abErrorsOut[MAX_BURN_STRUCT_SIZE][MAX_ANTENNA_NUMBER][MAX_REGION_NUMBER] = { 0 };

   CTxControl* m_CTxControl = new CTxControl(theApp.dutApi);
   if (m_CTxControl == NULL)
   {
       ErrorReport.DisableSfcsLink = FALSE;
       strCopy(ErrorReport.ErrorCode, _T("T156"));
       strCopy(ErrorReport.ErrorDetail, _T("NULL"));
       sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => Class(m_CTxControl) create Fail.");
       strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
       ErrorReport.TestResult = FALSE;
       return TM_RETURN_ERROR;
   }
   theApp.m_TxBurnCalGen6->dutFuncSet(m_CTxControl);
   NUMBER_OF_TX_CHANNELS = theApp.m_TxBurnCalGen6->getNumberOfChannels();
   memcpy(abErrorsOut, theApp.m_TxBurnCalGen6->getABError(), sizeof abErrorsOut);
   theApp.m_TxBurnCalGen6->printResults();

#if 1
   /*
   int allBandsTotalAntennaMask = stoi(tmpAction["ANTMASK"]);
   vector< double >  ER_ANT1_REG1;
   vector< double >  ER_ANT1_REG2;
   vector< double >  ER_ANT1_REG3;
   vector< double >  ER_ANT2_REG1;
   vector< double >  ER_ANT2_REG2;
   vector< double >  ER_ANT2_REG3;
   vector< double >  ER_ANT3_REG1;
   vector< double >  ER_ANT3_REG2;
   vector< double >  ER_ANT3_REG3;
   vector< double >  ER_ANT4_REG1;
   vector< double >  ER_ANT4_REG2;
   vector< double >  ER_ANT4_REG3;
   for (int i = 0; i < NUMBER_OF_TX_CHANNELS; i++)
   {
       for (int m = 0; m < MAX_DATA_STREAM; m++)
       {
           if ( ((allBandsTotalAntennaMask >> m) & 0x1) == 0)
               continue;

           for (int k = 0; k < MAX_REGION_NUMBER; k++)
           {
               if (abErrorsOut[i][m][k] == 0)
                   continue;

               switch (m)  //ant
               {
               case 0:
               {
                   switch (k)  //region
                   {
                   case 0:
                       ER_ANT1_REG1.push_back(abErrorsOut[i][m][k]);
                       break;
                   case 1:
                       ER_ANT1_REG2.push_back(abErrorsOut[i][m][k]);
                       break;
                   default:
                       ER_ANT1_REG3.push_back(abErrorsOut[i][m][k]);
                       break;
                   }
                   break;
               }

               case 1:
               {
                   switch (k)  //region
                   {
                   case 0:
                       ER_ANT2_REG1.push_back(abErrorsOut[i][m][k]);
                       break;
                   case 1:
                       ER_ANT2_REG2.push_back(abErrorsOut[i][m][k]);
                       break;
                   default:
                       ER_ANT2_REG3.push_back(abErrorsOut[i][m][k]);
                       break;
                   }
                   break;
               }
               case 2:
               {
                   switch (k)  //region
                   {
                   case 0:
                       ER_ANT3_REG1.push_back(abErrorsOut[i][m][k]);
                       break;
                   case 1:
                       ER_ANT3_REG2.push_back(abErrorsOut[i][m][k]);
                       break;
                   default:
                       ER_ANT3_REG3.push_back(abErrorsOut[i][m][k]);
                       break;
                   }
                   break;
               }
               default:
               {
                   switch (k)  //region
                   {
                   case 0:
                       ER_ANT4_REG1.push_back(abErrorsOut[i][m][k]);
                       break;
                   case 1:
                       ER_ANT4_REG2.push_back(abErrorsOut[i][m][k]);
                       break;
                   default:
                       ER_ANT4_REG3.push_back(abErrorsOut[i][m][k]);
                       break;
                   }
                   break;
               }
               }
           }
       }
   }
   //find max error
   double dummyAve, dummyMin, dummyMax;
   if (ER_ANT1_REG1.size()) ::AverageTestResult(&ER_ANT1_REG1[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT1_REG1, dummyMin);
   if (ER_ANT1_REG2.size()) ::AverageTestResult(&ER_ANT1_REG2[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT1_REG2, dummyMin);
   if (ER_ANT1_REG3.size()) ::AverageTestResult(&ER_ANT1_REG3[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT1_REG3, dummyMin);
   if (ER_ANT2_REG1.size()) ::AverageTestResult(&ER_ANT2_REG1[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT2_REG1, dummyMin);
   if (ER_ANT2_REG2.size()) ::AverageTestResult(&ER_ANT2_REG2[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT2_REG2, dummyMin);
   if (ER_ANT2_REG3.size()) ::AverageTestResult(&ER_ANT2_REG3[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT2_REG3, dummyMin);
   if (ER_ANT3_REG1.size()) ::AverageTestResult(&ER_ANT3_REG1[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT3_REG1, dummyMin);
   if (ER_ANT3_REG2.size()) ::AverageTestResult(&ER_ANT3_REG2[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT3_REG2, dummyMin);
   if (ER_ANT3_REG3.size()) ::AverageTestResult(&ER_ANT3_REG3[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT3_REG3, dummyMin);
   if (ER_ANT4_REG1.size()) ::AverageTestResult(&ER_ANT4_REG1[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT4_REG1, dummyMin);
   if (ER_ANT4_REG2.size()) ::AverageTestResult(&ER_ANT4_REG2[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT4_REG2, dummyMin);
   if (ER_ANT4_REG3.size()) ::AverageTestResult(&ER_ANT4_REG3[0], g_WiFi11ACGlobalSettingParam.NUMBER_OF_TX_CHANNELS, Linear, dummyAve, l_11ACBurnCalibrationReturn.AB_ERROR_ANT4_REG3, dummyMin);
   */
    ITxBurnCal::Status TssiCalDataApiErr = theApp.m_TxBurnCalGen6->writeTssiCalData();
    if (TssiCalDataApiErr != ITxBurnCal::Status::PASS)
    {
        ExportTestLog("WriteResultToDut => [WiFi_11AC] writeTssiCalData() return error.");
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T156"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => [WiFi_11AC] writeTssiCalData() return error.");
        strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }  
    else
        ExportTestLog("WriteResultToDut => [WiFi](writeTssiCalData()) return OK.");

    ITxBurnCal::Status WriteApiErr = theApp.m_TxBurnCalGen6->WriteProdFlag(false);
    //if (WriteApiErr != ITxBurnCal::Status::PASS)
    //{
    //    ExportTestLog("WriteResultToDut => [WiFi_11AC] WriteProdFlag() return error.");
    //    ErrorReport.DisableSfcsLink = FALSE;
    //    strCopy(ErrorReport.ErrorCode, _T("T156"));
    //    strCopy(ErrorReport.ErrorDetail, _T("NULL"));
    //    sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => [WiFi_11AC] WriteProdFlag() return error.");
    //    strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
    //    ErrorReport.TestResult = FALSE;
    //    return TM_RETURN_ERROR;
    //}
    //else
    //    ExportTestLog("WriteResultToDut => [WiFi](writeTssiCalData()) return OK.");

#endif

    //Rx Calibration Commit.
    RxDutApi * m_CRxControl = new RxDutApi(theApp.dutApi);
    if (m_CRxControl == NULL)
    {
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T156"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => Class(m_RxDutApi) create Fail.");
        strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    theApp.m_RxBurnCalGen6->dutFuncSet(m_CRxControl);
    if (theApp.m_RxBurnCalGen6->getNumberOfChannels() > 0)
    {
        IRxBurnCal::Status BurnApiErr = theApp.m_RxBurnCalGen6->writeRssiCalData();
        if (BurnApiErr != IRxBurnCal::Status::PASS)
        {
            ExportTestLog("WriteResultToDut => [WiFi_11AC] writeRssiCalData() return error.");
            ErrorReport.DisableSfcsLink = FALSE;
            strCopy(ErrorReport.ErrorCode, _T("T156"));
            strCopy(ErrorReport.ErrorDetail, _T("NULL"));
            sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => [WiFi_11AC] writeRssiCalData() return error.");
            strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
            ErrorReport.TestResult = FALSE;
            return TM_RETURN_ERROR;
            //LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi_11AC] writeRssiCalData() return error.\n");
            //throw logMessage;
        }
    }
    ITxBurnCal::Status BurnApiErr = theApp.m_TxBurnCalGen6->writeToNvMemory();
    if (BurnApiErr != ITxBurnCal::Status::PASS)
    {
        ExportTestLog("WriteResultToDut => [WiFi_11AC] writeToNvMemory() return error.");
        ErrorReport.DisableSfcsLink = FALSE;
        strCopy(ErrorReport.ErrorCode, _T("T156"));
        strCopy(ErrorReport.ErrorDetail, _T("NULL"));
        sprintf_s(ErrorReport.ErrorMsg, sizeof(ErrorReport.ErrorMsg), "WriteResultToDut => [WiFi_11AC] writeToNvMemory() return error.");
        strCopy(ErrorReport.FailedTest, _T("WriteResultToDut"));
        ErrorReport.TestResult = FALSE;
        return TM_RETURN_ERROR;
    }
    else
        ExportTestLog("WriteResultToDut => [WiFi_11AC] writeToNvMemory() return OK.");

    delete m_CTxControl;
    delete m_CRxControl;
    return TM_RETURN_PASS;
}
extern "C" TM_RETURN PASCAL EXPORT ReturnOemAppMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt,BOOL bRetry)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return TM_RETURN_ERROR;
}

extern "C" TM_RETURN PASCAL EXPORT EepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ReadCalFile *m_ReadCalFile =new ReadCalFile(theApp.dutApi);
   ReadCalFile::CAL_FILE_DATA m_CAL_FILE_DATA = m_ReadCalFile->read();
   //CalFileVerificationGen6 *m_CalFileVerificationGen6 = new CalFileVerificationGen6(m_CAL_FILE_DATA);
   //m_CalFileVerificationGen6->rxVerification();
  // int size = sizeof(Dut_CalDataStructVer6);
   //Dut_CalDataStructVer6 m_Dut_CalDataStructVer6;
  // DWORD m_xtal_data;
  // int m_number_of_channels;
  // int m_antMask;
  // CalFileVerificationGen6 *m_CalFileVerificationGen6 = new CalFileVerificationGen6(m_CAL_FILE_DATA, m_xtal_data, &m_Dut_CalDataStructVer6, m_number_of_channels, m_antMask);
  // printf("111111");
  // CalFileVerificationGen6(const ReadCalFile::CAL_FILE_DATA & cal_file_data_and_size, const DWORD & xtal_data, const Dut_CalDataStructVer6 * txDutInputStruct, const int& number_of_channels, const int& antMask);
   return TM_RETURN_PASS;
}
extern "C" TM_RETURN PASCAL EXPORT ReturnPreloadMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return TM_RETURN_ERROR;
}
extern "C" BOOL PASCAL EXPORT BtInitialize(BOOL bRetry, LPSTR lpszRunInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return FALSE;
}
extern "C" TM_RETURN PASCAL EXPORT BtForXtalCalibration(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport, funcBtGetPPM cbGetPPM)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return TM_RETURN_ERROR;
}
extern "C" BOOL PASCAL EXPORT BtGetTargetPower(BT_DUT_PARAM &DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());//1st GetTrgetPower	
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStartFrameModulatedTx(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 2nd StartFrameModulatedTx	
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStopFrameModulatedTx(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 3rd StopFrameModulatedTX	
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStartContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 2rd StopFrameModulatedTX	for BT
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStopContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 3rd StopFrameModulatedTX	for BT
	return FALSE;
}
//Jerry Add
extern "C" BOOL PASCAL EXPORT BtResetRxPacketCount(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStartRxPacketReceive(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtGetRxPacketCount(BT_DUT_PARAM DutParam, double &dbRxPacketCount, LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return FALSE;
}
extern "C" BOOL PASCAL EXPORT BtStopRxPacketReceive(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return FALSE;
}
extern "C" TM_RETURN PASCAL EXPORT BtEepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	return TM_RETURN_ERROR;
}

extern "C" TM_RETURN PASCAL EXPORT BtWriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	return TM_RETURN_ERROR;
}

extern "C" BOOL PASCAL EXPORT RestartDutTest()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TCHAR szDelFilePath[MAX_PATH]={0};
	TCHAR szArguments[MAX_PATH]={0};
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szLocalPath[MAX_PATH]={0};
	TCHAR szFilePath[MAX_PATH]={0};

	GetLocalDirectory(szLocalPath,szMessage);	
	//Clear .txt files.
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,UART_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,UART_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,PIPE_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,PIPE_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,TELNET_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,TELNET_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,SOCKET_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,SOCKET_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT APPLICATION_CLOSE_TO_DUT()
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}

//extern "C" BOOL PASCAL EXPORT ParserWiFiCableLossToDut(double *dbCableLoss, LPSTR lpszRunInfo)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());	
//   return FALSE;
//}

#if 0
extern "C" BOOL PASCAL EXPORT StartContModulatedTx(DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    BYTE txMask, RxMask;
    int TxAnt, phyType, band, bandwidth, channel, rate, SGI = 0, CP_MODE = 1, length, repetitions = 65535, calibrationMask = 65535, SetPowerLimit, offlineCalMask = 65535;
    double freq;
    BOOL isDataLong = 0;
    CTxControl::OpenClose isOpenClose = CTxControl::OPEN_LOOP;
    int IFS = 25;
    int txPacketRate = 0;

    string isCW = "FALSE";
    if (tmpAction["MODE"].c_str() == "CONT")
        isCW = "TRUE";

    CTxControl* m_CTxControl = new CTxControl(theApp.dutApi);
    freq = stod(tmpAction["FREQ_MHZ"].c_str());
    AntIndexMapping(freq);
    TxAnt     = Antenna[tmpAction["TX_CHAIN"]];
    phyType   = theApp.ConvertStandardToPhyType(freq, tmpAction["BAND"]);
    band      = theApp.ConvertFreqToBand(freq);
    bandwidth = theApp.ConvertBandwidth(tmpAction["WIDE_BAND"]);
    channel   = theApp.ConvertLoFreqtoLowChannel(tmpAction["WIDE_BAND"], freq);
    rate      = theApp.ConvertRate(1, tmpAction["RATE"], tmpAction["BAND"]);
    //SGI       = theApp.CalcValidGi(tmpAction["BAND"]);

    if (tmpAction["SGI"] != "")
        SGI = std::stoi(tmpAction["SGI"].c_str());
    else
        SGI = 0;

    if (tmpAction["CP_MODE"] != "")
        CP_MODE = std::stoi(tmpAction["CP_MODE"].c_str());
    else
        CP_MODE = 1;

    if (tmpAction["IS_DATALONG"] != "")
    {
        if (!tmpAction["IS_DATALONG"].compare("TRUE"))
            isDataLong = TRUE;
        else
            isDataLong = FALSE;
    }
    else
        isDataLong = FALSE;

    if (tmpAction["DEBUG"] == "TRUE")
        m_CTxControl->debug = "TRUE";

    if (tmpAction["PACKET_LENGTH"] == "")
        length = theApp.CalcPacketLength(tmpAction["BAND"], tmpAction["RATE"], tmpAction["WIDE_BAND"]);
    else
        length = atoi(tmpAction["PACKET_LENGTH"].c_str());

    SetPowerLimit = stof(tmpAction["OUTPUT_POWER"].c_str()) * 2;

    if (tmpAction["OPEN_CLOSE"] != "")
    {
        if (tmpAction["OPEN_CLOSE"] == "0")
            isOpenClose = CTxControl::OPEN_LOOP;
        else
            isOpenClose = CTxControl::CLOSE_LOOP;
    }
    if (tmpAction["IFS"] != "")
        IFS = atoi(tmpAction["IFS"].c_str());

    MT_RET status;
    status = m_CTxControl->Dut_GetTxAntennasMask(&txMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_GetTxAntennasMask Fail.");

    status = m_CTxControl->Dut_GetRxAntennasMask(&RxMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_GetRxAntennasMask Fail.");
    status = m_CTxControl->Dut_SetTxAntennasMask(txMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetTxAntennasMask Fail.");

    status = m_CTxControl->Dut_SetRxAntennasMask(RxMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetRxAntennasMask Fail.");

    status = m_CTxControl->Dut_SetTxAntenna(txMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail.");

    status = m_CTxControl->Dut_SetRxAntenna(RxMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail.");

    status = m_CTxControl->Dut_SetCloseLoop(isOpenClose);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetCloseLoop Fail.");

    status = m_CTxControl->Dut_SetHdkConfigure(offlineCalMask, 0);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetHdkConfigure Fail.");

    status = m_CTxControl->Dut_SetChannel(phyType, band, bandwidth, channel, calibrationMask, isOpenClose);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetChannel Fail.");

    status = m_CTxControl->Dut_SetTxAntenna(txMask);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail.");

    status = m_CTxControl->Dut_SetRxAntenna(0);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetRxAntenna Fail.");

    status = m_CTxControl->Dut_SetTxAntenna(TxAnt);
    if (!status)
        strcpy(lpszRunInfo, "Dut_SetTxAntenna Fail.");

    IXTAL::XTAL_PARAMS m_XTAL_PARAMS;
    m_XTAL_PARAMS.centralFreqMHz = freq;
    m_XTAL_PARAMS.cableLoss = 0;
    m_XTAL_PARAMS.powerIndex = stod(tmpAction["POWERINDEX"].c_str());
    CXtalCalibration* m_CXtalCalibration = new CXtalCalibration(theApp.dutApi, m_XTAL_PARAMS);
    if (m_CXtalCalibration->startTx() != MT_RET_OK)
        strcpy(lpszRunInfo, "Dut_TransmitCW startTx() Fail.");
    delete m_CXtalCalibration;
	return TRUE;
}
extern "C" BOOL PASCAL EXPORT StopContModulatedTx(DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    MT_RET status;
    IXTAL::XTAL_PARAMS m_XTAL_PARAMS;
    m_XTAL_PARAMS.centralFreqMHz = 0;
    m_XTAL_PARAMS.cableLoss = 0;
    m_XTAL_PARAMS.powerIndex = 255;
    CXtalCalibration* m_CXtalCalibration = new CXtalCalibration(theApp.dutApi, m_XTAL_PARAMS);
    status = m_CXtalCalibration->stopTx();
    delete m_CXtalCalibration;
    if (!status)
    {
        sprintf_s(lpszRunInfo, sizeof(lpszRunInfo), "Dut_TransmitCW stopTx() Fail.");
        return FALSE;
    }
    return TRUE;
}


extern "C" BOOL PASCAL EXPORT SfcsResponds(int MESSAGE_TYPE, LPSTR PREPOSING_CODE, LPSTR RESPONS_MSG, LPSTR lpszRunInfo)
{//Jerry Add Query SFCS Message
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ExportTestLog("PREPOSING_CODE = %s",PREPOSING_CODE);
	ExportTestLog("RESPONS_MSG = %s",RESPONS_MSG);
	ExportTestLog("szMessage = %s",lpszRunInfo);

	//if(strstr(PREPOSING_CODE,"FCSN") || strstr(PREPOSING_CODE,"CSN") || strstr(PREPOSING_CODE,"FCUSTSN")){
	if(strstr(PREPOSING_CODE,"FCSN") || strstr(PREPOSING_CODE,"CSN") ){
		zeroIt(Test_Info.SN);zeroIt(Net_Info.szSN);
		strCopy(Test_Info.SN,RESPONS_MSG);strCopy(Net_Info.szSN,RESPONS_MSG);
	}
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT PowerSupplyCtrl(funcSetPowerSupply cbPSCtrl, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CallBackFn.cbPSCtrl = cbPSCtrl;
	return TRUE;
}
#endif

extern "C" BOOL PASCAL EXPORT GetRdModeLicense()
{//Jerry Add
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT CableLossController(funcReadCableLoss cbReadCableLoss, LPSTR lpszRunInfo)
{
    theApp.m_funcReadCableLoss = cbReadCableLoss;
    return TRUE;
}

