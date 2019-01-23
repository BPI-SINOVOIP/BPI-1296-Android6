/*
 * powerctrl-rtk.c - Realtek power control interface and manager.
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
#include <linux/of_address.h>
#include <linux/power-control.h>
#include <linux/reset-controller.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/cpu_pm.h>
#include <linux/suspend.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <soc/realtek/rtd129x_cpu.h>
#include "power_control_driver.h"

/* powerctrl_timer */
__init int init_rtk_powerctrl_timer(struct device_node *np);
void rtk_powerctrl_timer_pm(int is_suspend);

/* Type */
#define RTK_POWERCTRL_ANALOG           BIT(1)
#define RTK_POWERCTRL_POWER_DOMAIN     BIT(2)
#define RTK_POWERCTRL_L4_ICG           BIT(3)
#define RTK_POWERCTRL_ISOLATION        BIT(4)
/* Sub-type */
#define RTK_POWERCTRL_SRAM             BIT(5)
#define RTK_POWERCTRL_GPU_CORE         BIT(6)
#define RTK_POWERCTRL_SRAM_CH             BIT(7)
/* Ref control-type */
#define RTK_POWERCTRL_REF_FORCE_ENABLE BIT(16)
#define RTK_POWERCTRL_REF_CHECK        BIT(17)
#define RTK_POWERCTRL_REF_MASK        (RTK_POWERCTRL_REF_FORCE_ENABLE | RTK_POWERCTRL_REF_CHECK)
/* Power Control Manager flags */
#define RTK_POWERCTRL_INIT_CALLED      BIT(24)
#define RTK_POWERCTRL_INIT_POWER_ON    BIT(25)
#define RTK_POWERCTRL_INIT_POWER_OFF   BIT(26)
#define RTK_POWERCTRL_INIT_MASK        (RTK_POWERCTRL_INIT_CALLED | \
    RTK_POWERCTRL_INIT_POWER_ON | RTK_POWERCTRL_INIT_POWER_OFF)

static inline int __is_l4_icg(unsigned int flags)
{
    return ((RTK_POWERCTRL_L4_ICG & flags) == RTK_POWERCTRL_L4_ICG);        
}

static inline int __is_analog(unsigned int flags)
{
    return ((RTK_POWERCTRL_ANALOG & flags) == RTK_POWERCTRL_ANALOG);
}

struct rtk_powerctrl_generic {
    struct power_control pctrl;
    unsigned int flags;
    struct list_head list_managed;
#ifdef CONFIG_PM
    struct list_head list_pm;
    int state_pm;
#endif
};
#define to_rtk_powerctrl_generic(_p) container_of((_p), struct rtk_powerctrl_generic, pctrl)

/* power-state */
enum {
    SYSTEM_DEFAULT = -1,
    POWER_OFF,
    POWER_ON,
    DRIVER_CONTROLLED ,
    MANAGED,
    ON_AND_MANAGED,
};

static int of_get_power_state(struct device_node *np)
{
    int power_on = SYSTEM_DEFAULT;
    const char *str;

    if (!of_property_read_u32(np, "power-on", &power_on)) {
        pr_warning("%s: ignore property 'power-state'\n", np->name);
    }

    if (!of_property_read_string(np, "power-state", &str))  {

        if (!strcmp(str, "off"))
            power_on = POWER_OFF;
        else if (!strcmp(str, "on"))
            power_on = POWER_ON;
        else if (!strcmp(str, "driver-controlled"))
            power_on = DRIVER_CONTROLLED;
        else if (!strcmp(str, "managed"))
            power_on = MANAGED;
        else if (!strcmp(str, "on,managed"))
            power_on = ON_AND_MANAGED;
    }

    return power_on;
}

static inline int of_check_revision(struct device_node *np, int rev_val)
{
    const u32 *prop;
    int i;
    int size;
    bool exclusive_mode = false;

    prop = of_get_property(np, "rev,inclusive", &size);
    if (prop)
        goto start;

    exclusive_mode = true;
    prop = of_get_property(np, "rev,exclusive", &size);
    if (prop)
        goto start;

    return 1;
start:

    size /= sizeof(u32);
    for (i = 0; i < size; i++) {
        u32 val = of_read_number(prop++, 1);

        if (val == rev_val)
            return exclusive_mode ? 0 : 1;
    }

    return exclusive_mode ? 1 : 0;
}

/**
 *  count_status_from_reference_devices - count available reference devices
 *  @np:    device node from which the property value is to be read
 *
 *  Return value is less than 0, if there is no reference device.
 */
static int of_count_available_reference_devices(struct device_node *np)
{
    int ret = 0;
    int n_name, n_comp, i;
    const char * str;
    struct device_node *ref_np;

    n_name = of_property_count_strings(np, "ref-status,by-name");
    for (i = 0; i < n_name; i++) {
        of_property_read_string_index(np, "ref-status,by-name", i, &str);

        ref_np = of_find_node_by_name(NULL, str);
        if (ref_np) {
            if (of_device_is_available(ref_np))
                ret ++;
            of_node_put(ref_np);
        }
    }

    n_comp = of_property_count_strings(np, "ref-status,by-compatible");
    for (i = 0; i < n_comp; i++) {
        of_property_read_string_index(np, "ref-status,by-compatible", i, &str);

        ref_np = of_find_compatible_node(NULL, NULL, str);
        if (ref_np) {
            if (of_device_is_available(ref_np))
                ret ++;
            of_node_put(ref_np);
        }
    }

    if (n_name <= 0 && n_comp <= 0)
        ret = -EINVAL;

    return ret;
}

