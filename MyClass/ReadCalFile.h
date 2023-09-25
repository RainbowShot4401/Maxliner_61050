#pragma once
#include "dutWrapper.h"
#include "DutControl.h"

class ReadCalFile
{
public:
	ReadCalFile(dutWrapper *dutApi);
	~ReadCalFile();

	struct CAL_FILE_DATA{
		int nv_memmory_size;
		BYTE raw_data_cal_file[MAX_BUFFER_SIZE];
	}cal_file_data;

	ReadCalFile::CAL_FILE_DATA ReadCalFile::read();

private:
	dutWrapper *dutApi;

};

