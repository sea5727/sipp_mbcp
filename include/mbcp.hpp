
#ifndef __MBCP__
#define __MBCP__

#include "mbcp.h"


class MBCP
{
#define MBCP_BUFFER_SIZE 8096
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
    ~MBCP();
    void Make_SSRC( unsigned char *dest, int len );
    void Set_MBCPHeader(_MBCP_HEADER *pstHeader, int nSubType, uint32_t unLocalssrc);
    void SetHeader(int nMsgName);
    int __Get_SubType(int nMsgName, int nAckFlag);
    int __Encode_Header(_MBCP_MSG *pstMsg, char *pBuf, int nLen);
    int __Decode_Header(char *pBuf, _MBCP_MSG *pstMsg);
    const char *StrMbcpSubType(int nSubType);
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
    void __SetSequenceNumber(_MBCP_MSG_SEQ_NUMBER *pstBuf, int nSeq);
    void __SetIndicator(_MBCP_FLOOR_INDICATOR *pstBuf, char cIndicator);
    void __SetDuration(_MBCP_FLOOR_DURATION *pstBuf, int nDuration);
    void __SetPriority(_MBCP_FLOOR_PRIORITY *pstBuf, int nPriority);
    void __SetUserId(_MBCP_USER_ID *pstBuf, const char *szUserId);
    void __SetQueueSize(_MBCP_QUEUE_SIZE *pstBuf, int nQueueSize);
    void __SetRejectCause(_MBCP_FLOOR_REJECTCAUSE *pstBuf, int nRejectCause);
    void __SetGrantPartyId(_MBCP_GRANT_PARTY_IDENTITY *pstBuf, const char *szGrantPartyId);
    void __SetGrantPermissionRequest(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstBuf, int nPermissionRequest);
};

class MBCP_IDLE : public MBCP {
public:
    MBCP_IDLE();
    ~MBCP_IDLE();
    std::string GetDump();
    void SetSequenceNumber(int nSeq);
    void SetIndicator(char cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};

class MBCP_GRANTED : public MBCP {
public:
    MBCP_GRANTED();
    ~MBCP_GRANTED();
    std::string GetDump();
    void SetDuration(int nDuration);
    void SetPriority( int nPriority);
    void SetUserId(const char *szUserId);
    void SetQueueSize(int nQueueSize);
    void SetIndicator(char cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};



class MBCP_DENY : public MBCP {
public:
    MBCP_DENY();
    ~MBCP_DENY();
    std::string GetDump();
    void SetRejectCause(int nRejectCause);
    void SetUserId(const char *szUserId);
    void SetIndicator(char cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};


class MBCP_REVOKE : public MBCP {
public:
    MBCP_REVOKE();
    ~MBCP_REVOKE();
    std::string GetDump();
    void SetRejectCause(int nRejectCause);
    void SetIndicator(char cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);
};


class MBCP_TAKEN : public MBCP {
public:
    MBCP_TAKEN();
    ~MBCP_TAKEN();
    std::string GetDump();
    void SetGrantPartyId(const char *szGrantPartyId);
    void SetPermissionRequest(int nPermissionRequest);
    void SetUserId(const char *szUserId);
    void SetSequenceNumber(int nSeq);
    void SetIndicator(char cIndicator);
    int __Encode( _MBCP_MSG *pstMsg, char *pcBuf);

};

#endif /* __MBCP__ */