static __maybe_unused LIST_HEAD(pm_data_list);
static struct workqueue_struct *rtk_powerctrl_workqueue;
static const char * power_on_str[]  =  { "POWER_ON",  "DISABLE_HW_PM" };
static const char * power_off_str[] =  { "POWER_OFF", "ENABLE_HW_PM" };
static DEFINE_SPINLOCK(rtk_powerctrl_lock);

/********************************************************************************
 * PCM - Power Control Manager
 ********************************************************************************/
#define PCM_DELAY_TIME_SEC 20

enum {
    POWERCTRL_PRE_INIT,
    POWERCTRL_INIT,
    POWERCTRL_INIT_DONE,
};

static void do_pcm_work(struct work_struct *work);

static LIST_HEAD(pcm_list);
static volatile int pcm_period = POWERCTRL_PRE_INIT;
static DECLARE_DELAYED_WORK(pcm_work, do_pcm_work);

static inline void pcm_power_on_hint(unsigned int *flags)
{
    if (unlikely(pcm_period == POWERCTRL_INIT)) {
        *flags |= RTK_POWERCTRL_INIT_CALLED;
        *flags |= RTK_POWERCTRL_INIT_POWER_ON;
    }
}

static inline void pcm_power_off_hint(unsigned int *flags)
{
    if (unlikely(pcm_period == POWERCTRL_INIT)) {
        *flags |= RTK_POWERCTRL_INIT_CALLED;
        *flags |= RTK_POWERCTRL_INIT_POWER_OFF;
    }
}

static void do_pcm_work(struct work_struct *work)
{
    struct list_head * it = NULL;

    pcm_period = POWERCTRL_INIT_DONE;

    pr_info("%s: begin\n", __func__);

    list_for_each(it, &pcm_list) {
        struct rtk_powerctrl_generic *generic = list_entry(it, struct rtk_powerctrl_generic, list_managed);
        struct power_control *pctrl = &generic->pctrl;

        if (generic->flags & RTK_POWERCTRL_INIT_MASK) {
            pr_debug("%s: unmanaged\n", pctrl->name);
            continue;
        } else {
            int ref_cnt = of_count_available_reference_devices(pctrl->of_node);
            int is_l4_icg = __is_l4_icg(generic->flags);
            int ret;

            if (is_l4_icg && ref_cnt == 0) {
                pr_debug("igrone %s: l4_icg/ref_cnt = 0\n", pctrl->name);
                continue;
            }

            if (!is_l4_icg && ref_cnt > 0) {
                pr_debug("igrone %s: non-l4_icg/ref_cnt = %d\n", pctrl->name, ref_cnt);
                continue;
            }

            if (power_control_is_powered_on(pctrl) != 0)
            {
                ret = power_control_power_off(pctrl);
                pr_info("%s::%s, ret = %d\n", pctrl->name, power_off_str[is_l4_icg], ret);
            } else
                pr_debug("ignore %s: already power off\n", pctrl->name);
        }

        mdelay(5);
    }

    pr_info("%s: done\n", __func__);
}


/********************************************************************************
 * Reference Checking - by phandle
 ********************************************************************************/

struct __ref_data {
    int br;
    int bc;
    int bp;
};


static inline int __ref_check(struct device_node *np)
{
    int n;
    int i;
    int fail;

    n = of_count_phandle_with_args(np, "powerctrls", "#powerctrl-cells");
    for (i = 0; i < n; i++) {
        struct power_control *p = __of_power_control_get_by_index(np, i);
        if (IS_ERR_OR_NULL(p))
            return -EINVAL;

        fail = power_control_is_powered_on(p) != 1;
        if (fail)
            return -EINVAL;
    }

    n = of_count_phandle_with_args(np, "clocks", "#clock-cells");
    for (i = 0; i < n; i++) {
        struct clk *c = of_clk_get(np, i);
        if (IS_ERR_OR_NULL(c))
            return -EINVAL;

        fail = __clk_is_enabled(c) == 0;

        clk_put(c);

        if (fail)
            return -EINVAL;
    }

    n = of_count_phandle_with_args(np, "reset", "#reset-cells");
    for (i = 0; i < n; i++) {
        struct reset_control *r = of_reset_control_get_by_index(np, i);
        if (IS_ERR_OR_NULL(r))
            return -EINVAL;

        fail = reset_control_status(r) > 0;

        reset_control_put(r);

        if (fail)
            return -EINVAL;
    }

    return 0;
}

static inline int __ref_enable_save(struct device_node *np, 
    struct __ref_data *data)
{
    int n;
    int i;

    memset(data, 0, sizeof(*data));

