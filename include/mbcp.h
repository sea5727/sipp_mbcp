/*
 * mbcp_msg.h
 *
 *  Created on: 2018. 3. 5.
 *      Author: LGH
 */

#ifndef INC_CMN_LIB_UTIL_MBCP_MBCP_MSG_H_
#define INC_CMN_LIB_UTIL_MBCP_MBCP_MSG_H_

#include<string>

//===================== J-din Type ========================//
#define PUT_16(p,v) ((p)[0]=((v)>>8)&0xff,(p)[1]=(v)&0xff)
#define PUT_32(p,v) ((p)[0]=((v)>>24)&0xff,(p)[1]=((v)>>16)&0xff,(p)[2]=((v)>>8)&0xff,(p)[3]=(v)&0xff)
#define GET_16(p) ((((p)[0]&0xff)<<8) | ((p)[1]&0xff))
#define GET_32(p) ((((p)[0]&0xff)<<24) | (((p)[1]&0xff)<<16) | (((p)[2]&0xff)<<8) | ((p)[3]&0xff))
#define GET_RTCP_LEN_FROM_LEN(nLen)         ((nLen*8)/32)-1
#define GET_LEN_FROM_RTCP_LEN(nRtcpLen)     ((nRtcpLen+1)*32)/8
#define ALIGN_LEN4(a)					( ((a)&0x3) ? (a)+(4-((a)&0x3)) : (a) )

#define MBCP_MAX_HD_LEN						12


typedef	enum {
	False = 0,
	True = 1
}Boolean;


#ifndef SUCC
    #define SUCC 0
#endif
#ifndef FAIL
    #define FAIL -1
#endif

#ifndef ON
    #define ON 1
#endif
#ifndef OFF
    #define OFF 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

//===================================================================================================///
enum _enMBCP_MsgName{
	MBCP_MSGNAME_FLOOR_REQUEST = 0,
	MBCP_MSGNAME_FLOOR_GRANTED,
	MBCP_MSGNAME_FLOOR_DENY,
	MBCP_MSGNAME_FLOOR_RELEASE,
	MBCP_MSGNAME_FLOOR_IDLE,
	MBCP_MSGNAME_FLOOR_TAKEN,
	MBCP_MSGNAME_FLOOR_REVOKE,
	MBCP_MSGNAME_FLOOR_QUEUE_POSITION_REQUEST,
	MBCP_MSGNAME_FLOOR_QUEUE_POSITION_INFO,
	MBCP_MSGNAME_FLOOR_ACK,

	MBCP_MSGNAME_T100,
	MBCP_MSGNAME_T101,
	MBCP_MSGNAME_T103,
	MBCP_MSGNAME_T104,

	MBCP_MSGNAME_UNKNOWN = 100,
};

/* SUBTYPE 정의 */
enum _enMBCP_SubType{
	MBCP_FLOOR_REQUEST = 0x00,
	MBCP_FLOOR_GRANTED = 0x01,
	MBCP_FLOOR_DENY = 0x03,
	MBCP_FLOOR_RELEASE = 0x04,
	MBCP_FLOOR_IDLE = 0x05,
	MBCP_FLOOR_TAKEN = 0x02,
	MBCP_FLOOR_REVOKE = 0x06,
	MBCP_FLOOR_QUEUE_POSITION_REQUEST = 0x08,
	MBCP_FLOOR_QUEUE_POSITION_INFO = 0x09,
	MBCP_FLOOR_ACK = 0x0a,

	MBCP_UNKNOWN_SUBTYPE = 0xff,
};

/* Field Name 정의 */
enum _enMBCP_Field_Name{
	MBCP_Field_Floor_Priority = 0x00,
	MBCP_Field_Duration,
	MBCP_Field_Reject_Cause,
	MBCP_Field_Queue_Info,
	MBCP_Field_Granted_party_Identity,
	MBCP_Field_Permission_Request,
	MBCP_Field_User_ID,
	MBCP_Field_Queue_Size,
	MBCP_Field_Message_Sequence_Number,
	MBCP_Field_Queued_User_ID,
	MBCP_Field_Source,
	MBCP_Field_Track_Info,
	MBCP_Field_Message_Type,
	MBCP_Field_Floor_Indicator,

	MBCP_Field_Unknown = 0xff,
};

/* User Event Type 정의 */
enum _enMBCP_UserEventType {
  FLOOR_IDLE = 0,
  FLOOR_TAKEN,
  FLOOR_GRANTED,
  FLOOR_DENY,
  QUEUE_POSITION,
  FLOOR_REVOKED,
  FLOOR_REQ_TIMEOUT,
  MBCP_STACK_FAULT,
};

