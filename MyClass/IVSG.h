#pragma once
#include "..\CommFunc.h"

static class IVSG
{
public:

	enum status { PASS, FAIL };

	bool isTransmitting = false;

	struct VsgStartCWParams
	{
		double loFreqMHz = 5500;
		double offsetFrequencyMHz = 1.25;
		double rfPowerLeveldBm = -10;
		double cableLoss = 20;
	};

	struct VsgInitParams {
		int port = 2; //2-left; 3-right
	};

	struct PathLossParams {
		signed int test_id = 10;
		double freqMHz = 2442;
		double outPathLossPerAnt[MAX_DATA_STREAM] = {};
		double outPathLossAverage = 0;
		int antennaMask = 15;
	};

	

	IVSG();
	virtual ~IVSG();
	virtual status vsgInitialize() = 0;
	virtual status vsgStartCW(VsgStartCWParams vsgStartCWParams) = 0;
	virtual status vsgStopCW() = 0;
	virtual status calcPathLoass(PathLossParams &pathLossParams) = 0;
	virtual status vsgStopPacket() = 0;
};