    n = of_count_phandle_with_args(np, "powerctrls", "#powerctrl-cells");
    for (i = 0; i < n; i++) {
        struct power_control *p = __of_power_control_get_by_index(np, i);
        if (IS_ERR_OR_NULL(p))
            return -EINVAL;

        if (power_control_is_powered_on(p) != 0)
            continue;

        power_control_power_on(p);
        data->bp |= 1 << i;           
    }

    n = of_count_phandle_with_args(np, "clocks", "#clock-cells");
    for (i = 0; i < n; i++) {
        struct clk *c = of_clk_get(np, i);
        if (IS_ERR_OR_NULL(c)) {
            return -EINVAL;
        }

        if (__clk_is_enabled(c) == 0) {
            clk_prepare_enable(c);
            data->bc |= 1 << i;
        }

        clk_put(c);
    }

    n = of_count_phandle_with_args(np, "resets", "#reset-cells");
    for (i = 0; i < n; i++) {
        struct reset_control *r = of_reset_control_get_by_index(np, i);
        if (IS_ERR_OR_NULL(r))
            return -EINVAL;

        if (reset_control_status(r)) {
            reset_control_deassert(r);
            data->br |= 1 << i;
        }

        reset_control_put(r);
    }

    return 0;
}

static inline int __ref_enable_restore(struct device_node *np,
    struct __ref_data *data)
{
    int i;

    for (i = 0; data->bp; data->bp >>= 1, i++) {
        if (data->bp & 1) {
            struct power_control *p = __of_power_control_get_by_index(np, i);
            if (IS_ERR_OR_NULL(p))
                return -EINVAL;

            power_control_power_off(p);
        }
    }
    
    for (i = 0; data->bc; data->bc >>= 1, i++) {
        if (data->bc & 1) {
            struct clk *c = of_clk_get(np, i);
            if (IS_ERR_OR_NULL(c))
                return -EINVAL;

            clk_disable_unprepare(c);
            clk_put(c);
        }
    }

    for (i = 0; data->br; data->br >>= 1, i++) {
        if (data->br & 1) {
            struct reset_control *r = of_reset_control_get_by_index(np, i);
            if (IS_ERR_OR_NULL(r))
                return -EINVAL;

            reset_control_assert(r);
            reset_control_put(r);
        }
    }

    return 0; 
}

/********************************************************************************
 * DEBUGFS
 ********************************************************************************/
#ifdef CONFIG_POWER_CONTROL_RTD129x_DEBUGFS

static struct dentry *power_control_root;

static int rtk_powerctrl_show(struct seq_file *s, void *data)
{
    struct power_control *pctrl = s->private;
    seq_printf(s, "%d\n", power_control_is_powered_on(pctrl));
    return 0;
}

static int rtk_powerctrl_open(struct inode *inode, struct file *file)
{
    return single_open(file, rtk_powerctrl_show, inode->i_private);
}

static ssize_t rtk_powerctrl_write(struct file *filp, const char __user *user_buf, size_t count, loff_t *f_pos)
{
    char buf[32];
    int buf_size;
    struct power_control *pctrl = filp->f_inode->i_private;
    bool val;

    buf_size = count < sizeof(buf) - 1 ? count : sizeof(buf) - 1;
    if (copy_from_user(buf, user_buf, buf_size))
        return -EFAULT;

    if (strtobool(buf, &val))
        return -EINVAL;

    if (val == 1)
        power_control_power_on(pctrl);
    else 
        power_control_power_off(pctrl);

    return count;      
}

static struct file_operations rtk_powerctrl_debugfs_ops = {
    .owner   = THIS_MODULE,
    .open    = rtk_powerctrl_open,
    .read    = seq_read,
    .write   = rtk_powerctrl_write,
    .release = single_release,
};

#endif /* CONFIG_POWER_CONTROL_RTD129x_DEBUGFS */


/********************************************************************************
 * PM
 ********************************************************************************/
#ifdef CONFIG_PM

static inline void rtk_powerctrl_pm_config(struct power_control *pctrl)
{
    struct rtk_powerctrl_generic *generic = to_rtk_powerctrl_generic(pctrl);
    list_add(&generic->list_pm, &pm_data_list);
}

static inline int rtk_powerctrl_pm_save(void)
{
    struct list_head * it = NULL;

#ifdef CONFIG_SUSPEND
    if (RTK_PM_STATE == PM_SUSPEND_STANDBY)
        return 0;
#endif

    list_for_each(it, &pm_data_list) {
        struct rtk_powerctrl_generic *generic = list_entry(it, struct rtk_powerctrl_generic, list_pm);
        struct power_control *pctrl = &generic->pctrl;

        generic->state_pm = power_control_is_powered_on(pctrl);        
    }

    return 0;
}

static inline int rtk_powerctrl_pm_restore(void)
{
    struct list_head * it = NULL;
    int ret;

#ifdef CONFIG_SUSPEND
    if (RTK_PM_STATE == PM_SUSPEND_STANDBY)
        return 0;
#endif

    list_for_each_prev(it, &pm_data_list) {
        struct rtk_powerctrl_generic *generic = list_entry(it, struct rtk_powerctrl_generic, list_pm);
        struct power_control *pctrl = &generic->pctrl;
        
        int powered = power_control_is_powered_on(pctrl);
        int x = __is_l4_icg(generic->flags);

        if (generic->state_pm == 1 && powered != 1) {

            ret = power_control_power_on(pctrl);
            pr_info("restore %s: %s, ret = %d\n", pctrl->name, power_off_str[x], ret);

        } else if (generic->state_pm == 0 && powered != 0) {
            ret = power_control_power_off(pctrl);
            pr_info("restore %s: %s, ret = %d\n", pctrl->name, power_off_str[x], ret);
        }
    }
    return 0;
}

