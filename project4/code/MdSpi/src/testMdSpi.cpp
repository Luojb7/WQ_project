#include <iostream>
#include "ThostFtdcMdApi.h"
#include "MdSpi.h"
#include "Ini.h"

using namespace std;

CThostFtdcMdApi* pUserApi;

char FRONT_ADDR[] = "tcp://180.168.146.187:10031";	
TThostFtdcBrokerIDType	BROKER_ID ;				
TThostFtdcInvestorIDType INVESTOR_ID ;
TThostFtdcPasswordType  PASSWORD ;			
char *ppInstrumentID[3];			
int iInstrumentID ;								

int iRequestID = 0;
int dataNum = 0;

int main(int argc, char* argv[])
{
	char *configFilePath = argv[1];

	CIni ini;
	ini.OpenFile(configFilePath,"r");
	char *broId = ini.GetStr("Broker","ID");
	char *invId = ini.GetStr("Investor","ID");
	char *pwd = ini.GetStr("Investor","Password");
	sprintf(BROKER_ID, broId);
	sprintf(INVESTOR_ID, invId);
	sprintf(PASSWORD, pwd);
	iInstrumentID = ini.GetInt("InstrumentNum", "Num");
	cout << iInstrumentID << endl;

	char *Instrument[iInstrumentID];
	char InstrumentCode[4] = {'I','n','s','1'};
	for(int i=0; i<iInstrumentID; i++)
	{
		InstrumentCode[3] = (char)(i+49);
		Instrument[i] = ini.GetStr("Instrument", InstrumentCode);
		cout<<Instrument[i]<<endl;
	}
	*ppInstrumentID = *Instrument;
	for(int i=0;i<iInstrumentID;i++){
		cout<<ppInstrumentID[i]<<endl;
	}

	/*pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						
	pUserApi->RegisterFront(FRONT_ADDR);					
	pUserApi->Init();

	pUserApi->Join();*/
	return 0;
}