#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/regmap.h>

#include <linux/fs.h>
#include <linux/file.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/suspend.h>
#include <soc/realtek/rtd129x_cpu.h>

#define GMT_G2227_DEBUG      0
#define GMT_G2227_MAX_REG    0x21

#if GMT_G2227_DEBUG
#define g2227_debug(...)   pr_err(__VA_ARGS__);
#else
#define g2227_debug(...)
#endif

struct rtk_pmic_dev {
    struct i2c_client *client;
    struct device *dev;
    unsigned int num_regulators;
    struct regmap *regmap;
    int chip_version;
};

struct regulator_reg_field {
    unsigned int reg;
    unsigned int mask;
};

static int inline __regmap_regulator_reg_field_read(struct regmap *regmap, 
    struct regulator_reg_field *rfield, unsigned int *val)
{
    int ret;
    unsigned int _val;
    ret = regmap_read(regmap, rfield->reg, &_val);
    if (ret) {
        pr_err("%s: Failed to read reg = 0x%02x: err = %d\n", __func__, rfield->reg, ret);
        return ret;
    }
                
    _val = (_val & rfield->mask) >> (ffs(rfield->mask) - 1);
    *val = _val;
    return 0;
}

static int inline __regmap_regulator_reg_field_write(struct regmap *regmap,
    struct regulator_reg_field *rfield, unsigned int val)
{
    int ret;
    val <<= ffs(rfield->mask) - 1;
    ret = regmap_update_bits(regmap, rfield->reg, rfield->mask, val);
    if (ret) {
        pr_err("%s: Failed to write reg = 0x%02x, val = 0x%02x: err = %d\n", 
            __func__, rfield->reg, val, ret);
    }
    return ret;
}


/* global regulator reg fields */
enum GLOBAL_REGULATOR_REG_FIELD {
    GRRF_DCDC4_SLP_MODE,
    GRRF_SYS_SOFTOFF,
    GRRF_PWRKEY_LPOFF_TO_DO,
    GRRF_PWRKEY_TIME_LPOFF,
    GRRF_MAX /* LAST ONE */
};

/* per regulator reg fields */
enum REGULATOR_REG_FIELD {
    PRRF_NRM_MODE,
    PRRF_SLP_VSEL,
    PRRF_SLP_MODE,
    PRRF_MAX /* LAST ONE */
};

struct gmt_g2227_desc {
    struct regulator_desc desc;
    struct regulator_reg_field fields[PRRF_MAX];
    struct regulator_dev *rdev;
    int volup_delay;
};

#define to_gmt_g2227_desc(_desc) container_of(_desc, struct gmt_g2227_desc, desc)

enum g2227_regulator_id {
	G2227_ID_DCDC1 = 0,
	G2227_ID_DCDC2,
	G2227_ID_DCDC3,
	G2227_ID_DCDC4,
	G2227_ID_DCDC5,
	G2227_ID_DCDC6,
	G2227_ID_LDO2,
	G2227_ID_LDO3,
};

static const unsigned int g2227_dcdc1_vtbl[] = {
	3000000, 3100000, 3200000, 3300000,
};

static const unsigned int g2227_dcdc2_vtbl[] = {
	800000, 812500, 825000, 837500, 850000, 862500, 875000, 887500,
	900000, 912500, 925000, 937500, 950000, 962500, 975000, 987500,
	1000000, 1012500, 1025000, 1037500, 1050000, 1062500, 1075000, 1087500,
	1100000, 1112500, 1125000, 1137500, 1150000, 1162500, 1175000, 1187500,
};

static const unsigned int g2227_ldo_vtbl[] = {
	800000, 850000, 900000, 950000, 1000000, 1100000, 1200000, 1300000,
	1500000, 1600000, 1800000, 1900000, 2500000, 2600000, 3000000, 3100000,
};

#define to_bytep(_p)  ((unsigned char *)(_p))

static int gmt_g2227_i2c_write(void *context, const void *data, size_t count)
{
    struct i2c_client *i2c = context;
    int ret = 0;

    g2227_debug("%s: reg = %02x val = %02x\n", __func__, 
        to_bytep(data)[0], to_bytep(data)[1]);

    ret = i2c_master_send(i2c, data, count);

    if (ret == count)
        return 0;
    else if (ret < 0)
        return ret;
    else
        return -EIO;
}