static int rtk_powerctrl_pm_notify(struct notifier_block *notify_block, unsigned long mode, void *unused)
{
    switch (mode) {        
    case CPU_CLUSTER_PM_ENTER:
        printk(KERN_INFO "[POWERCTRL] Enter %s, act = CPU_CLUSTER_PM_ENTER\n", __func__);
        rtk_powerctrl_timer_pm(1);
        rtk_powerctrl_pm_save();
        printk(KERN_INFO "[POWERCTRL] Exit %s,  act = CPU_CLUSTER_PM_ENTER\n", __func__);
        break;
    case CPU_CLUSTER_PM_EXIT:
        printk(KERN_INFO "[POWERCTRL] Enter %s, act = CPU_CLUSTER_PM_EXIT\n", __func__);
        rtk_powerctrl_pm_restore();
        printk(KERN_INFO "[POWERCTRL] Exit %s,  act = CPU_CLUSTER_PM_EXIT\n", __func__);
        break;
    default:
        break;
    }
    return NOTIFY_OK;
}

static struct notifier_block pm_notifier_block = {
    .notifier_call = rtk_powerctrl_pm_notify,
};

#else

void inline rtk_powerctrl_pm_config(struct power_control *pctrl) 
{
}

#endif /* CONFIG_PM */


static int rtk_powerctrl_generic_config(struct device_node * np, struct power_control *pctrl)
{
    int power_on = of_get_power_state(np);
    struct rtk_powerctrl_generic *generic = to_rtk_powerctrl_generic(pctrl);
    int is_l4_icg = __is_l4_icg(generic->flags);
    int ret;

    switch(power_on)
    {
    case POWER_OFF:           
        ret = power_control_power_off(pctrl);
        pr_info("%s::%s, ret = %d\n", pctrl->name, power_off_str[is_l4_icg], ret);
        break;

    case POWER_ON:
        ret = power_control_power_on(pctrl);
        pr_info("%s::%s, ret = %d\n", pctrl->name, power_on_str[is_l4_icg], ret);
        break;

    case MANAGED: 
        list_add_tail(&generic->list_managed, &pcm_list);
        break;

    case ON_AND_MANAGED:
        list_add_tail(&generic->list_managed, &pcm_list);

        ret = power_control_power_on(pctrl);
        pr_info("%s::%s, ret = %d\n", pctrl->name, power_on_str[is_l4_icg], ret);
        break;
    default:
        break;      
    }

#ifdef CONFIG_POWER_CONTROL_RTD129x_DEBUGFS
    do {
        struct dentry *dir = debugfs_create_dir(pctrl->name, power_control_root);
        debugfs_create_file("power", 0444, dir, pctrl, &rtk_powerctrl_debugfs_ops);
    } while (0);
#endif

    return 0;
}

/********************************************************************************
 * rtk_powerctrl
 ********************************************************************************/
struct rtk_powerctrl_config {
    void * __iomem reg;
    u32 shift;
    u32 width;
    u32 off_value;
    u32 off_value_st;
    u32 on_value;
    u32 on_value_st;
};

struct rtk_powerctrl {
    struct rtk_powerctrl_generic generic;
    int size;
    /* LAST MEMBER  */
    struct rtk_powerctrl_config config[0];
};

#define to_rtk_powerctrl(_p) container_of((_p), struct rtk_powerctrl, generic.pctrl)

static int rtk_power_on(struct power_control *pctrl)
{
    struct rtk_powerctrl *ctrl = to_rtk_powerctrl(pctrl);
    u32 val, mask, data;
    int i;
    struct __ref_data d;
    unsigned long flags;

    pcm_power_on_hint(&ctrl->generic.flags);

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_CHECK) {
        int ret = __ref_check(ctrl->generic.pctrl.of_node);
        if (ret)
            return ret;
    }   

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_FORCE_ENABLE) {        
        __ref_enable_save(ctrl->generic.pctrl.of_node, &d);
    }
    
    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    for (i = 0; i < ctrl->size; i++) {
        struct rtk_powerctrl_config *config = &ctrl->config[i];

        mask = (BIT(config->width) - 1) << config->shift;
        data = config->on_value << config->shift;

        val = readl(config->reg);
        val &= ~mask;
        val |= data & mask;
        writel(val, config->reg);
    }

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_FORCE_ENABLE) {
        __ref_enable_restore(ctrl->generic.pctrl.of_node, &d);
    }

    return 0;
}

