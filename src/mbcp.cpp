#include <string.h>
#include "mbcp.h"
#include "mbcp.hpp"
#include "memory"

MBCP::MBCP()
{
}
MBCP::~MBCP()
{

}
void MBCP::Make_SSRC( unsigned char *dest, int len )
{
	int i;
	unsigned int seed = time(NULL);
	for( i = 0; i < len; ++i )
		dest[i] = rand_r(&seed) & 0xff;
}

void MBCP::SetHeader(int nMsgName)
{
	uint32_t _unLocalssrc;
	Make_SSRC((unsigned char *)&_unLocalssrc, 4);
	Set_MBCPHeader(&this->pstMsg.stHeader, nMsgName, _unLocalssrc);
}

void MBCP::Set_MBCPHeader(_MBCP_HEADER *pstHeader, int nSubType, uint32_t unLocalssrc)
{
	pstHeader->version = 2;
	pstHeader->p = 0;
	pstHeader->subtype = nSubType;
	pstHeader->pt = 204;
	//Make_SSRC((unsigned char *)&pstHeader->ssrc, 4);
	pstHeader->ssrc = unLocalssrc;
	memcpy(pstHeader->cName, "MCPT", 4);
}


int MBCP::Encode()
{
	int nMsgName = this->nMsgName;
	_MBCP_MSG *pstMsg = &this->pstMsg;
	char *pcBuf = this->pcBuf;
	int *pnLen = &this->nBufLen;

	int nOffset = MBCP_MAX_HD_LEN;
	nOffset += __Encode(pstMsg, pcBuf + nOffset);
	pstMsg->nMsgName = nMsgName;
	__Encode_Header(pstMsg, pcBuf, nOffset);
	*pnLen = nOffset;
	this->pstMsg.stHeader.length = *pnLen;
	return SUCC;

}
std::string MBCP::GetDump(){
	char buf[8096] = "";
	snprintf(buf, sizeof(buf), "%s\tversion: %d, subtype: %#x(%s), pt: %d, length: %d, ssrc: %u\n%s", 
		"-----------------------------------------------------------------------------------------------\n",
		this->pstMsg.stHeader.version,
		this->pstMsg.stHeader.subtype, StrMbcpSubType(this->pstMsg.stHeader.subtype),
		this->pstMsg.stHeader.pt,
		this->pstMsg.stHeader.length,
		this->pstMsg.stHeader.ssrc, 
		"-----------------------------------------------------------------------------------------------\n");
	std::string dump(buf);
	return dump;
}
int MBCP::__Get_SubType(int nMsgName, int nAckFlag){
	unsigned int nSubtype = 0x00;

	switch(nMsgName){
	case MBCP_MSGNAME_FLOOR_REQUEST:									nSubtype |= MBCP_FLOOR_REQUEST; 						break;
	case MBCP_MSGNAME_FLOOR_GRANTED:									nSubtype |= MBCP_FLOOR_GRANTED; 						break;
	case MBCP_MSGNAME_FLOOR_DENY: 										nSubtype |= MBCP_FLOOR_DENY; 							break;
	case MBCP_MSGNAME_FLOOR_RELEASE:									nSubtype |= MBCP_FLOOR_RELEASE; 						break;
	case MBCP_MSGNAME_FLOOR_IDLE:										nSubtype |= MBCP_FLOOR_IDLE; 							break;
	case MBCP_MSGNAME_FLOOR_TAKEN:										nSubtype |= MBCP_FLOOR_TAKEN; 							break;
	case MBCP_MSGNAME_FLOOR_REVOKE:										nSubtype |= MBCP_FLOOR_REVOKE; 							break;
	case MBCP_MSGNAME_FLOOR_QUEUE_POSITION_REQUEST:						nSubtype |= MBCP_FLOOR_QUEUE_POSITION_REQUEST; 			break;
	case MBCP_MSGNAME_FLOOR_QUEUE_POSITION_INFO:						nSubtype |= MBCP_FLOOR_QUEUE_POSITION_INFO; 			break;
	case MBCP_MSGNAME_FLOOR_ACK:										nSubtype |= MBCP_FLOOR_ACK; 							break;
	case MBCP_MSGNAME_UNKNOWN:	 										nSubtype |= MBCP_UNKNOWN_SUBTYPE;						return nSubtype;
	}

	if(nAckFlag == ON){
		nSubtype |= 0x10;
	}

	return nSubtype;
}
/// Header parse. ===========================================================================================///
int MBCP::__Encode_Header(_MBCP_MSG *pstMsg, char *pBuf, int nLen){
	_MBCP_HEADER *pstHeader = &pstMsg->stHeader;
	int nIdx = 0;
	pBuf[nIdx] = 2 << 6;
	pBuf[nIdx] |= __Get_SubType(pstMsg->nMsgName, pstMsg->nAckFlag); nIdx += 1;
	pBuf[nIdx] = pstHeader->pt; nIdx += 1;
#if 0
	PUT_16(pBuf + nIdx, nLen); nIdx += 2;
#else
	int nRtcpLen = GET_RTCP_LEN_FROM_LEN(nLen);
	PUT_16(pBuf + nIdx, nRtcpLen); nIdx += 2;
#endif
	PUT_32(pBuf + nIdx, pstHeader->ssrc); nIdx += 4;
	memcpy(pstHeader->cName, (void *)"MCPT", 4);
	memcpy(pBuf+nIdx, (void *)pstHeader->cName, 4);

	return SUCC;
}

