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
	{ 0xbff2abe1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xbb52e72f, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0xc7b102a, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0x2a6e6109, __VMLINUX_SYMBOL_STR(__init_rwsem) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf5893abf, __VMLINUX_SYMBOL_STR(up_read) },
	{ 0x57a6ccd0, __VMLINUX_SYMBOL_STR(down_read) },
	{ 0x3b4ceb4a, __VMLINUX_SYMBOL_STR(up_write) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xdf3deb56, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x74cf6b1f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xe6e3b875, __VMLINUX_SYMBOL_STR(down_write) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8A5FF0CC0A3D4F88ABFACA2");
