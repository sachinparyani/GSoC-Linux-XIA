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
	{ 0x10de1094, __VMLINUX_SYMBOL_STR(release_sock) },
	{ 0x31ea9773, __VMLINUX_SYMBOL_STR(fib_mrd_dump) },
	{ 0x59fcade4, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x1e333ac2, __VMLINUX_SYMBOL_STR(fib_mrd_free) },
	{ 0x33e71f84, __VMLINUX_SYMBOL_STR(xia_shutdown) },
	{ 0xd25cecc4, __VMLINUX_SYMBOL_STR(xip_finish_skb) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0xd4202990, __VMLINUX_SYMBOL_STR(xia_bind) },
	{ 0x91507721, __VMLINUX_SYMBOL_STR(xia_add_socket) },
	{ 0x60a13e90, __VMLINUX_SYMBOL_STR(rcu_barrier) },
	{ 0xaf352b5b, __VMLINUX_SYMBOL_STR(xia_set_dest) },
	{ 0xac4304a5, __VMLINUX_SYMBOL_STR(vxt_register_xidty) },
	{ 0x85e44d61, __VMLINUX_SYMBOL_STR(vxt_unregister_xidty) },
	{ 0x4c71b256, __VMLINUX_SYMBOL_STR(xip_del_ppal_ctx) },
	{ 0x80e900cd, __VMLINUX_SYMBOL_STR(xip_append_data) },
	{ 0xbe17d25d, __VMLINUX_SYMBOL_STR(dst_release) },
	{ 0xd9d3bcd3, __VMLINUX_SYMBOL_STR(_raw_spin_lock_bh) },
	{ 0xe34f24b1, __VMLINUX_SYMBOL_STR(copy_from_iter) },
	{ 0x3471a228, __VMLINUX_SYMBOL_STR(fib_no_newroute) },
	{ 0x2a90ea96, __VMLINUX_SYMBOL_STR(sock_queue_rcv_skb) },
	{ 0x30c819af, __VMLINUX_SYMBOL_STR(fib_alloc_dnf) },
	{ 0x6729d3df, __VMLINUX_SYMBOL_STR(__get_user_4) },
	{ 0xf311c2d7, __VMLINUX_SYMBOL_STR(xia_dgram_connect) },
	{ 0x9a481953, __VMLINUX_SYMBOL_STR(ppal_add_map) },
	{ 0x49792401, __VMLINUX_SYMBOL_STR(ppal_del_map) },
	{ 0x10c98f0c, __VMLINUX_SYMBOL_STR(xia_test_addr) },
	{ 0x7d3df053, __VMLINUX_SYMBOL_STR(xia_del_socket_begin) },
	{ 0x9f5d34fd, __VMLINUX_SYMBOL_STR(xip_make_skb) },
	{ 0x9b39cc26, __VMLINUX_SYMBOL_STR(sk_common_release) },
	{ 0x17270715, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x26a92f79, __VMLINUX_SYMBOL_STR(xia_register_pernet_subsys) },
	{ 0x116557af, __VMLINUX_SYMBOL_STR(sock_no_mmap) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xb20f2c7e, __VMLINUX_SYMBOL_STR(rht_fib_delroute) },
	{ 0x364a5612, __VMLINUX_SYMBOL_STR(sock_no_socketpair) },
	{ 0xe26d8b83, __VMLINUX_SYMBOL_STR(unregister_pernet_subsys) },
	{ 0x1683e6f3, __VMLINUX_SYMBOL_STR(xdst_attach_to_anchor) },
	{ 0x19da2ed5, __VMLINUX_SYMBOL_STR(dev_loopback_xmit) },
	{ 0xeae6da82, __VMLINUX_SYMBOL_STR(xip_init_ppal_ctx) },
	{ 0x7d358f7d, __VMLINUX_SYMBOL_STR(skb_copy_datagram_iter) },
	{ 0x508cb3e9, __VMLINUX_SYMBOL_STR(xip_del_router) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x7608a986, __VMLINUX_SYMBOL_STR(xia_del_socket_end) },
	{ 0x525662d5, __VMLINUX_SYMBOL_STR(lock_sock_nested) },
	{ 0xf5de9d41, __VMLINUX_SYMBOL_STR(__skb_recv_datagram) },
	{ 0xb9ad3e69, __VMLINUX_SYMBOL_STR(fib_mrd_redirect) },
	{ 0x67ec0e17, __VMLINUX_SYMBOL_STR(skb_pull_xiphdr) },
	{ 0x64a005e6, __VMLINUX_SYMBOL_STR(sock_prot_inuse_add) },
	{ 0x8afaebe7, __VMLINUX_SYMBOL_STR(nla_put) },
	{ 0xf3e7b30f, __VMLINUX_SYMBOL_STR(sock_no_listen) },
	{ 0xfd68e1e2, __VMLINUX_SYMBOL_STR(xdst_init_anchor) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xab3237a3, __VMLINUX_SYMBOL_STR(xip_mark_addr_and_get_dst) },
	{ 0x4fd9dca4, __VMLINUX_SYMBOL_STR(sock_no_accept) },
	{ 0xbc87173e, __VMLINUX_SYMBOL_STR(xia_recvmsg) },
	{ 0xc2cdbf1, __VMLINUX_SYMBOL_STR(synchronize_sched) },
	{ 0x7084901e, __VMLINUX_SYMBOL_STR(lock_sock_fast) },
	{ 0x3e8808e9, __VMLINUX_SYMBOL_STR(xia_main_lock_table) },
	{ 0x1f1c5cd8, __VMLINUX_SYMBOL_STR(sk_dst_check) },
	{ 0x4f346f05, __VMLINUX_SYMBOL_STR(xip_release_ppal_ctx) },
	{ 0x6f087e4d, __VMLINUX_SYMBOL_STR(xip_add_router) },
	{ 0x830511cb, __VMLINUX_SYMBOL_STR(__sock_recv_ts_and_drops) },
	{ 0xa7cf0108, __VMLINUX_SYMBOL_STR(xia_getname) },
	{ 0x6c50664, __VMLINUX_SYMBOL_STR(xia_sendpage) },
	{ 0xb2fd5ceb, __VMLINUX_SYMBOL_STR(__put_user_4) },
	{ 0x91bb369, __VMLINUX_SYMBOL_STR(xip_trim_packet_if_needed) },
	{ 0xaba28159, __VMLINUX_SYMBOL_STR(check_sockaddr_xia) },
	{ 0xbba70a2d, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_bh) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x9961d3c5, __VMLINUX_SYMBOL_STR(datagram_poll) },
	{ 0xcd16f0c8, __VMLINUX_SYMBOL_STR(copy_n_and_shade_xia_addr) },
	{ 0x3e9613b4, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0xa8bc5cbb, __VMLINUX_SYMBOL_STR(xia_reset_dest) },
	{ 0xa94a3383, __VMLINUX_SYMBOL_STR(xip_send_skb) },
	{ 0xc95ecaad, __VMLINUX_SYMBOL_STR(xdst_free_anchor) },
	{ 0xf74d2b06, __VMLINUX_SYMBOL_STR(xip_add_ppal_ctx) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xc31374f6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x22bb7cd2, __VMLINUX_SYMBOL_STR(xip_recv_error) },
	{ 0x610cd108, __VMLINUX_SYMBOL_STR(sock_common_setsockopt) },
	{ 0x82adb0a, __VMLINUX_SYMBOL_STR(fib_mrd_newroute) },
	{ 0xc0970590, __VMLINUX_SYMBOL_STR(copy_n_and_shade_sockaddr_xia) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x4602e8f3, __VMLINUX_SYMBOL_STR(xia_ioctl) },
	{ 0x2bb25f08, __VMLINUX_SYMBOL_STR(sock_common_getsockopt) },
	{ 0x6c3d1bdc, __VMLINUX_SYMBOL_STR(xia_release) },
	{ 0x181fa4c9, __VMLINUX_SYMBOL_STR(fib_defer_dnf) },
	{ 0xd197106f, __VMLINUX_SYMBOL_STR(xip_start_skb) },
	{ 0xa9e4d1ed, __VMLINUX_SYMBOL_STR(xia_sendmsg) },
	{ 0xb0e602eb, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0xa39797bf, __VMLINUX_SYMBOL_STR(xip_flush_pending_frames) },
	{ 0xf52b7ab, __VMLINUX_SYMBOL_STR(xia_ppal_rht_rt_iops) },
	{ 0x7f1d329c, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
	{ 0x27fa66e1, __VMLINUX_SYMBOL_STR(nr_free_buffer_pages) },
	{ 0x7eda4413, __VMLINUX_SYMBOL_STR(skb_free_datagram_locked) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xia";


MODULE_INFO(srcversion, "FBC5DCB084F5EDE291A5ECF");
