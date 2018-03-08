#include "ThostFtdcTraderApi.h"
#include <stdio.h>
#include <fstream>

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
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	//
	virtual void OnFrontDisconnected(int nReason);
	
	//	
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	//
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	
	//
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);


	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
	
	void ReqUserLogin();
	
	void ReqSettlementInfoConfirm();
	
	void ReqQryInstrument();
	
	void ReqQryTradingAccount();
	
	void ReqQryInvestorPosition();
	
	void ReqOrderInsert();
	
	void ReqOrderAction(CThostFtdcOrderField *pOrder);

	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);	

	void ReqQryOrder(char *instrumentId, char *orderSysId);

	void SetOrderType();

};