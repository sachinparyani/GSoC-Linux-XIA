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
	{ 0x91ef8823, __VMLINUX_SYMBOL_STR(del_xdst_and_hold) },
	{ 0x60a13e90, __VMLINUX_SYMBOL_STR(rcu_barrier) },
	{ 0x44863fba, __VMLINUX_SYMBOL_STR(xdst_invalidate_redirect) },
	{ 0xac4304a5, __VMLINUX_SYMBOL_STR(vxt_register_xidty) },
	{ 0x85e44d61, __VMLINUX_SYMBOL_STR(vxt_unregister_xidty) },
	{ 0x4c71b256, __VMLINUX_SYMBOL_STR(xip_del_ppal_ctx) },
	{ 0xbe17d25d, __VMLINUX_SYMBOL_STR(dst_release) },
	{ 0x9a481953, __VMLINUX_SYMBOL_STR(ppal_add_map) },
	{ 0x49792401, __VMLINUX_SYMBOL_STR(ppal_del_map) },
	{ 0x729084ef, __VMLINUX_SYMBOL_STR(xdst_def_hop_limit_input_method) },
	{ 0x17270715, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x26a92f79, __VMLINUX_SYMBOL_STR(xia_register_pernet_subsys) },
	{ 0xe26d8b83, __VMLINUX_SYMBOL_STR(unregister_pernet_subsys) },
	{ 0x1683e6f3, __VMLINUX_SYMBOL_STR(xdst_attach_to_anchor) },
	{ 0x5d070b31, __VMLINUX_SYMBOL_STR(xia_ppal_list_rt_iops) },
	{ 0x6c8e44bb, __VMLINUX_SYMBOL_STR(xip_route_with_a_redirect) },
	{ 0x3b68e7e8, __VMLINUX_SYMBOL_STR(xip_route) },
	{ 0xeae6da82, __VMLINUX_SYMBOL_STR(xip_init_ppal_ctx) },
	{ 0x7547710, __VMLINUX_SYMBOL_STR(list_fib_delroute) },
	{ 0x508cb3e9, __VMLINUX_SYMBOL_STR(xip_del_router) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1a9de362, __VMLINUX_SYMBOL_STR(xdst_rcu_free) },
	{ 0x8afaebe7, __VMLINUX_SYMBOL_STR(nla_put) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xfd68e1e2, __VMLINUX_SYMBOL_STR(xdst_init_anchor) },
	{ 0x3e8808e9, __VMLINUX_SYMBOL_STR(xia_main_lock_table) },
	{ 0x4f346f05, __VMLINUX_SYMBOL_STR(xip_release_ppal_ctx) },
	{ 0x6f087e4d, __VMLINUX_SYMBOL_STR(xip_add_router) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x3e9613b4, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0xc95ecaad, __VMLINUX_SYMBOL_STR(xdst_free_anchor) },
	{ 0xf74d2b06, __VMLINUX_SYMBOL_STR(xip_add_ppal_ctx) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x208d5d4d, __VMLINUX_SYMBOL_STR(__pskb_copy_fclone) },
	{ 0xc31374f6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf6ebc03b, __VMLINUX_SYMBOL_STR(net_ratelimit) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xf06c099b, __VMLINUX_SYMBOL_STR(def_ppal_destroy) },
	{ 0xb0e602eb, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0x7f1d329c, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xia";


MODULE_INFO(srcversion, "03AC24C12C970968C579A0B");