static int gmt_g2227_i2c_read(void *context, const void *reg, size_t reg_size, 
    void *val, size_t val_size)
{
    struct i2c_client *i2c = context;
    unsigned char _reg = *(unsigned char *)reg;
    struct i2c_msg xfer[2];
    int ret = 0;
    
    xfer[0].addr = i2c->addr;
    xfer[0].flags = 0;
    xfer[0].len = reg_size;
    xfer[0].buf = &_reg;
    
    xfer[1].addr = i2c->addr;
    xfer[1].flags = I2C_M_RD;
    xfer[1].len = val_size;
    xfer[1].buf = val;

    ret = i2c_transfer(i2c->adapter, xfer, 2);

    g2227_debug("%s: reg = %02x val = %02x ret = %d\n", __func__, 
        _reg, to_bytep(val)[0], ret);

    if (ret == 2)
        return 0;
    else if (ret < 0)
        return ret;
    else
        return -EIO;
}
static struct regmap_bus gmt_g2227_regmap_bus = {
    .write = gmt_g2227_i2c_write,
    .read  = gmt_g2227_i2c_read,
};

static int gmt_g2227_set_mode_regmap(struct regulator_dev *rdev, unsigned int mode)
{
    struct gmt_g2227_desc *gdesc = to_gmt_g2227_desc(rdev->desc);
    unsigned int val = 0;

    if (mode & REGULATOR_MODE_FAST) 
        val = 2;

    return __regmap_regulator_reg_field_write(rdev->regmap,
        &gdesc->fields[PRRF_NRM_MODE], val);
}

static unsigned int gmt_g2227_get_mode_regmap(struct regulator_dev *rdev)
{
    struct gmt_g2227_desc *gdesc = to_gmt_g2227_desc(rdev->desc);
    unsigned int val;
    int ret;

    ret = __regmap_regulator_reg_field_read(rdev->regmap, 
        &gdesc->fields[PRRF_NRM_MODE], &val);
    if (ret)
        return 0;

    return val == 2 ? REGULATOR_MODE_FAST : REGULATOR_MODE_NORMAL;
}

static int gmt_g2227_set_voltage_time_sel(struct regulator_dev *rdev,
    unsigned int old_selector, unsigned int new_selector)
{
    struct gmt_g2227_desc *gdesc = to_gmt_g2227_desc(rdev->desc);

    if (new_selector > old_selector)
        return gdesc->volup_delay;
    return 0;
}


static struct regulator_ops g2227_regulator_ops  = {
    .list_voltage    = regulator_list_voltage_table,
    .map_voltage     = regulator_map_voltage_iterate,
    .set_voltage_sel = regulator_set_voltage_sel_regmap,
    .get_voltage_sel = regulator_get_voltage_sel_regmap,
    .enable          = regulator_enable_regmap,
    .disable         = regulator_disable_regmap,
    .is_enabled      = regulator_is_enabled_regmap,
    .get_mode        = gmt_g2227_get_mode_regmap,
    .set_mode        = gmt_g2227_set_mode_regmap,
    .set_voltage_time_sel = gmt_g2227_set_voltage_time_sel,
};

/* 
 * parse dt-bindings 
 */
static int gmt_g2227_parse_dt(struct device_node *np,
    const struct regulator_desc *desc,
    struct regulator_config *config)
{
    return 0;
}

static unsigned int gmt_g2227_of_map_mode(unsigned int mode)
{
    return mode ? REGULATOR_MODE_FAST : REGULATOR_MODE_NORMAL;
}

#define _SET_PRRF(_id, _reg, _mask) \
    .fields[_id] = { .reg = _reg, .mask = _mask }

