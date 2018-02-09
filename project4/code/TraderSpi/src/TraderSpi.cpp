#include "TraderSpi.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

extern CThostFtdcTraderApi* pUserApi;

extern char FRONT_ADDR[];		// front address
extern char BROKER_ID[];		// borker id
extern char INVESTOR_ID[];		// investor id
extern char PASSWORD[];			// investor password
extern char INSTRUMENT_ID[];	// instrument id
extern TThostFtdcPriceType	LIMIT_PRICE;	// limit price
extern TThostFtdcDirectionType	DIRECTION;	// direction

extern int iRequestID;

TThostFtdcFrontIDType	FRONT_ID;	
TThostFtdcSessionIDType	SESSION_ID;	 // session id
TThostFtdcOrderRefType	ORDER_REF;	 // order reference
TThostFtdcOrderRefType	EXECORDER_REF;	 // execorder reference
TThostFtdcOrderRefType	FORQUOTE_REF;	 // forquote reference
TThostFtdcOrderRefType	QUOTE_REF;	 // quote reference

// 
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

// called when connect succeed
void CTraderSpi::OnFrontConnected()
{
	cout << "====connect succeed====" << endl;
	ReqUserLogin();
}

// request for login
void CTraderSpi::ReqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
}

// responce for login
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

		cout << "====Login succeed====" << endl;
		cout << "Date: " << pRspUserLogin->TradingDay << endl;
		cout << "LoginTime: " << pRspUserLogin->LoginTime << endl;
		cout << "user: " << pRspUserLogin->UserID << endl;

		ReqSettlementInfoConfirm();
	}
	else{
		cout << "====Login default====" << endl;
	}
}

// request for confirming Settlement
void CTraderSpi::ReqSettlementInfoConfirm(){
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
}

// reponce for confirming Settlement
void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		cout << "====Settlement Information Confirm succeed====" << endl;
		ReqQryInstrument();
	}
	else{
		cout << "====Settlement Information Confirm default====" << endl;
	}
}

// request for querying instrument
void CTraderSpi::ReqQryInstrument(){
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true){
		int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
		cout << "ReqQryInstrument iResult: " << iResult << endl;
		if (!IsFlowControl(iResult)){
			//cout << "querying instrument "  << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else{
			cout << "querying instrument " << iResult << "is flow_controled" << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying instrument
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		cout << "====querying instrument succeed====" << endl;
		cout << "InstrumentID: " << pInstrument->InstrumentID << endl;
		cout << "DeliveryYear: " << pInstrument->DeliveryYear << endl;
		cout << "DeliveryMonth: " << pInstrument->DeliveryMonth << endl;
		cout << "IsTrading: " << pInstrument->IsTrading << endl;
		cout << "PriceTick: " << pInstrument->PriceTick << endl;

		ReqQryTradingAccount();
	}
	else{
		cout << "====querying instrument default====" << endl;
	}
}

