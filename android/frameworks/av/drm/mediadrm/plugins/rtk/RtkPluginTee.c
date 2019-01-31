#include <stdio.h>
#include <tee_client_api.h>
#include "RtkPluginTee.h"
#include <utils/Log.h>


static TEEC_Context rtk_ctx;
static TEEC_Session rtk_sess;
//static uint32_t Initialized = 0;
static uint8_t Drm_plugin_num = 0;
static uint8_t record = 0;

TEEC_Operation op;
TEEC_UUID uuid = UUID;
uint32_t err_origin;


int Rtk_Plugin_Initialize(void)
{
    TEEC_Result result;
    /* Initialize a context connecting us to the TEE */

    ALOGV("\033[0;32m  Rtk_Plugin_Initialize \033[m");

    if(!Drm_plugin_num)
    {
        ALOGV("\033[0;32m  Rtk_Plugin_Initialize first time =====================================\033[m");
        result = TEEC_InitializeContext(NULL, &rtk_ctx);
        if (result != TEEC_SUCCESS)
        {
            ALOGE("TEEC_InitializeContext failed with code 0x%x", result);
            return TA_FAILED;
        }

        result = TEEC_OpenSession(&rtk_ctx, &rtk_sess, &uuid,
                       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
        if (result != TEEC_SUCCESS)
        {
            ALOGE("TEEC_Opensession failed with code 0x%x origin 0x%x",
                result, err_origin);
            return TA_FAILED;
        }

    }
    else
    {
        ALOGV("\033[0;32m  Rtk_Plugin_Initialize skip [%x]\033[m", Drm_plugin_num);
    }


    return TA_SUCCESS;
}


void Rtk_Plugin_Finalize(void)
{
    ALOGV("\033[0;32m  Rtk_Plugin_Finalize \033[m");
    if(!Drm_plugin_num)
    {
        ALOGV("\033[0;32m  Rtk_Plugin_Finalize done =====================================\033[m");
        TEEC_CloseSession(&rtk_sess);
        TEEC_FinalizeContext(&rtk_ctx);  
    }
    else
    {
        ALOGV("\033[0;32m  Rtk_Plugin_Finalize skip [%x]\033[m", Drm_plugin_num);
    }

}


void Rtk_DrmFactory_Initialize(void)
{
    Rtk_Plugin_Initialize();
    Drm_plugin_num++;
    ALOGV("\033[0;32m  Rtk_Plugin Rtk_DrmFactory_Initialize  [%x]\033[m", Drm_plugin_num);
}


void Rtk_DrmFactory_Finalize(void)
{
    Drm_plugin_num--;
    Rtk_Plugin_Finalize();
    ALOGV("\033[0;32m Rtk_Plugin Rtk_DrmFactory_Finalize  [%x]\033[m", Drm_plugin_num);
}

int Rtk_plugin_Encrypt(unsigned char *src, unsigned char *dst, unsigned int data_length)
{
    TEEC_Result result;


    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT,
             TEEC_NONE,  TEEC_NONE);

    op.params[0].tmpref.buffer = src;
    op.params[0].tmpref.size = (data_length);

    op.params[1].tmpref.buffer = dst;
    op.params[1].tmpref.size = (data_length);

    result = TEEC_InvokeCommand(&rtk_sess, TA_TEE_RTK_PLUGIN_ENCRYPT, &op, &err_origin);

    if (result != TEEC_SUCCESS)
        return TA_FAILED;

    return TA_SUCCESS;

}


int Rtk_plugin_Decrypt(unsigned char *src, unsigned char *dst, unsigned int data_length)
{
    
    TEEC_Result result;
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT,
             TEEC_NONE,  TEEC_NONE);

    op.params[0].tmpref.buffer = src;
    op.params[0].tmpref.size = (data_length);

    op.params[1].tmpref.buffer = dst;
    op.params[1].tmpref.size = (data_length);

    result = TEEC_InvokeCommand(&rtk_sess, TA_TEE_RTK_PLUGIN_DECRYPT, &op, &err_origin);

    if (result != TEEC_SUCCESS)
        return TA_FAILED;

    return TA_SUCCESS;

}