int MBCP::__Decode_Header(char *pBuf, _MBCP_MSG *pstMsg){
	int nOffset = 0;

	_MBCP_HEADER *pstHeader = &pstMsg->stHeader;
	pstHeader->version = (pBuf[nOffset] & 0xc0) >> 6;
	pstHeader->subtype = pBuf[nOffset] & 0x1f; nOffset += 1;
	pstHeader->pt = pBuf[nOffset]; nOffset += 1;
#if 0
	pstHeader->length = GET_16(pBuf + nOffset); nOffset += 2;
#else
	int nRtcpLen = GET_16(pBuf + nOffset); nOffset += 2;
	pstHeader->length = GET_LEN_FROM_RTCP_LEN(nRtcpLen);
#endif
	pstHeader->ssrc = GET_32(pBuf + nOffset); nOffset += 4;
	strncpy(pstHeader->cName, pBuf + nOffset, 4); nOffset += 4;

	return nOffset;
}
///======================================================================================== Header parse End. ///
/// Field Encoder ===========================================================================================///
// Priority Field -------------------------------------------------------------------------------------------------------------------//
int __Encode_Priority(_MBCP_FLOOR_PRIORITY *pstPriority, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Floor_Priority; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	pBuf[nIdx] = pstPriority->nPriority; nIdx += 1;
	pBuf[nIdx] = 0; nIdx += 1;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- Priority Field End.//

// Duration Field ------------------------------------------------------------------------------------------------------------------//
int __Encode_Duration(_MBCP_FLOOR_DURATION *pstDuration, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Duration; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	PUT_16(pBuf + 2, pstDuration->nDuration); nIdx += 2;

    return nIdx;
}
//------------------------------------------------------------------------------------------------------------- Duration Field End.//

// Reject Cause Field -------------------------------------------------------------------------------------------------------------//
int __Encode_RejectCause(_MBCP_FLOOR_REJECTCAUSE *pstRejectCause, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Reject_Cause; nIdx += 1;
	pBuf[nIdx] = 2 + strlen(pstRejectCause->cReject_Cause); nIdx += 1;
	PUT_16(pBuf + nIdx, pstRejectCause->nReject_Cause); nIdx += 2;
	strcpy(pBuf + nIdx, pstRejectCause->cReject_Cause); nIdx = ALIGN_LEN4(strlen(pstRejectCause->cReject_Cause) + nIdx);

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------- Reject Cause Field End.//

// Queue Info Field ---------------------------------------------------------------------------------------------------------------//
int __Encode_Queue_Info(_MBCP_FLOOR_QUEUEINFO *pstQueueInfo, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Queue_Info; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	pBuf[nIdx] = pstQueueInfo->nQueue_Postion_Info; nIdx += 1;
	pBuf[nIdx] = pstQueueInfo->nQueue_Priority_Level; nIdx += 1;

    return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Queue Info Field End.//

// Granted Party's Identity Field -------------------------------------------------------------------------------------------------//
int __Encode_Granted_Party_Identity_Field(_MBCP_GRANT_PARTY_IDENTITY *pstGranted_Party_Identity, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Granted_party_Identity; nIdx += 1;
	pBuf[nIdx] = strlen(pstGranted_Party_Identity->cGrant_party_Identity); nIdx += 1;
	strcpy(pBuf + nIdx, pstGranted_Party_Identity->cGrant_party_Identity); nIdx = ALIGN_LEN4(strlen(pstGranted_Party_Identity->cGrant_party_Identity) + nIdx);

    return nIdx;
}
//-------------------------------------------------------------------------------------------- Granted Party's Identity Field End.//

// Permission to Request Field ---------------------------------------------------------------------------------------------------//
int __Encode_Permission_to_Request(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionReq, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Permission_Request; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	PUT_16(pBuf + 2, pstPermissionReq->nPermission_Request_Floor_Value); nIdx += 2;

    return nIdx;
}
//---------------------------------------------------------------------------------------------- Permission to Request Field End.//

// User ID Field -------------------------------------------------------------------------------------------------------------------//
int __Encode_UserID(_MBCP_USER_ID *pstUserID, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_User_ID; nIdx += 1;
	pBuf[nIdx] = strlen(pstUserID->cUserID); nIdx += 1;
	strcpy(pBuf+nIdx, pstUserID->cUserID); nIdx = ALIGN_LEN4(strlen(pstUserID->cUserID) + nIdx);
	return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- User ID Field End.//

// Queue Size Field ---------------------------------------------------------------------------------------------------------------//
int __Encode_Queue_Size(_MBCP_QUEUE_SIZE *pstQueueSize, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Queue_Size; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	PUT_16(pBuf + 2, pstQueueSize->nQueueSize); nIdx += 2;

    return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Queue Size Field End.//

// Sequence Number Field -------------------------------------------------------------------------------------------------------//
int __Encode_Seq_Number(_MBCP_MSG_SEQ_NUMBER *pstSeqNumber, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Message_Sequence_Number; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	PUT_16(pBuf + nIdx, pstSeqNumber->nSeqNum); nIdx += 2;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------- Sequence Number Field End.//

// Queue_User ID Field ----------------------------------------------------------------------------------------------------------//
int __Encode_Queue_UserID(_MBCP_QUEUE_USER_ID *pstQueueUserID, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Queued_User_ID; nIdx += 1;
	pBuf[nIdx] = strlen(pstQueueUserID->cUserID); nIdx += 1;
	strcpy(pBuf+nIdx, pstQueueUserID->cUserID); nIdx = ALIGN_LEN4(strlen(pstQueueUserID->cUserID) + nIdx);

	return nIdx;
}
//----------------------------------------------------------------------------------------------------- Queue_User ID Field End.//

// Source Field -------------------------------------------------------------------------------------------------------------------//
int __Encode_SourceField(_MBCP_SOURCE*pstSource, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Source; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	PUT_16(pBuf + 2, pstSource->nSource); nIdx += 2;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- Source Field End.//

// Track Info Field ---------------------------------------------------------------------------------------------------------------//
int __Encode_Track_Info(_MBCP_TRACK_INFO *pstTrackInfo, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Track_Info; nIdx += 1;
	pBuf[nIdx] = 1 + (pstTrackInfo->nParticipant_Reference_Count * 4); nIdx += 1;
	pBuf[nIdx] = pstTrackInfo->nQueue_Capability; nIdx += 1;
	pBuf[nIdx] = strlen(pstTrackInfo->cParticipant_Type); nIdx += 1;
	strcpy(pBuf+4, pstTrackInfo->cParticipant_Type); nIdx = ALIGN_LEN4(strlen(pstTrackInfo->cParticipant_Type) + nIdx);

	for(int for_i = 0; for_i < pstTrackInfo->nParticipant_Reference_Count; for_i ++){
		PUT_32(pBuf + 8, pstTrackInfo->nFloor_Participant_Reference[for_i]); nIdx += 4;
	}

	return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Track Info Field End.//

// Message Type Field ---------------------------------------------------------------------------------------------------------------//
int __Encode_MsgType(_MBCP_MSG_TYPE *pstMsgType, char* pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Message_Type; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;
	pBuf[nIdx] = pstMsgType->nMsgType; nIdx += 1;
	pBuf[nIdx] = 0; nIdx += 1;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------- Message Type Field End.//

// Indicator Field ------------------------------------------------------------------------------------------------------------------//
int __Encode_Indicator(_MBCP_FLOOR_INDICATOR *pstIndicator, char *pBuf){
	int nIdx = 0;
	pBuf[nIdx] = MBCP_Field_Floor_Indicator; nIdx += 1;
	pBuf[nIdx] = 2; nIdx += 1;

	unsigned short usIndicator = 0x0000;

	if(pstIndicator->usA_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_A;
	}
	if(pstIndicator->usB_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_B;
	}
	if(pstIndicator->usC_bit ==ON){
		usIndicator |= MBCP_INDICATOR_BIT_C;
	}
	if(pstIndicator->usD_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_D;
	}
	if(pstIndicator->usE_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_E;
	}
	if(pstIndicator->usF_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_F;
	}
	if(pstIndicator->usG_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_G;
	}
	if(pstIndicator->usH_bit == ON){
		usIndicator |= MBCP_INDICATOR_BIT_H;
	}

	PUT_16(pBuf + 2, usIndicator); nIdx+= 2;

	return nIdx;
}
//------------------------------------------------------------------------------------------------------------- Indicator Field End.//
///========================================================================================= Field Encoder End. ///

/// Field Decoder ===========================================================================================///
// Priority Field -------------------------------------------------------------------------------------------------------------------//
int __Decode_Priority(char* pBuf, _MBCP_FLOOR_PRIORITY *pstPriority){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstPriority->nPriority = pBuf[nIdx]; nIdx += 1;
	pstPriority->bPriority = TRUE;
	nIdx += 1; //Spare

	return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- Priority Field End.//

// Duration Field ------------------------------------------------------------------------------------------------------------------//
int __Decode_Duration(char* pBuf, _MBCP_FLOOR_DURATION *pstDuration){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstDuration->nDuration = GET_16(pBuf + 2); nIdx += 2;
	pstDuration->bDuration = TRUE;

    return nIdx;
}
//------------------------------------------------------------------------------------------------------------- Duration Field End.//

// Reject Cause Field -------------------------------------------------------------------------------------------------------------//
int __Decode_RejectCause(char* pBuf, _MBCP_FLOOR_REJECTCAUSE *pstRejectCause){
	int nIdx = 0;
	nIdx += 1; //Field ID
	int nReject_CauseLen = pBuf[nIdx]; nIdx += 1;
	pstRejectCause->nReject_Cause = GET_16(pBuf + nIdx); nIdx += 2;
	strncpy(pstRejectCause->cReject_Cause, pBuf + nIdx, nReject_CauseLen); nIdx = ALIGN_LEN4(strlen(pstRejectCause->cReject_Cause) + nIdx);
	pstRejectCause->bRejectCause = TRUE;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------- Reject Cause Field End.//

// Queue Info Field ---------------------------------------------------------------------------------------------------------------//
int __Decode_Queue_Info(char* pBuf, _MBCP_FLOOR_QUEUEINFO *pstQueueInfo){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstQueueInfo->nQueue_Postion_Info = pBuf[nIdx]; nIdx += 1;
	pstQueueInfo->nQueue_Priority_Level = pBuf[nIdx]; nIdx += 1;
	pstQueueInfo->bQUserInfo = TRUE;

    return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Queue Info Field End.//

// Granted Party's Identity Field -------------------------------------------------------------------------------------------------//
int __Decode_Granted_Party_Identity_Field(char* pBuf, _MBCP_GRANT_PARTY_IDENTITY *pstGranted_Party_Identity){
	int nIdx = 0;
	nIdx += 1; //Field ID
	int nGrant_party_IdentityLen = pBuf[nIdx]; nIdx += 1;
	strncpy(pstGranted_Party_Identity->cGrant_party_Identity, pBuf + nIdx, nGrant_party_IdentityLen);
	nIdx = ALIGN_LEN4(strlen(pstGranted_Party_Identity->cGrant_party_Identity) + nIdx);
	pstGranted_Party_Identity->bGrant_party_Identity = TRUE;

    return nIdx;
}
//-------------------------------------------------------------------------------------------- Granted Party's Identity Field End.//

// Permission to Request Field ---------------------------------------------------------------------------------------------------//
int __Decode_Permission_to_Request(char* pBuf, _MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionReq){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstPermissionReq->nPermission_Request_Floor_Value = GET_16(pBuf + nIdx); nIdx += 2;
	pstPermissionReq->bPermission_Request_Floor_Value = TRUE;

	return nIdx;
}
//---------------------------------------------------------------------------------------------- Permission to Request Field End.//

// User ID Field -------------------------------------------------------------------------------------------------------------------//
int __Decode_UserID(char* pBuf, _MBCP_USER_ID *pstUserID){
	int nIdx = 0;
	nIdx += 1; //Field ID
	int nUserIDLen = pBuf[1]; nIdx += 1;

	strncpy(pstUserID->cUserID, pBuf + nIdx, nUserIDLen); nIdx = ALIGN_LEN4(strlen(pstUserID->cUserID) + nIdx);
	pstUserID->bUserID = TRUE;

	return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- User ID Field End.//

// Queue Size Field ---------------------------------------------------------------------------------------------------------------//
int __Decode_Queue_Size(char* pBuf, _MBCP_QUEUE_SIZE *pstQueueSize){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstQueueSize->nQueueSize = GET_16(pBuf + nIdx); nIdx += 2;

    return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Queue Size Field End.//

// Sequence Number Field -------------------------------------------------------------------------------------------------------//
int __Decode_Message_Sequence_Number(char *pBuf, _MBCP_MSG_SEQ_NUMBER *pstSeqNum){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstSeqNum->nSeqNum = GET_16(pBuf + nIdx); nIdx += 2;
	pstSeqNum->bSeqNum = TRUE;

	return nIdx;
}
//-------------------------------------------------------------------------------------------------- Sequence Number Field End.//

// Queue_User ID Field ----------------------------------------------------------------------------------------------------------//
int __Decode_Queue_UserID(char* pBuf, _MBCP_QUEUE_USER_ID *pstQueueUserID){
	int nIdx = 0;
	nIdx += 1; //Field ID
	int nUserIDLen = pBuf[nIdx]; nIdx += 1;

	strncpy(pstQueueUserID->cUserID, pBuf + nIdx, nUserIDLen);
	nIdx = ALIGN_LEN4(strlen(pstQueueUserID->cUserID) + nIdx);
	pstQueueUserID->bUserID = TRUE;

	return nIdx;
}
//----------------------------------------------------------------------------------------------------- Queue_User ID Field End.//

// Source Field -------------------------------------------------------------------------------------------------------------------//
int __Decode_SourceField(char* pBuf, _MBCP_SOURCE*pstSource){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstSource->nSource = GET_16(pBuf + nIdx); nIdx += 2;
	pstSource->bSource = TRUE;

	return nIdx;
}
//-------------------------------------------------------------------------------------------------------------- Source Field End.//

// Track Info Field ---------------------------------------------------------------------------------------------------------------//
int __Decode_TrackInfo(char *pBuf, _MBCP_TRACK_INFO *pstTrackInfo){
	int nIdx = 0;
	nIdx += 1; //Field ID;

	int nTrackInfo_Len = pBuf[nIdx]; nIdx += 1;
	int nParticipantRefLen = nTrackInfo_Len - 1;

	pstTrackInfo->nParticipant_Reference_Count = nParticipantRefLen / 32;
	pstTrackInfo->nQueue_Capability = pBuf[2]; nIdx += 1;
	pstTrackInfo->nParticipant_TypeLen = pBuf[3]; nIdx += 1;

	strncpy(pstTrackInfo->cParticipant_Type, pBuf + nIdx, pstTrackInfo->nParticipant_TypeLen); nIdx += 4;

	for(int for_i = 0; for_i < pstTrackInfo->nParticipant_Reference_Count; for_i++){
		pstTrackInfo->nFloor_Participant_Reference[for_i] = pBuf[for_i]; nIdx += 4;
	}

	pstTrackInfo->bTrackInfo = TRUE;

	return nIdx;
}
//---------------------------------------------------------------------------------------------------------- Track Info Field End.//

// Message Type Field ---------------------------------------------------------------------------------------------------------------//
int __Decode_MsgType(char* pBuf, _MBCP_MSG_TYPE *pstMsgType){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	pstMsgType->nMsgType = pBuf[nIdx]; nIdx += 1;
	nIdx += 1; //Spare
	pstMsgType->bMsgType = TRUE;

    return nIdx;
}
//-------------------------------------------------------------------------------------------------------- Message Type Field End.//

// Indicator Field ------------------------------------------------------------------------------------------------------------------//
int __Decode_Indicator(char *pBuf, _MBCP_FLOOR_INDICATOR *pstIndicator){
	int nIdx = 0;
	nIdx += 2; //Field ID + LenValue
	unsigned short usIndicator = GET_16(pBuf + nIdx);

	if((usIndicator & MBCP_INDICATOR_BIT_A) == MBCP_INDICATOR_BIT_A){
		pstIndicator->usA_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_B) == MBCP_INDICATOR_BIT_B){
		pstIndicator->usB_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_C) == MBCP_INDICATOR_BIT_C){
		pstIndicator->usC_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_D) == MBCP_INDICATOR_BIT_D){
		pstIndicator->usD_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_E) == MBCP_INDICATOR_BIT_E){
		pstIndicator->usE_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_F) == MBCP_INDICATOR_BIT_F){
		pstIndicator->usF_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_G) == MBCP_INDICATOR_BIT_G){
		pstIndicator->usG_bit = ON;
	}
	if((usIndicator & MBCP_INDICATOR_BIT_H) == MBCP_INDICATOR_BIT_H){
		pstIndicator->usH_bit = ON;
	}

	nIdx += 2;

	pstIndicator->cIsBeFlag = TRUE;

	return nIdx;
}
//------------------------------------------------------------------------------------------------------------- Indicator Field End.//
///======================================================================================== Field Decoder End.///

const char *MBCP::StrMbcpSubType(int nSubType)
{
    switch(nSubType)
    {
        case 0:     return "Floor Request";
        case 1:     return "Floor Granted";
        case 17:    return "Floor Granted (Ack required)";
        case 2:     return "Floor Taken";
        case 18:    return "Floor Taken (Ack required)";
        case 3:     return "Floor Deny";
        case 19:    return "Floor Deny (Ack required)";
        case 4:     return "Floor Release";
        case 20:    return "Floor Release (Ack required)";
        case 5:     return "Floor Idle";
        case 21:    return "Floor Idle (Ack required)";
        case 6:     return "Floor Revoke";
        case 8:     return "Floor Queue Position Request";
        case 9:     return "Floor Queue Position Info";
        case 25:    return "Floor Queue Position Info (Ack required)";
        case 10:    return "Floor Ack";
        default:    return "Unknown SubType";
    }
}

std::string MBCP::Str_MBCP_Priority(_MBCP_FLOOR_PRIORITY *pstPriority)
{
	
	char buf[1024] = "";
	if (pstPriority->bPriority)
		snprintf(buf, sizeof(buf), "\tPriority: %d\n", pstPriority->nPriority);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_Duration(_MBCP_FLOOR_DURATION *pstDuration)
{
	char buf[1024] = "";
	if (pstDuration->bDuration)
		snprintf(buf, sizeof(buf), "\tDuration: %d\n", pstDuration->nDuration);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_RejectCause(_MBCP_FLOOR_REJECTCAUSE *pstRejectCause)
{
	char buf[1024] = "";
	if (pstRejectCause->bRejectCause)
		snprintf(buf, sizeof(buf), "\tRejectCause: %d(%s)\n", pstRejectCause->nReject_Cause, pstRejectCause->cReject_Cause);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_QueueInfo(_MBCP_FLOOR_QUEUEINFO *pstQueueInfo)
{
	char buf[2048] = "";
	if(pstQueueInfo->bQUserInfo == TRUE)
		snprintf(buf, sizeof(buf), "\tQueueInfo.QueuePositionInfo: %d\n\tQueueInfo.QueuePriorityLevel: %d\n", 
			pstQueueInfo->nQueue_Postion_Info, pstQueueInfo->nQueue_Priority_Level);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_GrantPartyIdentity(_MBCP_GRANT_PARTY_IDENTITY *pstGrantPartyIdentity)
{
	char buf[1024] = "";
    if(pstGrantPartyIdentity->bGrant_party_Identity == TRUE)
        snprintf(buf, sizeof(buf), "\tGrantPartyIdentity: %s\n", pstGrantPartyIdentity->cGrant_party_Identity);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_PermissionRequestFloorField(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionRequestFloorField)
{
	char buf[1024] = "";
    if(pstPermissionRequestFloorField->bPermission_Request_Floor_Value == TRUE)
        snprintf(buf, sizeof(buf), "\tPermissionRequestFloorField: %d\n", pstPermissionRequestFloorField->nPermission_Request_Floor_Value);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_UserID(_MBCP_USER_ID *pstUserID)
{
	char buf[1024] = "";
    if(pstUserID->bUserID == TRUE)
        snprintf(buf, sizeof(buf), "\tUserId: %s\n", pstUserID->cUserID);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_QueueSize(_MBCP_QUEUE_SIZE *pstQueueSize)
{
	char buf[1024] = "";
    snprintf(buf, sizeof(buf), "\tQueueSize: %d\n", pstQueueSize->nQueueSize);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_SeqNumber(_MBCP_MSG_SEQ_NUMBER *pstSeqNumber)
{
	char buf[1024] = "";
	if(pstSeqNumber->bSeqNum == TRUE)
		snprintf(buf,sizeof(buf), "\tSeqNumber: %d\n", pstSeqNumber->nSeqNum);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_QueueUserID(_MBCP_QUEUE_USER_ID *pstQueueUserID)
{
	char buf[1024] = "";
    if(pstQueueUserID->bUserID == TRUE)
        snprintf(buf,sizeof(buf), "\tQueueUserID: %s\n", pstQueueUserID->cUserID);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_Source(_MBCP_SOURCE *pstSource)
{
	char buf[1024] = "";
    if(pstSource->bSource == TRUE)
        snprintf(buf,sizeof(buf), "\tSource: %d\n", pstSource->nSource);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_TrackInfo(_MBCP_TRACK_INFO *pstTrackInfo)
{
	char buf[2048] = "";
    if(pstTrackInfo->bTrackInfo == TRUE)            //FIXME
    {
		snprintf(buf,sizeof(buf), "\tTrackInfo.nQueue_Capability: %d\n" \
		"\tTrackInfo.nParticipant_TypeLen: %d\n" \
		"\tTrackInfo.cParticipant_Type: %s\n" \
		"\tTrackInfo.nParticipant_Reference_Count: %d\n", 
		pstTrackInfo->nQueue_Capability, pstTrackInfo->nParticipant_TypeLen, pstTrackInfo->cParticipant_Type, pstTrackInfo->nParticipant_Reference_Count);
    }
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_MsgType(_MBCP_MSG_TYPE *pstMsgType)
{
	char buf[1024] = "";
    if(pstMsgType->bMsgType == TRUE)
        snprintf(buf,sizeof(buf), "\tMsgType: %s %d\n", StrMbcpSubType(pstMsgType->nMsgType), pstMsgType->nMsgType);
	std::string temp(buf);
	return temp;
}

std::string MBCP::Str_MBCP_Indicator(_MBCP_FLOOR_INDICATOR *pstIndicator)
{
	char buf[2048] = "";
    if(pstIndicator->cIsBeFlag == TRUE)
        snprintf(buf,sizeof(buf), "\tIndicator: A:%d, B:%d, C:%d, D:%d, E:%d, F:%d, G:%d, H:%d\n",
                pstIndicator->usA_bit, pstIndicator->usB_bit, pstIndicator->usC_bit, pstIndicator->usD_bit,
                pstIndicator->usE_bit, pstIndicator->usF_bit, pstIndicator->usG_bit, pstIndicator->usH_bit);
	std::string temp(buf);
	return temp;
}

void MBCP::__SetIndicator(_MBCP_FLOOR_INDICATOR *pstBuf, char cIndicator){
	if(cIndicator == 'A')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usA_bit = ON;
	}
	else if(cIndicator == 'B')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usB_bit = ON;
	}
	else if(cIndicator == 'C')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usC_bit = ON;
	}
	else if(cIndicator == 'D')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usD_bit = ON;
	}
	else if(cIndicator == 'E')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usE_bit = ON;
	}
	else if(cIndicator == 'F')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usF_bit = ON;
	}
	else if(cIndicator == 'G')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usG_bit = ON;
	}
	else if(cIndicator == 'H')
	{
		pstBuf->cIsBeFlag = TRUE;
		pstBuf->usH_bit = ON;
	}
}
void MBCP::__SetSequenceNumber(_MBCP_MSG_SEQ_NUMBER *pstBuf, int nSeq){
	if(nSeq >= 0 && nSeq <= 65535){
		pstBuf->bSeqNum = TRUE;
		pstBuf->nSeqNum = nSeq;
	}
}
void MBCP::__SetDuration(_MBCP_FLOOR_DURATION *pstBuf, int nDuration){
	pstBuf->bDuration = TRUE;
	pstBuf->nDuration = nDuration;
}
void MBCP::__SetPriority(_MBCP_FLOOR_PRIORITY *pstBuf, int nPriority){
	pstBuf->bPriority = TRUE;
	pstBuf->nPriority = nPriority;
}
void MBCP::__SetUserId(_MBCP_USER_ID *pstBuf, const char *szUserId){
	pstBuf->bUserID = TRUE;
	strcpy(pstBuf->cUserID, szUserId);
}
void MBCP::__SetQueueSize(_MBCP_QUEUE_SIZE *pstBuf, int nQueueSize){
	pstBuf->nQueueSize = nQueueSize;
}
void MBCP::__SetRejectCause(_MBCP_FLOOR_REJECTCAUSE *pstBuf, int nRejectCause){
	pstBuf->bRejectCause = TRUE;
	pstBuf->nReject_Cause = nRejectCause;
}
void MBCP::__SetGrantPartyId(_MBCP_GRANT_PARTY_IDENTITY *pstBuf, const char *szGrantPartyId){
	pstBuf->bGrant_party_Identity = TRUE;
	strcpy(pstBuf->cGrant_party_Identity, szGrantPartyId);
}
void MBCP::__SetGrantPermissionRequest(_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstBuf, int nPermissionRequest){
	pstBuf->bPermission_Request_Floor_Value = TRUE;
	pstBuf->nPermission_Request_Floor_Value = nPermissionRequest;
}


MBCP_IDLE::MBCP_IDLE(){
    this->nMsgName = MBCP_MSGNAME_FLOOR_IDLE; 
	SetHeader(MBCP_FLOOR_IDLE);
}
MBCP_IDLE::~MBCP_IDLE(){

}

std::string MBCP_IDLE::GetDump(){
	std::string dump = MBCP::GetDump();

    _MBCP_Floor_Idle *pstIdle = &this->pstMsg.stPayload.stFloor_Idle;

    dump += Str_MBCP_SeqNumber(&pstIdle->SeqNum);
	dump += Str_MBCP_TrackInfo(&pstIdle->Track_Info);
	dump += Str_MBCP_Indicator(&pstIdle->Indicator);

	return dump;
}


int MBCP_IDLE::__Encode( _MBCP_MSG *pstMsg, char *pcBuf){
	int nOffset = 0;

	_MBCP_Floor_Idle *pstFloor_Idle = &pstMsg->stPayload.stFloor_Idle;
	char *ptr = pcBuf;

	if(pstFloor_Idle->SeqNum.bSeqNum == TRUE){
		_MBCP_MSG_SEQ_NUMBER *pstSeqNumber = &pstFloor_Idle->SeqNum;
		nOffset += __Encode_Seq_Number(pstSeqNumber, ptr + nOffset);
	}

	if(pstFloor_Idle->Track_Info.bTrackInfo == TRUE){
		_MBCP_TRACK_INFO *pstTrackInfo = &pstFloor_Idle->Track_Info;
		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
	}

	if(pstFloor_Idle->Indicator.cIsBeFlag == TRUE){
		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstFloor_Idle->Indicator;
		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
	}

	return nOffset;
}

void MBCP_IDLE::SetSequenceNumber(int nSeq){
	_MBCP_MSG_SEQ_NUMBER *pstBuf = &this->pstMsg.stPayload.stFloor_Idle.SeqNum;
	__SetSequenceNumber(pstBuf, nSeq);
}

void MBCP_IDLE::SetIndicator(char cIndicator){
	_MBCP_FLOOR_INDICATOR *pstBuf = &this->pstMsg.stPayload.stFloor_Idle.Indicator;
	__SetIndicator(pstBuf, cIndicator);
}


MBCP_GRANTED::MBCP_GRANTED(){
    this->nMsgName = MBCP_MSGNAME_FLOOR_GRANTED; 
	SetHeader(MBCP_FLOOR_GRANTED);
}
MBCP_GRANTED::~MBCP_GRANTED(){

}

void MBCP_GRANTED::SetDuration(int nDuration){
	_MBCP_FLOOR_DURATION *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.Duration;
	__SetDuration(pstBuf, nDuration);
}
void MBCP_GRANTED::SetPriority(int nPriority){
	_MBCP_FLOOR_PRIORITY *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.Priority;
	__SetPriority(pstBuf, nPriority);
}
void MBCP_GRANTED::SetUserId( const char *szUserId){
	_MBCP_USER_ID *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.UserID;
	__SetUserId(pstBuf, szUserId);
}
void MBCP_GRANTED::SetQueueSize(int nQueueSize){
	_MBCP_QUEUE_SIZE *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.QueueSize;
	__SetQueueSize(pstBuf, nQueueSize);
}
void MBCP_GRANTED::SetIndicator(char cIndicator){
	_MBCP_FLOOR_INDICATOR *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.Indicator;
	__SetIndicator(pstBuf, cIndicator);
}
std::string MBCP_GRANTED::GetDump(){
	std::string dump = MBCP::GetDump();

    _MBCP_Floor_Granted *pstMbcp = &this->pstMsg.stPayload.stFloor_Granted;

    dump += Str_MBCP_Duration(&pstMbcp->Duration);
	dump += Str_MBCP_Priority(&pstMbcp->Priority);
	dump += Str_MBCP_UserID(&pstMbcp->UserID);
	dump += Str_MBCP_QueueSize(&pstMbcp->QueueSize);
	dump += Str_MBCP_Indicator(&pstMbcp->Indicator);

	return dump;
}


int MBCP_GRANTED::__Encode( _MBCP_MSG *pstMsg, char *pcBuf){
	int nOffset = 0;

	_MBCP_Floor_Granted *pstFloor_Granted = &pstMsg->stPayload.stFloor_Granted;
	char *ptr = pcBuf;

	if(pstFloor_Granted->Duration.bDuration == TRUE){
		_MBCP_FLOOR_DURATION *pstDuration = &pstFloor_Granted->Duration;
		nOffset += __Encode_Duration(pstDuration, ptr + nOffset);
	}

	if(pstFloor_Granted->Priority.bPriority == TRUE){
		_MBCP_FLOOR_PRIORITY *pstPriority = &pstFloor_Granted->Priority;
		nOffset += __Encode_Priority(pstPriority, ptr + nOffset);
	}

	if(pstFloor_Granted->UserID.bUserID == TRUE){
		_MBCP_USER_ID *pstUserId = &pstFloor_Granted->UserID;
		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
	}

	/* The Queue Size field is only applicable in off-network  */
	//_MBCP_QUEUE_SIZE *pstQueueSize = &pstFloor_Granted->QueueSize;
	//nOffset += __Encode_Queue_Size(pstQueueSize, ptr + nOffset);

	if(pstFloor_Granted->QueueUserID.bUserID == TRUE){
		_MBCP_QUEUE_USER_ID *pstQueueUserID = &pstFloor_Granted->QueueUserID;
		nOffset += __Encode_Queue_UserID(pstQueueUserID, ptr + nOffset);
	}

	if(pstFloor_Granted->QueueInfo.bQUserInfo == TRUE){
		_MBCP_FLOOR_QUEUEINFO *pstQueueInfo = &pstFloor_Granted->QueueInfo;
		nOffset += __Encode_Queue_Info(pstQueueInfo, ptr + nOffset);
	}

	if(pstFloor_Granted->Track_Info.bTrackInfo == TRUE){
		_MBCP_TRACK_INFO *pstTrackInfo = &pstFloor_Granted->Track_Info;
		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
	}

	if(pstFloor_Granted->Indicator.cIsBeFlag == TRUE){
		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstFloor_Granted->Indicator;
		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
	}

	return nOffset;
}



MBCP_DENY::MBCP_DENY(){
    this->nMsgName = MBCP_MSGNAME_FLOOR_DENY; 
	SetHeader(MBCP_FLOOR_DENY);
}
MBCP_DENY::~MBCP_DENY(){

}

void MBCP_DENY::SetRejectCause(int nRejectCause){
	_MBCP_FLOOR_REJECTCAUSE *pstBuf = &this->pstMsg.stPayload.stFloor_Deny.RejectCause;
	__SetRejectCause(pstBuf, nRejectCause);
}
void MBCP_DENY::SetUserId( const char *szUserId){
	_MBCP_USER_ID *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.UserID;
	__SetUserId(pstBuf, szUserId);
}
void MBCP_DENY::SetIndicator(char cIndicator){
	_MBCP_FLOOR_INDICATOR *pstBuf = &this->pstMsg.stPayload.stFloor_Granted.Indicator;
	__SetIndicator(pstBuf, cIndicator);
}

std::string MBCP_DENY::GetDump(){
	std::string dump = MBCP::GetDump();

    _MBCP_Floor_Deny *pstMbcp = &this->pstMsg.stPayload.stFloor_Deny;

    dump += Str_MBCP_RejectCause(&pstMbcp->RejectCause);
	dump += Str_MBCP_UserID(&pstMbcp->UserID);
	dump += Str_MBCP_Indicator(&pstMbcp->Indicator);

	return dump;
}


int MBCP_DENY::__Encode( _MBCP_MSG *pstMsg, char *pcBuf)
{
	int nOffset = 0;

	_MBCP_Floor_Deny *pstDeny = &pstMsg->stPayload.stFloor_Deny;
	char *ptr = pcBuf;

	if(pstDeny->RejectCause.bRejectCause == TRUE){
		_MBCP_FLOOR_REJECTCAUSE *pstRejectCause = &pstDeny->RejectCause;
		nOffset += __Encode_RejectCause(pstRejectCause, ptr + nOffset);
	}

	if(pstDeny->UserID.bUserID == TRUE){
		_MBCP_USER_ID *pstUserId = &pstDeny->UserID;
		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
	}

	if(pstDeny->Track_Info.bTrackInfo == TRUE){
		_MBCP_TRACK_INFO *pstTrackInfo = &pstDeny->Track_Info;
		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
	}

	if(pstDeny->Indicator.cIsBeFlag == TRUE){
		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstDeny->Indicator;
		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
	}

	return nOffset;
}

MBCP_REVOKE::MBCP_REVOKE(){
    this->nMsgName = MBCP_MSGNAME_FLOOR_REVOKE; 
	SetHeader(MBCP_FLOOR_REVOKE);
}
MBCP_REVOKE::~MBCP_REVOKE(){

}

void MBCP_REVOKE::SetRejectCause(int nRejectCause){
	_MBCP_FLOOR_REJECTCAUSE *pstBuf = &this->pstMsg.stPayload.stFloor_Revoke.RejectCause;
	__SetRejectCause(pstBuf, nRejectCause);
}
void MBCP_REVOKE::SetIndicator(char cIndicator){
	_MBCP_FLOOR_INDICATOR *pstBuf = &this->pstMsg.stPayload.stFloor_Revoke.Indicator;
	__SetIndicator(pstBuf, cIndicator);
}

std::string MBCP_REVOKE::GetDump(){
	std::string dump = MBCP::GetDump();

    _MBCP_Floor_Revoke *pstMbcp = &this->pstMsg.stPayload.stFloor_Revoke;

    dump += Str_MBCP_RejectCause(&pstMbcp->RejectCause);
	dump += Str_MBCP_Indicator(&pstMbcp->Indicator);

	return dump;
}


int MBCP_REVOKE::__Encode( _MBCP_MSG *pstMsg, char *pcBuf)
{
	int nOffset = 0;

	_MBCP_Floor_Revoke *pstRevoke = &pstMsg->stPayload.stFloor_Revoke;
	char *ptr = pcBuf;

	if(pstRevoke->RejectCause.bRejectCause == TRUE){
		_MBCP_FLOOR_REJECTCAUSE *pstRejectCause = &pstRevoke->RejectCause;
		nOffset += __Encode_RejectCause(pstRejectCause, ptr + nOffset);
	}

	if(pstRevoke->Track_Info.bTrackInfo == TRUE){
		_MBCP_TRACK_INFO *pstTrackInfo = &pstRevoke->Track_Info;
		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
	}

	if(pstRevoke->Indicator.cIsBeFlag == TRUE){
		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstRevoke->Indicator;
		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
	}

	return nOffset;
}


MBCP_TAKEN::MBCP_TAKEN(){
    this->nMsgName = MBCP_MSGNAME_FLOOR_TAKEN; 
	SetHeader(MBCP_FLOOR_TAKEN);
}
MBCP_TAKEN::~MBCP_TAKEN(){

}

void MBCP_TAKEN::SetGrantPartyId(const char *szGrantPartyId){
	_MBCP_GRANT_PARTY_IDENTITY *pstBuf = &this->pstMsg.stPayload.stFloor_Taken.Grant_party_Identity;
	__SetGrantPartyId(pstBuf, szGrantPartyId);
}
void MBCP_TAKEN::SetPermissionRequest(int nPermissionRequest){
	_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstBuf = &this->pstMsg.stPayload.stFloor_Taken.Grant_Permission_Req;
	__SetGrantPermissionRequest(pstBuf, nPermissionRequest);
}
void MBCP_TAKEN::SetUserId( const char *szUserId){
	_MBCP_USER_ID *pstBuf = &this->pstMsg.stPayload.stFloor_Taken.UserID;
	__SetUserId(pstBuf, szUserId);
}
void MBCP_TAKEN::SetIndicator(char cIndicator){
	_MBCP_FLOOR_INDICATOR *pstBuf = &this->pstMsg.stPayload.stFloor_Taken.Indicator;
	__SetIndicator(pstBuf, cIndicator);
}
void MBCP_TAKEN::SetSequenceNumber(int nSeq){
	_MBCP_MSG_SEQ_NUMBER *pstBuf = &this->pstMsg.stPayload.stFloor_Taken.SeqNum;
	__SetSequenceNumber(pstBuf, nSeq);
}
std::string MBCP_TAKEN::GetDump(){
	std::string dump = MBCP::GetDump();

    _MBCP_Floor_Taken *pstMbcp = &this->pstMsg.stPayload.stFloor_Taken;

	dump += Str_MBCP_GrantPartyIdentity(&pstMbcp->Grant_party_Identity);
	dump += Str_MBCP_PermissionRequestFloorField(&pstMbcp->Grant_Permission_Req);
	dump += Str_MBCP_UserID(&pstMbcp->UserID);
    dump += Str_MBCP_SeqNumber(&pstMbcp->SeqNum);
	dump += Str_MBCP_Indicator(&pstMbcp->Indicator);

	return dump;
}


int MBCP_TAKEN::__Encode( _MBCP_MSG *pstMsg, char *pcBuf)
{
	int nOffset = 0;

	_MBCP_Floor_Taken *pstFloor_Taken = &pstMsg->stPayload.stFloor_Taken;
	char *ptr = pcBuf;

	if(pstFloor_Taken->Grant_party_Identity.bGrant_party_Identity == TRUE){
		_MBCP_GRANT_PARTY_IDENTITY *pstGrantPartyIdentity = &pstFloor_Taken->Grant_party_Identity;
		nOffset += __Encode_Granted_Party_Identity_Field(pstGrantPartyIdentity, ptr + nOffset);
	}

	if(pstFloor_Taken->Grant_Permission_Req.bPermission_Request_Floor_Value == TRUE){
		_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionRequestFloorField = &pstFloor_Taken->Grant_Permission_Req;
		nOffset += __Encode_Permission_to_Request(pstPermissionRequestFloorField, ptr + nOffset);
	}

	if(pstFloor_Taken->UserID.bUserID == TRUE){
		_MBCP_USER_ID *pstUserId = &pstFloor_Taken->UserID;
		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
	}

	if(pstFloor_Taken->SeqNum.bSeqNum == TRUE){
		_MBCP_MSG_SEQ_NUMBER *pstSeqNumber = &pstFloor_Taken->SeqNum;
		nOffset += __Encode_Seq_Number(pstSeqNumber, ptr + nOffset);
	}

	if(pstFloor_Taken->Track_Info.bTrackInfo == TRUE){
		_MBCP_TRACK_INFO *pstTrackInfo = &pstFloor_Taken->Track_Info;
		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
	}

	if(pstFloor_Taken->Indicator.cIsBeFlag == TRUE){
		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstFloor_Taken->Indicator;
		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
	}

	return nOffset;
}
// int SIM_Floor_Taken_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Floor_Taken *pstFloor_Taken = &pstMsg->stPayload.stFloor_Taken;
// 	char *ptr = pcBuf;

// 	if(pstFloor_Taken->Grant_party_Identity.bGrant_party_Identity == TRUE){
// 		_MBCP_GRANT_PARTY_IDENTITY *pstGrantPartyIdentity = &pstFloor_Taken->Grant_party_Identity;
// 		nOffset += __Encode_Granted_Party_Identity_Field(pstGrantPartyIdentity, ptr + nOffset);
// 	}

// 	if(pstFloor_Taken->Grant_Permission_Req.bPermission_Request_Floor_Value == TRUE){
// 		_MBCP_PERMISSION_REQUEST_FLOOR_FIELD *pstPermissionRequestFloorField = &pstFloor_Taken->Grant_Permission_Req;
// 		nOffset += __Encode_Permission_to_Request(pstPermissionRequestFloorField, ptr + nOffset);
// 	}

// 	if(pstFloor_Taken->UserID.bUserID == TRUE){
// 		_MBCP_USER_ID *pstUserId = &pstFloor_Taken->UserID;
// 		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
// 	}

// 	if(pstFloor_Taken->SeqNum.bSeqNum == TRUE){
// 		_MBCP_MSG_SEQ_NUMBER *pstSeqNumber = &pstFloor_Taken->SeqNum;
// 		nOffset += __Encode_Seq_Number(pstSeqNumber, ptr + nOffset);
// 	}

// 	if(pstFloor_Taken->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstFloor_Taken->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	if(pstFloor_Taken->Indicator.cIsBeFlag == TRUE){
// 		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstFloor_Taken->Indicator;
// 		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
// 	}

// 	return nOffset;
// }

// int SIM_Floor_Granted_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Floor_Granted *pstFloor_Granted = &pstMsg->stPayload.stFloor_Granted;
// 	char *ptr = pcBuf;

// 	if(pstFloor_Granted->Duration.bDuration == TRUE){
// 		_MBCP_FLOOR_DURATION *pstDuration = &pstFloor_Granted->Duration;
// 		nOffset += __Encode_Duration(pstDuration, ptr + nOffset);
// 	}

// 	if(pstFloor_Granted->Priority.bPriority == TRUE){
// 		_MBCP_FLOOR_PRIORITY *pstPriority = &pstFloor_Granted->Priority;
// 		nOffset += __Encode_Priority(pstPriority, ptr + nOffset);
// 	}

// 	if(pstFloor_Granted->UserID.bUserID == TRUE){
// 		_MBCP_USER_ID *pstUserId = &pstFloor_Granted->UserID;
// 		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
// 	}

// 	/* The Queue Size field is only applicable in off-network  */
// 	//_MBCP_QUEUE_SIZE *pstQueueSize = &pstFloor_Granted->QueueSize;
// 	//nOffset += __Encode_Queue_Size(pstQueueSize, ptr + nOffset);

// 	if(pstFloor_Granted->QueueUserID.bUserID == TRUE){
// 		_MBCP_QUEUE_USER_ID *pstQueueUserID = &pstFloor_Granted->QueueUserID;
// 		nOffset += __Encode_Queue_UserID(pstQueueUserID, ptr + nOffset);
// 	}

// 	if(pstFloor_Granted->QueueInfo.bQUserInfo == TRUE){
// 		_MBCP_FLOOR_QUEUEINFO *pstQueueInfo = &pstFloor_Granted->QueueInfo;
// 		nOffset += __Encode_Queue_Info(pstQueueInfo, ptr + nOffset);
// 	}

// 	if(pstFloor_Granted->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstFloor_Granted->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	if(pstFloor_Granted->Indicator.cIsBeFlag == TRUE){
// 		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstFloor_Granted->Indicator;
// 		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
// 	}

// 	return nOffset;
// }

// int SIM_Floor_Deny_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Floor_Deny *pstDeny = &pstMsg->stPayload.stFloor_Deny;
// 	char *ptr = pcBuf;

// 	if(pstDeny->RejectCause.bRejectCause == TRUE){
// 		_MBCP_FLOOR_REJECTCAUSE *pstRejectCause = &pstDeny->RejectCause;
// 		nOffset += __Encode_RejectCause(pstRejectCause, ptr + nOffset);
// 	}

// 	if(pstDeny->UserID.bUserID == TRUE){
// 		_MBCP_USER_ID *pstUserId = &pstDeny->UserID;
// 		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
// 	}

// 	if(pstDeny->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstDeny->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	if(pstDeny->Indicator.cIsBeFlag == TRUE){
// 		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstDeny->Indicator;
// 		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
// 	}

// 	return nOffset;
// }

// int SIM_Floor_Revoke_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Floor_Revoke *pstRevoke = &pstMsg->stPayload.stFloor_Revoke;
// 	char *ptr = pcBuf;

// 	if(pstRevoke->RejectCause.bRejectCause == TRUE){
// 		_MBCP_FLOOR_REJECTCAUSE *pstRejectCause = &pstRevoke->RejectCause;
// 		nOffset += __Encode_RejectCause(pstRejectCause, ptr + nOffset);
// 	}

// 	if(pstRevoke->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstRevoke->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	if(pstRevoke->Indicator.cIsBeFlag == TRUE){
// 		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstRevoke->Indicator;
// 		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
// 	}

// 	return nOffset;
// }

// int SIM_Ack_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Ack *pstAck = &pstMsg->stPayload.stAck;
// 	char *ptr = pcBuf;

// 	if(pstAck->Source.bSource == TRUE){
// 		_MBCP_SOURCE *pstSource = &pstAck->Source;
// 		nOffset += __Encode_SourceField(pstSource, ptr + nOffset);
// 	}

// 	if(pstAck->MsgType.bMsgType == TRUE){
// 		_MBCP_MSG_TYPE *pstMsgType = &pstAck->MsgType;
// 		nOffset += __Encode_MsgType(pstMsgType, ptr + nOffset);
// 	}

// 	if(pstAck->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstAck->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	return nOffset;
// }

// int SIM_Floor_Queue_Position_Info_Encoder( _MBCP_MSG *pstMsg, char *pcBuf)
// {
// 	int nOffset = 0;

// 	_MBCP_Floor_Queue_Position_Info *pstQueuePositionInfoMsg = &pstMsg->stPayload.stFloor_Q_Position_Info;
// 	char *ptr = pcBuf;

// 	if(pstQueuePositionInfoMsg->UserID.bUserID == TRUE){
// 		_MBCP_USER_ID *pstUserId = &pstQueuePositionInfoMsg->UserID;
// 		nOffset += __Encode_UserID(pstUserId, ptr + nOffset);
// 	}

// 	if(pstQueuePositionInfoMsg->QueueUserID.bUserID == TRUE){
// 		_MBCP_QUEUE_USER_ID *pstQueueUserID = &pstQueuePositionInfoMsg->QueueUserID;
// 		nOffset += __Encode_Queue_UserID(pstQueueUserID, ptr + nOffset);
// 	}

// 	if(pstQueuePositionInfoMsg->QueueInfo.bQUserInfo == TRUE){
// 		_MBCP_FLOOR_QUEUEINFO *pstQueueInfo = &pstQueuePositionInfoMsg->QueueInfo;
// 		nOffset += __Encode_Queue_Info(pstQueueInfo, ptr + nOffset);
// 	}

// 	if(pstQueuePositionInfoMsg->Track_Info.bTrackInfo == TRUE){
// 		_MBCP_TRACK_INFO *pstTrackInfo = &pstQueuePositionInfoMsg->Track_Info;
// 		nOffset += __Encode_Track_Info(pstTrackInfo, ptr + nOffset);
// 	}

// 	if(pstQueuePositionInfoMsg->Indicator.cIsBeFlag == TRUE){
// 		_MBCP_FLOOR_INDICATOR *pstIndicator = &pstQueuePositionInfoMsg->Indicator;
// 		nOffset += __Encode_Indicator(pstIndicator, ptr + nOffset);
// 	}

// 	return nOffset;
// }
