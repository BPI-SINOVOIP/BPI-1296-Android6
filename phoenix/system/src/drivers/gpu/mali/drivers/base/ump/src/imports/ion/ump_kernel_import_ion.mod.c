#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfd490ea0, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xe80e7af9, __VMLINUX_SYMBOL_STR(ump_import_module_unregister) },
	{ 0x8f1a66bb, __VMLINUX_SYMBOL_STR(ump_import_module_register) },
	{ 0x26714f2f, __VMLINUX_SYMBOL_STR(ion_client_destroy) },
	{ 0x86974176, __VMLINUX_SYMBOL_STR(ion_client_create) },
	{ 0x9b4da327, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x5e3dd681, __VMLINUX_SYMBOL_STR(__module_get) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x4fa6228f, __VMLINUX_SYMBOL_STR(ump_dd_create_from_phys_blocks_64) },
	{ 0xd6ee688f, __VMLINUX_SYMBOL_STR(vmalloc) },
	{ 0x3fec048f, __VMLINUX_SYMBOL_STR(sg_next) },
	{ 0x5d3b0128, __VMLINUX_SYMBOL_STR(ion_sg_table) },
	{ 0xc5e9c169, __VMLINUX_SYMBOL_STR(ion_import_dma_buf) },
	{ 0x949fa7d1, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xecf40a9b, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0x484a2a4, __VMLINUX_SYMBOL_STR(xen_dma_ops) },
	{ 0x18fef9cb, __VMLINUX_SYMBOL_STR(xen_start_info) },
	{ 0x731dba7a, __VMLINUX_SYMBOL_STR(xen_domain_type) },
	{ 0x7c1372e8, __VMLINUX_SYMBOL_STR(panic) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x8ed5bdcd, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x6f189d7a, __VMLINUX_SYMBOL_STR(ion_free) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ump";


MODULE_INFO(srcversion, "28B87B3528B44DBAB2BEA6B");
