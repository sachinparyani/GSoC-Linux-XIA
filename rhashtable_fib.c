#include <linux/export.h>
#include <linux/jhash.h>
#include <linux/vmalloc.h>
#include <net/xia_locktbl.h>	/* Not quite sure if this is required */
#include <net/xia_vxidty.h>
#include <net/xia_rht_fib.h>
#include <linux/rhashtable.h>

static inline struct fib_xid *rfxid_fxid(struct rht_fib_xid *rfxid)
{
	return likely(rfxid)
		? container_of((void *)rfxid, struct fib_xid, fx_data)
		: NULL;
}

static inline struct rht_fib_xid *fxid_rfxid(struct fib_xid *fxid)
{
	return (struct rht_fib_xid *)fxid->fx_data;
}

static inline struct fib_xid_table *rxtbl_xtbl(struct rht_fib_xid_table *rxtbl)
{
	return likely(rxtbl)
		? container_of((void *)rxtbl, struct fib_xid_table, fxt_data)
		: NULL;
}

static inline struct rht_fib_xid_table *xtbl_rxtbl(struct fib_xid_table *xtbl)
{
	return (struct rht_fib_xid_table *)xtbl->fxt_data;
}

/* Not sure if this function should be inline or not */
static void free_fn(void *ptr, void *arg)
{
	int *rm_count = &arg->rm_count;
	struct rht_fib_xid *rfxid = ptr;
	struct fib_xid_table *xtbl = arg->xtbl;
	rhashtable_remove_fast(&xtbl_rxtbl(xtbl)->rht, &rfxid->node, (&xtbl_rxtbl(xtbl)->rht)->p);
	fxid_free_norcu(xtbl, rfxid_fxid(rfxid));
	(*rm_count)++;
}

static inline u32 rht_jhash2(const u8 *k, u32 length, u32 interval)
{
	BUILD_BUG_ON(XIA_XID_MAX != sizeof(u32) * 5);
	return jhash2((const u32 *)xid, length, interval);
}

static inline int rht_obj_cmpfn(struct rhashtable_compare_arg *arg, const void *obj)
{
	const struct rht_fib_xid *rfxid = obj;
	return (!are_xids_equal(rfxid_fxid(rfxid)->fx_xid, arg->key));
}

static inline u32 rht_obj_hashfn(const void *data, u32 len, u32 seed)
{
	const struct rht_fib_xid *rfxid = data;
	BUILD_BUG_ON(XIA_XID_MAX != sizeof(u32) * 5);
	return jhash2((const u32 *)(rfxid_fxid(rfxid)->fx_xid), len, seed);
}

/* Don't make this function inline, it's bigger than it looks like! */
static void bucket_unlock(struct list_fib_xid_table *lxtbl, u32 bucket)
	__releases(bucket)
{
	xia_lock_table_unlock(lxtbl->fxt_locktbl, hash_bucket(lxtbl, bucket));
}

/* Routing tables */

/* This function must be called in process context due to virtual memory. */
static int alloc_buckets(struct fib_xid_buckets *branch, size_t num)
{
	struct hlist_head *buckets;
	size_t size = sizeof(*buckets) * num;

	buckets = kmalloc(size, GFP_KERNEL);
	if (unlikely(!buckets)) {
		buckets = vmalloc(size);
		if (!buckets)
			return -ENOMEM;
		pr_warn("XIP %s: the kernel is running out of memory and/or memory is too fragmented. Allocated virtual memory for now; hopefully, it's going to gracefully degrade packet forwarding performance.\n",
			__func__);
	}
	memset(buckets, 0, size);
	branch->buckets = buckets;
	branch->divisor = num;
	return 0;
}

/* This function must be called in process context due to virtual memory. */
static inline void free_buckets(struct fib_xid_buckets *branch)
{
	if (unlikely(is_vmalloc_addr(branch->buckets)))
		vfree(branch->buckets);
	else
		kfree(branch->buckets);
	branch->buckets = NULL;
	branch->divisor = 0;
}

