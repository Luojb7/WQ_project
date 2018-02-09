#include <iostream>
#include "ThostFtdcMdApi.h"
#include "MdSpi.h"

using namespace std;

CThostFtdcMdApi* pUserApi;

char FRONT_ADDR[] = "tcp://180.168.146.187:10010";	
TThostFtdcBrokerIDType	BROKER_ID = "9999";				
TThostFtdcInvestorIDType INVESTOR_ID = "112585";
TThostFtdcPasswordType  PASSWORD = "qq824073152";			
char *ppInstrumentID[] = {"al1803"};			
int iInstrumentID = 1;								

int iRequestID = 0;
int dataNum = 0;

int main()
{
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						
	pUserApi->RegisterFront(FRONT_ADDR);					
	pUserApi->Init();


	pUserApi->Join();
	return 0;
}