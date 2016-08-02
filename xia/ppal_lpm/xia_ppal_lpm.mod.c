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
	{ 0x3fcfd5aa, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x59fcade4, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x1e333ac2, __VMLINUX_SYMBOL_STR(fib_mrd_free) },
	{ 0x60a13e90, __VMLINUX_SYMBOL_STR(rcu_barrier) },
	{ 0x44863fba, __VMLINUX_SYMBOL_STR(xdst_invalidate_redirect) },
	{ 0x5aa3b507, __VMLINUX_SYMBOL_STR(xia_xidtop) },
	{ 0xac4304a5, __VMLINUX_SYMBOL_STR(vxt_register_xidty) },
	{ 0x85e44d61, __VMLINUX_SYMBOL_STR(vxt_unregister_xidty) },
	{ 0x4c71b256, __VMLINUX_SYMBOL_STR(xip_del_ppal_ctx) },
	{ 0x179651ac, __VMLINUX_SYMBOL_STR(_raw_read_lock) },
	{ 0x30c819af, __VMLINUX_SYMBOL_STR(fib_alloc_dnf) },
	{ 0x9a481953, __VMLINUX_SYMBOL_STR(ppal_add_map) },
	{ 0x49792401, __VMLINUX_SYMBOL_STR(ppal_del_map) },
	{ 0x85f88990, __VMLINUX_SYMBOL_STR(fxid_free) },
	{ 0x17270715, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x26a92f79, __VMLINUX_SYMBOL_STR(xia_register_pernet_subsys) },
	{ 0xe26d8b83, __VMLINUX_SYMBOL_STR(unregister_pernet_subsys) },
	{ 0x1683e6f3, __VMLINUX_SYMBOL_STR(xdst_attach_to_anchor) },
	{ 0xeae6da82, __VMLINUX_SYMBOL_STR(xip_init_ppal_ctx) },
	{ 0x508cb3e9, __VMLINUX_SYMBOL_STR(xip_del_router) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb9ad3e69, __VMLINUX_SYMBOL_STR(fib_mrd_redirect) },
	{ 0x8afaebe7, __VMLINUX_SYMBOL_STR(nla_put) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xfd68e1e2, __VMLINUX_SYMBOL_STR(xdst_init_anchor) },
	{ 0xe8db8dd2, __VMLINUX_SYMBOL_STR(_raw_write_lock) },
	{ 0xc2cdbf1, __VMLINUX_SYMBOL_STR(synchronize_sched) },
	{ 0x3e8808e9, __VMLINUX_SYMBOL_STR(xia_main_lock_table) },
	{ 0x4f346f05, __VMLINUX_SYMBOL_STR(xip_release_ppal_ctx) },
	{ 0x6f087e4d, __VMLINUX_SYMBOL_STR(xip_add_router) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xc95ecaad, __VMLINUX_SYMBOL_STR(xdst_free_anchor) },
	{ 0x6b2dc060, __VMLINUX_SYMBOL_STR(dump_stack) },
	{ 0xf74d2b06, __VMLINUX_SYMBOL_STR(xip_add_ppal_ctx) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xc31374f6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x181fa4c9, __VMLINUX_SYMBOL_STR(fib_defer_dnf) },
	{ 0xb0e602eb, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0x7f1d329c, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xia";


MODULE_INFO(srcversion, "88D8BFED27E80CBC404EBD2");
