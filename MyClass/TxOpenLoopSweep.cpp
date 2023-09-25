#include "TxOpenLoopSweep.h"
#include <algorithm>
#include <numeric>
#include <queue>

using namespace std;

OpenLoopSweep::OpenLoopSweep(CTxControl* dutFunc, dutWrapper* dutApi, funcGetTxPower m_funcGetTxPower, funcGetTxPowerEvm m_funcGetTxPowerEvm)
{
	_dutFunc             = dutFunc;
	_dutApiPonter        = dutApi;
	_m_funcGetTxPower    = m_funcGetTxPower;
	_m_funcGetTxPowerEvm = m_funcGetTxPowerEvm;
	MEASURE_EVM		     = TRUE;
}

OpenLoopSweep::~OpenLoopSweep()
{
}

int OpenLoopSweep::measSaturIndex(const ITxCalibration::TxOpenLoopSweepParams& txOpenLoopSweepParams)
{
	char szReturnMessage[512] = {'\0'};
	ITxCalibration::TxOpenLoopSweepParams _txOpenLoopSweepParams = txOpenLoopSweepParams;

	CTxControl::Status dutError = _dutFunc->SetPowerLimit(MAX_INDEX, CTxControl::OpenClose::OPEN_LOOP);
	if (CTxControl::Status::PASS != dutError)
		return ITxCalibration::Status::FAIL;

	double measPower;
	_m_funcGetTxPower(3, m_DUT_PARAM.CHANNEL, m_DUT_PARAM.TX_POWER_TARGET, m_DUT_PARAM.BAND, m_DUT_PARAM.TX_CHAIN, m_DUT_PARAM.RATE, m_DUT_PARAM.WIDE_BAND, measPower, szReturnMessage);
	//_txOpenLoopSweepParams.setVsaParams.rfAmplDb = 10;
    //IVSA* _vsa = new VSA(_txOpenLoopSweepParams.setVsaAmpTolParams, _txOpenLoopSweepParams.setVsaParams, _txOpenLoopSweepParams.captureVsaParams, _txOpenLoopSweepParams.analyzeParams);

	//vsaError = (IVSA::Status)_vsa->vsaInitialize();
	//if (TxDutApi::Status::PASS != dutError)
	//{
	//	_logPrint->print(string("vsaInitialize"));
	//	return ITxCalibration::Status::FAIL;
	//}

	//vsaError = (IVSA::Status)_vsa->vsaCapture();
	//if (IVSA::Status::PASS != vsaError)
	//{
		//_logPrint->print(string("vsaCapture"));
		//return ITxCalibration::Status::FAIL;
	//}

	//vsaError = (IVSA::Status)_vsa->vsaAnalyzeOnlyPower();
	//if (IVSA::Status::PASS != vsaError)
	//{
	//	_logPrint->print(string("vsaAnalyzeOnlyPower"));
	//	return ITxCalibration::Status::FAIL;
	//}
	return round(2 * measPower);
}

double CalcAverageOfVectorLin(const vector<double>& v)
{
	return (accumulate(v.begin(), v.end(), 0.0) / v.size());
}

int CalcAverageOfVectorLin(const vector<int>& v)
{
	return static_cast<int>(accumulate(v.begin(), v.end(), 0.0) / v.size());
}

double OpenLoopSweep::CalcAverageOfVectorLog(const vector<double>& v)
{
	vector<double> v_temp(v.begin(), v.end());
	std::transform(v_temp.begin(), v_temp.end(), v_temp.begin(), [](double x) -> double {return pow(10.0, x / 10.0); });
	double linAverage = CalcAverageOfVectorLin(v_temp);
	double average = 10.0 * log10(linAverage);
	return average;
}

#define NA_NUMBER 0 // 20230629, Zack add.
ITxCalibration::Status OpenLoopSweep::setConstMaxPower(const double& constPower)
{
	_maxPowerParams.maxPowerEvm_dB = NA_NUMBER;
	_maxPowerParams.maxPower_dBm = constPower;
	_maxPowerParams.maxPower_Index = static_cast<int>(round(2 * constPower));
	return ITxCalibration::Status::PASS;
}

ITxCalibration::Status OpenLoopSweep::setConstUltimateEvm(const double& constPower)
{
	_ultimateEvmParams.ultimateEvmPowerEvm_dB = NA_NUMBER;
	_ultimateEvmParams.ultimateEvmPower_dBm = constPower;
	_ultimateEvmParams.ultimateEvmPower_Index = static_cast<int>(round(2 * constPower));
	return ITxCalibration::Status::PASS;
}

