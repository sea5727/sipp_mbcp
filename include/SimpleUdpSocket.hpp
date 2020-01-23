#ifndef __SIMPLE_UDP_SOCKET_HPP__
#define __SIMPLE_UDP_SOCKET_HPP__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MBCP_BUFFER_SIZE 8096

class SimpleUdpSocket{
public:
    int nSocket;
public:
    SimpleUdpSocket(){
        this->nSocket = -1;
        this->nSocket = socket(AF_INET, SOCK_DGRAM, 0);
    }
    ~SimpleUdpSocket(){
        if(this->nSocket > 0){
            close(this->nSocket);
        }
        
    }
    int Bind(const char *szListenIp, int nLocalPort){
        if(this->nSocket < 0){
            return -1;
        }
        sockaddr_in addrListen = {}; // zero-int, sin_port is 0, which picks a random port for bind.
        addrListen.sin_family = AF_INET;
        addrListen.sin_addr.s_addr=htonl(inet_addr(szListenIp));
        addrListen.sin_port = htons(nLocalPort);
        int result = bind(this->nSocket, (sockaddr*)&addrListen, sizeof(addrListen));
        return result;
    }
    int Sendto(const char *szRemoteIp, int nRemotePort, char szBuf[MBCP_BUFFER_SIZE], int nDataLen){
        struct sockaddr_in servAddr;
        memset(&servAddr, 0, sizeof(servAddr));
    
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = inet_addr(szRemoteIp);
        servAddr.sin_port = htons(nRemotePort);

        int result = sendto(this->nSocket, szBuf, nDataLen, 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        return result;
       
    }
    int Recvfrom(){ //not used
        struct sockaddr_in clntAddr;
        int clntLen = sizeof(clntAddr);
        int recvLen = -1;
        char recvBuffer[8096] = "";
        if((recvLen=recvfrom(this->nSocket, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&clntAddr, (socklen_t*)&clntLen)) == -1) {
            return recvLen;
        }
        return recvLen;
    }
};

#endif