/* User Call Type 정의 */
enum _enMCPTT_UserCallType {
    MBCP_CALLTYPE_NORMAL      = 0,
    MBCP_CALLTYPE_BROADCAST,
    MBCP_CALLTYPE_SYSTEM,
    MBCP_CALLTYPE_EMERGENCY,
    MBCP_CALLTYPE_IMMINENT_PERIL,

	MBCP_CALLTYPE_MAX,
};

/* MBCP State 정의 */
enum _enMBCP_UserState {
  U_FLOOR_IDLE = 0,
  U_FLOOR_TAKEN,
  U_FLOOR_GRANTED,
  U_FLOOR_QUEUED
};

enum _enIndicator{
	MBCP_INDICATOR_BIT_A	 	= 0x8000,
	MBCP_INDICATOR_BIT_B	 	= 0x4000,
	MBCP_INDICATOR_BIT_C	 	= 0x2000,
	MBCP_INDICATOR_BIT_D 	= 0x1000,
	MBCP_INDICATOR_BIT_E 	= 0x0800,
	MBCP_INDICATOR_BIT_F 	= 0x0400,
	MBCP_INDICATOR_BIT_G 	= 0x0200,
	MBCP_INDICATOR_BIT_H	= 0x0100,

};

enum _enSource{
	MBCP_SOURCE_PARTICIPANT = 0,
	MBCP_SOURCE_PARTICIPANT_MCPTT_FUNC,
	MBCP_SOURCE_CONTROLLING_MCPTT_FUNC,
	MBCP_SOURCE_NONE_CONTROLLING_MCPTT_FUNC,
};
///================================================================================================ Enum Zone End. ///

/// Field Zone. ===================================================================================================///
/* Priority */
typedef struct _TagMBCP_Floor_Priority{
	int nPriority;

	bool bPriority;

	void init(){
		bPriority = FALSE;
	}
}_MBCP_FLOOR_PRIORITY;

/* Duration */
typedef struct _TagMBCP_Floor_Duration{
	int nDuration;

	bool bDuration;

	void init(){
		bDuration = FALSE;
	}
}_MBCP_FLOOR_DURATION;

/* RejectCause */
typedef struct _TagMBCP_Floor_RejectCause{
	int nReject_Cause;
	char cReject_Cause[255];

	bool bRejectCause;

	void init(){
		bRejectCause = FALSE;
	}
}_MBCP_FLOOR_REJECTCAUSE;

/* Queue Info */
typedef struct _TagMBCP_Floor_QueueInfo{
	int nQueue_Postion_Info;
	int nQueue_Priority_Level;

	bool bQUserInfo;

	void init(){
		bQUserInfo = FALSE;
	}
}_MBCP_FLOOR_QUEUEINFO;

/* Grantparty Identity */
typedef struct _TagMBCP_GrantpartyIdentity{
	char cGrant_party_Identity[255];

	bool bGrant_party_Identity;

	void init(){
		bGrant_party_Identity = FALSE;
	}
}_MBCP_GRANT_PARTY_IDENTITY;

/* Permission_Request_Floor_Field */
typedef struct _TagMBCP_Permission_Request_Floor_Field{
	int nPermission_Request_Floor_Value;					//0: 발언권 요청 불가능, 1:발언권 요청 가능

	bool bPermission_Request_Floor_Value;

	void init(){
		bPermission_Request_Floor_Value = FALSE;
	}
}_MBCP_PERMISSION_REQUEST_FLOOR_FIELD;

/* User ID */
typedef struct _TagMBCP_UserID{
	char cUserID[255];

	bool bUserID;

	void init(){
		bUserID = FALSE;
	}
}_MBCP_USER_ID;

/* Queue_Size */
typedef struct _TagMBCP_Queue_Size{
	int nQueueSize;
}_MBCP_QUEUE_SIZE;

/* Msg_Seq_Number */
typedef struct _TagMBCP_Msg_Seq_Number{
	int nSeqNum;

	bool bSeqNum;
	void init(){
		bSeqNum = FALSE;
	}
}_MBCP_MSG_SEQ_NUMBER;

/* Queue_UserID */
typedef struct _TagMBCP_Queue_UserID{
	char cUserID[255];
	bool bUserID;

	void init(){
		bUserID = FALSE;
	}
}_MBCP_QUEUE_USER_ID;

/* Source */
typedef struct _TagMBCP_Source{
	int nSource;

	bool bSource;

	void init(){
		bSource = FALSE;
	}
}_MBCP_SOURCE;