static int rtk_power_off(struct power_control *pctrl)
{
    struct rtk_powerctrl *ctrl = to_rtk_powerctrl(pctrl);
    u32 val, mask, data;
    int i;
    struct __ref_data d;
    unsigned long flags;

    pcm_power_off_hint(&ctrl->generic.flags);

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_CHECK) {
        int ret = __ref_check(ctrl->generic.pctrl.of_node);
        if (ret)
            return ret;
    }

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_FORCE_ENABLE) {
        __ref_enable_save(ctrl->generic.pctrl.of_node, &d);
    }

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    for (i = ctrl->size-1; i >= 0; i--) {
        struct rtk_powerctrl_config *config = &ctrl->config[i];

        mask = (BIT(config->width) - 1) << config->shift;
        data = config->off_value << config->shift;

        val = readl(config->reg);
        val &= ~mask;
        val |= data & mask;
        writel(val, config->reg);
    }

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_FORCE_ENABLE) {
        __ref_enable_restore(ctrl->generic.pctrl.of_node, &d);
    }
    
    return 0;
}

static int rtk_is_powered_on(struct power_control *pctrl)
{
    struct rtk_powerctrl * ctrl = to_rtk_powerctrl(pctrl);
    int on = 0, off = 0, i;
    u32 val;
    unsigned long flags;

    if (ctrl->generic.flags & RTK_POWERCTRL_REF_MASK) {
        int ret = __ref_check(ctrl->generic.pctrl.of_node);
        if (ret)
            return ret;
    }

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    for (i = 0; i < ctrl->size; i++) {
        const struct rtk_powerctrl_config *config = &ctrl->config[i];

        val = readl(config->reg);
        if (val != 0xdeadbeef)
        {
            val = (val >> config->shift) & (BIT(config->width) - 1);
            if (val == config->on_value_st)
                on ++;
            if (val == config->off_value_st)
                off ++;
        }
    }

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    if (on == i)
        return 1;

    if (off == i)
        return 0;

    return -EINVAL;
}

static DEFINE_POWER_CONTROL_OPS(rtk);

static int __init init_rtk_powerctrl_simple(struct device_node *np)
{
    struct rtk_powerctrl *ctrl;
    int size, i, ret;
    u32 temp[20];
    
    /* get size of array */
    size = of_property_count_u32_elems(np, "width");
    
    ctrl = kzalloc(sizeof(*ctrl) + sizeof(struct rtk_powerctrl_config) * size, GFP_KERNEL);
    if (!ctrl)
        return -ENOMEM;

    ctrl->size = size;
    ctrl->generic.pctrl.ops     = &rtk_power_control_ops;
    ctrl->generic.pctrl.name    = np->name;
    ctrl->generic.pctrl.of_node = np;

    /* read propertise */
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].reg = of_iomap(np, i);

    ret = of_property_read_u32_array(np, "width", temp, size);
    if (ret)
        goto err;
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].width = temp[i];

    ret = of_property_read_u32_array(np, "shift", temp, size);
    if (ret)
        goto err;
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].shift = temp[i];

    ret = of_property_read_u32_array(np, "on-value", temp, size);
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].on_value = ret ? BIT(ctrl->config[i].width) - 1 : temp[i];

    ret = of_property_read_u32_array(np, "off-value", temp, size);
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].off_value = ret ? 0 : temp[i];

    ret = of_property_read_u32_array(np, "state,on-value", temp, size);
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].on_value_st = ret ? ctrl->config[i].on_value : temp[i];

    ret = of_property_read_u32_array(np, "state,off-value", temp, size);
    for (i = 0 ; i < ctrl->size; i++)
        ctrl->config[i].off_value_st = ret ? ctrl->config[i].off_value : temp[i];   

    if (of_find_property(np, "is-l4-icg", NULL))
        ctrl->generic.flags |= RTK_POWERCTRL_L4_ICG | RTK_POWERCTRL_REF_CHECK;
    if (of_find_property(np, "is-analog", NULL))
        ctrl->generic.flags |= RTK_POWERCTRL_ANALOG | RTK_POWERCTRL_REF_FORCE_ENABLE;
    else
        ctrl->generic.flags |= RTK_POWERCTRL_POWER_DOMAIN | RTK_POWERCTRL_REF_CHECK;;

    if (!of_find_property(np, "clocks", NULL) &&
        !of_find_property(np, "resets", NULL) &&
        !of_find_property(np, "powerctrls", NULL)) {
        ctrl->generic.flags &= ~RTK_POWERCTRL_REF_MASK;
    }

    /* use pctrl-name as name instead of node name*/
    if (of_property_read_string(np, "pctrl-name", &ctrl->generic.pctrl.name)) {
        ctrl->generic.pctrl.name  = np->name;
    }

    /* register */
    power_control_register(&ctrl->generic.pctrl);
    if (__is_l4_icg(ctrl->generic.flags))
        power_control_mark_as_hw_pm(&ctrl->generic.pctrl);

    /* set power state */
    rtk_powerctrl_generic_config(np, &ctrl->generic.pctrl);

    /* add list_pm */
    rtk_powerctrl_pm_config(&ctrl->generic.pctrl);

    return 0;
err:
    kfree(ctrl);
    return -EINVAL;
}

/********************************************************************************
 * rtk_powerctrl sram
 ********************************************************************************/
#define SRAM_MAX_RSTC 2

struct rtk_powerctrl_sram {
    struct rtk_powerctrl_generic generic;
    void __iomem * addr_iso_cell;
    void __iomem * addr_sram_pwr3;
    void __iomem * addr_sram_pwr4;

