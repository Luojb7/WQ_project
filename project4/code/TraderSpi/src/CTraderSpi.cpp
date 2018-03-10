#include "CTraderSpi.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <Ini.h>

using namespace std;

CTraderSpi::CTraderSpi()
{
	pUserApi = nullptr;
	strcpy(FRONT_ADDR, "");
	strcpy(BROKER_ID, "");
	strcpy(INVESTOR_ID, "");
	strcpy(PASSWORD, "");
	strcpy(INSTRUMENT_ID, "");
	isConnected = 0;
	isLogin = 0;
	iRequestID = 0;
}

void CTraderSpi::SetConfigInfo(char *configFilePath)
{
	CIni ini;
	ini.OpenFile(configFilePath,"r");
	char *broId = ini.GetStr("Broker","ID");
	strcpy(BROKER_ID, broId);
	char *invId = ini.GetStr("Investor","ID");
	strcpy(INVESTOR_ID, invId);
	char *pwd = ini.GetStr("Investor","Password");
	strcpy(PASSWORD, pwd);
	char *insId = ini.GetStr("Instrument", "Ins");
	strcpy(INSTRUMENT_ID, insId);
	char *frontAddr = ini.GetStr("FrontAddr", "Addr");
	strcpy(FRONT_ADDR, frontAddr);
}

/*void CTraderSpi::Init()
{
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
}*/

bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected()
{
	cout << "--->>> connect succeed" << endl;
	isConnected = 1;
}

void CTraderSpi::ReqUserLogin()
{
	while(!isConnected) { }
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		isLogin = 1;
		cout << "--->>> Login succeed" << endl;
		cout << "Date: " << pRspUserLogin->TradingDay << endl;
		cout << "LoginTime: " << pRspUserLogin->LoginTime << endl;
		cout << "BrokerID: " << pRspUserLogin->BrokerID << endl;
		cout << "userID: " << pRspUserLogin->UserID << endl;
		cout << "SystemName: " << pRspUserLogin->SystemName << endl;
		cout << "FrontID: " << pRspUserLogin->FrontID << endl;
		cout << "SessionID: " << pRspUserLogin->SessionID << endl;
		cout << "MaxOrderRef: " << pRspUserLogin->MaxOrderRef << endl << endl;

		ReqSettlementInfoConfirm();

		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_order.csv", pRspUserLogin->UserID);
		ofstream outFile;
		outFile.open(filePath, ios::out);
		outFile << "OrderSysID" << ","
				<< "InstrumentID" << "," 
				<< "LimitPrice" << "," 
				<< "Volume" << "," 
				<< "OrderStatus" << "," << endl; 
	}
	else
	{
		cout << "--->>> Login default" << endl;
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Settlement Information Confirm succeed" << endl;
		cout << "Confirm date: " << pSettlementInfoConfirm->ConfirmDate << endl;
		cout << "Confirm time: " << pSettlementInfoConfirm->ConfirmTime << endl << endl;
	}
	else
	{
		cout << "--->>> Settlement Information Confirm default" << endl;
	}
}

void CTraderSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			break;
		}
		else{
			cout << "--->>> Query instrument iResult: " << iResult << endl;
			usleep(10000);
		}
	} // while
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Query instrument succeed" << endl;
		cout << "InstrumentID: " << pInstrument->InstrumentID << endl;
		cout << "DeliveryYear: " << pInstrument->DeliveryYear << endl;
		cout << "DeliveryMonth: " << pInstrument->DeliveryMonth << endl;
		cout << "MaxMarketOrderVolume: " << pInstrument->MaxMarketOrderVolume << endl;
		cout << "MinMarketOrderVolume: " << pInstrument->MinMarketOrderVolume << endl;
		cout << "MaxLimitOrderVolume: " << pInstrument->MaxLimitOrderVolume << endl;
		cout << "MinLimitOrderVolume: " << pInstrument->MinLimitOrderVolume << endl;
		cout << "PriceTick: " << pInstrument->PriceTick << endl;
		cout << "IsTrading: " << pInstrument->IsTrading << endl;
		cout << "ExpireDate: " << pInstrument->ExpireDate << endl;
		cout << "InstLifePhase: " << pInstrument->InstLifePhase << endl;
		cout << "CombinationType: " << pInstrument->CombinationType << endl;
		cout << "MinBuyVolume: " << pInstrument->MinBuyVolume << endl;
		cout << "MinSellVolume: " << pInstrument->MinSellVolume << endl << endl;
	}
	else
	{
		cout << "--->>> Query instrument default" << endl;
	}
}

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
			break;
		}
		else
		{
			cout << "--->>> Query account iResult: " << iResult << endl;
			usleep(10000);
		}
	} // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Query account succeed" << endl;
		cout << "AccountID: " << pTradingAccount->AccountID << endl;
		cout << "CurrMargin: " << pTradingAccount->CurrMargin << endl;
		cout << "CashIn: " << pTradingAccount->CashIn << endl;
		cout << "Commission: " << pTradingAccount->Commission << endl;
		cout << "CloseProfit: " << pTradingAccount->CloseProfit << endl;
		cout << "PositionProfit: " << pTradingAccount->PositionProfit << endl;
		cout << "Available: " << pTradingAccount->Available << endl;
		cout << "WithdrawQuota: " << pTradingAccount->WithdrawQuota << endl << endl;
	}
	else
	{
		cout << "--->>> Query account default" << endl;
	}
}

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
			break;
		}
		else
		{
			cout << "--->>> Query investor position iResult " << iResult << endl;
			usleep(10000);
		}
	} // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

		cout << "--->>> Query investor position succeed" << endl;

		if(pInvestorPosition)
		{
			cout << "InstrumentID: " << pInvestorPosition->InstrumentID << endl;
			cout << "InvestorID: " << pInvestorPosition->InvestorID << endl;
			cout << "YdPosition: " << pInvestorPosition->YdPosition << endl;
			cout << "Position: " << pInvestorPosition->Position << endl;
			cout << "OpenVolume: " << pInvestorPosition->OpenVolume << endl;
			cout << "OpenAmount: " << pInvestorPosition->OpenAmount << endl;
			cout << "CloseVolume: " << pInvestorPosition->CloseVolume << endl;
			cout << "CloseAmount: " << pInvestorPosition->CloseAmount << endl;
			cout << "PositionCost: " << pInvestorPosition->PositionCost << endl;
			cout << "UseMargin: " << pInvestorPosition->UseMargin << endl;
			cout << "PreSettlementPrice: " << pInvestorPosition->PreSettlementPrice << endl;
			cout << "SettlementPrice: " << pInvestorPosition->SettlementPrice << endl << endl;		}
		else
		{
			cout << "No position" << endl;
		}
	}
	else
	{
		cout << "--->>> Query investor position default" << endl;
	}
}

void CTraderSpi::ReqOrderInsert(char *OrderFilePath)
{
	int iNextOrderRef = atoi(ORDER_REF);
	iNextOrderRef++;
	sprintf(ORDER_REF, "%d", iNextOrderRef);

	SetOrderType(OrderFilePath);

	int iResult = pUserApi->ReqOrderInsert(&InsertOrder, ++iRequestID);
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

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, pOrder->BrokerID);

	strcpy(req.InvestorID, pOrder->InvestorID);

//	TThostFtdcOrderActionRefType	OrderActionRef;

	strcpy(req.OrderRef, pOrder->OrderRef);

//	TThostFtdcRequestIDType	RequestID;

	req.FrontID = pOrder->FrontID;
	// req.FrontID = FRONT_ID;

	req.SessionID = pOrder->SessionID;
	// req.SessionID = SESSION_ID;

//	TThostFtdcExchangeIDType	ExchangeID;

//	TThostFtdcOrderSysIDType	OrderSysID;

	req.ActionFlag = THOST_FTDC_AF_Delete;

//	TThostFtdcPriceType	LimitPrice;

//	TThostFtdcVolumeType	VolumeChange;

//	TThostFtdcUserIDType	UserID;

	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pUserApi->ReqOrderAction(&req, ++iRequestID);
	//cout << iResult << endl;

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

void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	cout << "--->>> Receive order response"  << endl;
	if (IsMyOrder(pOrder))
	{
		cout << "InstruentID: " << pOrder->InstrumentID << endl;
		cout << "OrderSysID: " << pOrder->OrderSysID << endl;
		cout << "OrderStatus: " << pOrder->OrderStatus << endl << endl;
		if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled){
			cout << "--->>> Revoke order succeed" << endl << endl;
		}
		if(pOrder->OrderSysID){
			char filePath[100] = {'\0'};
			sprintf(filePath, "%s_order.csv", pOrder->InvestorID);
			ofstream outFile;
			outFile.open(filePath, ios::app);
			outFile << pOrder->OrderSysID << ","
					<< pOrder->InstrumentID << ","
					<< pOrder->LimitPrice << ","
					<< pOrder->VolumeTotalOriginal << ","
					<< pOrder->OrderStatus << endl;
		}
	}
	else {
		cout << endl;
	}
}

