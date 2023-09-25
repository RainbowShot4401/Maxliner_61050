#pragma once

#include "ITxCalibration.h"

class ITxGainTableCal
{
public:
	ITxGainTableCal();
	virtual ~ITxGainTableCal();

	virtual ITxCalibration::Status txGainTableCal(const ITxCalibration::TxGainTableParams &txGainTableParams) = 0;
	virtual ITxCalibration::TxGainTableParams getGainTableResultParams() = 0;
};