// request for querying trading account
void CTraderSpi::ReqQryTradingAccount(){
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	while (true){
		int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		cout << "ReqQryTradingAccount iResult: " << iResult << endl;
		if (!IsFlowControl(iResult)){
			cout << "querying account " << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else{
			cout << "querying account "  << iResult << "is flow_controled" << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying trading account
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		ReqQryInvestorPosition();
	}
}

// request for querying investor position
void CTraderSpi::ReqQryInvestorPosition(){
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true){
		int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		cout << "ReqQryInvestorPosition iResult: " << iResult << endl;
		if (!IsFlowControl(iResult)){
			cout << "querying investor position "  <<((iResult == 0) ? "succeed" : "default") << endl;
			break;}
		else{
			cout << "querying investor position "  << iResult << "is flow_controled" << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying investor position
void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		//ReqOrderInsert(); 

		//ReqExecOrderInsert();

		//ReqForQuoteInsert();

		//ReqQuoteInsert();
	}
}

// request for inserting order
void CTraderSpi::ReqOrderInsert()
{
	CThostFtdcInputOrderField req;

	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);

	strcpy(req.InvestorID, INVESTOR_ID);

	strcpy(req.InstrumentID, INSTRUMENT_ID);

	strcpy(req.OrderRef, ORDER_REF);

//	TThostFtdcUserIDType	UserID;

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;

	req.Direction = DIRECTION;

	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;

	req.LimitPrice = LIMIT_PRICE;

	req.VolumeTotalOriginal = 1;

	req.TimeCondition = THOST_FTDC_TC_GFD;

//	TThostFtdcDateType	GTDDate;

	req.VolumeCondition = THOST_FTDC_VC_AV;

	req.MinVolume = 1;

	req.ContingentCondition = THOST_FTDC_CC_Immediately;

//	TThostFtdcPriceType	StopPrice;

	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

	req.IsAutoSuspend = 0;

//	TThostFtdcBusinessUnitType	BusinessUnit;

//	TThostFtdcRequestIDType	RequestID;

	req.UserForceClose = 0;

	int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
	cout << "order insert " << ((iResult == 0) ? "succeed" : "default") << endl;
}

// request for inserting execorder
void CTraderSpi::ReqExecOrderInsert()
{
	CThostFtdcInputExecOrderField req;

	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);

	strcpy(req.InvestorID, INVESTOR_ID);

	strcpy(req.InstrumentID, INSTRUMENT_ID);

	strcpy(req.ExecOrderRef, EXECORDER_REF);

	//	TThostFtdcUserIDType	UserID;

	req.Volume=1;

	//TThostFtdcRequestIDType	RequestID;

	//TThostFtdcBusinessUnitType	BusinessUnit;

	req.OffsetFlag=THOST_FTDC_OF_Close;

	req.HedgeFlag=THOST_FTDC_HF_Speculation;

	req.ActionType=THOST_FTDC_ACTP_Exec;

	req.PosiDirection=THOST_FTDC_PD_Long;

	req.ReservePositionFlag=THOST_FTDC_EOPF_UnReserve;

	req.CloseFlag=THOST_FTDC_EOCF_AutoClose;

	int iResult = pUserApi->ReqExecOrderInsert(&req, ++iRequestID);
	cout << "execorder insert " << ((iResult == 0) ? "succeed" : "default") << endl;
}

// request for inserting forquote
void CTraderSpi::ReqForQuoteInsert()
{
	CThostFtdcInputForQuoteField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);

	strcpy(req.InvestorID, INVESTOR_ID);

	strcpy(req.InstrumentID, INSTRUMENT_ID);

	strcpy(req.ForQuoteRef, EXECORDER_REF);

	//	TThostFtdcUserIDType	UserID;

	int iResult = pUserApi->ReqForQuoteInsert(&req, ++iRequestID);
	cout << "forquote insert " << ((iResult == 0) ? "succeed" : "default") << endl;
}

// request for inserting order
void CTraderSpi::ReqQuoteInsert()
{
	CThostFtdcInputQuoteField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);

	strcpy(req.InvestorID, INVESTOR_ID);

	strcpy(req.InstrumentID, INSTRUMENT_ID);

	strcpy(req.QuoteRef, QUOTE_REF);

	req.AskPrice=LIMIT_PRICE;

	req.BidPrice=LIMIT_PRICE-1.0;

	req.AskVolume=1;

	req.BidVolume=1;

	//TThostFtdcRequestIDType	RequestID;

	//TThostFtdcBusinessUnitType	BusinessUnit;

	req.AskOffsetFlag=THOST_FTDC_OF_Open;

	req.BidOffsetFlag=THOST_FTDC_OF_Open;

	req.AskHedgeFlag=THOST_FTDC_HF_Speculation;

	req.BidHedgeFlag=THOST_FTDC_HF_Speculation;
	
	int iResult = pUserApi->ReqQuoteInsert(&req, ++iRequestID);
	cout << "quote insert " << ((iResult == 0) ? "succeed" : "default") << endl;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "====request for inserting order default====" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "====request for inserting execorder default====" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "====request for inserting forquote default====" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "====request for inserting quote default====" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, pOrder->BrokerID);

	strcpy(req.InvestorID, pOrder->InvestorID);

//	TThostFtdcOrderActionRefType	OrderActionRef;

	strcpy(req.OrderRef, pOrder->OrderRef);

//	TThostFtdcRequestIDType	RequestID;

	req.FrontID = FRONT_ID;

	req.SessionID = SESSION_ID;

//	TThostFtdcExchangeIDType	ExchangeID;

//	TThostFtdcOrderSysIDType	OrderSysID;

	req.ActionFlag = THOST_FTDC_AF_Delete;

//	TThostFtdcPriceType	LimitPrice;

//	TThostFtdcVolumeType	VolumeChange;

//	TThostFtdcUserIDType	UserID;

	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pUserApi->ReqOrderAction(&req, ++iRequestID);
	cout << "order action " << ((iResult == 0) ? "succeed" : "default") << endl;
	ORDER_ACTION_SENT = true;
}