void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	cout << "--->>> Trade succeed" << endl;
	cout << "Time: " << pTrade->TradeTime << endl;
	cout << "InstrumentID: " << pTrade->InstrumentID << endl;
	cout << "Price: " << pTrade->Price << endl;
	cout << "Volume: " << pTrade->Volume << endl;
	cout << "OffsetFlag: " << pTrade->OffsetFlag << endl;
	cout << "HedgeFlag: " << pTrade->HedgeFlag << endl;
	cout << "Direction: " << pTrade->Direction << endl;
	cout << "TradeType: " << pTrade->TradeType << endl;
	cout << "SettlementID: " << pTrade->SettlementID << endl << endl;
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
			(pOrder->SessionID == SESSION_ID));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


void CTraderSpi::ReqQryOrder(char *instrumentId, char *orderSysId)
{
	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, instrumentId);
	strcpy(req.OrderSysID, orderSysId);
	
	while (true)
	{
		int iResult = pUserApi->ReqQryOrder(&req, ++iRequestID);
		if (!IsFlowControl(iResult)){
			break;
		}
		else{
			cout << "--->>> Query instrument iResult: " << iResult << endl;
			usleep(10000);
		}
	} // while
}

void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		cout << "--->>> Query order succeed" << endl;
		cout << "InstrumentID: " << pOrder->InstrumentID << endl;
		cout << "LimitPrice: " << pOrder->LimitPrice << endl;
		cout << "OrderStatus: " << pOrder->OrderStatus << endl << endl;
		
		ReqOrderAction(pOrder);
	}
	else
	{
		cout << "--->>> Query order default" << endl;
	}
}

void CTraderSpi::SetOrderType(char *OrderFilePath)
{
	CIni ini;
	ini.OpenFile(OrderFilePath,"r");

	memset(&InsertOrder, 0, sizeof(InsertOrder));

	int OrderType = ini.GetInt("OrderType", "ot");

	strcpy(InsertOrder.BrokerID, BROKER_ID);

	strcpy(InsertOrder.InvestorID, INVESTOR_ID);

	strcpy(InsertOrder.OrderRef, ORDER_REF);

//	TThostFtdcUserIDType	UserID;

	switch(OrderType){
		case 1:
			InsertOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			InsertOrder.TimeCondition = THOST_FTDC_TC_GFD;
			InsertOrder.VolumeCondition = THOST_FTDC_VC_AV;
			InsertOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 2:
			InsertOrder.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			InsertOrder.TimeCondition = THOST_FTDC_TC_IOC;
			InsertOrder.VolumeCondition = THOST_FTDC_VC_AV;
			InsertOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 3:
			InsertOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			InsertOrder.TimeCondition = THOST_FTDC_TC_IOC;
			InsertOrder.VolumeCondition = THOST_FTDC_VC_CV;
			InsertOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 4:
			InsertOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			InsertOrder.TimeCondition = THOST_FTDC_TC_IOC;
			InsertOrder.VolumeCondition = THOST_FTDC_VC_CV;
			InsertOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		default:
			cout << "this type can not be identified" << endl;
			break;
	}

	strcpy(InsertOrder.InstrumentID, ini.GetStr("Instrument" ,"ins"));
	InsertOrder.Direction = ini.GetStr("Direction", "dir")[0];
	InsertOrder.CombOffsetFlag[0] = ini.GetStr("CombOffsetFlag", "cof")[0];
	InsertOrder.CombHedgeFlag[0] = ini.GetStr("CombHedgeFlag", "chf")[0];
	InsertOrder.LimitPrice = (double)ini.GetInt("LimitPrice", "pri");
	InsertOrder.VolumeTotalOriginal = ini.GetInt("VolumeTotalOriginal", "vto");
	InsertOrder.MinVolume = ini.GetInt("MinVolume", "mv");
	InsertOrder.ForceCloseReason = ini.GetStr("ForceCloseReason", "fcr")[0];
	InsertOrder.IsAutoSuspend = ini.GetInt("IsAutoSuspend", "ias");
	InsertOrder.UserForceClose = ini.GetInt("UserForceClose", "ufc");

//	req.TimeCondition = THOST_FTDC_TC_GFD;

//	TThostFtdcDateType	GTDDate;

//	TThostFtdcPriceType	StopPrice;
}

bool CTraderSpi::CompairInfo(CTraderSpi &spiTemp)
{
	if(strcmp(spiTemp.GetBrokerID(), BROKER_ID) == 0 && strcmp(spiTemp.GetInvstorID(), INVESTOR_ID) == 0){
		return 1;
	}
	else {
		return 0;
	}
}