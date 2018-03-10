#include "ThostFtdcTraderApi.h"
#include "CTraderSpi.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include "Ini.h"

using namespace std;

int main(int argc, char *argv[])
{
	CTraderSpi *pUserSpi = new CTraderSpi();

	char ch;
	while((ch = getopt(argc, argv, "f:h")) != -1){
        switch(ch){
			case 'f':
				char *configFilePath;
				configFilePath = optarg;
				pUserSpi->SetConfigInfo(configFilePath);
				pUserSpi->Init();
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

	char *order = new char[256];
	while(1){
		cin.getline(order, 256);
		if(strcmp(order, "login") == 0){
			pUserSpi->ReqUserLogin();
		}
		else if(strcmp(order, "queryInstrument") == 0){
			cout << "input the instrument id: ";
			char *instrumentID = new char[256];
			cin.getline(instrumentID, 256);
			pUserSpi->ReqQryInstrument();
			delete[] instrumentID;
		}
		else if(strcmp(order, "queryAccount") == 0){
			pUserSpi->ReqQryTradingAccount();
		}
		else if(strcmp(order, "queryPosition") == 0){
			pUserSpi->ReqQryInvestorPosition();
		}
		else if(strcmp(order, "insertOrder") == 0){
			cout << "input order infomation file path: ";
			char *orderFilePath = new char[256];
			cin.getline(orderFilePath, 256);
			cout << orderFilePath << endl;
			pUserSpi->ReqOrderInsert(orderFilePath);
			delete[] orderFilePath;
		}
		else if(strcmp(order, "cancalOrder") == 0){
			cout << "input orderSysID: ";
			string orderTemp;
			getline(cin,orderTemp);
			while(orderTemp.size() < 12) { orderTemp = ' ' + orderTemp; }
			pUserSpi->ReqQryOrder((char*)orderTemp.c_str());
		}
		else if(strcmp(order, "logout") == 0){
			
		}
		else{
			cout << "order error" << endl;
		}
	}
	delete[] order;

	return 0;
}