/********************/

void CTraderSpi::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
	static bool EXECORDER_ACTION_SENT = false;		//ÊÇ·ñ·¢ËÍÁË±¨µ¥
	if (EXECORDER_ACTION_SENT)
		return;

	CThostFtdcInputExecOrderActionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID,pExecOrder->BrokerID);

	strcpy(req.InvestorID,pExecOrder->InvestorID);

	//TThostFtdcOrderActionRefType	ExecOrderActionRef;

	strcpy(req.ExecOrderRef,pExecOrder->ExecOrderRef);

	//TThostFtdcRequestIDType	RequestID;

	req.FrontID=FRONT_ID;

	req.SessionID=SESSION_ID;

	//TThostFtdcExchangeIDType	ExchangeID;

	//TThostFtdcExecOrderSysIDType	ExecOrderSysID;

	req.ActionFlag=THOST_FTDC_AF_Delete;

	//TThostFtdcUserIDType	UserID;

	strcpy(req.InstrumentID,pExecOrder->InstrumentID);

	int iResult = pUserApi->ReqExecOrderAction(&req, ++iRequestID);
	cout << "execorder action " << ((iResult == 0) ? "succeed" : "default") << endl;
	EXECORDER_ACTION_SENT = true;
}

void CTraderSpi::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
	static bool QUOTE_ACTION_SENT = false;		//ÊÇ·ñ·¢ËÍÁË±¨µ¥
	if (QUOTE_ACTION_SENT)
		return;

	CThostFtdcInputQuoteActionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, pQuote->BrokerID);

	strcpy(req.InvestorID, pQuote->InvestorID);

	//TThostFtdcOrderActionRefType	QuoteActionRef;

	strcpy(req.QuoteRef,pQuote->QuoteRef);

	//TThostFtdcRequestIDType	RequestID;

	req.FrontID=FRONT_ID;

	req.SessionID=SESSION_ID;

	//TThostFtdcExchangeIDType	ExchangeID;

	//TThostFtdcOrderSysIDType	QuoteSysID;

	req.ActionFlag=THOST_FTDC_AF_Delete;

	//TThostFtdcUserIDType	UserID;

	strcpy(req.InstrumentID,pQuote->InstrumentID);

	int iResult = pUserApi->ReqQuoteAction(&req, ++iRequestID);
	cout << "quote action " << ((iResult == 0) ? "succeed" : "default") << endl;
	QUOTE_ACTION_SENT = true;
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInpuExectOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspExecOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInpuQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspQuoteAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	cout << "--->>> " << "OnRtnOrder"  << endl;
	if (IsMyOrder(pOrder))
	{
		if (IsTradingOrder(pOrder))
			ReqOrderAction(pOrder);
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			cout << "papapa" << endl;
	}
}

void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	cout << "--->>> " << "OnRtnExecOrder"  << endl;
	if (IsMyExecOrder(pExecOrder))
	{
		if (IsTradingExecOrder(pExecOrder))
			ReqExecOrderAction(pExecOrder);
		else if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
			cout << "papapa" << endl;
	}
}

void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	cout << "--->>> " << "OnRtnForQuoteRsp"  << endl;
}

void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	cout << "--->>> " << "OnRtnQuote"  << endl;
	if (IsMyQuote(pQuote))
	{
		if (IsTradingQuote(pQuote))
			ReqQuoteAction(pQuote);
		else if (pQuote->QuoteStatus == THOST_FTDC_OST_Canceled)
			cout << "papapa" << endl;
	}
}

void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	cout << "--->>> " << "OnRtnTrade"  << endl;
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cout << "--->>> " << "OnFrontDisconnected" << endl;
	cout << "--->>> Reason = " << nReason << endl;
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "--->>> " << "OnHeartBeatWarning" << endl;
	cout << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTraderSpi::IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return ((pExecOrder->FrontID == FRONT_ID) &&
		(pExecOrder->SessionID == SESSION_ID) &&
		(strcmp(pExecOrder->ExecOrderRef, EXECORDER_REF) == 0));
}

bool CTraderSpi::IsMyQuote(CThostFtdcQuoteField *pQuote)
{
	return ((pQuote->FrontID == FRONT_ID) &&
		(pQuote->SessionID == SESSION_ID) &&
		(strcmp(pQuote->QuoteRef, QUOTE_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool CTraderSpi::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool CTraderSpi::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
	return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}