/* XTBL_INITIAL_DIV must be a power of 2. */
#define XTBL_INITIAL_DIV 1

static void rehash_work(struct work_struct *work);
static void list_xtbl_death_work(struct work_struct *work);

static int rht_xtbl_init(struct xip_ppal_ctx *ctx, struct net *net,
			  struct xia_lock_table *locktbl,
			  const xia_ppal_all_rt_eops_t all_eops,
			  const struct xia_ppal_rt_iops *all_iops)
{
	struct fib_xid_table *new_xtbl;
	struct rht_fib_xid_table *rxtbl;
	struct bucket_table *tbl;
	int rc, err;

	if (ctx->xpc_xtbl) {
		rc = -EEXIST;
		goto out; /* Duplicate. */
	}

	rc = -ENOMEM;
	new_xtbl = kzalloc(sizeof(*new_xtbl) + sizeof(*rxtbl), GFP_KERNEL);
	if (!new_xtbl)
		goto out;
	rxtbl = xtbl_rxtbl(new_xtbl);
	err = rhashtable_init(&rxtbl->rht,&rht_params);
	if(err)
		goto new_xtbl;
	
	new_xtbl->fxt_ppal_type = ctx->xpc_ppal_type;
	new_xtbl->fxt_net = net;
	atomic_set(&new_xtbl->fxt_count, 0);
	new_xtbl->all_eops = all_eops;
	new_xtbl->all_iops = all_iops;

	atomic_set(&new_xtbl->refcnt, 1);
	INIT_WORK(&new_xtbl->fxt_death_work, rht_xtbl_death_work);
	ctx->xpc_xtbl = new_xtbl;

	rc = 0;
	goto out;

new_xtbl:
	kfree(new_xtbl);
out:
	return rc;
}

static void *rht_fxid_ppal_alloc(size_t ppal_entry_size, gfp_t flags)
{
	return kmalloc(ppal_entry_size + sizeof(struct rht_fib_xid), flags);
}

static void rht_fxid_init(struct fib_xid *fxid, int table_id, int entry_type)
{
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	struct rhash_head *pnode;
	pnode = &rfxid->node;
	pnode->next = NULL;

	BUILD_BUG_ON(XRTABLE_MAX_INDEX >= 0x100);
	BUG_ON(table_id >= XRTABLE_MAX_INDEX);
	fxid->fx_table_id = table_id;

	BUG_ON(entry_type > 0xff);
	fxid->fx_entry_type = entry_type;

	fxid->dead.xtbl = NULL;
}

static void rht_xtbl_death_work(struct work_struct *work)
{
	struct fib_xid_table *xtbl = container_of(work, struct fib_xid_table,
		fxt_death_work);
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	
	int rm_count = 0;
	int c;

	struct rhashtable_free_and_destroy_arg arg = {
		.xtbl = xtbl;
		.rm_count = &rm_count;
	};

	rhashtable_free_and_destroy(&rxtbl->rht, free_fun, &arg);
	
	/* It doesn't return an error here because there's nothing
	 * the caller can do about this error/bug.
	 */
	c = atomic_read(&xtbl->fxt_count);
	if (c != rm_count) {
		pr_err("While freeing XID table of principal %x, %i entries were found, whereas %i are counted! Ignoring it, but it's a serious bug!\n",
		       __be32_to_cpu(xtbl->fxt_ppal_type), rm_count, c);
		dump_stack();
	}

	kfree(xtbl);
}

static struct fib_xid *rht_fxid_find(struct fib_xid_table *xtbl,
				     const u8 *xid)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid;
	rfxid = rhashtable_lookup_fast(&rxtbl->rht, xid, rht_params);
	
	if(rfxid)
		return rfxid_fxid(rfxid);
	else
		return NULL;
}

/* Coming Soon */
static struct fib_xid *rht_fxid_find_locked(struct fib_xid_table *xtbl,
					     u32 bucket, const u8 *xid,
					     struct hlist_head **phead)
{
	return NULL;
}

