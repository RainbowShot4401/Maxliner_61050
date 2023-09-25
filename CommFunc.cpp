#include "StdAfx.h"
#include "CommFunc.h"


CCommFunc::CCommFunc(void)
{	
}

CCommFunc::~CCommFunc(void)
{
}

BOOL CCommFunc::SerialPortQueryData(LPSTR szInput, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szTestKeyWord, LPSTR szTestKeySuccess)
{
	char szMessage[CONSOLE_MAX_LENGTH]={0},szTemp[CONSOLE_MAX_LENGTH];
	BOOL bRtn = TRUE;
	double dbStartTime = GetTickCount();
	double dbTestTime = GetTickCount();
	int nBootTestKeyTime = 5;
	DWORD dwBytesRead;	
	TCHAR szCmd[1024]={0};
	int   nFailCnt=0;
	TCHAR szTimeMsg[DLL_INFO_SIZE]={0};
	CString strMessage="";
	int	  nLens;

	OutputCommand(UART_COMMAND, szInput);
	sprintf_s(szCmd,sizeof(szCmd),"%s\n",szInput);
	if(!SerialPortWriteData(szCmd,lpszRunInfo)) return FALSE;
	int n=0;
	do
	{
		n++;
		zeroIt(szTemp);
		if(!SerialPortReadData(szTemp, sizeof(szTemp), dwBytesRead, lpszRunInfo))
			return FALSE;
		strMessage += szTemp;

		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{				
			bRtn = FALSE;
			break;
		}else if((strlen(szTestKeyWord) > 0) && (strlen(szMessage) == 0)) {//Check DUT is ready.
			if(((GetTickCount() - dbTestTime)/1000) > nBootTestKeyTime)
			{
				if(SerialPortWriteData(szTestKeyWord,lpszRunInfo))
				{
					Sleep(100);
					if(!SerialPortReadData(szTemp, sizeof(szTemp), dwBytesRead, lpszRunInfo))
						return FALSE;

					char szDbgMsg[DLL_INFO_SIZE];
					sprintf_s(szDbgMsg,DLL_INFO_SIZE,"szTemp = [%s],lens = %d; szTestKeySuccess = [%s], lens = %d",szTemp,strlen(szTemp),szTestKeySuccess,strlen(szTestKeySuccess));
					if(strstr(szTemp, szTestKeySuccess))
						return TRUE;
				}
				dbTestTime = GetTickCount();
			}
		}
		if(strMessage.Find(szKeyWord) != SEARCH_NOT_FOUND)
			break;
	} while(TRUE);

	memset(lpszRunInfo,0x00,DLL_INFO_SIZE);
	zeroIt(szMessage);
	nLens = strMessage.GetLength();
	if(nLens > 500)
		sprintf_s(szMessage,DLL_INFO_SIZE,strMessage.Right(500));
	else
		sprintf_s(szMessage,DLL_INFO_SIZE,"%s",strMessage.GetBuffer());

	if(bRtn){
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s",szMessage);	
	}else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get serial port keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
		if(nLens >= LOG_MAX_SIZE)
			ExportTestLog(strMessage.Right(LOG_MAX_SIZE-1));
		else
			ExportTestLog(strMessage);
	}


	OutputMessage(UART_MESSAGE, strMessage.GetBuffer());
	return bRtn;
}

BOOL CCommFunc::SerialPortGetKeyWord(LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szTestKeyWord, LPSTR szTestKeySuccess)
{
	char szMessage[CONSOLE_MAX_LENGTH]={0},szTemp[CONSOLE_MAX_LENGTH];
	BOOL bRtn = TRUE;
	double dbStartTime = GetTickCount();
	double dbTestTime = GetTickCount();
	int nBootTestKeyTime = 5;
	DWORD dwBytesRead;
	CString strMessage="";
	int	  nLens;


	do
	{
		zeroIt(szTemp);
		if(!SerialPortReadData(szTemp, sizeof(szTemp), dwBytesRead, lpszRunInfo))
			return FALSE;
		strMessage += szTemp;
		if(strlen(szTemp)>0){
			ExportTestLog("Console Output szMessage = {%s}",szTemp);
		}
/*
		if((strlen(szMessage)+strlen(szTemp)) >= sizeof(szMessage))
			strCopy(szMessage,&szMessage[strlen(szMessage)-100]);		
		strcat_s(szMessage,strlen(szMessage)+strlen(szTemp)+1,szTemp);
*/
		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{
			bRtn = FALSE;
			break;
		}else if((strlen(szTestKeyWord) > 0) && (strlen(szMessage) == 0)) {//Check DUT is ready.
			if(((GetTickCount() - dbTestTime)/1000) > nBootTestKeyTime)
			{
				if(SerialPortWriteData(szTestKeyWord,lpszRunInfo))
				{
					Sleep(100);
					if(!SerialPortReadData(szTemp, sizeof(szTemp), dwBytesRead, lpszRunInfo))
						return FALSE;

					char szDbgMsg[DLL_INFO_SIZE];
					sprintf_s(szDbgMsg,DLL_INFO_SIZE,"szTemp = [%s],lens = %d; szTestKeySuccess = [%s], lens = %d",szTemp,strlen(szTemp),szTestKeySuccess,strlen(szTestKeySuccess));
					if(strstr(szTemp, szTestKeySuccess))
						return TRUE;
				}
				dbTestTime = GetTickCount();
			}
		}		
		if(strMessage.Find(szKeyWord) != SEARCH_NOT_FOUND)
			break;
	} while (TRUE);

	memset(lpszRunInfo,0x00,DLL_INFO_SIZE);
	zeroIt(szMessage);
	nLens = strMessage.GetLength();
	if(nLens > 500)
		sprintf_s(szMessage,DLL_INFO_SIZE,strMessage.Right(500));
	else
		sprintf_s(szMessage,DLL_INFO_SIZE,"%s",strMessage.GetBuffer());

	if(bRtn){
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s",szMessage);	
	}else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get serial port keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
		if(nLens >= LOG_MAX_SIZE)
			ExportTestLog(strMessage.Right(LOG_MAX_SIZE-1));
		else
			ExportTestLog(strMessage);
	}	

	OutputMessage(UART_MESSAGE, strMessage.GetBuffer());
	return bRtn;
}

