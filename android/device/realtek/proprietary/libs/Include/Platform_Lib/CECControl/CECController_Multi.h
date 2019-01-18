#ifndef __CEC_CONTROL_MULTI_H__
#define __CEC_CONTROL_MULTI_H__

#include "CECController.h"

    
class MultiDeviceCECController : public CECController
{
public:

    MultiDeviceCECController(CECAPBridge* pBridge, unsigned short PrefferedLogicalAddress=0x8000);
    
    virtual ~MultiDeviceCECController();
        
protected:
    unsigned short        m_RxMask;  
    CEC_LOGICAL_ADDRESS   m_CurLogicalAddress[CEC_DEVICE_TYPE_MAX];      
    CEC_LOGICAL_ADDRESS   m_PreLogicalAddress[CEC_DEVICE_TYPE_MAX];  
    
public:
    virtual int UpdateStatus(CEC_CTRL_STATUS_ID Id);   
    
protected:
   
    virtual CEC_LOGICAL_ADDRESS LogicalAddressAllocation(CEC_DEVICE_TYPE Type, CEC_LOGICAL_ADDRESS PrevLogicalAddress);

public:
    
    virtual int GetInterfaceAddress(CEC_DEVICE_TYPE type, CEC_LOGICAL_ADDRESS* pLogicalAddress);   
    
    virtual unsigned short GetLastLogicalAddresses();           
};


#endif  // __CEC_CONTROL_MULTI_H__


