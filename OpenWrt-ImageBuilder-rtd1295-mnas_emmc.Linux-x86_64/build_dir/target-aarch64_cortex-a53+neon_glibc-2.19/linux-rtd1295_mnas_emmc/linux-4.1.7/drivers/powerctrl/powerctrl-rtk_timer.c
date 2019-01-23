/*
 * powerctrl-rtk_timer.c - power control timer wrapper
 *
 * Copyright (C) 2016, Realtek Semiconductor Corporation
 *  Cheng-Yu Lee <cylee12@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) "pctrl-rtk: " fmt

#include <linux/slab.h>
#include <linux/of.h>
#include <linux/power-control.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include "power_control_driver.h"

static DEFINE_SPINLOCK(rtk_powerctrl_timer_spinlock);

struct rtk_powerctrl_timer {
#ifdef CONFIG_PM
    struct list_head list_pm;
#endif
    struct power_control pctrl;
    struct power_control *parent;
    struct timer_list timer;
    int    expired_time;
    volatile int do_power_off;
};

#ifdef CONFIG_PM

static LIST_HEAD(pm_timer_list);

void rtk_powerctrl_timer_pm(int is_suspend)
{
    struct list_head * it = NULL;

    if (is_suspend) {
        list_for_each(it, &pm_timer_list) {
            struct rtk_powerctrl_timer *ctrl = list_entry(it, struct rtk_powerctrl_timer, list_pm);
            if (ctrl->do_power_off)
                power_control_power_off(ctrl->parent);
        }
    }
}

#endif

#define to_rtk_powerctrl_timer(_p) container_of((_p), struct rtk_powerctrl_timer, pctrl)

static int rtk_timer_power_on(struct power_control * pctrl)
{
    struct rtk_powerctrl_timer * ctrl = to_rtk_powerctrl_timer(pctrl);
    int ret;
    unsigned long flags;

    spin_lock_irqsave(&rtk_powerctrl_timer_spinlock, flags);

    pr_debug("[PCT] %s::%s\n", pctrl->name, __func__);

    ctrl->do_power_off = 0;

    ret = power_control_is_powered_on(ctrl->parent);
    if (!ret) {
        pr_debug("[PCT] %s::%s: do power_on\n", pctrl->name, __func__);
        ret = power_control_power_on(ctrl->parent);
    } else
        ret = -EPERM;

    spin_unlock_irqrestore(&rtk_powerctrl_timer_spinlock, flags);

    return ret;
}

static void rtk_powerctrl_timer_expired(unsigned long data)
{
    struct rtk_powerctrl_timer * ctrl = (struct rtk_powerctrl_timer *)data;

    spin_lock(&rtk_powerctrl_timer_spinlock);

    pr_debug("[PCT] %s::%s\n", ctrl->pctrl.name, __func__);

    if (ctrl->do_power_off) {
        pr_debug("[PCT] %s::%s: do power_off\n", ctrl->pctrl.name, __func__);
        power_control_power_off(ctrl->parent);
    }

    spin_unlock(&rtk_powerctrl_timer_spinlock);
}

static int rtk_timer_power_off(struct power_control * pctrl)
{
    struct rtk_powerctrl_timer * ctrl = to_rtk_powerctrl_timer(pctrl);
    unsigned long flags;

    spin_lock_irqsave(&rtk_powerctrl_timer_spinlock, flags);

    pr_debug("[PCT] %s::%s\n", pctrl->name, __func__);

    ctrl->do_power_off = 1;
    if (!timer_pending(&ctrl->timer)) {
        ctrl->timer.expires  = get_jiffies_64() + ctrl->expired_time;
        add_timer(&ctrl->timer);
    } else {
        mod_timer(&ctrl->timer, get_jiffies_64() + ctrl->expired_time);
    }

    spin_unlock_irqrestore(&rtk_powerctrl_timer_spinlock, flags);

    return 0;
}

static int rtk_timer_is_powered_on(struct power_control * pctrl)
{
    struct rtk_powerctrl_timer * ctrl = to_rtk_powerctrl_timer(pctrl);
    int ret;

    ret = power_control_is_powered_on(ctrl->parent);
    return ret;
}

static struct power_control_ops rtk_power_timer_ops = {
    .power_on  = rtk_timer_power_on,
    .power_off = rtk_timer_power_off,
    .is_powered_on = rtk_timer_is_powered_on,
};

__init int init_rtk_powerctrl_timer(struct device_node *np)
{
    struct rtk_powerctrl_timer *ctrl;

    ctrl = kzalloc(sizeof(*ctrl), GFP_KERNEL);

    ctrl->pctrl.ops     = &rtk_power_timer_ops;
    ctrl->pctrl.of_node = np;

     /* get properties */
    if (of_property_read_u32(np, "expired-time-in-sec", &ctrl->expired_time)) {
        ctrl->expired_time = 30;
    }
    ctrl->expired_time *= HZ;

    /* use pctrl-name as name instead of node name*/
    if (of_property_read_string(np, "pctrl-name", &ctrl->pctrl.name)) {
        ctrl->pctrl.name  = np->name;
    }

    ctrl->parent = of_power_control_get(np, NULL);
    if (IS_ERR_OR_NULL(ctrl->parent)){
        pr_err("%s get parent failed\n", np->name);
        goto err;
    }

    init_timer(&ctrl->timer);
    ctrl->timer.data     = (unsigned long)ctrl;
    ctrl->timer.function = rtk_powerctrl_timer_expired;

    power_control_register(&ctrl->pctrl);

    list_add(&ctrl->list_pm, &pm_timer_list);

    return 0;
err:
    kfree(ctrl);
    return -EINVAL;
}

