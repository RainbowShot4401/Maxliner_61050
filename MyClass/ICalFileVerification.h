#pragma once
#include "ReadCalFile.h"

enum return_status{ Pass = 0, Fail = 1, XTAL_fail = 2 };


class ICalFileVerification
{
public:
	virtual ~ICalFileVerification();

	virtual return_status rxVerification() = 0;
	virtual return_status txVerification() = 0;
};

