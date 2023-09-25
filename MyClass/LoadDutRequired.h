#pragma once
#include "..\CommFunc.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

class CLoadDutRequired
{
public:
   CLoadDutRequired();
   ~CLoadDutRequired();
public:
   vector<string> m_API;
   map<string, vector<string>> API_flow;
public:
   void insertAPI(string flow);
   void API_Create(string testItem, vector<string> flow);
};

