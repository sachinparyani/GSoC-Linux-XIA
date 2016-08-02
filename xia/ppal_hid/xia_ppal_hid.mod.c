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
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0x59fcade4, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x60a13e90, __VMLINUX_SYMBOL_STR(rcu_barrier) },
	{ 0x5aa3b507, __VMLINUX_SYMBOL_STR(xia_xidtop) },
	{ 0x79aa04a2, __VMLINUX_SYMBOL_STR(get_random_bytes) },
	{ 0xac4304a5, __VMLINUX_SYMBOL_STR(vxt_register_xidty) },
	{ 0x85e44d61, __VMLINUX_SYMBOL_STR(vxt_unregister_xidty) },
	{ 0x4c71b256, __VMLINUX_SYMBOL_STR(xip_del_ppal_ctx) },
	{ 0x16de3b9c, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0xd2da1048, __VMLINUX_SYMBOL_STR(register_netdevice_notifier) },
	{ 0x30c819af, __VMLINUX_SYMBOL_STR(fib_alloc_dnf) },
	{ 0x9580deb, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xff7d557b, __VMLINUX_SYMBOL_STR(xip_dst_ops_template) },
	{ 0x9a481953, __VMLINUX_SYMBOL_STR(ppal_add_map) },
	{ 0x4e873d0e, __VMLINUX_SYMBOL_STR(skb_set_owner_w) },
	{ 0x49792401, __VMLINUX_SYMBOL_STR(ppal_del_map) },
	{ 0xfce3ae0a, __VMLINUX_SYMBOL_STR(skb_realloc_headroom) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x9d0d6206, __VMLINUX_SYMBOL_STR(unregister_netdevice_notifier) },
	{ 0x85f88990, __VMLINUX_SYMBOL_STR(fxid_free) },
	{ 0x17270715, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x26a92f79, __VMLINUX_SYMBOL_STR(xia_register_pernet_subsys) },
	{ 0x836d5b9, __VMLINUX_SYMBOL_STR(__pskb_pull_tail) },
	{ 0x706d051c, __VMLINUX_SYMBOL_STR(del_timer_sync) },
	{ 0xe26d8b83, __VMLINUX_SYMBOL_STR(unregister_pernet_subsys) },
	{ 0x1683e6f3, __VMLINUX_SYMBOL_STR(xdst_attach_to_anchor) },
	{ 0x5d070b31, __VMLINUX_SYMBOL_STR(xia_ppal_list_rt_iops) },
	{ 0xeae6da82, __VMLINUX_SYMBOL_STR(xip_init_ppal_ctx) },
	{ 0x508cb3e9, __VMLINUX_SYMBOL_STR(xip_del_router) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0x8afaebe7, __VMLINUX_SYMBOL_STR(nla_put) },
	{ 0xfd68e1e2, __VMLINUX_SYMBOL_STR(xdst_init_anchor) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xccc68a5c, __VMLINUX_SYMBOL_STR(dev_remove_pack) },
	{ 0x16e5c2a, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0xb2fb0f2f, __VMLINUX_SYMBOL_STR(nla_reserve_nohdr) },
	{ 0xcc090d9e, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0x3e8808e9, __VMLINUX_SYMBOL_STR(xia_main_lock_table) },
	{ 0x4f346f05, __VMLINUX_SYMBOL_STR(xip_release_ppal_ctx) },
	{ 0x42160169, __VMLINUX_SYMBOL_STR(flush_workqueue) },
	{ 0x6f087e4d, __VMLINUX_SYMBOL_STR(xip_add_router) },
	{ 0x78ce6687, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x91bb369, __VMLINUX_SYMBOL_STR(xip_trim_packet_if_needed) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x3e9613b4, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0xc95ecaad, __VMLINUX_SYMBOL_STR(xdst_free_anchor) },
	{ 0x6b2dc060, __VMLINUX_SYMBOL_STR(dump_stack) },
	{ 0xf74d2b06, __VMLINUX_SYMBOL_STR(xip_add_ppal_ctx) },
	{ 0x9e4f5567, __VMLINUX_SYMBOL_STR(pskb_expand_head) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xc31374f6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xf6ebc03b, __VMLINUX_SYMBOL_STR(net_ratelimit) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x643e0ce5, __VMLINUX_SYMBOL_STR(call_rcu_sched) },
	{ 0x181fa4c9, __VMLINUX_SYMBOL_STR(fib_defer_dnf) },
	{ 0x4dc0854, __VMLINUX_SYMBOL_STR(dev_add_pack) },
	{ 0xb0e602eb, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0xfb707294, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0x85670f1d, __VMLINUX_SYMBOL_STR(rtnl_is_locked) },
	{ 0x169c069b, __VMLINUX_SYMBOL_STR(dev_queue_xmit) },
	{ 0x9bc59e3c, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0x7f1d329c, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xia";


MODULE_INFO(srcversion, "07977DC03252FA84DAEF0A2");
