
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sipp.hpp"
#include "mbcp.h"
#include "mbcp_factory.hpp"



FactoryMbcp* FactoryMbcp::instance = NULL;

using namespace std;

atomic<int> mbcp_epoll;

char map_callid_and_mbcpport[MBCP_MAXFDS][64] = { 0,};

void OriginalprintBIN(int x);
void OriginalDumpMsgBicode(char *szBuff, int nLen);
char* printBIN(char *szDestBuf, int x);
char* DumpMsgBicode(char *szDestBuf, char *szSourceBuf, int nSourceLen);
void* mbcp_thread(void* param){

    mbcp_epoll = epoll_create1(0);
    if(mbcp_epoll <= 0){
        printf("epoll create fail\n");
        exit(0);
    }
    char recvBuffer[MBCP_BUFFER_SIZE] = "";

    while(1){
        struct epoll_event stEvents[EPOLL_SIZE];
        int count = epoll_wait(mbcp_epoll, stEvents, EPOLL_SIZE, 30); // 0 ~ 30 
        if(count <= 0){
            continue;       
        }

        for(int i = 0 ; i < count ; i++){
            int sock = stEvents[i].data.fd;
            if(sock < 0){
                continue;
            }
            
            struct sockaddr_in clntAddr;
            int clntLen = sizeof(clntAddr);
            int recvLen = -1;
            if((recvLen=recvfrom(sock, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&clntAddr, (socklen_t*)&clntLen)) == -1) {
                printf("recvfrom fail %d\n", recvLen);
            }

            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(sock, (struct sockaddr *)&sin, &len) != -1){
                int listen_portnum = ntohs(sin.sin_port);
                listener *call_listener = get_listener(map_callid_and_mbcpport[listen_portnum]);

                MBCP *mbcp = FactoryMbcp::Instance()->Execute(recvBuffer, recvLen);

                char szLogBuffer[MBCP_BUFFER_SIZE] = "";
                TRACE_MSG("[TESTDEBUG] RECV message...from port %d .. \n%s\n", 
                    listen_portnum, 
                    DumpMsgBicode(szLogBuffer, recvBuffer, recvLen));
                if(mbcp){
                    TRACE_MSG("[TESTDEBUG] MBCP:%s\n", mbcp->GetDump().c_str());
                    call_listener->mbcp_incoming(mbcp, NULL);
                }
                    
            }
        }
    }

    // int ret = epoll_ctl(mbcp_epoll, EPOLL_CTL_DEL, nSocket, NULL);
	// if (ret == 0){
    //     // succ
    // }

}
char* printBIN(char *szDestBuf, int x)
{
	for(int i=7; i >= 0; --i){
        sprintf(szDestBuf, "%d", (x >> i) & 1);
        szDestBuf++;
    }
	*szDestBuf = ' ';
    szDestBuf++;
    return szDestBuf;
}

char* DumpMsgBicode(char *szDestBuf, char *szSourceBuf, int nSourceLen)
{
    char *p = szDestBuf;
	for(int i=0; i<nSourceLen; i++){
		if(i%4 == 0) {
            *p = '\n';
            p++;
        }
		p = printBIN(p, (int)szSourceBuf[i]);
	}
    *p = '\n';
    p++;
    *p = '\n';
    p++;
    p = '\0';
    return szDestBuf;
}


void OriginalprintBIN(int x)
{
	for(int i=7; i >= 0; --i)
		printf("%d", (x >> i) & 1);
	printf(" ");
}

void OriginalDumpMsgBicode(char *szBuff, int nLen)
{
	for(int i=0; i<nLen; i++)
	{
		if(i%4 == 0) printf("\n");
		OriginalprintBIN((int)szBuff[i]);
	}
	printf("\n\n");
}
