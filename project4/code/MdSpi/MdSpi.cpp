#include "MdSpi.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

extern CThostFtdcMdApi* pUserApi;

extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;

extern int iRequestID;
extern int dataNum;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontConnected(){
	cout << "====connect succeed====" << endl;
	ReqUserLogin();
}

void CMdSpi::OnFrontDisconnected(int nReason){
	cout << "connect default for " << nReason<<endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse){
	cout << "nTimeLapse: " << nTimeLapse << endl;
}

void CMdSpi::ReqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cout << "try login " << ((iResult == 0) ? "succeed" : "default") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if(bIsLast && !IsErrorRspInfo(pRspInfo)){
		cout << "====login succeed!====" << endl;
		cout << "date: " << pRspUserLogin->TradingDay << endl;
		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData(){
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cout << "iResult: " << iResult << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if(!IsErrorRspInfo(pRspInfo)){
		cout << "====order market data succeed====" << endl;
		cout << "iInstrumentID: " << pSpecificInstrument->InstrumentID << endl;
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		ofstream outFile;
		outFile.open(filePath, ios::out);
		outFile << "InstrumentID" << ","
				<< "UpdateTime" << "," //gengxinshijian
				<< "LastPrice" << "," //zuixinjia
				<< "PreSettlementPrice" << "," //shangci jiesuanjia
				<< "PreClosePrice" << ","  //zuo shoupan
				<< "PreOpenInterest" << "," //zuo chicangliang
				<< "OpenPrice" << "," //jin kaipan
				<< "HighestPrice" << "," //zuigaojia
				<< "LowestPrice" << "," //zuidijia
				<< "Volume" << "," //shuliang
				<< "Turnover" << "," //chengjiao jin'er
				<< "OpenInterest" << "," //chicangliang
				<< "ClosePrice" << "," //jin shoupan
				<< "SettlementPrice" << endl; //benci jiesuanjia 
	}
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "balabala" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	dataNum++;
	cout << dataNum << endl;
	/*cout << "====depth market data====" << endl;
	cout << "date: " << pDepthMarketData->TradingDay << endl;
	cout << "InstrumentID: " << pDepthMarketData->InstrumentID << endl;
	cout << "LastPrice: " << pDepthMarketData->LastPrice << endl;
	cout << "Volume: " << pDepthMarketData->Volume << endl;
	cout << "OpenInterest: " << pDepthMarketData->OpenInterest << endl;
	cout << "ClosePrice: " << pDepthMarketData->ClosePrice << endl;
	cout << "UpdateTime: " << pDepthMarketData->UpdateTime << endl;*/
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	ofstream outFile;
	outFile.open(filePath, ios::app);
	outFile << pDepthMarketData->InstrumentID << ","
			<< pDepthMarketData->UpdateTime << ","
			<< pDepthMarketData->LastPrice << ","
			<< pDepthMarketData->PreSettlementPrice << ","
			<< pDepthMarketData->PreClosePrice << ","
			<< pDepthMarketData->PreOpenInterest << ","
			<< pDepthMarketData->OpenPrice << ","
			<< pDepthMarketData->HighestPrice << ","
			<< pDepthMarketData->LowestPrice << ","
			<< pDepthMarketData->Volume << ","
			<< pDepthMarketData->Turnover << ","
			<< pDepthMarketData->OpenInterest << ","
			<< pDepthMarketData->ClosePrice << ","
			<< pDepthMarketData->SettlementPrice << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo){
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}