BOOL CCommFunc::TelnetGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szRecvMsg)
{
	char szMessage[TELNET_MAX_LENGTH]={0},szTemp[TELNET_MAX_LENGTH];
	BOOL bRtn = TRUE;
	double dbStartTime = GetTickCount();
	double dbTestTime = GetTickCount();	

	do
	{
		zeroIt(szTemp);
		if(!TelnetReadData(nID, szTemp, lpszRunInfo))
			return FALSE;
		if((strlen(szMessage)+strlen(szTemp)) >= sizeof(szMessage))
			strCopy(szMessage,&szMessage[strlen(szMessage)-100]);
		strcat_s(szMessage,strlen(szMessage)+strlen(szTemp)+1,szTemp);
		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{
			sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get serial port keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
			bRtn = FALSE;
			break;
		}
	} while (!strstr(szMessage, szKeyWord));

	strcpy_s(szRecvMsg,TELNET_MAX_LENGTH,szMessage);
	return bRtn;
}

BOOL CCommFunc::PipeLineQueryData(int nID, LPSTR szInput, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo)
{
	TCHAR szMessage[PIPE_MAX_LENGTH],szTemp[PIPE_MAX_LENGTH];
	BOOL bRtn = TRUE;
	DWORD dbStartTime = GetTickCount();
	DWORD dbTestTime = GetTickCount();
	TCHAR szCmd[1024]={0};
	CString strMessage="";
	int	  nLens;

	OutputCommand(PIPE_COMMAND, szInput);
	sprintf_s(szCmd,sizeof(szCmd),"%s\n",szInput);
	if(!PipeLineWriteData(nID, szCmd,lpszRunInfo)) return FALSE;
	do
	{
		zeroIt(szTemp);
		if(!PipeLineReadData(nID, szTemp, sizeof(szTemp), lpszRunInfo))
			return FALSE;
		strMessage += szTemp;

		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{
			bRtn = FALSE;
			break;
		}
		if(strMessage.Find(szKeyWord) != SEARCH_NOT_FOUND)
			break;
	} while(TRUE);

	strMessage.Replace("%","%%");
	memset(lpszRunInfo,0x00,DLL_INFO_SIZE);
	zeroIt(szMessage);
	nLens = strMessage.GetLength();
	if(nLens > (DLL_INFO_SIZE-1))
		sprintf_s(szMessage,DLL_INFO_SIZE,strMessage.Right(DLL_INFO_SIZE-1));
	else
		sprintf_s(szMessage,DLL_INFO_SIZE,"%s",strMessage.GetBuffer());

	if(bRtn){
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s",szMessage);	
	}else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get pipe keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
		if(nLens >= LOG_MAX_SIZE)
			ExportTestLog(strMessage.Right(LOG_MAX_SIZE-1));
		else
			ExportTestLog(strMessage);
	}


	OutputMessage(PIPE_MESSAGE, strMessage.GetBuffer());
	return bRtn;
}

BOOL CCommFunc::PipeLineGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo)
{
	char szMessage[PIPE_MAX_LENGTH]={0},szTemp[PIPE_MAX_LENGTH];
	BOOL bRtn = TRUE;
	double dbStartTime = GetTickCount();
	double dbTestTime = GetTickCount();
	CString strMessage="";
	int nLens;


	do
	{
		zeroIt(szTemp);
		if(!PipeLineReadData(nID, szTemp, sizeof(szTemp), lpszRunInfo))
			return FALSE;
		strMessage += szTemp;
		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{
			bRtn = FALSE;
			break;
		}
		if(strMessage.Find(szKeyWord) != SEARCH_NOT_FOUND)
			break;
	} while (TRUE);

	memset(lpszRunInfo,0x00,DLL_INFO_SIZE);
	zeroIt(szMessage);
	nLens = strMessage.GetLength();
	if(nLens > 500)
		sprintf_s(szMessage,DLL_INFO_SIZE,strMessage.Right(500));
	else
		sprintf_s(szMessage,DLL_INFO_SIZE,"%s",strMessage.GetBuffer());

	if(bRtn){
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s",szMessage);	
	}else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get pipe keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
		if(nLens >= LOG_MAX_SIZE)
			ExportTestLog(strMessage.Right(LOG_MAX_SIZE-1));
		else
			ExportTestLog(strMessage);
	}	

	OutputMessage(PIPE_MESSAGE, strMessage.GetBuffer());
	return bRtn;
}

BOOL CCommFunc::ExportTestMessage(funcGetMessage cbMessage, const TCHAR *format, ...)
{
	TCHAR szBuffer[DLL_INFO_SIZE];
	va_list marker;
	va_start(marker, format);
	vsprintf_s(szBuffer, sizeof(szBuffer), format, marker);
	va_end(marker);
	cbMessage(szBuffer);
	return TRUE;
}

BOOL CCommFunc::ExtractString(LPSTR szSrc, LPSTR szDest, int nDestLens, LPSTR szStart, LPSTR szEnd)
{
	CString strSrc,strTemp,strDest;
	int nAddr;
	strSrc.Format(_T("%s"),szSrc);
	if((nAddr = strSrc.Find(szStart)) == -1) return FALSE;
	strTemp = strSrc.Mid(nAddr+strlen(szStart));
	if((nAddr = strTemp.Find(szEnd)) == -1)	return FALSE;

	strDest = strTemp.Left(strTemp.Find(szEnd));	
	strDest = strDest.Trim();
	sprintf_s(szDest,nDestLens,"%s",strDest.GetBuffer());
	return TRUE;
}

