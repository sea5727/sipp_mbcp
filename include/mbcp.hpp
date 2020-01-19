
#ifndef __MBCP__
#define __MBCP__

#include <stdio.h>
#include "mbcp.h"
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
        printf("sendto result=%d, socket=%d, datalen=%d remoteIp=%s, port=%d\n", result, this->nSocket, nDataLen, szRemoteIp, nRemotePort);
        return result;
       
    }
    int Recvfrom(){ //not used
        struct sockaddr_in clntAddr;
        int clntLen = sizeof(clntAddr);
        int recvLen = -1;
        char recvBuffer[8096] = "";
        if((recvLen=recvfrom(this->nSocket, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&clntAddr, (socklen_t*)&clntLen)) == -1) {
            printf("recvfrom fail %d\n", recvLen);
            return recvLen;
        }
        return recvLen;
    }
};


class MBCP
{
public:
static int test;

public:
    int nMsgName;
    _MBCP_MSG pstMsg;
    char pcBuf[MBCP_BUFFER_SIZE];
    int nBufLen;
public:
    virtual int Encode();
    virtual int __Encode( _MBCP_MSG *pstMsg, char *pcBuf){};
    virtual std::string GetDump();
public:
    MBCP();
    MBCP(char* szBuf, int nBufLen);
    ~MBCP();
    void Make_SSRC( unsigned char *dest, int len );
    void Set_MBCPHeader(_MBCP_HEADER *pstHeader, int nSubType, uint32_t unLocalssrc);
    void SetHeader(int nMsgName);
    int __Get_SubType(int nMsgName, int nAckFlag);
    int __Encode_Header(_MBCP_MSG *pstMsg, char *pBuf, int nLen);
    int __Decode_Header(char *pBuf, _MBCP_MSG *pstMsg);
    int Decode();
    int Decode(_MBCP_MSG *pstMsg, char *pcBuf, int nLen);
    int SIM_Floor_Release_Decoder(char *pcBuf, _MBCP_MSG *pstMBCPMsg, int nLen);
    int SIM_Ack_Decoder(char *pcBuf, _MBCP_MSG *pstMBCPMsg, int nLen);
    int SIM_Floor_Queue_Position_Request_Decoder(char *pcBuf, _MBCP_MSG *pstMBCPMsg, int nLen);
    int SIM_Floor_Request_Decoder(char *pcBuf, _MBCP_MSG *pstMBCPMsg, int nLen);
    int __Floor_Idle_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Floor_Taken_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Floor_Granted_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Floor_Deny_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Floor_Queue_Position_Info_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Floor_Revoke_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    int __Decode_Priority(char* pBuf, _MBCP_FLOOR_PRIORITY *pstPriority);
    int __Decode_Duration(char* pBuf, _MBCP_FLOOR_DURATION *pstDuration);
    int __Decode_RejectCause(char* pBuf, _MBCP_FLOOR_REJECTCAUSE *pstRejectCause);
    int __Decode_Queue_Info(char* pBuf, _MBCP_FLOOR_QUEUEINFO *pstQueueInfo);
    int __Decode_Granted_Party_Identity_Field(char* pBuf, _MBCP_GRANT_PARTY_IDENTITY *pstGranted_Party_Identity);
    int __Decode_Permission_to_Request(char* pBuf, _MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionReq);
    int __Decode_UserID(char* pBuf, _MBCP_USER_ID *pstUserID);
    int __Decode_Queue_Size(char* pBuf, _MBCP_QUEUE_SIZE *pstQueueSize);
    int __Decode_Message_Sequence_Number(char *pBuf, _MBCP_MSG_SEQ_NUMBER *pstSeqNum);
    int __Decode_Queue_UserID(char* pBuf, _MBCP_QUEUE_USER_ID *pstQueueUserID);
    int __Decode_SourceField(char* pBuf, _MBCP_SOURCE*pstSource);
    int __Decode_TrackInfo(char *pBuf, _MBCP_TRACK_INFO *pstTrackInfo);
    int __Decode_MsgType(char* pBuf, _MBCP_MSG_TYPE *pstMsgType);
    int __Decode_Indicator(char *pBuf, _MBCP_FLOOR_INDICATOR *pstIndicator);
    int __Floor_Ack_Decoder(char *pcBuf, _MBCP_MSG *pstMsg, int nLen);
    char* DumpMsgBicode(char *szDestBuf, char *szSourceBuf, int nSourceLen);
    char* printBIN(char *szDestBuf, int x);
    const char *StrMbcpSubType(int nSubType);
    const char *GetName() { return StrMbcpSubType(this->nMsgName);}
    std::string Str_MBCP_MsgType(_MBCP_MSG_TYPE *pstMsgType);
    std::string Str_MBCP_Priority(_MBCP_FLOOR_PRIORITY *pstPriority);
    std::string Str_MBCP_Duration(_MBCP_FLOOR_DURATION *pstDuration);
    std::string Str_MBCP_RejectCause(_MBCP_FLOOR_REJECTCAUSE *pstRejectCause);
    std::string Str_MBCP_QueueInfo(_MBCP_FLOOR_QUEUEINFO *pstQueueInfo);
    std::string Str_MBCP_GrantPartyIdentity(_MBCP_GRANT_PARTY_IDENTITY *pstGrantPartyIdentity);
    std::string Str_MBCP_PermissionRequestFloorField(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionRequestFloorField);
    std::string Str_MBCP_UserID(_MBCP_USER_ID *pstUserID);
    std::string Str_MBCP_QueueSize(_MBCP_QUEUE_SIZE *pstQueueSize);
    std::string Str_MBCP_SeqNumber(_MBCP_MSG_SEQ_NUMBER *pstSeqNumber);
    std::string Str_MBCP_QueueUserID(_MBCP_QUEUE_USER_ID *pstQueueUserID);
    std::string Str_MBCP_Source(_MBCP_SOURCE *pstSource);
    std::string Str_MBCP_TrackInfo(_MBCP_TRACK_INFO *pstTrackInfo);
    std::string Str_MBCP_Indicator(_MBCP_FLOOR_INDICATOR *pstIndicator);
    virtual void SetSequenceNumber(int nSeq){printf("MBCP SetSequenceNumber\n");}
    virtual void SetIndicator(const char * cIndicator){printf("MBCP SetIndicator\n");}
    virtual void SetDuration(int nDuration){printf("MBCP SetDuration\n");}
    virtual void SetPriority(int nPriority){}
    virtual void SetUserId(const char *szUserId){}
    virtual void SetQueueSize(int nQueueSize){}
    virtual void SetnRejectCause(int nRejectCause){}
    virtual void SetstrRejectCause(const char *szRejectCause){}
    virtual void SetGrantPartyId(const char *szGrantPartyId){}
    virtual void SetGrantPermissionRequest(int nPermissionRequest){}
    virtual void SetSource(int nSource){}
	virtual void SetMessageType(int nMsgType){}

