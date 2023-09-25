#pragma once
#include <vector>
using namespace std;

class ParsedData_XTAL
{
public:
	ParsedData_XTAL();
	~ParsedData_XTAL();

	struct XTAL_CIS{
		BYTE xtalCal : 8;
		BYTE xtalbias : 8;
	}xtal_cis;

	void parseThisCIS(vector<BYTE> CIS_raw_data);
};