/* Coming Soon */
static struct fib_xid *rht_fxid_find_rcu(struct fib_xid_table *xtbl,
					  const u8 *xid)
{
	return NULL;
}

static u32 list_fib_lock_bucket_xid(struct fib_xid_table *xtbl, const u8 *xid)
	__acquires(xip_bucket_lock)
{
	struct list_fib_xid_table *lxtbl = xtbl_lxtbl(xtbl);
	u32 bucket;

	read_lock(&lxtbl->fxt_writers_lock);
	bucket = get_bucket(xid, lxtbl->fxt_active_branch->divisor);
	bucket_lock(lxtbl, bucket);

	/* Make sparse happy with only one __acquires. */
	__release(bucket);

	return bucket;
}

static inline u32 list_fib_lock(struct fib_xid_table *xtbl,
				struct fib_xid *fxid)
				__acquires(xip_bucket_lock)
{
	return list_fib_lock_bucket_xid(xtbl, fxid->fx_xid);
}

/* For the relativistic hashtable FIB, @parg represents a unsigned int hash. */
static inline unsigned int parg_hash(void *parg)
{
	if (unlikely(!parg))
		BUG();
	return *(unsigned int *)parg;
}

/* Not sure about the correctness of this code. Maybe the right bucket table is not being dereferenced */
static void rht_fib_unlock(struct fib_xid_table *xtbl, void *parg)
	__releases(xip_bucket_lock)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	unsigned int hash = parg_hash(parg);
	spinlock_t *lock;
	struct bucket_table *tbl = rht_dereference_rcu((&rxtbl->rht)->tbl, ht);
	lock = rht_bucket_lock(tbl, hash);
	
	/* Make sparse happy with only one __releases. */
	__acquire(hash);
	
	spin_unlock_bh(lock);
}

static struct fib_xid *list_fxid_find_lock(void *parg,
	struct fib_xid_table *xtbl, const u8 *xid) __acquires(xip_bucket_lock)
{
	struct hlist_head *head;
	u32 *pbucket = parg;
	*pbucket = list_fib_lock_bucket_xid(xtbl, xid);
	return list_fxid_find_locked(xtbl, *pbucket, xid, &head);
}

/* This function holds and releases an rcu read lock, so the caller must not hold one */  
static int rht_iterate_xids(struct fib_xid_table *xtbl,
			     int (*locked_callback)(struct fib_xid_table *xtbl,
						    struct fib_xid *fxid,
						    const void *arg),
			     const void *arg)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid;
	struct rhashtable_iter hti;
	int err, rc = 0;

	err = rhashtable_walk_init(&rxtbl->rht, &hti);
	if(err) {
		pr_warn("Allocation error");
		return err;
	}

	err = rhashtable_walk_start(&hti);
	if(err && err != -EAGAIN) {
		pr_warn("Iterator failed: %d\n",err);
		rhashtable_walk_stop(&hti);
		return err;
	}
	
	while((rfxid = rhashtable_walk_next(&hti))) {
		if(PTR_ERR(rfxid) == -EAGAIN) {
			pr_info("Info: encountered resize\n");
			continue;
		}
		
		else if(IS_ERR(rfxid)) {
			pr_warn("rhashtable_walk_next() error: %ld\n", PTR_ERR(rfxid));
			break;
		}
		
		rc = locked_callback(xtbl, rfxid_fxid(rfxid), arg);
		if(rc)
			goto out;
	}

out:
	rhashtable_walk_stop(&hti);
	rhashtable_walk_exit(&hti);
	return rc;
}

/* Coming Soon */
static int rht_iterate_xids_rcu(struct fib_xid_table *xtbl,
				 int (*rcu_callback)(struct fib_xid_table *xtbl,
						     struct fib_xid *fxid,
						     const void *arg),
				 const void *arg)
{
	return 0;
}