/* helper macro to fill desc */
#define DESC(_name, _id, _tbl, _vu_delay,   \
    _vsel_reg,     _vsel_mask,     \
    _enable_reg,   _enable_mask,   \
    _mode_reg,     _mode_mask,     \
    _slp_vsel_reg, _slp_vsel_mask, \
    _slp_mode_reg, _slp_mode_mask  \
){                                              \
    .desc.owner       = THIS_MODULE,            \
    .desc.name        = _name,                  \
    .desc.id          = _id,                    \
    .desc.ops         = &g2227_regulator_ops,   \
    .desc.type        = REGULATOR_VOLTAGE,      \
    .desc.of_match    = _name,                  \
    .desc.of_parse_cb = gmt_g2227_parse_dt,     \
    .desc.of_map_mode = gmt_g2227_of_map_mode,  \
    .desc.volt_table  = _tbl,                   \
    .desc.n_voltages  = ARRAY_SIZE(_tbl),       \
    .desc.vsel_reg    = _vsel_reg,              \
    .desc.vsel_mask   = _vsel_mask,             \
    .desc.enable_reg  = _enable_reg,            \
    .desc.enable_mask = _enable_mask,           \
    _SET_PRRF(PRRF_NRM_MODE, _mode_reg, _mode_mask),         \
    _SET_PRRF(PRRF_SLP_VSEL, _slp_vsel_reg, _slp_vsel_mask), \
    _SET_PRRF(PRRF_SLP_MODE, _slp_mode_reg, _slp_mode_mask), \
    .volup_delay = _vu_delay,                   \
}

static struct gmt_g2227_desc g2227_desc[] = {
    DESC("dcdc1", G2227_ID_DCDC1, g2227_dcdc1_vtbl, 0,
        0x13, 0xC0, /* Normal Vsel */ 
        0x05, 0x80, /* Normal Enable */
        0x07, 0xC0, /* Normal Mode */
        0x18, 0xC0, /* Sleep Vsel */
        0x07, 0x30  /* Sleep Mode */
    ),
    DESC("dcdc2", G2227_ID_DCDC2, g2227_dcdc2_vtbl, 0,
        0x10, 0x1F, /* Normal Vsel */
        0x05, 0x40, /* Normal Enable */
        0x07, 0x0C, /* Normal Mode */
        0x15, 0x1F, /* Sleep Vsel */
        0x07, 0x03  /* Sleep Mode */
    ),
    DESC("dcdc3", G2227_ID_DCDC3, g2227_dcdc2_vtbl, 40,
        0x11, 0x1F, /* Normal Vsel */
        0x05, 0x20, /* Normal Enable */
        0x08, 0xC0, /* Normal Mode */
        0x16, 0x1F, /* Sleep Vsel */
        0x08, 0x30  /* Sleep Mode */
    ),
    DESC("dcdc5", G2227_ID_DCDC5, g2227_dcdc2_vtbl, 0,
        0x12, 0x1F, /* Normal Vsel */
        0x05, 0x08, /* Normal Enable */
        0x09, 0xC0, /* Normal Mode */
        0x17, 0x1F, /* Sleep Vsel */
        0x09, 0x30  /* Sleep Mode */
    ),
    DESC("dcdc6", G2227_ID_DCDC6, g2227_dcdc2_vtbl, 0,
        0x13, 0x1F, /* Normal Vsel */
        0x05, 0x04, /* Normal Enable */
        0x09, 0x0C, /* Normal Mode */
        0x18, 0x1F, /* Sleep Vsel */
        0x09, 0x03  /* Sleep Mode */
    ),
    DESC("ldo2", G2227_ID_LDO2, g2227_ldo_vtbl, 0,
        0x14, 0xF0, /* Normal Vsel */
        0x05, 0x02, /* Normal Enable */
        0x0A, 0xC0, /* Normal Mode */
        0x19, 0xF0, /* Sleep Vsel */
        0x0A, 0x30  /* Sleep Mode */
    ),
    DESC("ldo3", G2227_ID_LDO3, g2227_ldo_vtbl, 0,
        0x14, 0x0F, /* Normal Vsel */
        0x05, 0x01, /* Normal Enable */
        0x0A, 0x0C, /* Normal Mode */
        0x19, 0x0F, /* Sleep Vsel */
        0x0A, 0x03  /* Sleep Mode */
    ),
};

#define _SET_GRRF(_id, _reg, _mask) \
    [_id] = { .reg = _reg, .mask = _mask }

