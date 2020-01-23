#ifndef __SIMPLE_TCP_SOCKET_HPP__
#define __SIMPLE_TCP_SOCKET_HPP__

#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TCP_BUFFER_SIZE 8096


class SimpleSocketScenario{
public:
    std::string type;
    char *szBuf;
    int nBufLen;
    SimpleSocketScenario(){
        szBuf = NULL;
        nBufLen = 0;
    }
    void Init(const char *type, char *szBuf, int nBufLen){
        this->type = std::string(type);
        this->szBuf = szBuf;
        this->nBufLen = nBufLen;
        printf("SimpleSocketScenario.. Init.. this->szBuf=%s \n", szBuf);
    }
    ~SimpleSocketScenario(){
        printf("SimpleSocketScenario.. ~Destructor call\n");
        if(szBuf != NULL) free(szBuf);

    }

};


class SimpleTcpSocket{
typedef enum{
    _SERVER = 0,
    _CLIENT,
}_TCP_STATE;

public:
    int nSocket;
    struct sockaddr_in addr;
    std::string strName;
    _TCP_STATE state;
public:
    SimpleTcpSocket(const char *ip, int port, int state, const char* name){
        memset(&this->addr, 0x00, sizeof(this->addr));

        this->addr.sin_family = AF_INET;
        this->addr.sin_addr.s_addr  = inet_addr(ip);
        this->addr.sin_port = htons(port);

        this->nSocket = -1;
        this->nSocket = socket(AF_INET, SOCK_STREAM, 0);

        this->state = (_TCP_STATE)state;
        this->strName = std::string(name);
    }
    ~SimpleTcpSocket(){
        if(this->nSocket > 0){
            close(this->nSocket);
        }
        
    }
    const char *getIp(){
        return inet_ntoa(this->addr.sin_addr);
    }
    int getPort(){
        return (int)htons(this->addr.sin_port);
    }
    int Connect(){
        if(this->state != _CLIENT) return -1;
        if(connect(this->nSocket, (struct sockaddr *)&this->addr, sizeof(this->addr)) < 0){
            return -1;
        }
        return 0;
    }
    int Write(char *szBuf, int nBufLen){
        if(nBufLen < 0) return 0;
        return write(this->nSocket, szBuf, nBufLen);
    }
  
};

#endif