ITxCalibration::Status OpenLoopSweep::txOpenLoopSweep(const ITxCalibration::TxOpenLoopSweepParams& txOpenLoopSweepParams)
{
	int currentIndex = 0;
	int targetPower = 0;
	_txOpenLoopSweepParams = txOpenLoopSweepParams;
	int saturationIndex = measSaturIndex(txOpenLoopSweepParams);
	//double rfAmplDbFor0dBm = txOpenLoopSweepParams.setVsaParams.rfAmplDb;
	char tmpMessage[512];
	char szReturnMessage[256];
	queue<string> LOG_Q;

	vector<int> voltageForAllAnts;
	vector<double> forAveragePower;
	vector<int> forAverageVoltage;
	vector<double> forAverageEvm;

	_OpenLoopSweepResultsParams.evm.clear();
	_OpenLoopSweepResultsParams.voltage.clear();
	_OpenLoopSweepResultsParams.indexes.clear();
	_OpenLoopSweepResultsParams.power.clear();

	for (int i_index = 0; i_index < _txOpenLoopSweepParams.indexes.size(); i_index++)
	{
		if (_txOpenLoopSweepParams.indexes.at(i_index) <= saturationIndex)
		{
			currentIndex = _txOpenLoopSweepParams.indexes.at(i_index);
			targetPower = currentIndex / 2;
			CTxControl::Status dutError = _dutFunc->SetPowerLimit(currentIndex, CTxControl::OpenClose::OPEN_LOOP);
			if (CTxControl::Status::PASS != dutError)
			{
				//_logPrint->print(string("SetPowerLimit"));
				sprintf(tmpMessage, "txOpenLoopSweep => SetPowerLimit Fail.");
				m_funcGetMessage(tmpMessage);
				return ITxCalibration::Status::FAIL;
			}

			voltageForAllAnts.clear();
			forAveragePower.clear();
			forAverageVoltage.clear();
			forAverageEvm.clear();

			for (int i_packet = 0; i_packet < _txOpenLoopSweepParams.analyzedPacketsPerIndex; i_packet++)
			{
				//vsaError = (IVSA::Status)_vsa->vsaCapture();
				////	Sleep(1500);
				//if (IVSA::Status::PASS != vsaError)
				//{
				//	_logPrint->print(string("\n### FAIL ON vsaCapture###\n"));
				//	return ITxCalibration::Status::FAIL;
				//}

				//if (!_txOpenLoopSweepParams.constantResultsParams.measureEvm && _txOpenLoopSweepParams.constantResultsParams.enableConstValue)
				//{
				//	vsaError = (IVSA::Status)_vsa->vsaAnalyzeOnlyPower();
				//	if (IVSA::Status::PASS != vsaError)
				//	{
				//		_logPrint->print(string("\n### FAIL ON vsaAnalyzeOnlyPower###\n"));
				//		return ITxCalibration::Status::FAIL;
				//	}
				//}
				//else
				//{
				//	for (int i_try = 0; i_try < 3; i_try++)
				//	{
				//		vsaError = (IVSA::Status)_vsa->vsaAnalyze();
				//		if (IVSA::Status::PASS != vsaError)
				//		{
				//			vsaError = (IVSA::Status)_vsa->vsaCapture();
				//			if (IVSA::Status::PASS != vsaError)
				//			{
				//				_logPrint->print(string("\n### FAIL ON vsaCapture###\n"));
				//				return ITxCalibration::Status::FAIL;
				//			}
				//		}
				//		else
				//			break;
				//		if (i_try == 2)
				//		{
				//			_logPrint->print(string("\n### FAIL ON vsaCapture i = 2###\n"));
				//			return ITxCalibration::Status::FAIL;
				//		}
				//	}
				//
				//	forAverageEvm.push_back(_vsa->vsaReturnEvm());
					
				//}
				double measPower, measEVM;
				int retry_count = 3;
				for (int i_try = 0; i_try < retry_count; i_try++)
				{
					if (MEASURE_EVM == TRUE)
					{
						if (_m_funcGetTxPowerEvm(0, m_DUT_PARAM.CHANNEL, targetPower, m_DUT_PARAM.BAND, m_DUT_PARAM.TX_CHAIN, m_DUT_PARAM.RATE, m_DUT_PARAM.WIDE_BAND, measPower, measEVM, szReturnMessage))
							break;
						else
						{
							if ((i_try + 1) == retry_count)
							{
								//_logPrint->print(string("SetPowerLimit"));
								sprintf(tmpMessage, "txOpenLoopSweep => _m_funcGetTxPowerEvm Fail.");
								m_funcGetMessage(tmpMessage);
								return ITxCalibration::Status::FAIL;
							}
						}
					}	
					else
						_m_funcGetTxPower(3, m_DUT_PARAM.CHANNEL, m_DUT_PARAM.TX_POWER_TARGET, m_DUT_PARAM.BAND, m_DUT_PARAM.TX_CHAIN, m_DUT_PARAM.RATE, m_DUT_PARAM.WIDE_BAND, measPower, szReturnMessage);		
				}
				voltageForAllAnts.clear();
				dutError = _dutFunc->GetVoltage(voltageForAllAnts);
				if (ITxCalibration::Status::PASS != dutError)
				{
					sprintf(tmpMessage, "txOpenLoopSweep => GetVoltage Fail.");
					m_funcGetMessage(tmpMessage);
					//_logPrint->print(string("\n### FAIL ON vsaCapture###\n"));
					return ITxCalibration::Status::FAIL;
				}
				forAverageVoltage.push_back(voltageForAllAnts.at(_txOpenLoopSweepParams.antennaNumber));
				forAveragePower.push_back(measPower);
				forAverageEvm.push_back(measEVM);
			}
			double averagePower;
			double averageEvm;
			int averageVoltage;

			averagePower   = CalcAverageOfVectorLog(forAveragePower);
			averageVoltage = CalcAverageOfVectorLin(forAverageVoltage);
			averageEvm     = CalcAverageOfVectorLog(forAverageEvm);

			_OpenLoopSweepResultsParams.evm.push_back(averageEvm);
			_OpenLoopSweepResultsParams.indexes.push_back(currentIndex);
			_OpenLoopSweepResultsParams.voltage.push_back(averageVoltage);
			_OpenLoopSweepResultsParams.power.push_back(averagePower);
			//if (debug == TRUE)
			//{
				sprintf(tmpMessage, "[DEBUG_CAL]=> INDEX_POWER:%3d  PWR: %6.2f  Volt: %5d  Evm: %6.2f", currentIndex, averagePower, averageVoltage, averageEvm);
				m_funcGetMessage(tmpMessage);
				LOG_Q.push(tmpMessage);
			//}
		}
	}
	//for Output log export.
	string tmpData = "", RAW_data = "", ant = "";
	int iStart = 0, tmpDataLength = 0;
	int size = LOG_Q.size();
	for (int i = 0; i < size; i++)
	{
		tmpData = LOG_Q.front();
		RAW_data += tmpData;
		LOG_Q.pop();
		if (RAW_data.length() > 65000)
		{
			string tmp;
			int index_start, index_stop;
			for (; RAW_data.length() != 0;)
			{
				index_start = RAW_data.length() / 65000;
				index_stop = RAW_data.length() % 65000;
				tmp = RAW_data.substr(index_start-1, index_stop);
				ExportTestLog("%s", tmp);
				RAW_data.erase((index_start-1), index_stop);
			}
			RAW_data.clear();
		}
		else if (i == (size - 1))
			ExportTestLog("%s", RAW_data);
	}

	if (_txOpenLoopSweepParams.constantResultsParams.enableConstValue)
	{
		setConstMaxPower(_txOpenLoopSweepParams.constantResultsParams.const_max_power);
		setConstUltimateEvm(_txOpenLoopSweepParams.constantResultsParams.const_ultimat_evm);
	}
	else
	{
		//_logPrint->print(string("SetPowerLimit"));
		sprintf(tmpMessage, "txOpenLoopSweep => _txOpenLoopSweepParams.constantResultsParams.enableConstValue Value error.");
		m_funcGetMessage(tmpMessage);
		ExportTestLog(tmpMessage);
		return ITxCalibration::Status::FAIL;
		/*ret = calcMaxPower();
		if (ITxCalibration::Status::PASS != ret)
		{
			_logPrint->print(string("CalmaxPower"));
			return ITxCalibration::Status::FAIL;
		}
		ret = calcUltimateEvm();
		if (ITxCalibration::Status::PASS != ret)
		{
			_logPrint->print(string("calacUlt"));
			return ITxCalibration::Status::FAIL;
		}*/
	}
	return ITxCalibration::Status::PASS;
}

ITxCalibration::OpenLoopSweepResultsParams OpenLoopSweep::getOpenLoopSweepResults()
{
	return _OpenLoopSweepResultsParams;
}

ITxCalibration::MaxPowerParams OpenLoopSweep::getMaxPower()
{
	return _maxPowerParams;
}

ITxCalibration::UltimateEvmParams OpenLoopSweep::getUltimateEvm()
{
	return _ultimateEvmParams;
}