static struct regulator_reg_field gfields [] = {
    _SET_GRRF(GRRF_PWRKEY_TIME_LPOFF,  0x01, 0x03),
    _SET_GRRF(GRRF_PWRKEY_LPOFF_TO_DO, 0x01, 0x80),
    _SET_GRRF(GRRF_SYS_SOFTOFF,        0x04, 0x80),
    _SET_GRRF(GRRF_DCDC4_SLP_MODE,     0x08, 0x03),
};

enum {
    VOLATILE_FIRST   = 0,
    VOLATILE_CLEARED = 1,
    VOLATILE_ALWAYS  = 2,
};
static int __volatile_reg_val[GMT_G2227_MAX_REG];

static bool gmt_g2227_volatile_reg(struct device *dev, unsigned int reg)
{
    if (likely(__volatile_reg_val[reg] == VOLATILE_CLEARED))
        return false;

    if (__volatile_reg_val[reg] == VOLATILE_ALWAYS)
        return true;

    /* if reg is uncached, mark it as volatile */
    __volatile_reg_val[reg] = VOLATILE_CLEARED;
    return true;
}

static const struct regmap_config gmt_g2227_regmap = {
    .reg_bits = 8,
    .val_bits = 8,
    .max_register = GMT_G2227_MAX_REG,
    .cache_type   = REGCACHE_FLAT,
    .volatile_reg = gmt_g2227_volatile_reg,
};

static int gmt_g2227_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct rtk_pmic_dev *pmic_dev;
	struct regulator_dev *rdev;
	int i, ret;
	
	pmic_dev = devm_kzalloc(&client->dev, sizeof(struct rtk_pmic_dev), GFP_KERNEL);
	if (!pmic_dev)
		return -ENOMEM;

    __volatile_reg_val[0x01] = VOLATILE_ALWAYS;

    pmic_dev->regmap = devm_regmap_init(&client->dev, &gmt_g2227_regmap_bus, client, &gmt_g2227_regmap);
	pmic_dev->client = client;
	pmic_dev->dev    = &client->dev;
	pmic_dev->chip_version = get_rtd129x_cpu_revision();
	pmic_dev->num_regulators = ARRAY_SIZE(g2227_desc);
	for (i = 0; i < pmic_dev->num_regulators; i++) {
		struct regulator_desc *desc = &g2227_desc[i].desc;
        struct regulator_config cfg = {};

        pr_info("%s: init\n",  desc->name);

		cfg.dev = &client->dev;
		cfg.driver_data = pmic_dev;
        cfg.regmap = pmic_dev->regmap;

		rdev = devm_regulator_register(&client->dev, desc, &cfg);
		if (IS_ERR(rdev)) {
            ret = PTR_ERR(rdev);
			dev_err(&client->dev, "Failed to register %s: %d\n", desc->name, ret);
			return ret;
		}
        
        g2227_desc[i].rdev = rdev;

        /* all for mode change */
        rdev->constraints->valid_modes_mask |= REGULATOR_MODE_NORMAL | REGULATOR_MODE_FAST;
        rdev->constraints->valid_ops_mask |= REGULATOR_CHANGE_MODE;


        if (desc->id == G2227_ID_DCDC6 && (
            pmic_dev->chip_version == RTD129x_CHIP_REVISION_A00 ||
            pmic_dev->chip_version == RTD129x_CHIP_REVISION_B00)) {
            dev_info(&rdev->dev, "%s: no slpmode\n", rdev->desc->name);
            rdev->constraints->state_mem.disabled = 0;
		}
	}

	i2c_set_clientdata(client, pmic_dev);

    __regmap_regulator_reg_field_write(pmic_dev->regmap,
        &gfields[GRRF_PWRKEY_LPOFF_TO_DO], 0);
    __regmap_regulator_reg_field_write(pmic_dev->regmap,
        &gfields[GRRF_PWRKEY_TIME_LPOFF], 2);
	return 0;
}

static struct rtk_pmic_dev * __pmic_dev;
extern void (*rtk_do_poweroff)(void);

static void gmt_g2227_do_poweroff(void)
{
    pr_err("[PMIC] %s: poweroff\n", __func__);
    __regmap_regulator_reg_field_write(__pmic_dev->regmap,
        &gfields[GRRF_SYS_SOFTOFF], 0x1);
}

