#include "ThostFtdcTraderApi.h"
#include "TraderSpi.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include "Ini.h"

using namespace std;

CThostFtdcTraderApi* pUserApi;

char  FRONT_ADDR[] = "tcp://180.168.146.187:10030";		
TThostFtdcBrokerIDType	BROKER_ID = "9999";				
TThostFtdcInvestorIDType INVESTOR_ID = "112585";			
TThostFtdcPasswordType  PASSWORD = "qq824073152";			
TThostFtdcInstrumentIDType INSTRUMENT_ID = "al1803";	

int LoginFlag = 0;
int iRequestID = 0;
char *OrderFilePath = new char[256];

void loadInfoFromFile(char *filePath)
{
	CIni ini;
	ini.OpenFile(filePath,"r");
	char *broId = ini.GetStr("Broker","ID");
	strcpy(BROKER_ID, broId);
	char *invId = ini.GetStr("Investor","ID");
	strcpy(INVESTOR_ID, invId);
	char *pwd = ini.GetStr("Investor","Password");
	strcpy(PASSWORD, pwd);
	char *insId = ini.GetStr("Instrument", "Ins");
	strcpy(INSTRUMENT_ID, insId);
}

int main(int argc, char *argv[])
{
	
	char ch;
	while((ch = getopt(argc, argv, "b:i:p:s:f:t:h")) != -1){
        switch(ch){
            case 'b':
            	strcpy(BROKER_ID, optarg);
                break;
            case 'i':
                strcpy(INVESTOR_ID, optarg); 
				break;
            case 'p':
                strcpy(PASSWORD, optarg);
				break;
			case 's':
				strcpy(INSTRUMENT_ID, optarg);
				break;
			case 'f':
				char *configFilePath;
				configFilePath = optarg;
				loadInfoFromFile(configFilePath);
				break;
            case 'h':
                cout << "-h help" << endl;
                cout << "-b broker id" << endl;
                cout << "-i investor id" << endl;
                cout << "-p password" << endl;
                cout << "-s instrument id" << endl;
                cout << "-f config.ini file" << endl;
                break;
            default:
                cout << "input -h to check usage of the code" << endl;
                break;
        }
    }

	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			
	CTraderSpi* pUserSpi = new CTraderSpi();
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			
	pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				
	pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				
	pUserApi->RegisterFront(FRONT_ADDR);							
	pUserApi->Init();

	char *order = new char[256];
	while(!LoginFlag){ }
	cout << "input your order: ";
	while(1){
		cin.getline(order, 256);
		if(strcmp(order, "login") == 0){
			while(!LoginFlag){ }
			pUserSpi->ReqUserLogin();
		}
		else if(strcmp(order, "queryInstrument") == 0){
			cout << "input the instrument id: ";
			cin.getline(INSTRUMENT_ID, 256);
			pUserSpi->ReqQryInstrument();
		}
		else if(strcmp(order, "queryAccount") == 0){
			pUserSpi->ReqQryTradingAccount();
		}
		else if(strcmp(order, "queryPosition") == 0){
			pUserSpi->ReqQryInvestorPosition();
		}
		else if(strcmp(order, "insertOrder") == 0){
			cout << "input order infomation file path: ";
			cin.getline(OrderFilePath, 256);
			cout << OrderFilePath << endl;
			pUserSpi->ReqOrderInsert();
		}
		else if(strcmp(order, "cancalOrder") == 0){
			cout << "input instrument ID: ";
			string instrumentTemp;
			getline(cin,instrumentTemp);
			cout << "input orderSysID: ";
			string orderTemp;
			getline(cin,orderTemp);
			while(orderTemp.size() < 12) { orderTemp = ' ' + orderTemp; }
			pUserSpi->ReqQryOrder((char*)instrumentTemp.c_str(), (char*)orderTemp.c_str());
		}
		else if(strcmp(order, "logout") == 0){
			LoginFlag = 0;
		}
		else{
			cout << "order error" << endl;
		}
	}
	delete[] order;

	pUserApi->Join();
//	pUserApi->Release();
	return 0;
}