/* Grow table as needed. */
static void rehash_work(struct work_struct *work)
{
	struct list_fib_xid_table *lxtbl = container_of(work,
		struct list_fib_xid_table, fxt_rehash_work);
	struct fib_xid_buckets *abranch = lxtbl->fxt_active_branch;
	int aindex = lxtbl_branch_index(lxtbl, abranch);
	int nindex = 1 - aindex;
	/* The next branch. */
	struct fib_xid_buckets *nbranch = &lxtbl->fxt_branch[nindex];
	int old_divisor = abranch->divisor;
	int new_divisor = old_divisor * 2;
	int mv_count = 0;
	int rc, i, c, should_rehash;

	/* Allocate memory before aquiring write lock because it sleeps. */
	BUG_ON(!is_power_of_2(new_divisor));
	rc = alloc_buckets(nbranch, new_divisor);
	if (rc) {
		pr_err(
		"Rehashing XID table %x was not possible due to error %i.\n",
			__be32_to_cpu(lxtbl_xtbl(lxtbl)->fxt_ppal_type), rc);
		dump_stack();
		return;
	}

	write_lock(&lxtbl->fxt_writers_lock);

	/* We must test if we @should_rehash again because we may be
	 * following another rehash_work that just finished.
	 * Even if we're not following another rehash_work, fxt_count may have
	 * changed while we waited on write_lock() or to be scheduled, and
	 * a rehash became unnecessary.
	 */
	should_rehash = atomic_read(&lxtbl_xtbl(lxtbl)->fxt_count) /
				    old_divisor > 2;
	if (!should_rehash) {
		/* The calling order here is very important because
		 * function free_buckets sleeps.
		 */
		write_unlock(&lxtbl->fxt_writers_lock);
		free_buckets(nbranch);
		return;
	}

	/* Add entries to @nbranch. */
	for (i = 0; i < old_divisor; i++) {
		struct list_fib_xid *lfxid;
		struct hlist_head *head = &abranch->buckets[i];

		hlist_for_each_entry(lfxid, head, fx_branch_list[aindex]) {
			struct hlist_head *new_head =
				xidhead(nbranch, lfxid_fxid(lfxid)->fx_xid);
			hlist_add_head(&lfxid->fx_branch_list[nindex],
				       new_head);
			mv_count++;
		}
	}
	rcu_assign_pointer(lxtbl->fxt_active_branch, nbranch);

	/* It doesn't return an error here because there's nothing
	 * the caller can do about this error/bug.
	 */
	c = atomic_read(&lxtbl_xtbl(lxtbl)->fxt_count);
	if (c != mv_count) {
		pr_err("While rehashing XID table of principal %x, %i entries were found, whereas %i are registered! Fixing the counter for now, but it's a serious bug!\n",
		       __be32_to_cpu(lxtbl_xtbl(lxtbl)->fxt_ppal_type),
				     mv_count, c);
		dump_stack();
		/* "Fixing" bug. */
		atomic_set(&lxtbl_xtbl(lxtbl)->fxt_count, mv_count);
	}

	write_unlock(&lxtbl->fxt_writers_lock);

	/* Make sure that there's no reader in @abranch. */
	synchronize_rcu();

	/* From now on, all readers are using @nbranch. */

	free_buckets(abranch);
}

/* Coming Soon */
static int rht_fxid_add_locked(void *parg, struct fib_xid_table *xtbl,
				struct fib_xid *fxid)
{
	return 0;
}

static int rht_fxid_add(struct fib_xid_table *xtbl, struct fib_xid *fxid)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	int rc;
	
	rc =  rhashtable_lookup_insert_key(&rxtbl->rht, fxid->fx_xid, &rfxid->node, &rht_params);
	if(IS_ERR(rc))
		goto out;
	
	atomic_inc(&xtbl->fxt_count);
 out: 	
	return rc;
}

/* Coming Soon */
static void rht_fxid_rm_locked(void *parg, struct fib_xid_table *xtbl,
				struct fib_xid *fxid)
{
}

/* This function needs to be changed if functions rht_fxid_find_lock and rht_fxid_rm_locked get implemented */
static struct fib_xid *rht_xid_rm(struct fib_xid_table *xtbl, const u8 *xid)
{
	struct fib_xid *fxid = rht_fxid_find(xtbl, xid);