static void gmt_g2227_shutdown(struct i2c_client *client)
{
    struct rtk_pmic_dev *pmic_dev = i2c_get_clientdata(client);

    pr_err("[PMIC] %s: prepare poweroff\n", __func__);
    __pmic_dev = pmic_dev;
    rtk_do_poweroff = gmt_g2227_do_poweroff;
}

#ifdef CONFIG_SUSPEND
static int get_suspend_mode(void)
{
	struct file *fp;
	mm_segment_t oldfs;
	char buf[64], mode[64];
	int i, cnt, startcp=0, ret=0;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	fp = filp_open("/sys/kernel/suspend/mode", O_RDONLY, 0);
	if(fp) {
		cnt = fp->f_op->read(fp, buf, 64, &fp->f_pos);
		for(i=0; i<cnt; i++) {
			if(buf[i]=='=' && buf[i+1]=='>') {
				i=i+3;
				startcp = i;
			}
			if(startcp) {
				if(buf[i]!='\n')
					mode[i-startcp] = buf[i];
				else {
					mode[i-startcp] = '\0';
					break;
				}
			}
		}
		if(strcmp("coolboot", mode)==0)
			ret=1;
	}

	set_fs(oldfs);
	return ret;
}

static int gmt_g2227_suspend(struct device *dev)
{
	struct rtk_pmic_dev *pmic_dev = dev_get_drvdata(dev);
	int i, mode = 0;

	printk("[PMIC] Enter %s\n", __FUNCTION__);

	mode = get_suspend_mode();
	if (RTK_PM_STATE == PM_SUSPEND_STANDBY)
        goto done;
    
    for (i = 0; i < ARRAY_SIZE(g2227_desc); i++) {
        struct gmt_g2227_desc *gdesc = &g2227_desc[i];
        struct regulator_state *state = &gdesc->rdev->constraints->state_mem;

        if (state->disabled) {
            __regmap_regulator_reg_field_write(pmic_dev->regmap,
                &gdesc->fields[PRRF_SLP_MODE], 0x3);
        } else if (state->uV != 0) {
            int vsel = regulator_map_voltage_iterate(gdesc->rdev, state->uV, state->uV);
            if (vsel < 0) {
                dev_err(&gdesc->rdev->dev, "Failed to map voltage %d: err = %d\n",
                    state->uV, vsel);
                continue;
            }
            __regmap_regulator_reg_field_write(pmic_dev->regmap,
                &gdesc->fields[PRRF_SLP_VSEL], vsel);
        } 
    }

    /* if suspend_to_coldboot set dcdc4 to slpmode */
    __regmap_regulator_reg_field_write(pmic_dev->regmap,
        &gfields[GRRF_DCDC4_SLP_MODE], mode ? 0x3 : 0x1);

done:
	printk("[PMIC] Exit %s\n", __FUNCTION__);
	return 0;
}

static int gmt_g2227_resume(struct device *dev)
{
	printk("[PMIC] Enter %s\n", __FUNCTION__);

	printk("[PMIC] Exit %s\n", __FUNCTION__);
	return 0;
}
#else

#define gmt_g2227_suspend NULL
#define gmt_g2227_resume NULL

#endif

static const struct dev_pm_ops gmt_g2227_pm_ops = {
	.suspend    = gmt_g2227_suspend,
	.resume     = gmt_g2227_resume,
};

static const struct i2c_device_id gmt_g2227_ids[] = {
	{"gmt-g2227", 0},
	{ },
};
MODULE_DEVICE_TABLE(i2c, gmt_g2227_ids);

static struct i2c_driver gmt_g2227_driver = {
	.driver = {
		.name = "gmt-g2227",
		.owner = THIS_MODULE,
		.pm = &gmt_g2227_pm_ops,
	},
    .id_table = gmt_g2227_ids,
    .probe    = gmt_g2227_probe,
    .shutdown = gmt_g2227_shutdown,
};

module_i2c_driver(gmt_g2227_driver);

MODULE_DESCRIPTION("RTD1295 Giraffe PMU Regulator Driver");
MODULE_AUTHOR("Simon Hsu");
MODULE_AUTHOR("CY Lee <cylee12@realtek.com>");
MODULE_LICENSE("GPL");