/* Track_Info */
//FIXME 다시봐야함
typedef struct _TagMBCP_Track_Info{
	int nQueue_Capability;								//0: Queueing 미지원, 1: Queueing 지원
	int nParticipant_TypeLen;
	char cParticipant_Type[255];
	int nParticipant_Reference_Count;
	u_int nFloor_Participant_Reference[32];

	bool bTrackInfo;

	void Init(){
		bTrackInfo = FALSE;
	}
}_MBCP_TRACK_INFO;

/* MsgName */
typedef struct _TagMBCP_MsgName{
	int nMsgType;

	bool bMsgType;

	void init(){
		bMsgType = FALSE;
	}
}_MBCP_MSG_TYPE;

/* Indicator */
typedef struct _TagMBCP_Floor_Indicator{
	char cIsBeFlag;		// 파라미터가 있는지 없는지 판단 Flag
	char cFiller;

	unsigned short usA_bit:1;			//A	=	Normal call
	unsigned short usB_bit:1;			//B	=	Broadcast group call
	unsigned short usC_bit:1;			//C	=	System call
	unsigned short usD_bit:1;			//D	=	Emergency call
	unsigned short usE_bit:1;			//E	=	Imminent peril call
	unsigned short usF_bit:1;			//F	=	Queueing supported
	unsigned short usG_bit:1;			//G	=	Dual floor
	unsigned short usH_bit:1;			//H	=	Temporary group call (NOTE 2)
	unsigned short usI_Reserved:1;
	unsigned short usJ_Reserved:1;
	unsigned short usK_Reserved:1;
	unsigned short usL_Reserved:1;
	unsigned short usM_Reserved:1;
	unsigned short usN_Reserved:1;
	unsigned short usO_Reserved:1;
	unsigned short usP_Reserved:1;

}_MBCP_FLOOR_INDICATOR;
///================================================================================================ Field Zone End. ///

/// Message Header  ===============================================================================================///
typedef struct stMBCPMsgHeader{
	unsigned int version:2;   /* protocol version */
	unsigned int p:1;         /* padding flag */
	unsigned int subtype:5;     /* varies by packet type */
	unsigned int pt:8;        /* RTCP packet type */
	uint16_t length;           /* pkt len in words, w/o this word */
	uint32_t ssrc;             /* data source being reported */

	char cName[4];	//MCPT
}_MBCP_HEADER;
///============================================================================================ Message Header End. ///