    void __SetSequenceNumber(_MBCP_MSG_SEQ_NUMBER *pstBuf, int nSeq);
    void __SetIndicator(_MBCP_FLOOR_INDICATOR *pstBuf, const char *szIndicator);
    void __SetDuration(_MBCP_FLOOR_DURATION *pstBuf, int nDuration);
    void __SetPriority(_MBCP_FLOOR_PRIORITY *pstBuf, int nPriority);
    void __SetUserId(_MBCP_USER_ID *pstBuf, const char *szUserId);
    void __SetQueueSize(_MBCP_QUEUE_SIZE *pstBuf, int nQueueSize);
    void __SetnRejectCause(_MBCP_FLOOR_REJECTCAUSE *pstBuf, int nRejectCause);
    void __SetstrRejectCause(_MBCP_FLOOR_REJECTCAUSE *pstBuf, const char *szRejectCause);
    void __SetGrantPartyId(_MBCP_GRANT_PARTY_IDENTITY *pstBuf, const char *szGrantPartyId);
    void __SetGrantPermissionRequest(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstBuf, int nPermissionRequest);
    void __SetSource(_MBCP_SOURCE *pstBuf, int nSource);
	void __SetMessageType(_MBCP_MSG_TYPE *pstBuf, int nMsgType);
};


class MBCP_REQUEST : public MBCP {
public:
    MBCP_REQUEST();
    MBCP_REQUEST(char *szBuf, int nBufLen);
    ~MBCP_REQUEST();
    std::string GetDump();
    
    virtual void SetPriority( int nPriority);
    virtual void SetUserId(const char *szUserId);
    virtual void SetIndicator(const char *cIndicator);
    // SetTrackInfo..
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};

class MBCP_RELEASE : public MBCP {
public:
    MBCP_RELEASE();
    MBCP_RELEASE(char *szBuf, int nBufLen);
    ~MBCP_RELEASE();
    std::string GetDump();
    
    virtual void SetUserId(const char *szUserId);
    virtual void SetIndicator(const char *cIndicator);
    // SetTrackInfo..
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};


class MBCP_IDLE : public MBCP {
public:
    MBCP_IDLE();
    MBCP_IDLE(char *szBuf, int nBufLen);
    ~MBCP_IDLE();
    std::string GetDump();
    virtual void SetSequenceNumber(int nSeq);
    virtual void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};

class MBCP_GRANTED : public MBCP {
public:
    MBCP_GRANTED();
    MBCP_GRANTED(char *szBuf, int nBufLen);
    ~MBCP_GRANTED();
    std::string GetDump();
    virtual void SetDuration(int nDuration);
    virtual void SetPriority( int nPriority);
    virtual void SetUserId(const char *szUserId);
    virtual void SetQueueSize(int nQueueSize);
    virtual void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};



class MBCP_DENY : public MBCP {
public:
    MBCP_DENY();
    MBCP_DENY(char *szBuf, int nBufLen);
    ~MBCP_DENY();
    std::string GetDump();
    void SetnRejectCause(int nRejectCause);
    void SetstrRejectCause(const char *szRejectCause);
    void SetUserId(const char *szUserId);
    void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};


class MBCP_REVOKE : public MBCP {
public:
    MBCP_REVOKE();
    MBCP_REVOKE(char *szBuf, int nBufLen);
    ~MBCP_REVOKE();
    std::string GetDump();
    void SetnRejectCause(int nRejectCause);
    void SetstrRejectCause(const char *szRejectCause);
    void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};


class MBCP_TAKEN : public MBCP {
public:
    MBCP_TAKEN();
    MBCP_TAKEN(char *szBuf, int nBufLen);
    ~MBCP_TAKEN();
    std::string GetDump();
    void SetGrantPartyId(const char *szGrantPartyId);
    void SetPermissionRequest(int nPermissionRequest);
    void SetUserId(const char *szUserId);
    void SetSequenceNumber(int nSeq);
    void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);

};


class MBCP_ACK : public MBCP {
public:
    MBCP_ACK();
    MBCP_ACK(char *szBuf, int nBufLen);
    ~MBCP_ACK();
    std::string GetDump();
    void SetSource(int nSource);
    void SetMessageType(int nMsgType);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);

};

class MBCP_QUEUE_INFO : public MBCP {
public:
    MBCP_QUEUE_INFO();
    MBCP_QUEUE_INFO(char *szBuf, int nBufLen);
    ~MBCP_QUEUE_INFO();
    std::string GetDump();
    void SetUserId(const char *szUserId);
    void SetIndicator(const char *cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};
#endif /* __MBCP__ */
