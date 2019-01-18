/*
 * power_control_core.c - power control core
 *
 * Copyright (C) 2016, Realtek Semiconductor Corporation
 *  Cheng-Yu Lee <cylee12@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/slab.h>
#include <linux/power-control.h>
#include <linux/of.h>
#include <linux/spinlock.h>
#include "power_control_internal.h"
#include "power_control_driver.h"

DEFINE_MUTEX(power_control_list_lock);
int power_control_list_lock_r;
LIST_HEAD(power_control_list);

const char *power_control_get_name(struct power_control *pctrl)
{
    if (!pctrl)
        return NULL;
    return pctrl->name;     
}
EXPORT_SYMBOL(power_control_get_name);

int power_control_power_on(struct power_control *pctrl)
{
    int ret = 0;

    if (!pctrl)
        return -EINVAL;
    
    __power_on_called(priv_get(pctrl));

    if (pctrl->ops->power_on)
        ret = pctrl->ops->power_on(pctrl);
    
    return ret;
}
EXPORT_SYMBOL(power_control_power_on);


int power_control_power_off(struct power_control *pctrl)
{
    int ret = 0;

    if (!pctrl)
        return -EINVAL;

    __power_off_called(priv_get(pctrl));

    if (pctrl->ops->power_off);
        ret = pctrl->ops->power_off(pctrl);

    return ret;
}
EXPORT_SYMBOL(power_control_power_off);

int power_control_is_powered_on(struct power_control *pctrl)
{
    if (!pctrl)
        return -EINVAL;

    if (pctrl->ops->is_powered_on)
        return pctrl->ops->is_powered_on(pctrl);

    return -EINVAL;    
}
EXPORT_SYMBOL(power_control_is_powered_on);

struct power_control *__power_control_get(const char * name)
{
    struct power_control *ctrl = NULL;
    struct list_head * it = NULL;

    list_for_each(it, &power_control_list) {
        ctrl = to_power_control(it);
        if (!strcmp(ctrl->name, name)) 
            return ctrl;
    }

    return NULL;
}
EXPORT_SYMBOL(__power_control_get);

struct power_control *power_control_get(const char * name)
{
    struct power_control *ctrl;
 
    __list_lock();
    ctrl = __power_control_get(name);
    __list_unlock();
    return ctrl;
}
EXPORT_SYMBOL(power_control_get);

struct power_control *__of_power_control_get_by_index(struct device_node *node,
    int index)
{
    struct power_control *ctrl = NULL, *r;
    struct of_phandle_args args;
    int ret;

    ret = of_parse_phandle_with_args(node, "powerctrls", "#powerctrl-cells",
        index, &args);
    if (ret)
        return ERR_PTR(ret);

    list_for_each_entry(r, &power_control_list, list) {
        if (args.np == r->of_node) {
            ctrl = r;
            break;
        }
    }
    return r;
}
EXPORT_SYMBOL(__of_power_control_get_by_index);

struct power_control *of_power_control_get_by_index(struct device_node *node,
    int index)
{
    struct power_control *ctrl = NULL;

    __list_lock();
    ctrl = __of_power_control_get_by_index(node, index);
    __list_unlock();

    return ctrl;
}
EXPORT_SYMBOL(of_power_control_get_by_index);

struct power_control *__of_power_control_get(struct device_node *node,
    const char *id)
{
    int index = 0;
    if (id)
        index = of_property_match_string(node, "powerctrl-names", id);

    return __of_power_control_get_by_index(node, index);
}
EXPORT_SYMBOL(__of_power_control_get);

struct power_control *of_power_control_get(struct device_node *node,
    const char *id)
{
    int index = 0;

    if (id)
        index = of_property_match_string(node, "powerctrl-names", id);    

    return of_power_control_get_by_index(node, index);
}
EXPORT_SYMBOL(of_power_control_get);


int power_control_register(struct power_control *pctrl)
{
    struct power_control_priv *priv = kzalloc(sizeof(struct power_control_priv), GFP_KERNEL);

    if (!priv)
        return -ENOMEM;
    priv_set(pctrl, priv);

    __list_lock();
    list_add(&pctrl->list, &power_control_list);
    __list_unlock();

    return 0;
}
EXPORT_SYMBOL(power_control_register);

void power_control_unregister(struct power_control *pctrl)
{
    __list_lock();
    list_del(&pctrl->list);
    __list_unlock();
    
    kfree(priv_get(pctrl));
}
EXPORT_SYMBOL(power_control_unregister);