/// Message Body. ================================================================================================///
typedef struct _TagMBCP_Floor_Request{
	_MBCP_FLOOR_PRIORITY priority;
	_MBCP_USER_ID UserID;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Request;

typedef struct _TagMBCP_Floor_Granted{
	_MBCP_FLOOR_DURATION Duration;
	//SSRC of granted floor participant field ??
	_MBCP_FLOOR_PRIORITY Priority;
	_MBCP_USER_ID UserID;
	_MBCP_QUEUE_SIZE QueueSize;
	//SSRC of queued floor participant field ??
	_MBCP_QUEUE_USER_ID QueueUserID;
	_MBCP_FLOOR_QUEUEINFO QueueInfo;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Granted;

typedef struct _TagMBCP_Floor_Deny{
	_MBCP_FLOOR_REJECTCAUSE RejectCause;
	_MBCP_USER_ID UserID;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Deny;

typedef struct _TagMBCP_Floor_Release{
	_MBCP_USER_ID UserID;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Release;

typedef struct _TagMBCP_Floor_Idle{
	_MBCP_MSG_SEQ_NUMBER SeqNum;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Idle;

typedef struct _TagMBCP_Floor_Taken{
	_MBCP_GRANT_PARTY_IDENTITY	Grant_party_Identity;
	_MBCP_PERMISSION_REQUEST_FLOOR_FIELD Grant_Permission_Req;
	_MBCP_USER_ID UserID;
	_MBCP_MSG_SEQ_NUMBER SeqNum;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
	//SSRC of granted floor participant field??
}_MBCP_Floor_Taken;

typedef struct _TagMBCP_Floor_Revoke{
	_MBCP_FLOOR_REJECTCAUSE RejectCause;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Revoke;

typedef struct _TagMBCP_Floor_Queue_Position_Req{
	_MBCP_USER_ID UserID;
	_MBCP_TRACK_INFO Track_Info;
}_MBCP_Floor_Queue_Position_Req;

typedef struct _TagMBCP_Floor_Queue_Position_Info{
	_MBCP_USER_ID UserID;
	//SSRC of queued floor participant field ??
	_MBCP_QUEUE_USER_ID QueueUserID;
	_MBCP_FLOOR_QUEUEINFO QueueInfo;
	_MBCP_TRACK_INFO Track_Info;
	_MBCP_FLOOR_INDICATOR Indicator;
}_MBCP_Floor_Queue_Position_Info;

typedef struct _TagMBCP_Ack{
	_MBCP_SOURCE Source;
	_MBCP_MSG_TYPE MsgType;
	_MBCP_TRACK_INFO Track_Info;
}_MBCP_Ack;

typedef union TagMBCP_Payload{
	_MBCP_Floor_Request 					stFloor_Req;
	_MBCP_Floor_Granted						stFloor_Granted;
	_MBCP_Floor_Deny 							stFloor_Deny;
	_MBCP_Floor_Release						stFloor_Release;
	_MBCP_Floor_Idle							stFloor_Idle;
	_MBCP_Floor_Taken						stFloor_Taken;
	_MBCP_Floor_Revoke						stFloor_Revoke;
	_MBCP_Floor_Queue_Position_Req	stFloor_Q_Position_Req;
	_MBCP_Floor_Queue_Position_Info	stFloor_Q_Position_Info;
	_MBCP_Ack										stAck;
}_MBCP_PALOAD;

typedef struct TagMBCF_Message{
	_MBCP_HEADER			stHeader;

	int nContextID;
	int nMsgName;
	int nAckFlag;

	_MBCP_PALOAD	stPayload;
}_MBCP_MSG;
///============================================================================================= Message Body End. ///

/// MBCP_Get_UserState ============================================================================================///
typedef struct TagMBCP_Sts_Info_Context{
    char 	*pszLocal_IP;
    int 	nLocal_Port;
    char 	*pszPeer_IP;
    int 	nPeer_Port;
    int 	nCB_WorkerID;
    u_int unMy_SSRC;
}_MBCP_STS_INFO_CONTEXT;

typedef struct TagMBCP_UserSts_Idle{
	_MBCP_STS_INFO_CONTEXT stContext;
    Boolean enPermissionToReqFloor;
    _MBCP_FLOOR_INDICATOR stFloorIndicator;
}_MBCP_USER_STS_IDLE;

typedef struct TagMBCP_UserSts_Taken{
	_MBCP_STS_INFO_CONTEXT stContext;
    u_int unTalker_SSRC;
    Boolean enPermissionToReqFloor;
    _MBCP_FLOOR_INDICATOR stFloorIndicator;
}_MBCP_USER_STS_TAKEN;

typedef struct TagMBCP_UserSts_Granted{
	_MBCP_STS_INFO_CONTEXT stContext;
    int nPriority;
    int nDuration;
    _MBCP_FLOOR_INDICATOR stFloorIndicator;
}_MBCP_USER_STS_GRANTED;

typedef struct TagMBCP_UserSts_Queued{
	_MBCP_STS_INFO_CONTEXT stContext;
    int nQueue_Priority;
    _MBCP_FLOOR_INDICATOR stFloorIndicator;
}_MBCP_USER_STS_QUEUED;

typedef union TagMBCP_UserStsInfo{
    _MBCP_USER_STS_IDLE         stIdle;
    _MBCP_USER_STS_TAKEN      stTaken;
    _MBCP_USER_STS_GRANTED  stGranted;
    _MBCP_USER_STS_QUEUED    stQueued;
}_MBCP_USER_STS_INFO;
///========================================================================================= MBCP_Get_UserState End.///

/// MBCP_CB_Event ===============================================================================================///
typedef struct TagMBCP_Evt_ReqTimeout{
    int nMessaeType;
}_MBCP_Evt_ReqTimeout;

typedef struct TagMBCP_Evt_StackFault{
    int nCause;
}_MBCP_Evt_StackFault;

typedef union TagMBCP_UserEventInfo{
    _MBCP_Floor_Idle         					stIdle;
    _MBCP_Floor_Taken      					stTaken;
    _MBCP_Floor_Granted   					stGranted;
    _MBCP_Floor_Deny       					stDeny;
    _MBCP_Floor_Queue_Position_Info 	stQPosition;
    _MBCP_Floor_Revoke     					stRevoke;
    _MBCP_Evt_ReqTimeout 					stTimeout;
    _MBCP_Evt_StackFault   					stStackFault;
}_MBCP_USER_EVENT;
///============================================================================================ MBCP_CB_Event End.///

#endif /* INC_CMN_LIB_UTIL_MBCP_MBCP_MSG_H_ */