	if (!fxid) {
		return NULL;
	}
	
	rht_fxid_rm(xtbl, fxid);
	atomic_dec(&xtbl->fxt_count);
	return fxid;
}

static void rht_fxid_rm(struct fib_xid_table *xtbl, struct fib_xid *fxid)
{
	struct rht_fib_xid_table *rxtlb = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	rhashtable_remove_fast(&rxtbl->rht, &rfxid->node, rht_params);
	atomic_dec(&xtbl->fxt_count);
}

/* Coming Soon */
static void rht_fxid_replace_locked(struct fib_xid_table *xtbl,
				     struct fib_xid *old_fxid,
				     struct fib_xid *new_fxid)
{
}

static void rht_fxid_replace(struct fib_xid_table *xtbl,
				     struct fib_xid *old_fxid,
				     struct fib_xid *new_fxid)
{
	struct rht_fib_xid *old_rfxid = fxid_rfxid(old_fxid);
	struct rht_fib_xid *new_rfxid = fxid_rfxid(new_fxid);
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	rhashtable_replace_fast(&rxtbl->rht, &old_rfxid->node, &new_rfxid->node, rht_params);
}

int rht_fib_delroute(struct xip_ppal_ctx *ctx, struct fib_xid_table *xtbl,
		      struct xia_fib_config *cfg)
{
	u32 bucket;
	return all_fib_delroute(ctx, xtbl, cfg, &bucket);
}
EXPORT_SYMBOL_GPL(rht_fib_delroute);

static int rht_fib_newroute(struct fib_xid *new_fxid,
			     struct fib_xid_table *xtbl,
			     struct xia_fib_config *cfg, int *padded)
{
	u32 bucket;
	return all_fib_newroute(new_fxid, xtbl, cfg, padded, &bucket);
}

static int rht_xtbl_dump_rcu(struct fib_xid_table *xtbl,
			      struct xip_ppal_ctx *ctx, struct sk_buff *skb,
			      struct netlink_callback *cb)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(rxtbl);
	struct bucket_table *tbl = rht_dereference((&rxtbl->rht)->tbl, &rxtbl->rht);
	long i, j = 0;
	long first_j = cb->args[2];
	int rc;

	for (i = cb->args[1]; i < tbl->size; i++, first_j = 0) {
		struct rht_fib_xid *rfxid;
		struct rhash_head *head;

		j = 0;
		rhtt_for_each_entry_rcu(rfxid, head, tbl, i, node)
		{
			if (j < first_j)
				goto next;
			rc = xtbl->all_eops[rfxid_fxid(rfxid)->fx_table_id].
			     dump_fxid(rfxid_fxid(rfxid), xtbl, ctx, skb, cb);
			if (rc < 0)
				goto out;
next:
			j++;
		}
	}
	rc = 0;

out:
	cb->args[1] = i;
	cb->args[2] = j;
	return rc;
}

const struct xia_ppal_rt_iops xia_ppal_rht_rt_iops = {
	.xtbl_init = list_xtbl_init,
	.xtbl_death_work = list_xtbl_death_work,

	.fxid_ppal_alloc = list_fxid_ppal_alloc,
	.fxid_init = list_fxid_init,

	.fxid_find_rcu = list_fxid_find_rcu,
	.fxid_find_lock = list_fxid_find_lock,
	.iterate_xids = list_iterate_xids,
	.iterate_xids_rcu = list_iterate_xids_rcu,

	.fxid_add = list_fxid_add,
	.fxid_add_locked = list_fxid_add_locked,

	.fxid_rm = list_fxid_rm,
	.fxid_rm_locked = list_fxid_rm_locked,
	.xid_rm = list_xid_rm,

	.fxid_replace_locked = list_fxid_replace_locked,

	.fib_unlock = list_fib_unlock,

	.fib_newroute = list_fib_newroute,
	.fib_delroute = list_fib_delroute,

	.xtbl_dump_rcu = list_xtbl_dump_rcu,
};
EXPORT_SYMBOL_GPL(xia_ppal_rht_rt_iops);
