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
	{ 0x51eafc8e, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x52cadbb6, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0xfe6efc5c, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x2f1572a2, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0xb1fd5b7b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x55074282, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x6bcb3eba, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xd7542827, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xfab41eb4, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xc3562aef, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xba8eb57f, __VMLINUX_SYMBOL_STR(usb_driver_release_interface) },
	{ 0xac1a55be, __VMLINUX_SYMBOL_STR(unregister_reboot_notifier) },
	{ 0x7681946c, __VMLINUX_SYMBOL_STR(unregister_pm_notifier) },
	{ 0x3517383e, __VMLINUX_SYMBOL_STR(register_reboot_notifier) },
	{ 0x9cc4f70a, __VMLINUX_SYMBOL_STR(register_pm_notifier) },
	{ 0xa58dee99, __VMLINUX_SYMBOL_STR(_raw_write_unlock) },
	{ 0xdf3cbf95, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x1ea06663, __VMLINUX_SYMBOL_STR(_raw_write_lock) },
	{ 0xf98f9e1c, __VMLINUX_SYMBOL_STR(usb_driver_claim_interface) },
	{ 0x8908dd1, __VMLINUX_SYMBOL_STR(usb_ifnum_to_if) },
	{ 0xab40cca9, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x5ea03874, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xd1731b11, __VMLINUX_SYMBOL_STR(usb_disable_autosuspend) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0xd6ee688f, __VMLINUX_SYMBOL_STR(vmalloc) },
	{ 0x4ffe5c4a, __VMLINUX_SYMBOL_STR(release_firmware) },
	{ 0x3d7f6c6f, __VMLINUX_SYMBOL_STR(request_firmware) },
	{ 0xdb6ca3e1, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0x95c3f60b, __VMLINUX_SYMBOL_STR(vfs_read) },
	{ 0xf919522b, __VMLINUX_SYMBOL_STR(filp_open) },
	{ 0xe3ea0194, __VMLINUX_SYMBOL_STR(usb_get_from_anchor) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x8fa8f791, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irq) },
	{ 0x20ffa7f6, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irq) },
	{ 0x31f3fb, __VMLINUX_SYMBOL_STR(usb_interrupt_msg) },
	{ 0x7bdd2a23, __VMLINUX_SYMBOL_STR(usb_control_msg) },
	{ 0xcd64dac5, __VMLINUX_SYMBOL_STR(nonseekable_open) },
	{ 0x36beb31e, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x5d3d3fba, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x97fdbab9, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x96220280, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0x9c5bc552, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0xcb128141, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xdcb764ad, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xd0157bd9, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0xfbc74f64, __VMLINUX_SYMBOL_STR(__copy_from_user) },
	{ 0x87ce3c09, __VMLINUX_SYMBOL_STR(usb_set_interface) },
	{ 0xae8c4d0c, __VMLINUX_SYMBOL_STR(set_bit) },
	{ 0xd3259d65, __VMLINUX_SYMBOL_STR(test_and_set_bit) },
	{ 0xd2ab202f, __VMLINUX_SYMBOL_STR(usb_scuttle_anchored_urbs) },
	{ 0xd97792be, __VMLINUX_SYMBOL_STR(usb_autopm_put_interface) },
	{ 0xaafbce4c, __VMLINUX_SYMBOL_STR(usb_autopm_get_interface) },
	{ 0xa87cf413, __VMLINUX_SYMBOL_STR(clear_bit) },
	{ 0x88bfa7e, __VMLINUX_SYMBOL_STR(cancel_work_sync) },
	{ 0x9a908b80, __VMLINUX_SYMBOL_STR(test_and_clear_bit) },
	{ 0x9a923583, __VMLINUX_SYMBOL_STR(usb_kill_anchored_urbs) },
	{ 0xeae3dfd6, __VMLINUX_SYMBOL_STR(__const_udelay) },
	{ 0x65345022, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x8b85bb06, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x4310adbd, __VMLINUX_SYMBOL_STR(__pskb_copy_fclone) },
	{ 0x6193c573, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xe4689576, __VMLINUX_SYMBOL_STR(ktime_get_with_offset) },
	{ 0x4829a47e, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xad287242, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0xb0250134, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x9b4da327, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x949fa7d1, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x46a36d7d, __VMLINUX_SYMBOL_STR(usb_unanchor_urb) },
	{ 0x11d49bfc, __VMLINUX_SYMBOL_STR(usb_free_urb) },
	{ 0x4722261d, __VMLINUX_SYMBOL_STR(usb_submit_urb) },
	{ 0x27d61f34, __VMLINUX_SYMBOL_STR(usb_anchor_urb) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x7500ce2d, __VMLINUX_SYMBOL_STR(usb_alloc_urb) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0xf33847d3, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0x5cd885d5, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v0BDAp*d*dc*dsc*dp*icE0isc01ip01in*");
MODULE_ALIAS("usb:v13D3p*d*dc*dsc*dp*icE0isc01ip01in*");

MODULE_INFO(srcversion, "77017628000DF469F83C4D0");
