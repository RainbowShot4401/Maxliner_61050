#include "..\stdafx.h"
#include "LoadDutRequired.h"

CLoadDutRequired::CLoadDutRequired()
{
   
}

CLoadDutRequired::~CLoadDutRequired()
{

}

void CLoadDutRequired::insertAPI(string flow)
{
   m_API.push_back(flow);
}

void CLoadDutRequired::API_Create(string testItem, vector<string> flow)
{
   API_flow.insert(pair<string, vector<string>>(testItem, flow));
}