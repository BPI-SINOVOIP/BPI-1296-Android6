/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_CEC_INTERFACE_H
#define ANDROID_CEC_INTERFACE_H

#include <hardware/hardware.h>

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "CEC_Def.h"
#include "CECAdapter.h"

enum {
    CEC_ENABLE,
    CEC_SET_LOGICAL_ADDRESS,
    CEC_SET_POWER_STATUS,
    CEC_SEND_MESSAGE,
    CEC_RCV_MESSAGE,
    CEC_SET_PHYSICAL_ADDRESS,
    CEC_SET_STANDBY_MODE,
    CEC_SET_RX_MASK,
    CEC_SET_CEC_VERSION,
    CEC_SET_DEVICE_VENDOR_ID,
};


typedef struct {
    unsigned char*      buf;
    unsigned char       len;
}MARS_CEC_MSG;


#define CEC_FILE1        "/dev/cec/0"
#define CEC_FILE2        "/dev/cec-0"

#define DEC_RET(x)      ((x<0) ? CEC_CTRL_FAIL : CEC_CTRL_OK)


/**
 * The id of this module
 */
#define CEC_HARDWARE_MODULE_ID "rtk_cec"
#define CEC_HARDWARE_HDMITX		"CEC_HDMI_TX"
#define CEC_HARDWARE_HDMIRX		"CEC_HDMI_RX"


/**
 * Every hardware module must have a data structure named HAL_MODULE_INFO_SYM
 * and the fields of this data structure must begin with hw_module_t
 * followed by module specific information.
 */
struct cec_module_t {
    struct hw_module_t common;
};

/**
 * Every device data structure must begin with hw_device_t
 * followed by module specific public methods and attributes.
 */
struct cec_device_t {
    struct hw_device_t common;
	int (*set_hw)	   (struct cec_device_t *dev, unsigned char opcode, unsigned long val);
	int (*send_message)(struct cec_device_t *dev, const CEC_MSG* pMsg);
	int (*read_message)(struct cec_device_t *dev, CEC_MSG* pMsg);

};


/** convenience API for opening and closing a device */

static inline int cec_dev_open(const struct hw_module_t* module, const char* name, struct cec_device_t** device) {
    return module->methods->open(module, 
            name, (struct hw_device_t**)device);
}

static inline int cec_dev_close(struct cec_device_t* device) {
    return device->common.close(&device->common);
}


//__END_DECLS

#endif  // ANDROID_CEC_INTERFACE_H

