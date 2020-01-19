

#include "mbcp.hpp"

class FactoryMbcp{
    //class MBCP_REQUEST;
    // class MBCP_DENY;
    // //class MBCP_RELASE;
    // class MBCP_IDLE;
    // class MBCP_TAKEN;
    // class MBCP_REVOKE;
    // class MBCP_QUEUE_INFO;
public:
    MBCP* Execute(char *szMessage){
        if(!strcmp(szMessage, "REQUEST")) return new MBCP_REQUEST();
        else if(!strcmp(szMessage, "RELEASE")) return new MBCP_RELEASE();
        else if(!strcmp(szMessage, "IDLE")) return new MBCP_IDLE();
        else if(!strcmp(szMessage, "GRANTED")) return new MBCP_GRANTED();
        else if(!strcmp(szMessage, "DENY")) return new MBCP_DENY();
        else if(!strcmp(szMessage, "REVOKE")) return new MBCP_REVOKE();
        else if(!strcmp(szMessage, "TAKEN")) return new MBCP_TAKEN();
        else if(!strcmp(szMessage, "ACK")) return new MBCP_ACK();
        else if(!strcmp(szMessage, "QUEUE_INFO")) return new MBCP_QUEUE_INFO();
        TRACE_MSG("MBCP Execute... return null\n");
        return NULL;
    }
    MBCP* Execute(char *szBuf, int nBufLen){
        int nSubType = szBuf[0] & 0x1f;
        TRACE_MSG("nSubType=%d\n", nSubType);
        switch(nSubType & 0x0f){
	        case MBCP_FLOOR_REQUEST: return new MBCP_REQUEST(szBuf, nBufLen);
	        case MBCP_FLOOR_GRANTED: return new MBCP_GRANTED(szBuf, nBufLen);
	        case MBCP_FLOOR_DENY: return new MBCP_DENY(szBuf, nBufLen);
	        case MBCP_FLOOR_RELEASE: return new MBCP_RELEASE(szBuf, nBufLen);
	        case MBCP_FLOOR_IDLE: return new MBCP_IDLE(szBuf, nBufLen);
	        case MBCP_FLOOR_TAKEN: return new MBCP_TAKEN(szBuf, nBufLen);
	        case MBCP_FLOOR_REVOKE: return new MBCP_REVOKE(szBuf, nBufLen);
	        //case MBCP_FLOOR_QUEUE_POSITION_REQUEST: return new MBCP_QUEUE_INFO(szBuf, nBufLen);
	        case MBCP_FLOOR_QUEUE_POSITION_INFO: return new MBCP_QUEUE_INFO(szBuf, nBufLen);
            case MBCP_FLOOR_ACK: return new MBCP_ACK(szBuf, nBufLen);
        }
        return NULL;
    }
    static FactoryMbcp* Instance(){
        if(!instance){
            instance = new FactoryMbcp();
        }
        return instance;
    }
    
private:
    static FactoryMbcp* instance;
    FactoryMbcp(){}
    ~FactoryMbcp(){}
};
