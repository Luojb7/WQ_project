#include "../package/ThostFtdcTraderApi.h"
#include "TraderSpi.h"

CThostFtdcTraderApi* pUserApi;

// ÅäÖÃ²ÎÊý
char  FRONT_ADDR[] = "tcp://180.168.146.187:10001";		
TThostFtdcBrokerIDType	BROKER_ID = "9999";				
TThostFtdcInvestorIDType INVESTOR_ID = "112585";			
TThostFtdcPasswordType  PASSWORD = "qq824073152";			
TThostFtdcInstrumentIDType INSTRUMENT_ID = "al1803";	
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Sell;	
TThostFtdcPriceType	LIMIT_PRICE = 2818; 

int iRequestID = 0;

void main(void)
{
	
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			
	CTraderSpi* pUserSpi = new CTraderSpi();
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			
	pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				
	pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				
	pUserApi->RegisterFront(FRONT_ADDR);							
	pUserApi->Init();

	pUserApi->Join();
//	pUserApi->Release();
}