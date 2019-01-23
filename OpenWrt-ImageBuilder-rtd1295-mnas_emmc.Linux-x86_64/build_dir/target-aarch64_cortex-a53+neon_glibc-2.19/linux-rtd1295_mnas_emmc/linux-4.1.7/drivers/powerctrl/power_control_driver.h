/*
 * power_control_driver.h - for driver implementation
 *
 * Copyright (C) 2016, Realtek Semiconductor Corporation
 *  Cheng-Yu Lee <cylee12@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _POWER_CONTROL_DRIVER_H_
#define _POWER_CONTROL_DRIVER_H_

struct device_node;
struct power_control_priv;

struct power_control_ops {
    int (*power_on)(struct power_control *);
    int (*power_off)(struct power_control *);
    int (*is_powered_on)(struct power_control *);
};

struct power_control {
    struct list_head list;
    const char *name;
    struct power_control_ops *ops;
    struct power_control_priv *priv;
    struct device_node *of_node;
};

/* unlocked version API */
struct power_control *__power_control_get(const char *name);
struct power_control *__of_power_control_get_by_index(struct device_node *np, int index);
struct power_control *__of_power_control_get(struct device_node *np, const char *id);

int power_control_mark_as_hw_pm(struct power_control *pctrl);

#define DEFINE_POWER_CONTROL_OPS(_prefix) \
    struct power_control_ops _prefix ## _power_control_ops = { \
        .power_on      = _prefix ## _power_on,  \
        .power_off     = _prefix ## _power_off, \
        .is_powered_on = _prefix ## _is_powered_on, \
    };

#endif /* _POWER_CONTROL_DRIVER_H_ */ 
