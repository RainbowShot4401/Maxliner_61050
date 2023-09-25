#pragma once

#include "resource.h"		// main symbols
#include "../../Common/MVTDef.h"
#include "Intel_WAVDef.h"
#include "../../Common/UtilityDllDef.h"
#include "MyDef.h"
#include <tlhelp32.h>
#include "direct.h"
#include <string>

using namespace std;


//for Maxliner RF Tx Rx test.
#define MAX_DATA_STREAM 4
#define DUT_CAL_MAX_FREQS 96
#define MAX_ANTENNA_NUMBER 4
//#define MAX_SUB_BAND_CROS_POINT 7
#define MAX_SUB_BAND_CROS_POINT 16
#define MAX_SUB_BAND_CROS_POINT_LUT 7
#define MAX_FLATNESS_POINTS 16
#define NUM_OF_POWER_POINTS 6
#define MAX_REGION_NUMBER 3


#define CONSOLE_MAX_LENGTH 10000
#define PIPE_MAX_LENGTH  10000
#define TELNET_MAX_LENGTH 10000
#define SOCKET_MAX_LENGTH 10000
#define szSeparateKey  "\n"
#define UART_MESSAGE _T("UART_Message.txt")
#define UART_COMMAND _T("UART_Command.txt")
#define PIPE_MESSAGE _T("PIPE_Message.txt")
#define PIPE_COMMAND _T("PIPE_Command.txt")
#define TELNET_MESSAGE _T("TELNET_Message.txt")
#define TELNET_COMMAND _T("TELNET_Command.txt")
#define SOCKET_MESSAGE _T("SOCKET_Message.txt")
#define SOCKET_COMMAND _T("SOCKET_Command.txt")

static enum INI_TYPE
{
	INI_TYPE_STRING = 0,
	INI_TYPE_INTEGER = 1,
	INI_TYPE_LONG = 2,
	INI_TYPE_FLOAT =3,
	INI_TYPE_DOUBLE =4,
	INI_TYPE_BOOL = 5
};

class CCommFunc
{
public:
	CCommFunc(void);
public:
	~CCommFunc(void);

public:
	static BOOL SerialPortQueryData(LPSTR szInput, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szTestKeyWord="", LPSTR szTestKeySuccess="");
	static BOOL SerialPortGetKeyWord(LPSTR szKeyWord, UINT nTimeOutSec, LPSTR lpszRunInfo, LPSTR szTestKeyWord="", LPSTR szTestKeySuccess="");
	static BOOL TelnetGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szRecvMsg);
	static BOOL PipeLineQueryData(int nID, LPSTR szInput, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo);
	static BOOL PipeLineGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo);
	static BOOL ExportTestMessage(funcGetMessage cbMessage, const TCHAR *format, ...);
	static BOOL ExtractString(LPSTR szSrc, LPSTR szDest, int nDestLens, LPSTR szStart, LPSTR szEnd);
	static BOOL DelFolderAllFile(LPSTR lpszDelPath, LPSTR lpszCmd, LPSTR lpszRunInfo);
	static double GetlFileSize(const char* fname);
	static BOOL OutputCommand(LPSTR szFileName, const TCHAR *format, ...);
	static BOOL OutputMessage(LPSTR szFileName, const TCHAR *format, ...);
	static CString TraceStringMid(LPSTR szMessage, LPSTR szStart, LPSTR szEnd);
	static CString TraceStringMid(LPSTR szMessage, LPSTR szStart, LPSTR szEnd, BOOL bReverseFind);
public://Raphael add
	static CString MacAddrToCalcPlus(LPSTR MacAddress,int count);
	static void mydelay(long msec);
	static BOOL ClearSerialPort(UINT DelayTime);
	static BOOL ConSoleWithLog(LPSTR szCmd,LPSTR szKeyWord,LPSTR szMessage,UINT TimeOut);
	static BOOL PipeCalibration(int Pipeid,LPSTR Path,LPSTR KeyWord);
	static BOOL OpenExecSoft(char* Folder,char* SoftName);
	static BOOL FindExecSoft(LPSTR Name);
	static BOOL CloseExecSoft(LPSTR Name);
	static CString StrAddSpeace(LPSTR Source,int len);
	static BOOL IsSame(LPSTR Str_1,LPSTR Str_2);
	static BOOL PipeLineMsgClear(int Pipe);
	static BOOL PipeWithLog(int Pipeid,LPSTR szCmd,LPSTR szKeyWord,LPSTR szMessage,UINT TimeOut =5);
	static void RefreshTaskbarIcon();
	static CString InsetIt(LPSTR szOrg,LPSTR szInsert,int cnt);
	static int CStringHexToIntDec(CString csTemp);
public:
	static BOOL PipePlinkInit(int &Pipeid,LPSTR szCmd,LPSTR UserName,LPSTR PassWord,LPSTR szKeyWord);
	static BOOL RegDelete(LPSTR Key);
	static BOOL OutputMsgToBoth(funcGetMessage cbMessage, const TCHAR *format, ...);
	static BOOL CombineMessage(LPSTR lpszSrcData, LPSTR lpszAvailData, int nAvailLens, LPSTR szKey);
public:
	static BOOL SocketGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szRecvMsg);
	//Maxliner
	static int ConvertStandardToPhyType(const double& freq, const string& standard);
	static int ConvertFreqToBand(const double& freq);
	static int ConvertBandwidth(const string& Bandwidth);
	static int ConvertLoFreqtoLowChannel(const string& bandwidth, const int& freq);
	static int ConvertRate(const int& num_of_streams, const string& rate, const string& standard);
	static int CalcPacketLength(const string& standard, const string& rate, const string& Bandwidth);
	static int CalcValidGi(const string& standard);
	static int ConvertBandwidthForBurn(const string& Bandwidth, const string& standard);
	static int CCommFunc::WIFI_FrequencyMHzToChannel(int freq);
};