BOOL CCommFunc::DelFolderAllFile(LPSTR lpszDelPath, LPSTR lpszCmd, LPSTR lpszRunInfo)
{
	char szMessage[DLL_INFO_SIZE]={0};
	char szDestFileName[MAX_PATH]={0};
	FILE *stream = NULL;
	errno_t err;

	//Check delete folder is exist or not.
	if(!CheckFileExist(lpszDelPath,lpszRunInfo))
		return FALSE;

	sprintf_s(szDestFileName,sizeof(szDestFileName),"%s\\%s",lpszDelPath,"MyDelete.bat");	
	if((err = fopen_s(&stream, szDestFileName, "w")) != NULL)
	{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't create  cableLoss from DUT.[Name:%s]",szDestFileName);
		return FALSE;
	}

	fprintf(stream,"cd %s\n",lpszDelPath);
	fprintf(stream,"%c:\n",lpszDelPath[0]);
	fprintf(stream,"%s\n",lpszCmd);
	fclose(stream);

	WinExec(szDestFileName,SC_MINIMIZE);

	//Delete bat file
	//	if(CheckFileExist(szDestFileName,lpszRunInfo)) DeleteFile(szDestFileName);

	return TRUE;
}

double CCommFunc::GetlFileSize(const char* fname)
{
	if (!fname && !*fname)
		return 0.0;
	HANDLE h;
	WIN32_FIND_DATA info;

	if ((h=FindFirstFile(fname,&info))
		!= INVALID_HANDLE_VALUE)
	{
		FindClose(h);
		if ((info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			== 0) // Is it a file?
		{
			union
			{
				struct { DWORD low, high; } lh;
				__int64 size; // MS large int extension
			} file;
			file.lh.low = info.nFileSizeLow;
			file.lh.high= info.nFileSizeHigh;
			return file.size; // will be casted to double
		}
		// It's a directory, not a file
	}
	return 0.0; // No such name.
}


BOOL CCommFunc::OutputCommand(LPSTR szFileName, const TCHAR *format, ...)
{
	DWORD dwWriteByte = 0;

	//#ifdef _DEBUG
	TCHAR szBuffer[65535];
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szLocalPath[MAX_PATH]={0};
	va_list marker;
	va_start(marker, format);
	vsprintf_s(szBuffer, sizeof(szBuffer), format, marker);
	va_end(marker);

	CString strLogPath;

	if(GetLocalDirectory(szLocalPath,szMessage) != ERROR_SUCCESS)
		strLogPath = _T("c:\\");
	strLogPath.Format("%s",szLocalPath);
	strLogPath += szFileName;

	CString strOutput;// = cTime.Format(_T("%Y-%m-%d %H:%M:%S "));

	strOutput += szBuffer;
	strOutput += _T("\r\n");

	HANDLE hFile = ::CreateFile(	strLogPath, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRC = ::SetFilePointer(hFile,
			0,
			NULL,
			FILE_END);

		//always append
		if(dwRC != INVALID_SET_FILE_POINTER)
			::WriteFile(hFile, strOutput, (DWORD)strOutput.GetLength()*sizeof(TCHAR), &dwWriteByte, NULL);

		::CloseHandle(hFile);
	}
	//#endif

	return dwWriteByte;
}

BOOL CCommFunc::OutputMessage(LPSTR szFileName, const TCHAR *format, ...)
{
	DWORD dwWriteByte = 0;

	//#ifdef _DEBUG
	TCHAR szBuffer[65535];
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szLocalPath[MAX_PATH]={0};
	va_list marker;
	va_start(marker, format);
	vsprintf_s(szBuffer, sizeof(szBuffer), format, marker);
	va_end(marker);

	CString strLogPath;

	if(GetLocalDirectory(szLocalPath,szMessage) != ERROR_SUCCESS)
		strLogPath = _T("c:\\");
	strLogPath.Format("%s",szLocalPath);
	strLogPath += szFileName;

	CString strOutput;// = cTime.Format(_T("%Y-%m-%d %H:%M:%S "));

	strOutput += szBuffer;
	strOutput += _T("\r\n");

	HANDLE hFile = ::CreateFile(	strLogPath, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRC = ::SetFilePointer(hFile,
			0,
			NULL,
			FILE_END);

		//always append
		if(dwRC != INVALID_SET_FILE_POINTER)
			::WriteFile(hFile, strOutput, (DWORD)strOutput.GetLength()*sizeof(TCHAR), &dwWriteByte, NULL);

		::CloseHandle(hFile);
	}
	//#endif

	return dwWriteByte;
}

CString CCommFunc::TraceStringMid(LPSTR szMessage, LPSTR szStart, LPSTR szEnd)
{
	/*CString strMsg="";
	int addr;

	strMsg.Format("%s",szMessage);
	addr = strMsg.Find(szStart);
	if(addr < 0 ) return _T("");
	strMsg = strMsg.Mid(addr+strlen(szStart));
	addr = strMsg.Find(szEnd);
	if(addr < 0) return _T("");
	return strMsg.Left(addr).Trim();*/
	CString strMsg="";
	int addr;

	strMsg.Format("%s",szMessage);

	if(strlen(szStart)>0){
		addr = strMsg.Find(szStart);
		if(addr < 0 ) return _T("");
		strMsg = strMsg.Mid(addr+strlen(szStart));
	}

	if (strlen(szEnd)>0){
		addr = strMsg.Find(szEnd);
		if(addr < 0) return _T("");
		strMsg= strMsg.Left(addr).Trim();
	}

	return strMsg;
}

CString CCommFunc::TraceStringMid(LPSTR szMessage, LPSTR szStart, LPSTR szEnd, BOOL bReverseFind)
{
	/*CString strMsg="";
	int addr;

	strMsg.Format("%s",szMessage);
	addr = strMsg.Find(szStart);
	if(addr < 0 ) return _T("");
	strMsg = strMsg.Mid(addr+strlen(szStart));
	addr = strMsg.Find(szEnd);
	if(addr < 0) return _T("");
	return strMsg.Left(addr).Trim();*/
	CString strMsg="";
	CString strTemp = "";
	int addr;
	int addr2 = 0;

	strMsg.Format("%s",szMessage);

	if(strlen(szStart)>0){
		addr = strMsg.Find(szStart);
		if(addr < 0 ) return _T("");
		strMsg = strMsg.Mid(addr+strlen(szStart));
	}

	strTemp = strMsg;
	if (strlen(szEnd)>0){
		do
		{
			addr = strTemp.Find(szEnd);
			if(!bReverseFind)
				break;
			else if(addr == -1)
			{
				addr = addr2-strlen(szEnd);
				break;
			}
			addr2 += addr+strlen(szEnd);
			strTemp = strMsg.Mid(addr2);
		}while(1);
		if(addr < 0) return _T("");
		strMsg= strMsg.Left(addr);
	}

	strMsg.Trim();
	return strMsg;
}

CString CCommFunc::MacAddrToCalcPlus(LPSTR MacAddress,int count){
	CString tmp="";
	TCHAR Results[DLL_INFO_SIZE];
	TCHAR szMsg[DLL_INFO_SIZE];

	if (!MacAddrToCalc(MacAddress,Results,count,szMsg,DLL_INFO_SIZE)){
		return tmp;// follow mac rule count
	}
	else{
		tmp.Format("%s",Results);
		tmp.MakeUpper();
	}
	return tmp;
}
void CCommFunc::mydelay(long msec){
	DWORD stime,etime;
	MSG         m_MSG;
	int i=0;

	stime = GetTickCount();

	do{

		if ( ((i++) % 5) == 0)
		{

			if (::PeekMessage( &m_MSG, NULL, 0, 0, PM_REMOVE )) 
			{
				::TranslateMessage(&m_MSG);
				::DispatchMessage(&m_MSG);
			}
		}
		else
			Sleep(30);

		etime = GetTickCount();
	}while(((long)( etime - stime)) < msec);
}
BOOL CCommFunc::ClearSerialPort(UINT DelayTime){
	char szMessage[DLL_INFO_SIZE]={0},szTemp[CONSOLE_MAX_LENGTH];
	DWORD dwBytesRead;	
	do{
		zeroIt(szTemp);
		if(!SerialPortReadData(szTemp, sizeof(szTemp), dwBytesRead, szMessage))
			return FALSE;
		if(strlen(szTemp)==0){
			break;
		}
		Sleep(DelayTime);
	} while(TRUE);

	return TRUE;
}
BOOL CCommFunc::ConSoleWithLog(LPSTR szCmd,LPSTR szKeyWord,LPSTR szMessage,UINT TimeOut){
	BOOL res;
	CCommFunc::ClearSerialPort(100);

	ExportTestLog("Input szCmd = {%s}",szCmd);

	res=CCommFunc::SerialPortQueryData(szCmd,szKeyWord,TimeOut,szMessage);

	ExportTestLog("Output szMessage = {%s}",szMessage);


	return res;

}
BOOL CCommFunc::PipeCalibration(int Pipeid,LPSTR Path,LPSTR KeyWord){

	CString szDisk;
	TCHAR	szCmd[MAX_PATH];
	TCHAR	szKeyWord[MAX_PATH];
	TCHAR	szLocalPath[MAX_PATH];
	TCHAR	szMessage[PIPE_MAX_LENGTH];
	ExportTestLog("=====PipeCalibration=====");
	GetLocalDirectory(szLocalPath,szMessage);

	/**********Input Local Disk**********/
	szDisk = CCommFunc::TraceStringMid(szMessage, ":",":");
	sprintf_s(szCmd,sizeof(szCmd),"%s:",szDisk);
	sprintf_s(szKeyWord,sizeof(szKeyWord),"%s:",szDisk);
	ExportTestLog("Local Disk is %s",szDisk);
	if(!CCommFunc::PipeLineQueryData(Pipeid,szCmd,szKeyWord,5,szMessage))
		return FALSE;
	ExportTestLog("Input to Local Disk szMessage:{%s}",szMessage);
	/**********Input Local Disk**********/
	/**********Input Local Script Path**********/
	sprintf_s(szCmd,sizeof(szCmd),"cd %s",szLocalPath);
	sprintf_s(szKeyWord,sizeof(szKeyWord),"%s",szLocalPath);

	szKeyWord[strlen(szKeyWord)-1]='>';
	if(!CCommFunc::PipeLineQueryData(Pipeid,szCmd,szKeyWord,5,szMessage))
		return FALSE;
	ExportTestLog("Input to Local Script Path szMessage:{%s}",szMessage);
	/**********Input Local Script Path**********/
	/**********Input Path**********/
	ExportTestLog("=====PipeCalibration=====");
	if (strlen(Path)>0){
		ExportTestLog("=====Input to %s=====",Path);
		sprintf_s(szKeyWord,sizeof(szKeyWord),"%s%s\\",szLocalPath,Path);
		sprintf_s(szCmd,sizeof(szCmd),"cd %s",szKeyWord);
		szKeyWord[strlen(szKeyWord)-1]='>';
		if(!CCommFunc::PipeLineQueryData(Pipeid,szCmd,szKeyWord,5,szMessage))
			return FALSE;
		ExportTestLog("Input to  Path szMessage:{%s}",szMessage);
		ExportTestLog("=====Input to %s=====",Path);
	}/**********Input Path**********/
	sprintf_s(KeyWord,DLL_INFO_SIZE,"%s",szKeyWord);
	return TRUE;


}
BOOL CCommFunc::OpenExecSoft(char* Folder,char* SoftName){
	TCHAR szLocalPath[MAX_PATH];
	TCHAR szMessage[DLL_INFO_SIZE];
	BOOL flag=FALSE;

	if (FindExecSoft(SoftName)){
		ExportTestLog("Find %s is already open",SoftName);
		return TRUE;
	}

	GetLocalDirectory(szLocalPath,szMessage);
	sprintf_s(szMessage,MAX_PATH,"%sOEM\\%s\\",szLocalPath,Folder);

	if (_chdir(szMessage)!=0){
		return flag;
	}

	sprintf_s(szMessage,MAX_PATH,"%sOEM\\%s\\%s.exe",szLocalPath,Folder,SoftName);

	ShellExecute(NULL, "open",szMessage, NULL, NULL, SW_SHOWMINNOACTIVE);
	ExportTestLog("Open %s",szMessage);
	mydelay(2000);
	if (FindExecSoft(SoftName)){
		flag=TRUE;
		HWND mainHwnd = ::GetForegroundWindow();
		if (mainHwnd!=g_GlobalInfo.Main_Handle){
			SendMessage(mainHwnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
		}
	}

	if (_chdir(szLocalPath)!=0){
		return flag;
	}
	return flag;
}
BOOL CCommFunc::FindExecSoft(LPSTR Name){
	BOOL flag=FALSE;
	HANDLE hProcessSnap;
	CString SoftName_Upper="";
	CString SoftName_Lower="";

	SoftName_Upper.Format("%s",Name);
	SoftName_Lower.Format("%s",Name);
	SoftName_Upper=SoftName_Upper.MakeUpper();
	SoftName_Lower=SoftName_Lower.MakeLower();

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if( hProcessSnap != INVALID_HANDLE_VALUE ) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof( PROCESSENTRY32 );
		if( !Process32First( hProcessSnap, &pe32 ) )  {
			CloseHandle( hProcessSnap ); 
		}
		else{
			do{
				if(strstr(pe32.szExeFile,Name)!=NULL){
					flag=TRUE;
					ExportTestLog("Find %s in WorkManager",Name);
					break;
				}
				if(strstr(pe32.szExeFile,SoftName_Upper.GetBuffer())!=NULL){
					flag=TRUE;
					ExportTestLog("Find %s in WorkManager",SoftName_Upper.GetBuffer());
					break;
				}
				if(strstr(pe32.szExeFile,SoftName_Lower.GetBuffer())!=NULL){
					ExportTestLog("Find %s in WorkManager",SoftName_Lower.GetBuffer());
					flag=TRUE;
					break;
				}

			}while( Process32Next( hProcessSnap, &pe32 ) );
		}
	}//big if
	return flag;
}
BOOL CCommFunc::CloseExecSoft(LPSTR Name){
	TCHAR szCmd[MAX_PATH];
	TCHAR szMessage[DLL_INFO_SIZE];
	sprintf_s(szCmd,MAX_PATH,"Taskkill /F /IM %s",Name);

	while(FindExecSoft(Name)){
		if (!PipeLineQueryData(g_GlobalInfo.Pipe_SYS_ID,szCmd,"PID",5,szMessage)){
			ExportTestLog("Close %s Soft Fail",Name);
			return FALSE;
		}
		ExportTestLog("Output szMessage ={%s}",szMessage);
		CCommFunc::mydelay(1000);
	}
	return TRUE;
}
CString CCommFunc::StrAddSpeace(LPSTR Source,int len){
	CString str=Source;
	for(int i=0;i<(int)(len-strlen(Source));i++){
		str=str+" ";
	}
	return str;
}
BOOL CCommFunc::IsSame(LPSTR Str_1,LPSTR Str_2){
	BOOL res=FALSE;

	if(strstr(Str_1,Str_2)!=NULL && strlen(Str_1)==strlen(Str_2)){
		res=TRUE;
	}

	return res;
}
BOOL CCommFunc::PipeLineMsgClear(int Pipe){
	TCHAR szMessage[PIPE_MAX_LENGTH],szTemp[PIPE_MAX_LENGTH];
	do {//Clear Message
		zeroIt(szTemp);
		if(!PipeLineReadData(Pipe,szTemp,sizeof(szTemp),szMessage))
			return FALSE;
		Sleep(10);
	} while (strlen(szTemp)>0);
	return TRUE;
}
BOOL CCommFunc::PipeWithLog(int Pipeid,LPSTR szCmd,LPSTR szKeyWord,LPSTR szMessage,UINT TimeOut){
	BOOL res=FALSE;//Can display Test Log and Query data.

	CCommFunc::PipeLineMsgClear(Pipeid);

	ExportTestLog("Input szCmd = {%s}",szCmd);

	res=CCommFunc::PipeLineQueryData(Pipeid,szCmd,szKeyWord,TimeOut,szMessage);

	ExportTestLog("Output szMessage = {%s}",szMessage);

	return res;
}
void CCommFunc::RefreshTaskbarIcon(){
	HWND hShellTrayWnd = ::FindWindow("Shell_TrayWnd",NULL);

	HWND hTrayNotifyWnd = ::FindWindowEx(hShellTrayWnd,0,"TrayNotifyWnd",NULL);

	HWND hSysPager = ::FindWindowEx(hTrayNotifyWnd,0,"SysPager",NULL);

	HWND hToolbarWindow32;
	if (hSysPager)
	{
		hToolbarWindow32 = ::FindWindowEx(hSysPager,0,"ToolbarWindow32",NULL);
	}
	else
	{
		hToolbarWindow32 = ::FindWindowEx(hTrayNotifyWnd,0,"ToolbarWindow32",NULL);
	}
	if (hToolbarWindow32)
	{
		RECT r;
		::GetWindowRect(hToolbarWindow32,&r);
		int width = r.right - r.left;
		int height = r.bottom - r.top;

		for (int x = 1; x<width; x++)
		{
			::SendMessage(hToolbarWindow32,WM_MOUSEMOVE,0,MAKELPARAM(x,height/2));
		}
	}


}
BOOL CCommFunc::PipePlinkInit(int &Pipeid,LPSTR szCmd,LPSTR UserName,LPSTR PassWord,LPSTR szKeyWord){

	TCHAR szMessage[DLL_INFO_SIZE];//cd ../OEM/plink and input cmd(telnet or the other protocol)
	TCHAR szPath[10000];
	BOOL res=FALSE;
	BOOL flag=FALSE;
	CString str="";
	//DWORD dbRead=0;
	if (!CCommFunc::PipeCalibration(Pipeid,"OEM\\plink",szPath))
		return FALSE;
	if(!CCommFunc::PipeLineMsgClear(Pipeid))
		return FALSE;
	double StartTime=GetTickCount();

	while(((GetTickCount()-StartTime)/1000)<Test_Info.TimeOut){

		//loop to check connect
		if(strstr(szCmd,"telnet")!=NULL){//telnet work
			if(CCommFunc::PipeWithLog(Pipeid,szCmd,"login",szMessage,5)){
				flag=TRUE;
			}
		}
		else{//ssh work
			if(CCommFunc::PipeWithLog(Pipeid,szCmd,"y/n",szMessage,5)){
				CCommFunc::mydelay(500);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"y\n",UserName);
				if(PipeLineWriteData(Pipeid,szPath,szMessage)){
					flag=TRUE;
				}
			}
		}//if ssh

		if(flag){
			if(strlen(UserName)>0){
				CCommFunc::mydelay(500);
				//CCommFunc::PipeLineMsgClear(Pipeid);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"%s\r\n",UserName);
				ExportTestLog("UserName = {%s}",UserName);
				if(!PipeLineWriteData(Pipeid,szPath,szMessage)){
					break;
				}
			}
			if(strlen(PassWord)>=0){
				CCommFunc::mydelay(500);
				//CCommFunc::PipeLineMsgClear(Pipeid);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"%s\r\n",PassWord);
				ExportTestLog("PassWord = {%s}",PassWord);
				if(!PipeLineWriteData(Pipeid,szPath,szMessage)){
					break;
				}
			}
			CCommFunc::mydelay(500);zeroIt(szPath);
			PipeLineReadData(Pipeid,szPath,sizeof(szPath),szMessage);

			if(strstr(szPath,Test_Info.szKeyWord)!=NULL){

				res=TRUE;
				break;
			}
		}// if flag
		if(!res){
			CCommFunc::CloseExecSoft("plink.exe");
		}

	}//while

	for(int i=0;i<2;i++){
		PipeLineWriteData(Pipeid,"\n",szMessage);
		CCommFunc::PipeLineMsgClear(Pipeid);
	}


	return res;
}
BOOL CCommFunc::RegDelete(LPSTR Key){

	LONG lResult;// Kill HKEY 
	HKEY hKeyRoot;
	CString str = CCommFunc::TraceStringMid(Key,"","\\");

	ExportTestLog("Address = %s",Key);

	if(strstr("HKEY_CURRENT_USER",str.GetBuffer())){
		hKeyRoot=HKEY_CURRENT_USER;
	}
	else if(strstr("HKEY_CLASSES_ROOT",str.GetBuffer())){
		hKeyRoot=HKEY_CLASSES_ROOT;
	}
	else if(strstr("HKEY_LOCAL_MACHINE",str.GetBuffer())){
		hKeyRoot=HKEY_LOCAL_MACHINE;
	}
	else if(strstr("HKEY_USERS",str.GetBuffer())){
		hKeyRoot=HKEY_USERS;
	}
	else if(strstr("HKEY_CURRENT_CONFIG",str.GetBuffer())){
		hKeyRoot=HKEY_CURRENT_CONFIG;
	}
	else{
		return FALSE;
	}

	str=CCommFunc::TraceStringMid(Key,str.GetBuffer(),"");
	str=CCommFunc::TraceStringMid(str.GetBuffer(),"\\","");

	lResult = RegDeleteKey(hKeyRoot, str.GetBuffer());


	if(lResult==ERROR_SUCCESS || lResult==ERROR_FILE_NOT_FOUND){
		return TRUE;
	}else{
		return FALSE;
	}
}
BOOL CCommFunc::OutputMsgToBoth(funcGetMessage cbMessage, const TCHAR *format, ...)
{
	TCHAR szBuffer[DLL_INFO_SIZE];//Output Both Message
	va_list marker;
	va_start(marker, format);
	vsprintf_s(szBuffer, sizeof(szBuffer), format, marker);
	va_end(marker);
	cbMessage(szBuffer);
	ExportTestLog("%s",szBuffer);
	return TRUE;
}
BOOL CCommFunc::CombineMessage(LPSTR lpszSrcData, LPSTR lpszAvailData, int nAvailLens, LPSTR szKey)
{
	TCHAR *pszToken;
	CString strKey,strSrcData;
	int nAddr;
	pszToken = strstr(lpszSrcData, szKey);
	if(pszToken)
	{
		nAddr = pszToken-lpszSrcData;
		strSrcData.Format(_T("%s"),lpszSrcData);
		//create available data
		strKey = strSrcData.Left(nAddr-1);//Skip chr(13)
		strcpy_s(lpszAvailData,nAvailLens,strKey);

		//separate unused data
		strKey = strSrcData.Mid(nAddr+1);
		strcpy_s(lpszSrcData, SOCKET_MAX_LENGTH, strKey);
		return TRUE;
	}

	return FALSE;
}
BOOL CCommFunc::SocketGetKeyWord(int nID, LPSTR szKeyWord, UINT nTimeOutSec,LPSTR lpszRunInfo, LPSTR szRecvMsg)
{
	char szMessage[SOCKET_MAX_LENGTH]={0},szTemp[SOCKET_MAX_LENGTH];
	BOOL bRtn = TRUE;
	double dbStartTime = GetTickCount();
	double dbTestTime = GetTickCount();	
	DWORD dwBytesRead;
	do
	{
		zeroIt(szTemp);
		if(!SocketReceive(szTemp, sizeof(szTemp)-1, dwBytesRead, lpszRunInfo))
			return FALSE;
		if((strlen(szMessage)+strlen(szTemp)) >= sizeof(szMessage))
			strCopy(szMessage,&szMessage[strlen(szMessage)-100]);
		strcat_s(szMessage,strlen(szMessage)+strlen(szTemp)+1, szTemp);
		Sleep(10);
		if(((GetTickCount() - dbStartTime)/1000) > nTimeOutSec)
		{
			sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get socket keyword is timeout, [KeyWord = %s, TimeOut = %d]",szKeyWord,nTimeOutSec);
			bRtn = FALSE;
			break;
		}
	} while (!strstr(szMessage, szKeyWord));

	strcpy_s(szRecvMsg,SOCKET_MAX_LENGTH,szMessage);
	return bRtn;
}

