#include "TraderSpi.h"
#include "../package/ThostFtdcTraderApi.h"
#include <iostream>
#include <cstring>

extern CThostFtdcTraderApi* pUserApi;

extern char FRONT_ADDR[];		
extern char BROKER_ID[];		
extern char INVESTOR_ID[];		
extern char PASSWORD[];			
extern char INSTRUMENT_ID[];	
extern TThostFtdcPriceType	LIMIT_PRICE;	
extern TThostFtdcDirectionType	DIRECTION;	

extern int iRequestID;

TThostFtdcFrontIDType	FRONT_ID;	
TThostFtdcSessionIDType	SESSION_ID;	
TThostFtdcOrderRefType	ORDER_REF;	
TThostFtdcOrderRefType	EXECORDER_REF;	
TThostFtdcOrderRefType	FORQUOTE_REF;	
TThostFtdcOrderRefType	QUOTE_REF;	

bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected(){
	cout << "====connect succeed====" << endl;
	ReqUserLogin();
}

void CTraderSpi::ReqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cout << "try login " << ((iResult == ?)"succeed":"default") <<endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if(bIsLast && !IsErrorRspInfo(pRspInfo)){
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		sprintf(EXECORDER_REF, "%d", 1);
		sprintf(FORQUOTE_REF, "%d", 1);
		sprintf(QUOTE_REF, "%d", 1);

		cout << "date: " << pRspUserLogin->TradingDay << endl;

		ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm(){
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cout << "SettlementInfoConfirm: " << ((iResult == 0) ? "succeed" : "default") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		ReqQryInstrument();
	}
}

void CTraderSpi::ReqQryInstrument(){
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true){
		int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			cout << "check request "  << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else{
			cout << "check request " << iResult << "is flow_controled" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryTradingAccount(){
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	while (true){
		int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			cout << "check account " << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else{
			cout << "check account "  << iResult << "is flow_controled" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		ReqQryInvestorPosition();
	}
}

void CTraderSpi::ReqQryInvestorPosition(){
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true){
		int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			cout << "check investor position "  <<((iResult == 0) ? "succeed" : "default") << endl;
			break;}
		else{
			cout << "check investor position "  << iResult << "is flow_controled" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		ReqOrderInsert();

		ReqExecOrderInsert();

		ReqForQuoteInsert();

		ReqQuoteInsert();
	}
}

