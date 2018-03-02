#include "ThostFtdcTraderApi.h"
#include <stdio.h>

class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	// used before connected
	virtual void OnFrontConnected();

	// response for login
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// response for settlement information confirm
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// query instrument
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// query trading account
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// query investor position
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// 
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	//virtual void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// 
	//virtual void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// 
	//virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	//virtual void OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	//virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	//
	virtual void OnFrontDisconnected(int nReason);
	
	//	
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	//
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	//
	//virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

	//
	//virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	//
	//virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);
	
	//
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

private:
	
	void ReqUserLogin();
	
	void ReqSettlementInfoConfirm();
	
	void ReqQryInstrument();
	
	void ReqQryTradingAccount();
	
	void ReqQryInvestorPosition();
	
	void ReqOrderInsert();
	
	//void ReqExecOrderInsert();
	
	//void ReqForQuoteInsert();
	
	//void ReqQuoteInsert();
	
	void ReqOrderAction(CThostFtdcOrderField *pOrder);
	
	//void ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder);
	
	//void ReqQuoteAction(CThostFtdcQuoteField *pQuote);

	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	
	//bool IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder);
	
	//bool IsMyQuote(CThostFtdcQuoteField *pQuote);
	
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);	
	
	//bool IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder);
	
	//bool IsTradingQuote(CThostFtdcQuoteField *pQuote);
};