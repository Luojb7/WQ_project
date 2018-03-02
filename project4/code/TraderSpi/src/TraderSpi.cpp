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

// 
void CTraderSpi::OnFrontConnected()
{
	cout << "--->>> connect succeed" << endl;
	ReqUserLogin();
}

// request for login
void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
}

// responce for login
void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		sprintf(EXECORDER_REF, "%d", 1);
		sprintf(FORQUOTE_REF, "%d", 1);
		sprintf(QUOTE_REF, "%d", 1);

		cout << "--->>> Login succeed" << endl;
		cout << "Date: " << pRspUserLogin->TradingDay << endl;
		cout << "LoginTime: " << pRspUserLogin->LoginTime << endl;
		cout << "userID: " << pRspUserLogin->UserID << endl;

		ReqSettlementInfoConfirm();
	}
	else
	{
		cout << "--->>> Login default" << endl;
	}
}

// request for confirming Settlement
void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
}

// reponce for confirming Settlement
void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Settlement Information Confirm succeed" << endl;
		cout << "Confirm date: " << pSettlementInfoConfirm->ConfirmDate << endl;
		cout << "Confirm time: " << pSettlementInfoConfirm->ConfirmTime << endl;

		ReqQryInstrument();
	}
	else
	{
		cout << "--->>> Settlement Information Confirm default" << endl;
	}
}

// request for querying instrument
void CTraderSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			//cout << "Query instrument "  << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else{
			cout << "--->>> Query instrument iResult: " << iResult << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying instrument
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Query instrument succeed" << endl;
		cout << "InstrumentID: " << pInstrument->InstrumentID << endl;
		cout << "DeliveryYear: " << pInstrument->DeliveryYear << endl;
		cout << "DeliveryMonth: " << pInstrument->DeliveryMonth << endl;
		cout << "IsTrading: " << pInstrument->IsTrading << endl;
		cout << "PriceTick: " << pInstrument->PriceTick << endl;

		ReqQryTradingAccount();
	}
	else
	{
		cout << "--->>> Query instrument default" << endl;
	}
}

// request for querying trading account
void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	while (true){
		int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			//cout << "Query account " << ((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else
		{
			cout << "--->>> Query account iResult: " << iResult << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying trading account
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Query account succeed" << endl;
		cout << "AccountID: " << pTradingAccount->AccountID << endl;
		cout << "Available money: " << pTradingAccount->Available << endl;
		ReqQryInvestorPosition();
	}
	else
	{
		cout << "--->>> Query account default" << endl;
	}
}

// request for querying investor position
void CTraderSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true){
		int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			//cout << "querying investor position "  <<((iResult == 0) ? "succeed" : "default") << endl;
			break;
		}
		else
		{
			cout << "--->>> Query investor position iResult " << iResult << endl;
			usleep(1000);
		}
	} // while
}

// reponce for querying investor position
void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

		cout << "--->>> Query investor position succeed" << endl;

		if(pInvestorPosition)
		{
			cout << "InstrumentID: " << pInvestorPosition->InstrumentID << endl;
			cout << "OpenAmount: " << pInvestorPosition->OpenAmount << endl;
			cout << "OpenVolume: " << pInvestorPosition->OpenVolume << endl;
		}
		else
		{
			cout << "No position" << endl;
		}

		ReqOrderInsert(); 

		//ReqExecOrderInsert();

		//ReqForQuoteInsert();

		//ReqQuoteInsert();
	}
	else
	{
		cout << "--->>> Query investor position default" << endl;
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

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  // 2

	req.Direction = DIRECTION;

	req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; 

	req.LimitPrice = LIMIT_PRICE;

	req.VolumeTotalOriginal = 5;

	req.TimeCondition = THOST_FTDC_TC_GFD;

//	TThostFtdcDateType	GTDDate;

	req.VolumeCondition = THOST_FTDC_VC_AV;

	req.MinVolume = 1;

	req.ContingentCondition = THOST_FTDC_CC_Immediately; // 1

//	TThostFtdcPriceType	StopPrice;

	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

	req.IsAutoSuspend = 0;

//	TThostFtdcBusinessUnitType	BusinessUnit;

//	TThostFtdcRequestIDType	RequestID;

	req.UserForceClose = 0;

	int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
	
	cout << "--->>> ReqOrderInsert iResult: " << iResult << endl;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Order insert succeed" << endl;
		cout << "InstrumentID: " << pInputOrder->InstrumentID << endl;
		cout << "LimitPrice: " << pInputOrder->LimitPrice << endl;
		cout << "VolumeTotalOriginal: " << pInputOrder->VolumeTotalOriginal << endl;
		cout << "Direction: " << pInputOrder->Direction << endl;
	}
	else
	{
		cout << "--->>> Order insert default" << endl;
	}
}
/*
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
}

void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> request for inserting execorder default" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> request for inserting forquote default" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> request for inserting quote default" << endl;
	IsErrorRspInfo(pRspInfo);
}
*/
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

	cout << "--->>> ReqOrderAction iResult: " << iResult << endl;

	ORDER_ACTION_SENT = true;
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Order action succeed" << endl;
		cout << "InstrumentID: " << pInputOrderAction->InstrumentID << endl;
		cout << "ActionFlag: " << pInputOrderAction->ActionFlag <<endl;
	}
	else
	{
		cout << "--->>> Order action default" << endl;
	}
}

/********************/
/*
void CTraderSpi::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
	static bool EXECORDER_ACTION_SENT = false;		
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

	EXECORDER_ACTION_SENT = true;
}

void CTraderSpi::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
	static bool QUOTE_ACTION_SENT = false;		
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

	QUOTE_ACTION_SENT = true;
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
*/
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	cout << "--->>> Receive order response"  << endl;
	if (IsMyOrder(pOrder))
	{
		cout << "direction: " << pOrder->Direction << endl;
		if (IsTradingOrder(pOrder))
		{
			//ReqOrderAction(pOrder);
			cout << "--->>> Waiting for trading" << endl;
		}
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			cout << "--->>> Revoke order succeed" << endl;
	}
}
/*
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	cout << "--->>> " << "OnRtnExecOrder"  << endl;
	if (IsMyExecOrder(pExecOrder))
	{
		if (IsTradingExecOrder(pExecOrder))
			ReqExecOrderAction(pExecOrder);
		else if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
			cout << "Revoke order succeed" << endl;
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
*/
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	cout << "--->>> Trade succeed" << endl;
	cout << "Time: " << pTrade->TradeTime << endl;
	cout << "InstrumentID: " << pTrade->InstrumentID << endl;
	cout << "Price: " << pTrade->Price << endl;
	cout << "Volume: " << pTrade->Volume << endl;
	cout << "Direction: " << pTrade->Direction << endl;
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
/*
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
*/
bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}
/*
bool CTraderSpi::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool CTraderSpi::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
	return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}
*/