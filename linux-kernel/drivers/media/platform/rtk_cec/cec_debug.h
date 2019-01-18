/*
 *  cec_debug.h - RTK cec driver header file
 *
 * Copyright (C) 2017 Realtek Semiconductor Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __CEC_DEBUG_H__
#define __CEC_DEBUG_H__


//-- cec debug messages
//#define CONFIG_CEC_DBG
//#define CEC_TX_DBG
//#define CEC_RX_DBG

#ifdef CONFIG_CEC_DBG
#define cec_dbg(format, ...) printk("[CEC_DBG] " format, ## __VA_ARGS__)
#else
#define cec_dbg(args...)	
#endif


#ifdef CEC_TX_DBG
#define cec_tx_dbg(format, ...) printk("[CEC_TX] " format, ## __VA_ARGS__)
#else
#define cec_tx_dbg(args...)	
#endif



#ifdef CEC_RX_DBG
#define cec_rx_dbg(format, ...) printk("[CEC_RX] " format, ## __VA_ARGS__)
#else
#define cec_rx_dbg(args...)	
#endif



#define cec_info(format, ...) printk("[CEC_INFO] " format, ## __VA_ARGS__)

#define cec_warning(format, ...) printk("[CEC_WARN] " format, ## __VA_ARGS__)	

#endif  //__CEC_DEBUG_H__



