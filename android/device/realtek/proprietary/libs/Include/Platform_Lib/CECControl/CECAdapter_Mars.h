#ifndef __MARS_CEC_ADAPTER_H__
#define __MARS_CEC_ADAPTER_H__

#include "CECAdapter.h"
#include "CEC/rtk_cec.h"

class MarsCECAdapter : public CECAdapter
{

public:
    MarsCECAdapter();
    
    virtual ~MarsCECAdapter();
    
private:
        
    int     m_fd;
	struct cec_module_t* m_cec_module;
	struct cec_device_t* m_cec_device;
    
    struct {        
        unsigned char       init_rdy : 1;
    }m_status;           
            
protected:    

    virtual int Init();  
        
    virtual int UnInit();            
        
    virtual int SetHW(unsigned char opcode, unsigned long val);         
        
    virtual int SendMessage(const CEC_MSG* pMsg);    

    virtual int ReadMessage(CEC_MSG* pMsg);   
    
private:
    virtual void PatchMsgHeader(CEC_MSG* pMsg);
};


#endif  // __MARS_CEC_ADAPTER_H__


