#ifndef _NET_XIA_RHT_FIB_H
#define _NET_XIA_RHT_FIB_H

#ifdef __KERNEL__

#include <net/xia_fib.h>
#include <linux/rhashtable.h>

struct rht_fib_xid {
	struct rhash_head	node;
};

struct rht_fib_xid_table {
	struct rhashtable rht;
};

struct rhashtable_free_and_destroy_arg {
	struct fib_xid_table	*xtbl;
	int			*rm_count;
};

/*
 *	Exported by list_fib.c
 */

int rht_fib_delroute(struct xip_ppal_ctx *ctx, struct fib_xid_table *xtbl,
	struct xia_fib_config *cfg);

extern const struct xia_ppal_rt_iops xia_ppal_rht_rt_iops;

#define XIP_RHT_FIB_REDIRECT_MAIN [XRTABLE_MAIN_INDEX] = {		\
	.newroute = fib_mrd_newroute,					\
	.delroute = rht_fib_delroute,					\
	.dump_fxid = fib_mrd_dump,					\
	.free_fxid = fib_mrd_free,					\
}

#endif /* __KERNEL__ */
#endif /* _NET_XIA_RHT_FIB_H */
