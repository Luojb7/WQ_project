#include <iostream>
#include "ThostFtdcMdApi.h"
#include "MdSpi.h"
#include "Ini.h"
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

CThostFtdcMdApi* pUserApi;

char FRONT_ADDR[] = "tcp://180.168.146.187:10031";	
TThostFtdcBrokerIDType	BROKER_ID ;				
TThostFtdcInvestorIDType INVESTOR_ID ;
TThostFtdcPasswordType  PASSWORD ;			
char *ppInstrumentID[10];
int iInstrumentID = 0;								

bool LoginFlag = 0;
int iRequestID = 0;
int dataNum = 0;

vector<string> split(const string &str,const string &pattern);

void InstrumentCopy(vector<string> myvector);

void loadInfoFromFile(char *filePath);

void loadInstrumentFromFile(char *filePath);


int main(int argc, char* argv[])
{
	char ch;
	string insTmp;
	string pattern = ",";
	vector<string> myInstrumentList;

	while((ch = getopt(argc, argv, "b:i:p:f:h")) != -1){
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
                cout << "-f config.ini file" << endl;
                break;
            default:
                cout << "input -h to check usage of the code" << endl;
                break;
        }
    }

	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			
	CMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						
	pUserApi->RegisterFront(FRONT_ADDR);					
	pUserApi->Init();

	char *order = new char[256];
	while(!LoginFlag){ }
	cout << "input your order: ";
	while(1){
		cin.getline(order,256);
		if(strcmp(order, "login") == 0){
			while(!LoginFlag){ }
			pUserSpi->ReqUserLogin();
		}
		else if(strcmp(order, "SMD") == 0){
			cout << "input the number of instrument list: ";
			cin >> iInstrumentID;
			cout << "input the instrument list divided with ',': ";
			cin >> insTmp;
			myInstrumentList.clear();
			myInstrumentList = split(insTmp, pattern);
			InstrumentCopy(myInstrumentList);
			pUserSpi->SubscribeMarketData();
		}
		else if(strcmp(order, "USMD") == 0){
			cout << "input the number of instrument list: ";
			cin >> iInstrumentID;
			cout << "input the instrument list divided with ',': ";
			cin >> insTmp;
			myInstrumentList.clear();
			myInstrumentList = split(insTmp, pattern);
			InstrumentCopy(myInstrumentList);
			pUserSpi->UnSubscribeMarketData();
		}
		else if(strcmp(order, "SMDFF") == 0){
			cout << "input file path: ";
			char *InstrumentFile = new char[256];
			cin.getline(InstrumentFile, 256);
			loadInstrumentFromFile(InstrumentFile);
			pUserSpi->SubscribeMarketData();
		}
		else if(strcmp(order, "SQ") == 0){

		}
		else if(strcmp(order, "USQ") == 0){
			
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
	return 0;
}

vector<string> split(const string &str,const string &pattern)
{
    //const char* convert to char*
    char * strc = new char[strlen(str.c_str())+1];
    strcpy(strc, str.c_str());
    vector<string> resultVec;
    char* tmpStr = strtok(strc, pattern.c_str());
    while (tmpStr != NULL)
    {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }

    delete[] strc;

    return resultVec;
}

void InstrumentCopy(vector<string> myvector)
{
	for (int i=0; i<iInstrumentID; i++){
		ppInstrumentID[i] = (char*)myvector[i].data();
	}
}

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
}

void loadInstrumentFromFile(char *filePath)
{
	CIni ini;
	ini.OpenFile(filePath, "r");
	iInstrumentID = ini.GetInt("InstrumentNum", "Num");
	char InstrumentCode[4] = {'I','n','s','1'};
	string temp[iInstrumentID];
	for(int i=0;i<iInstrumentID;i++)
	{
		InstrumentCode[3] = (char)(i+49);
		temp[i] = ini.GetStr("Instrument", InstrumentCode);
		ppInstrumentID[i] = (char*)temp[i].data();
	}
}