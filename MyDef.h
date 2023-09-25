#define OemAppFileName  _T("IQrun_Console.exe")
#define OemAppLogFile  _T("Log\\log_all.txt")
#define OemDestFileName	_T("Test.txt")
#define EEPROM_SIZE     65536
#define XML_DATA_SIZE 64
#define DATA_VAL_INIT	999
#define TX_PREFIX _T("0x3f 0x00 0xc2 0x02 0x00 0x09 0x00 0x12 0x34 0x04 0x50")
//Jerry Add
#define CTRL_C 3
#define KEYBORAD_ENTER 13

typedef struct _CALLBACKFN
{
	funcSetPowerSupply cbPSCtrl;

}CALLBACKFN, *pCALLBACKFN;

typedef struct _POWER_TABLE
{
	double BT_1M;
	double BT_2M;
	double BT_3M;
	double BT_LE;
}POWER_TABLE;

typedef struct _GLOBAL_INFO
{
	TCHAR szLocalPath[MAX_PATH];
	TCHAR szOP[64];
	TCHAR szBarcode[64];
	TCHAR szPN[64];
	HWND Main_Handle;
	TCHAR CUSTOMER[64];
	TCHAR INI_STR[MAX_PATH];

	int Pipe_SYS_ID;
	int Pipe_OEM_ID;
	int Pipe_PROG_ID;

	float Packet_Lens;
	BOOL	IsArt2;
	BOOL    IsColon;
	BOOL    IsConsole;
	BOOL    No_Pn_Name;

	TCHAR XTAL_METHOD_ON_PASS[MAX_PATH];
	TCHAR XTAL_METHOD_ON_FAIL[MAX_PATH];

	int Bt_Pwr;

}GLOBAL_INFO, *pGLOBAL_INFO;

typedef struct _CABLELOSS
{
	double TxPath[WIFI_CABLELOSS_ANTENNA][WIFI_CABLELOSS_MAX_ARY][WIFI_CABLELOSS_RESULT];
	double RxPath[WIFI_CABLELOSS_ANTENNA][WIFI_CABLELOSS_MAX_ARY][WIFI_CABLELOSS_RESULT];
	double GuPath[WIFI_CABLELOSS_ANTENNA][WIFI_CABLELOSS_MAX_ARY][WIFI_CABLELOSS_RESULT];
	inline _CABLELOSS()
	{
		zeroIt(TxPath);
		zeroIt(RxPath);
		zeroIt(GuPath);
	}
	TCHAR Iqfact_CableLoss[32];
}CABLELOSS, *pCABLELOSS;

typedef struct _TEST_INFO{
	
	int		TimeOut;
	TCHAR	szCmd[MAX_PATH];
	TCHAR	szKeyWord[MAX_PATH];
	TCHAR	szTestName[MAX_PATH];
	TCHAR   szScript_Name[32];
	//TCHAR   DontCare_Msg[MAX_PATH];
	TCHAR   DontCare_Msg[MAX_PATH];
	int     MAC_RULE;
	int     MAC_COUNT;
	TCHAR   Country_Code[3];
	TCHAR   Str_Start[12];
	TCHAR   Str_End[12];
	TCHAR	szItem[16];
	TCHAR   ErrorCode[MAX_PATH];
	TCHAR   SN[DLL_INFO_SIZE];
	TCHAR   Console_Msg[MAX_PATH];
	TCHAR   Paser_CableLoss[MAX_PATH];
	//BT
	TCHAR   ChipMode[MAX_PATH];

}TEST_INFO;

typedef struct _NET_INFO{
	TCHAR UserName[32];
	TCHAR PassWord[32];
	int   nPort;
    TCHAR szSN[DLL_INFO_SIZE];
}NET_INFO;
typedef struct _IQFACT_SCRIPT{
	CStringArray  Mac_Rules;
	//CStringArray 
	TCHAR		  Test_Script[32];
	TCHAR		  Sample_Script[32];
}IQFACT_SCRIPT;


extern CALLBACKFN		CallBackFn;
extern GLOBAL_INFO		g_GlobalInfo;
extern CABLELOSS		g_CableLoss;
extern TEST_INFO		Test_Info;
extern NET_INFO			Net_Info;
extern IQFACT_SCRIPT	Iqfact_Script;