CString CCommFunc::InsetIt(LPSTR szOrg,LPSTR szInsert,int cnt){//Jerry Add

	CString str="";CString tmp="";

	for (int i=1;i<=(int)strlen(szOrg);i++){
		tmp.Format("%c",szOrg[i-1]);
		str=str+(((i-1)%cnt==0)?(str.GetLength()>0)?szInsert:"":"")+tmp;
	}

	return str;
}
//Jerry Add
int CCommFunc::CStringHexToIntDec(CString csTemp)
{
	TCHAR *end = NULL;
	int value = _tcstol (csTemp, &end, 16);
	return  value;
}

int CCommFunc::ConvertStandardToPhyType(const double& freq, const string& standard)
{
	if (freq >= 4900)
	{
		if (!standard.compare("11ax"))
			return 7;
		else if (!standard.compare("11ac"))
			return 6;
		else if (!standard.compare("11n"))
			return 4;
		else if (!standard.compare("11a"))
			return 0;
	}
	else
	{
		if (!standard.compare("11ax"))
			return 7;
		else if (!standard.compare("11ac"))
			return 5;
		else if (!standard.compare("11g"))
			return 2;
		else if (!standard.compare("11n"))
			return 5;
	}
	return TRUE;
}

int CCommFunc::ConvertFreqToBand(const double& freq)  //to do: need to  test range 4900 to 5000
{
	if (freq >= 4900)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int CCommFunc::ConvertBandwidth(const string& Bandwidth)
{
	if (string::npos != Bandwidth.find("240"))
		return 4;
	else if (string::npos != Bandwidth.find("160"))
		return 3;
	else if (string::npos != Bandwidth.find("80"))
		return 2;
	else if (string::npos != Bandwidth.find("40"))
		return 1;
	else if (string::npos != Bandwidth.find("20"))
		return 0;
}

int CCommFunc::ConvertLoFreqtoLowChannel(const string& bandwidth, const int& freq)
{
	int LoChannel = 0;
	int baseFreq = 2407;
	if (freq >= 5000)  //to do: need to  test range 4900 to 5000
	{
		baseFreq = 5000;
	}
	else if (freq >= 4900)
	{
		baseFreq = 4000;
	}
	else
	{
		baseFreq = 2407;
	}

	LoChannel = (freq - baseFreq) / 5;

	if (string::npos != bandwidth.find("160"))
		return LoChannel - 14;
	else if (string::npos != bandwidth.find("80"))
		return LoChannel - 6;
	else if (string::npos != bandwidth.find("40"))
		return LoChannel - 2;
	else if (string::npos != bandwidth.find("20"))
		return LoChannel;

}

int CCommFunc::ConvertRate(const int& num_of_streams, const string& rate, const string& standard)
{
	int _num_of_streams = (num_of_streams < 1) ? 1 : num_of_streams;
	int index_rate = 0;
	if (!standard.compare("11b"))
	{
		if (!rate.compare("R_1M")) index_rate = 4;
		if (!rate.compare("R_2M")) index_rate = 1;
		if (!rate.compare("R_5_5M")) index_rate = 2;
		if (!rate.compare("R_11M")) index_rate = 3;
	}
	else if ((!standard.compare("11a")) || (!standard.compare("11g")))
	{
		if (!rate.compare("R_6M")) index_rate = 0;
		if (!rate.compare("R_9M")) index_rate = 1;
		if (!rate.compare("R_12M")) index_rate = 2;
		if (!rate.compare("R_18M")) index_rate = 3;
		//if (!rate.compare("OFDM-24")) index_rate = 4;
		//if (!rate.compare("OFDM-36")) index_rate = 5;
		if (!rate.compare("R_48M")) index_rate = 6;
		if (!rate.compare("R_54M")) index_rate = 7;
	}
	else if (!standard.compare("11n"))
	{
		index_rate = atoi(&(rate[3])) + (_num_of_streams - 1) * 8;
	}
	else if ((!standard.compare("11ax")) || (!standard.compare("11ac")) || (!rate.compare("MCS8")) || (!rate.compare("MCS9")) || (!rate.compare("MCS10")) || (!rate.compare("MCS11")))
	{
		index_rate = atoi(&(rate[3])) + (_num_of_streams - 1) * 16;
	}
	return index_rate;
}

int CCommFunc::CalcPacketLength(const string& standard, const string& rate, const string& Bandwidth)
{

	int bw = ConvertBandwidth(Bandwidth);

	if (!standard.compare("11b"))
	{
		return 2300;
	}
	else if ((!standard.compare("11g")) || (!standard.compare("11a")))
	{
		if (!rate.compare("R_6M")) { return 44; }		//0
		//else if (!rate.compare("OFDM-9")) { return 68; }	//1
		//else if (!rate.compare("OFDM-12")) { return 90; }	//2
		//else if (!rate.compare("OFDM-18")) { return 140; } //3
		//else if (!rate.compare("OFDM-24")) { return 180; } //4
		//else if (!rate.compare("OFDM-36")) { return 280; } //5
		else if (!rate.compare("R_48M")) { return 360; } //6
		else { return 420; }							//7
	}

	else if (!standard.compare("11n"))
	{
		if (bw == 0)
		{
			if (!rate.compare("MCS0")) { return 48; }
			else if (!rate.compare("MCS1")) { return 96; }
			else if (!rate.compare("MCS2")) { return 150; }
			else if (!rate.compare("MCS3")) { return 200; }
			else if (!rate.compare("MCS4")) { return 290; }
			else if (!rate.compare("MCS5")) { return 400; }
			else if (!rate.compare("MCS6")) { return 440; }
			else { return 500; }
		}
		else
		{
			if (!rate.compare("MCS0")) { return 100; }
			else if (!rate.compare("MCS1")) { return 200; }
			else if (!rate.compare("MCS2")) { return 320; }
			else if (!rate.compare("MCS3")) { return 420; }
			else if (!rate.compare("MCS4")) { return 620; }
			else if (!rate.compare("MCS5")) { return 820; }
			else if (!rate.compare("MCS6")) { return 920; }
			else { return 1020; }
		}
	}
	else if ((!standard.compare("11ac")) || (!standard.compare("11ax")))
	{
		if (bw == 0)
		{
			if (!rate.compare("MCS0")) { return 48; }
			else if (!rate.compare("MCS1")) { return 100; }
			else if (!rate.compare("MCS2")) { return 150; }
			else if (!rate.compare("MCS3")) { return 200; }
			else if (!rate.compare("MCS4")) { return 300; }
			else if (!rate.compare("MCS5")) { return 400; }
			else if (!rate.compare("MCS6")) { return 460; }
			else if (!rate.compare("MCS7")) { return 500; }
			else { return 600; }
		}
		else if (bw == 1)
		{
			if (!rate.compare("MCS0")) { return 100; }
			else if (!rate.compare("MCS1")) { return 200; }
			else if (!rate.compare("MCS2")) { return 320; }
			else if (!rate.compare("MCS3")) { return 420; }
			else if (!rate.compare("MCS4")) { return 620; }
			else if (!rate.compare("MCS5")) { return 820; }
			else if (!rate.compare("MCS6")) { return 920; }
			else if (!rate.compare("MCS7")) { return 1020; }
			else if (!rate.compare("MCS8")) { return 1220; }
			else { return 1420; }
		}
		else if (bw == 2)
		{
			if (!rate.compare("MCS0")) { return 220; }
			else if (!rate.compare("MCS1")) { return 440; }
			else if (!rate.compare("MCS2")) { return 660; }
			else if (!rate.compare("MCS3")) { return 880; }
			else if (!rate.compare("MCS4")) { return 1400; }
			else if (!rate.compare("MCS5")) { return 1800; }
			else if (!rate.compare("MCS6")) { return 2000; }
			else if (!rate.compare("MCS7")) { return 2200; }
			else if (!rate.compare("MCS8")) { return 2800; }
			else { return 3000; }
		}
		else
		{
			if (!rate.compare("MCS0")) { return 440; }
			else if (!rate.compare("MCS1")) { return 880; }
			else if (!rate.compare("MCS2")) { return 1400; }
			else if (!rate.compare("MCS3")) { return 1800; }
			else if (!rate.compare("MCS4")) { return 2800; }
			else if (!rate.compare("MCS5")) { return 3600; }
			else if (!rate.compare("MCS6")) { return 4000; }
			else if (!rate.compare("MCS7")) { return 4000; }
			else if (!rate.compare("MCS8")) { return 4000; }
			else { return 4000; }
		}
	}
	return 0;
}

int CCommFunc::CalcValidGi(const string& standard)
{
	//if ((!standard.compare("802.11ag")) || (!standard.compare("802.11b")))
	//{
	//	return 1;
	//}
	/*else*/ if (!standard.compare("11ax"))
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

int CCommFunc::ConvertBandwidthForBurn(const string& Bandwidth, const string& standard)
{
	if (!standard.compare("11b"))
	{
		return 3;
	}
	else
	{
		return ConvertBandwidth(Bandwidth);
	}
}

int CCommFunc::WIFI_FrequencyMHzToChannel(int freq)
{
	int Channel = 0;
	int baseFreq = 2407;
	if (freq >= 5000)  //to do: need to  test range 4900 to 5000
	{
		baseFreq = 5000;
	}
	else if (freq >= 4900)
	{
		baseFreq = 4000;
	}
	else
	{
		baseFreq = 2407;
	}
	if (freq == 2484)
		return 14;
	if (((freq - baseFreq) % 5) != 0)
		return -1;
	else
	{
		Channel = (freq - baseFreq) / 5;
		return Channel;
	}
}
