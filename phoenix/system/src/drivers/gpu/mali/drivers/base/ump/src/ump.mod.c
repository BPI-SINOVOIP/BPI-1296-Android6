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
	{ 0xfe6efc5c, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x9b4da327, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xd7542827, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
	{ 0xfbc74f64, __VMLINUX_SYMBOL_STR(__copy_from_user) },
	{ 0x1a4752a5, __VMLINUX_SYMBOL_STR(up_read) },
	{ 0x51eafc8e, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x55074282, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x36beb31e, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xa87cf413, __VMLINUX_SYMBOL_STR(clear_bit) },
	{ 0x230ec0fa, __VMLINUX_SYMBOL_STR(__alloc_pages_nodemask) },
	{ 0xc2d64c6a, __VMLINUX_SYMBOL_STR(down_read) },
	{ 0x62a79a6c, __VMLINUX_SYMBOL_STR(param_ops_charp) },
	{ 0xdcb764ad, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xdf3cbf95, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x7c1372e8, __VMLINUX_SYMBOL_STR(panic) },
	{ 0x479c3c86, __VMLINUX_SYMBOL_STR(find_next_zero_bit) },
	{ 0xf8e398fc, __VMLINUX_SYMBOL_STR(memstart_addr) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0x5d3d3fba, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xfab41eb4, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xef761b7f, __VMLINUX_SYMBOL_STR(up_write) },
	{ 0xe4755540, __VMLINUX_SYMBOL_STR(down_write) },
	{ 0x92a8274, __VMLINUX_SYMBOL_STR(contig_page_data) },
	{ 0x6bcb3eba, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x8ed5bdcd, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0x84bb7182, __VMLINUX_SYMBOL_STR(__free_pages) },
	{ 0xb362f284, __VMLINUX_SYMBOL_STR(ump_ops_register) },
	{ 0x949fa7d1, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x884ff8d0, __VMLINUX_SYMBOL_STR(vm_insert_mixed) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x4829a47e, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x68733f91, __VMLINUX_SYMBOL_STR(flush_cache_all) },
	{ 0xae8c4d0c, __VMLINUX_SYMBOL_STR(set_bit) },
	{ 0xb1fd5b7b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xc3562aef, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xd74bb6b1, __VMLINUX_SYMBOL_STR(__init_rwsem) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xe099825d, __VMLINUX_SYMBOL_STR(try_module_get) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B754E2AC2C8A3B4AD270102");
