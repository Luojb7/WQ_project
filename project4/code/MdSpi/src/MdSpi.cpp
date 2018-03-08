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

extern bool LoginFlag;
extern int iRequestID;
extern int dataNum;

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontConnected()
{
	cout << "--->>> connect succeed" << endl;
	LoginFlag = 1;
	//ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cout << "--->>> connect error for " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "--->>> nTimeLapse: " << nTimeLapse << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> login succeed!" << endl;
		cout << "Login date: " << pRspUserLogin->TradingDay << endl;
		cout << "FrontID: " << pRspUserLogin->FrontID << endl;
		cout << "SessionID: " << pRspUserLogin->SessionID << endl;
		//SubscribeMarketData();
		//SubscribeForQuoteRsp();
	}
	else
	{
		cout << "--->>> ErrorID: " << pRspInfo->ErrorID << endl;
		cout << "--->>> ErrorMsg: " << pRspInfo->ErrorMsg << endl;
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	//cout << "--->>> Subscribe market data: " << ((iResult == 0) ? "succeed" : "default") << endl;
}

void CMdSpi::SubscribeForQuoteRsp()
{
	int iResult = pUserApi->SubscribeForQuoteRsp(ppInstrumentID, iInstrumentID);
	//cout << "--->>> Subscribe for quote response: " << ((iResult == 0) ? "succeed" : "default") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Subscribe market data succeed" << endl;
		cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << endl;
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
	else
	{
		cout << "--->>> ErrorID: " << pRspInfo->ErrorID << endl;
		cout << "--->>> ErrorMsg: " << pRspInfo->ErrorMsg << endl;
	}
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Unsubscribe market data succeed" << endl;
		cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << endl;
	}
	else
	{
		cout << "--->>> ErrorID: " << pRspInfo->ErrorID << endl;
		cout << "--->>> ErrorMsg: " << pRspInfo->ErrorMsg << endl;
	}
}

void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Subscribe for quote response succeed" << endl;
		cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << endl;
	}
	else
	{
		cout << "--->>> ErrorID: " << pRspInfo->ErrorID << endl;
		cout << "--->>> ErrorMsg: " << pRspInfo->ErrorMsg << endl;
	}
}

void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Unsubscribe for quote response succeed" << endl;
		cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << endl;
	}
	else
	{
		cout << "--->>> ErrorID: " << pRspInfo->ErrorID << endl;
		cout << "--->>> ErrorMsg: " << pRspInfo->ErrorMsg << endl;
	}
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
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

void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	cout << "--->>> Quote response: " << endl;
	cout << "Date: " << pForQuoteRsp->TradingDay << endl;
	cout << "InstrumentID: " << pForQuoteRsp->InstrumentID << endl;
}

void CMdSpi::UnSubscribeMarketData()
{
	int iResult = pUserApi->UnSubscribeMarketData(ppInstrumentID, iInstrumentID);
}