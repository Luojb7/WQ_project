#include "MdSpi.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>

using namespace std;

extern CThostFtdcMdApi* pUserApi;

extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;

extern int iRequestID;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cerr << "--->>> "<< "OnRspError" << endl;
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
	cout<<"login"<<endl;
	if(bIsLast && !IsErrorRspInfo(pRspInfo)){
		cout << "date: " <<pRspUserLogin->TradingDay << endl;
		cout << "time: " <<pRspUserLogin->LoginTime << endl;
		cout << "BrokerID: " <<pRspUserLogin->BrokerID << endl;
		cout << "UserID: " <<pRspUserLogin->UserID << endl;

		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData(){
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cout << "iResult: " << iResult << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if(!IsErrorRspInfo(pRspInfo)){
		cout << "order market data succeed" << endl;
		cout << "iInstrumentID: " << pSpecificInstrument->InstrumentID << endl;
	}
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "balabala" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	cout << "depth market data" << endl;
	cout << "date: " << pDepthMarketData->TradingDay << endl;
	cout << "ExchangeID: " << pDepthMarketData->ExchangeID << endl;
	cout << "InstrumentID: " << pDepthMarketData->InstrumentID << endl;
	cout << "LastPrice: " << pDepthMarketData->LastPrice << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo){
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}