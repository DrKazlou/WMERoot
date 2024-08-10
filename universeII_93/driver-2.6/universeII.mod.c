#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xcad995a5, "struct_module" },
	{ 0x56cf98bd, "init_timer" },
	{ 0xa14f2924, "register_chrdev" },
	{ 0x7a17520b, "dma_alloc_coherent" },
	{ 0x26e96637, "request_irq" },
	{ 0x3762cb6e, "ioremap_nocache" },
	{ 0xd045d3dc, "pci_bus_read_config_dword" },
	{ 0x6368c8ae, "pci_bus_write_config_dword" },
	{ 0x21bdb041, "pci_find_device" },
	{ 0x3cdc0b74, "dma_free_coherent" },
	{ 0x2b58603e, "mem_map" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x37a0cba, "kfree" },
	{ 0x9eac042a, "__ioremap" },
	{ 0x2642591c, "kmem_cache_alloc" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0xd49501d4, "__release_region" },
	{ 0xedc03953, "iounmap" },
	{ 0xd2fe103e, "remap_pfn_range" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0x1a1a4f09, "__request_region" },
	{ 0xf1d0cdab, "__check_region" },
	{ 0x9efed5af, "iomem_resource" },
	{ 0xd842e98e, "finish_wait" },
	{ 0x5b531014, "del_timer" },
	{ 0x4292364c, "schedule" },
	{ 0x86267276, "prepare_to_wait" },
	{ 0x736e81f3, "__mod_timer" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x7d11c268, "jiffies" },
	{ 0x547627fd, "remove_proc_entry" },
	{ 0x359d1c60, "create_proc_entry" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x1b7d4074, "printk" },
	{ 0xbce89f77, "__wake_up" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x98bd6f46, "param_set_int" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "03F7C9A388B76468C75BC62");
