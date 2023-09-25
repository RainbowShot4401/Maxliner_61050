#include "..\stdafx.h"
#include "ReadCalFile.h"

ReadCalFile::ReadCalFile(dutWrapper * dutApi) : dutApi(dutApi)
{
}

ReadCalFile::~ReadCalFile()
{
}

ReadCalFile::CAL_FILE_DATA ReadCalFile::read()
{
	dutApi->Dut_GetEEPROMSize(&cal_file_data.nv_memmory_size);
	dutApi->Dut_ReadEEPROM(0, DWORD(cal_file_data.nv_memmory_size), cal_file_data.raw_data_cal_file);
	return cal_file_data;
}




