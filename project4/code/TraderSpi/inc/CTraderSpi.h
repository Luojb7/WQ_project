#ifndef TRADERSPI_H_
#define TRADERSPI_H_

#include "ThostFtdcTraderApi.h"
#include <stdio.h>
#include <fstream>
#include <cstring>

class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	virtual void OnFrontConnected();
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
	void ReqUserLogin();
	void ReqSettlementInfoConfirm();
	void ReqQryInstrument();
	void ReqQryTradingAccount();
	void ReqQryInvestorPosition();
	void ReqOrderInsert(char *OrderFilePath);
	void ReqOrderAction(CThostFtdcOrderField *pOrder);
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);	
	void ReqQryOrder(char *instrumentId, char *orderSysId);
	void SetOrderType(char *OrderFilePath);
	void SetConfigInfo(char *configFilePath);
	//void Init();
	void Init(){
		if(pUserApi == nullptr)
		{
			pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
			pUserApi->RegisterSpi(this);
		}
		if(!isConnected)
		{
			pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				
			pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				
			pUserApi->RegisterFront(FRONT_ADDR);							
			pUserApi->Init();
		}
	}
	CTraderSpi();

	char* GetBrokerID(){return BROKER_ID;}
	char* GetInvstorID(){return INVESTOR_ID;}
	bool GetIsLogin(){return isLogin;}
	bool CompairInfo(CTraderSpi &spiTemp);

private:
	CThostFtdcTraderApi* pUserApi = nullptr;
	char FRONT_ADDR[256];
	TThostFtdcBrokerIDType	BROKER_ID = "";
	TThostFtdcInvestorIDType INVESTOR_ID = "";
	TThostFtdcPasswordType  PASSWORD = "";	
	TThostFtdcInstrumentIDType INSTRUMENT_ID = "";
	bool isConnected = 0;
	bool isLogin = 0;
	int iRequestID = 0;

	TThostFtdcFrontIDType	FRONT_ID;
	TThostFtdcSessionIDType	SESSION_ID;	 // session id
	TThostFtdcOrderRefType	ORDER_REF;	 // order reference

	CThostFtdcInputOrderField InsertOrder;

};

#endif