    u32  sram_pwr4_on_value;
    u32  sram_pwr4_off_value;
    u32  iso_cell_shift;

    unsigned int flags;

    u32  rstc_num;
    struct reset_control *rstc[SRAM_MAX_RSTC];
};

#define to_rtk_powerctrl_sram(_p) container_of((_p), struct rtk_powerctrl_sram, generic.pctrl)

static int rtk_sram_power_on(struct power_control *pctrl)
{
    struct rtk_powerctrl_sram * ctrl = to_rtk_powerctrl_sram(pctrl);
    u32 val;
    int i;
    unsigned long flags;

    pcm_power_on_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    /* trigger power on */
    writel(ctrl->sram_pwr4_on_value, ctrl->addr_sram_pwr4);

    /* reset logic modules */
    for (i = 0; i < ctrl->rstc_num; i++) 
        reset_control_assert(ctrl->rstc[i]);
    for (i = 0; i < ctrl->rstc_num; i++)
        reset_control_deassert(ctrl->rstc[i]);    

    /* disable isolation cell */
    if (ctrl->addr_iso_cell) {
        val = readl(ctrl->addr_iso_cell);
        val &= ~BIT(ctrl->iso_cell_shift);
        writel(val, ctrl->addr_iso_cell);
    }

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_sram_power_off(struct power_control *pctrl)
{
    struct rtk_powerctrl_sram *ctrl = to_rtk_powerctrl_sram(pctrl);
    u32 val;
    unsigned long flags;

    pcm_power_off_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    /* enable isolation cell */
    if (ctrl->addr_iso_cell) {
        val = readl(ctrl->addr_iso_cell);
        val |= BIT(ctrl->iso_cell_shift);
        writel(val, ctrl->addr_iso_cell);
    }

    /* set power-off auto mode */
    if (!strcmp(ctrl->generic.pctrl.of_node->name, "pctrl_ve2"))
        writel(1, ctrl->addr_sram_pwr3);
    else
        writel(0, ctrl->addr_sram_pwr3);

    /* trigger power-off */
    writel(ctrl->sram_pwr4_off_value, ctrl->addr_sram_pwr4);

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_sram_is_powered_on(struct power_control *pctrl)
{
    struct rtk_powerctrl_sram *ctrl = to_rtk_powerctrl_sram(pctrl);
    u32 val;
    int ret = -EINVAL;
    unsigned long flags;

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    val = readl(ctrl->addr_sram_pwr4);
    if ((val & 0x1) == (ctrl->sram_pwr4_off_value & 0x1))
        ret = 0;
    else if ((val & 0x1) == (ctrl->sram_pwr4_on_value & 0x1))
        ret = 1;

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return ret;
}

static DEFINE_POWER_CONTROL_OPS(rtk_sram);

static int __init init_rtk_powerctrl_sram(struct device_node *np)
{
    struct rtk_powerctrl_sram *ctrl;
    int i, num_of_names;

    /* get rtsc num */
    num_of_names = of_property_count_strings(np, "reset-names");
    if (num_of_names < 0) {
        num_of_names = 0;
    }
    BUG_ON(num_of_names > SRAM_MAX_RSTC);

    ctrl = kzalloc(sizeof(*ctrl), GFP_KERNEL);
    if (!ctrl)
        return -ENOMEM;

    ctrl->addr_iso_cell  = of_iomap(np, 0);
    ctrl->addr_sram_pwr3 = of_iomap(np, 1);
    ctrl->addr_sram_pwr4 = of_iomap(np, 2);
    ctrl->rstc_num       = num_of_names;
    ctrl->generic.pctrl.ops   = &rtk_sram_power_control_ops;
    ctrl->generic.pctrl.of_node = np;
    ctrl->generic.flags |= RTK_POWERCTRL_POWER_DOMAIN | RTK_POWERCTRL_SRAM;

    /* get properties */
    if (ctrl->addr_iso_cell && of_property_read_u32(np, "iso-cell-shift", &ctrl->iso_cell_shift))
        goto err;

    if (of_property_read_string(np, "pctrl-name", &ctrl->generic.pctrl.name)) {
        ctrl->generic.pctrl.name = np->name;
    }

    if (of_property_read_u32(np, "sram-pwr4-on-value", &ctrl->sram_pwr4_on_value) ||
        of_property_read_u32(np, "sram-pwr4-off-value", &ctrl->sram_pwr4_off_value)) {
        ctrl->sram_pwr4_on_value  = 0x00000f00;
        ctrl->sram_pwr4_off_value = 0x00000f01;
    }

    /* get reset control list */
    for (i = 0; i < num_of_names; i++) {
        const char *reset_name;
        if (!of_property_read_string_index(np, "reset-names", i, &reset_name)) {
            ctrl->rstc[i] = of_reset_control_get(np, reset_name);
            if (IS_ERR(ctrl->rstc)) {
                ctrl->rstc_num = i; // set current index for free mem
                pr_err("%s: get rstc failed, name = %s, index = %d\n", np->name, reset_name, i);
                goto err;                
            }
        }
    }

    /* register */
    power_control_register(&ctrl->generic.pctrl);

    /* set power state */
    rtk_powerctrl_generic_config(np, &ctrl->generic.pctrl);

    /* add to list_pm */
    rtk_powerctrl_pm_config(&ctrl->generic.pctrl);
    return 0;

err:
    for (i = 0; i < ctrl->rstc_num; i++)
        kfree(ctrl->rstc[i]);
    kfree(ctrl);
    return -EINVAL;
}

/********************************************************************************
 * rtk_powerctrl gpu_core
 ********************************************************************************/
struct rtk_powerctrl_gpu_core {
    struct rtk_powerctrl_generic generic;
    void __iomem * on_addr;
    void __iomem * off_addr;

    int last_call;

    u32  on_shift;
    u32  off_shift;
};

#define to_rtk_powerctrl_gpu_core(_p) container_of((_p), struct rtk_powerctrl_gpu_core, generic.pctrl)

static int rtk_gpu_core_power_on(struct power_control * pctrl)
{
    struct rtk_powerctrl_gpu_core * ctrl = to_rtk_powerctrl_gpu_core(pctrl);
    u32 val;
    unsigned long flags;

    //pcm_power_on_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    ctrl->last_call = 1;

    val = readl(ctrl->on_addr);
    val |= 1 << ctrl->on_shift;
    writel(val, ctrl->on_addr);

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_gpu_core_power_off(struct power_control * pctrl)
{
    struct rtk_powerctrl_gpu_core * ctrl = to_rtk_powerctrl_gpu_core(pctrl);
    u32 val;
    unsigned long flags;

    //pcm_power_off_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);
    
    ctrl->last_call = 0;

    val = readl(ctrl->off_addr);
    val |= 1 << ctrl->off_shift;
    writel(val, ctrl->off_addr);

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_gpu_core_is_powered_on(struct power_control * pctrl) 
{
    struct rtk_powerctrl_gpu_core * ctrl = to_rtk_powerctrl_gpu_core(pctrl);    
    int ret;

    ret = ctrl->last_call;

    return ret;
}

static DEFINE_POWER_CONTROL_OPS(rtk_gpu_core);

static int __init init_rtk_powerctrl_gpu_core(struct device_node *np)
{
    struct rtk_powerctrl_gpu_core *ctrl;

    ctrl = kzalloc(sizeof(*ctrl), GFP_KERNEL);

    ctrl->on_addr     = of_iomap(np, 0);
    ctrl->off_addr    = of_iomap(np, 1);
    ctrl->generic.pctrl.of_node = np;
    ctrl->generic.pctrl.ops     = &rtk_gpu_core_power_control_ops;
    ctrl->generic.flags         |= RTK_POWERCTRL_POWER_DOMAIN | RTK_POWERCTRL_GPU_CORE;

    /* get properties */
    if (of_property_read_u32(np, "on-shift", &ctrl->on_shift) ||
        of_property_read_u32(np, "off-shift", &ctrl->off_shift))
        goto err;

    if (of_property_read_string(np, "pctrl-name", &ctrl->generic.pctrl.name)) {
        ctrl->generic.pctrl.name = np->name;
    }

    /* register */
    power_control_register(&ctrl->generic.pctrl);

    /* set power state */
    rtk_powerctrl_generic_config(np, &ctrl->generic.pctrl);

    /* save info for pm */
    rtk_powerctrl_pm_config(&ctrl->generic.pctrl);
    return 0;
err:
    kfree(ctrl);
    return -EINVAL;
}

/********************************************************************************
 * rtk_powerctrl_sram_ch
 ********************************************************************************/

struct rtk_powerctrl_sram_ch_shared {
    void __iomem * mux_reg;
    void __iomem * sd_reg;
};

struct rtk_powerctrl_sram_ch {
    struct rtk_powerctrl_generic generic;
    struct rtk_powerctrl_sram_ch_shared *shared;
    unsigned int mask;
};
#define to_rtk_powerctrl_sram_ch(_p) container_of((_p), struct rtk_powerctrl_sram_ch, generic.pctrl)

static int rtk_sram_ch_power_on(struct power_control * pctrl)
{
    struct rtk_powerctrl_sram_ch * ctrl = to_rtk_powerctrl_sram_ch(pctrl);
    unsigned int val;
    unsigned long flags;

    pcm_power_on_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    val = readl(ctrl->shared->sd_reg);
    val &= ~ctrl->mask;
    writel(val, ctrl->shared->sd_reg);

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_sram_ch_power_off(struct power_control * pctrl)
{
    struct rtk_powerctrl_sram_ch * ctrl = to_rtk_powerctrl_sram_ch(pctrl);
    unsigned int val;
    unsigned long flags;

    pcm_power_off_hint(&ctrl->generic.flags);

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    val = readl(ctrl->shared->mux_reg);
    val |= ctrl->mask;
    writel(val, ctrl->shared->mux_reg);

    val = readl(ctrl->shared->sd_reg);
    val |= ctrl->mask;
    writel(val, ctrl->shared->sd_reg);

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);

    return 0;
}

static int rtk_sram_ch_is_powered_on(struct power_control * pctrl)
{
    struct rtk_powerctrl_sram_ch * ctrl = to_rtk_powerctrl_sram_ch(pctrl);
    int ret;
    unsigned int val;
    unsigned long flags;

    spin_lock_irqsave(&rtk_powerctrl_lock, flags);

    val = readl(ctrl->shared->sd_reg);
    ret = (val & ctrl->mask) == 0;

    spin_unlock_irqrestore(&rtk_powerctrl_lock, flags);    

    return ret;
}

static DEFINE_POWER_CONTROL_OPS(rtk_sram_ch);

static __init int init_rtk_powerctrl_sram_ch(struct device_node *np)
{
    struct device_node *child;
    struct rtk_powerctrl_sram_ch_shared *shared;
    int ref = 0;
    int ret = 0;

    shared = kzalloc(sizeof(*shared), GFP_KERNEL);
    shared->mux_reg = of_iomap(np, 1);
    shared->sd_reg = of_iomap(np, 0);

    for_each_child_of_node(np, child) {
        struct rtk_powerctrl_sram_ch *ctrl;

        ctrl = kzalloc(sizeof(*ctrl), GFP_KERNEL);
        if (!ctrl) {
            ret = -ENOMEM;
            break;
        }

        if (of_property_read_u32(child, "mask", &ctrl->mask))
            goto free_sub_node;

        if (of_property_read_string(child, "pctrl-name", &ctrl->generic.pctrl.name)) 
            ctrl->generic.pctrl.name = child->name;

        ctrl->shared = shared;
        ctrl->generic.pctrl.of_node = child;
        ctrl->generic.pctrl.ops     = &rtk_sram_ch_power_control_ops;
        ctrl->generic.flags         |= RTK_POWERCTRL_POWER_DOMAIN | RTK_POWERCTRL_SRAM_CH;

        /* register */
        power_control_register(&ctrl->generic.pctrl);

        /* set power state */
        rtk_powerctrl_generic_config(child, &ctrl->generic.pctrl);

        /* save info for pm */
        rtk_powerctrl_pm_config(&ctrl->generic.pctrl);

        ref ++;
        continue;
free_sub_node:
        kfree(ctrl);
    }

    if (!ref)
        kfree(shared);
    return ret;
}

/********************************************************************************
 * Set Register Once at beginning of Boot :
 *   power off the devices that are already using reset controller & common 
 *   clk framework in their drivers.
 ********************************************************************************/
static __init int init_rtk_powerctrl_once(struct device_node *np)
{
    void __iomem * reg = of_iomap(np, 0);
    int ret = 0;
    u32 mask;
    u32 val, old_val;

    if (of_property_read_u32(np, "mask", &mask)) {
        pr_err("%s: can't read mask\n", np->name);
        ret = -EINVAL;
        goto out;
    }

    if (of_get_power_state(np) == POWER_OFF) {
        const char *str;

        old_val = val = readl(reg);
        val &= ~mask; 
        writel(val, reg);

        str = strrchr(np->full_name, '@');
        if (!str)
            str = np->name;
        else 
            str += 1;

        pr_info("%s: value [0x%08x -> 0x%08x], clear-bits 0x%08x, mask 0x%08x\n",
            str, old_val, val, old_val ^ val, mask);
    }

out:
    of_node_put(np);
    return ret;    
}

static const struct of_device_id rtk_powerctrl_match[] = {
    {.compatible = "realtek,powerctrl-once",      .data = init_rtk_powerctrl_once},
    {.compatible = "realtek,powerctrl-simple",    .data = init_rtk_powerctrl_simple},
    {.compatible = "realtek,powerctrl-sram",      .data = init_rtk_powerctrl_sram}, 
    {.compatible = "realtek,powerctrl-gpu-core",  .data = init_rtk_powerctrl_gpu_core},
    {.compatible = "realtek,powerctrl-sram-ch",   .data = init_rtk_powerctrl_sram_ch},
    {.compatible = "realtek,powerctrl-timer",     .data = init_rtk_powerctrl_timer},
    {}
};

static int __init rtk_init_powerctrl(void)
{
    struct device_node *np;
    const struct of_device_id *match;
    int ret = 0;
    int chip_rev = get_rtd129x_cpu_revision() >> 16;

#ifdef CONFIG_POWER_CONTROL_RTD129x_DEBUGFS
    power_control_root = debugfs_create_dir("rtk_powerctrl", NULL);
#endif

    for_each_matching_node_and_match(np, rtk_powerctrl_match, &match) {
        int (*func)(struct device_node *) = match->data;

        if (!of_device_is_available(np))
            continue;
        
        if (!of_check_revision(np, chip_rev)) {
            pr_info("%s: rev not match\n", np->name);
            continue;
        }

        ret = func(np);
        if (ret)
            pr_err("%s: Failed to init, ret = %d\n", np->name, ret);
    }

    rtk_powerctrl_workqueue = create_workqueue("powerctrl");
    queue_delayed_work(rtk_powerctrl_workqueue, &pcm_work, PCM_DELAY_TIME_SEC * HZ);

#ifdef CONFIG_PM
    cpu_pm_register_notifier(&pm_notifier_block);
#endif

    pcm_period = POWERCTRL_INIT;

    return 0;
}
early_initcall(rtk_init_powerctrl);

