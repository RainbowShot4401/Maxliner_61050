#include "..\stdafx.h"
#include "ParsedData_XTAL.h"

ParsedData_XTAL::ParsedData_XTAL()
{
}


ParsedData_XTAL::~ParsedData_XTAL()
{
}

void ParsedData_XTAL::parseThisCIS(vector<BYTE> CIS_raw_data)
{
	memcpy(&xtal_cis, &(CIS_raw_data[0]), sizeof(xtal_cis));
}


