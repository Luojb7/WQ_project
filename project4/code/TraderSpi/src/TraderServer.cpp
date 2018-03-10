#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <vector>
#include <pthread.h>

#include "Ini.h"
#include "ThostFtdcTraderApi.h"
#include "CTraderSpi.h"

using namespace std;

#define BUFFER_SIZE 1024

int socketBuild(char *ip, int port, int backlog);
void socketGetClient(int &fd, CTraderSpi *mySpi);
void orderMatch(char *order, int &connfd, CTraderSpi *mySpi);
void recvFile(char *filePath, int &connfd);

int main(int argc, char *argv[])
{
	char *localIP = "192.168.0.6";
	int localBacklog = 10;
	int localPort = 0;
	char *configFilePath = new char[256];

	bool isError = 0;

	char ch;
	while((ch = getopt(argc, argv, "p:c:h")) != -1){
        switch(ch){
            case 'p':
            	localPort = atoi(optarg);
            	break;
            case 'c':
            	strcpy(configFilePath, optarg);
            	break;
            case 'h':
                cout << "-h: help list" << endl;
                cout << "-p: tcp port" << endl;
                cout << "-c: config file path" << endl;
                isError = 1;
                break;
            default:
                cout << "input -h to check usage of the code" << endl;
                isError = 1;
                break;
        }
    }

    if(!isError){
    	cout << localPort << " " << configFilePath<< endl;

	    CTraderSpi *pUserSpi = new CTraderSpi;
	    pUserSpi->SetConfigInfo(configFilePath);
	    pUserSpi->Init();
	    pUserSpi->ReqUserLogin();

	    int fd = socketBuild(localIP, localPort, localBacklog);
	    while(1){
	    	socketGetClient(fd, pUserSpi);
	    }
	    close(fd);
    }

    delete[] configFilePath;
    return 0;
}

int  socketBuild(char *ip, int port, int backlog)
{
    int fd;
    int check_ret;

    fd = socket(PF_INET,SOCK_STREAM , 0);
    assert(fd >= 0);

    struct sockaddr_in address;
    bzero(&address,sizeof(address));

    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    inet_pton(AF_INET, ip, &address.sin_addr);

    int recvbuf = 4096;
    int len = sizeof( recvbuf );
    setsockopt( fd, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof( recvbuf ) );
    getsockopt( fd, SOL_SOCKET, SO_RCVBUF, &recvbuf, ( socklen_t* )&len );
    printf( "the receive buffer size after settting is %d\n", recvbuf );

    check_ret = bind(fd,(struct sockaddr*)&address,sizeof(address));
    assert(check_ret >= 0);

    check_ret = listen(fd, backlog);
    assert(check_ret >= 0);

    return fd;
}

void socketGetClient(int &fd, CTraderSpi *mySpi)
{
	struct sockaddr_in addressClient;
    socklen_t clientLen = sizeof(addressClient);

    int connfd = accept(fd, (struct sockaddr*)&addressClient, &clientLen);
    if(connfd < 0){
        cout << "accept error" << endl;
    }
    else{
        char showData[INET_ADDRSTRLEN];
        cout <<inet_ntop(AF_INET,&addressClient.sin_addr,showData,INET_ADDRSTRLEN)<<":" <<ntohs(addressClient.sin_port)<<endl;

        const int BUF_LEN = 1024;
        char sockBuf[BUF_LEN];
        size_t ret;

        memset(sockBuf, '\0' ,BUF_LEN);
        ret = recv(connfd, sockBuf, BUF_LEN-1, 0);
        if(ret > 0){
            printf("ret=%ld,msg=%s\n",ret,sockBuf);
            orderMatch(sockBuf, connfd, mySpi);
            bzero(sockBuf, BUF_LEN);
        }
    }
}

void orderMatch(char *order, int &connfd, CTraderSpi *mySpi)
{
    const int BUF_LEN = 1024;
    char sockBuf[BUF_LEN];
    bzero(sockBuf, BUF_LEN);

    if(strcmp(order, "queryAccount") == 0){
        mySpi->ReqQryTradingAccount();
    }
    else if(strcmp(order, "queryPosition") == 0){
        mySpi->ReqQryInvestorPosition();
    }
    else if(strcmp(order, "insertOrder") == 0){
    	int ret = recv(connfd, sockBuf, BUF_LEN-1, 0);
        char *filePath = new char[256];
        strcpy(filePath, sockBuf);
        recvFile(filePath, connfd);

        mySpi->ReqOrderInsert(filePath);
    }
    else if(strcmp(order, "cancalOrder") == 0){
    	int ret = recv(connfd, sockBuf, BUF_LEN-1, 0);
    	char *orderSysID = new char[256];
    	strcpy(orderSysID, sockBuf);

    }
    else if(strcmp(order, "logout") == 0){

    }
    else if(strlen(order)>0){
        cout << "order error" << endl;
    }
    bzero(sockBuf, BUF_LEN);
}

void recvFile(char *filePath, int &connfd)
{
    FILE *fp = fopen(filePath, "w");
    int FILESIZE = 1024;
    char orderFile[FILESIZE];
    if(fp == NULL){
        printf("File:\t%s Can Not Open To Write!\n", filePath);  
        exit(1); 
    }
    else{
        bzero(orderFile, FILESIZE);  

        int length = recv(connfd, orderFile, FILESIZE, 0);
        if (length < 0){  
            printf("Recieve Data Failed!\n");  
        }  
        int write_length = fwrite(orderFile, sizeof(char), length, fp); 
        if (write_length < length){  
            printf("File:\t%s Write Failed!\n", filePath);  
        }  
        printf("Recieve File:\t%s Finished!\n", filePath);  
      
        fclose(fp);
    }
}
