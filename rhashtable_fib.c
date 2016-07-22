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

static struct rhashtable_params rht_params = {
	.head_offset = offsetof(struct rht_fib_xid, node),
	.key_len = 5,
	.hashfn = rht_jhash2,
	.obj_cmpfn = rht_obj_cmpfn,
	.obj_hashfn = rht_obj_hashfn,
	.automatic_shrinking = true,
	.max_size = 50000;
	.nulls_base = (3U << RHT_BASE_SHIFT),
};

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

static void rht_xtbl_death_work(struct work_struct *work);

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

static struct fib_xid *rht_fxid_find_rcu(struct fib_xid_table *xtbl,
					  const u8 *xid)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rhashtable_compare_arg arg = {
		.ht = &rxtbl->rht,
		.key = xid,
	};
	const struct bucket_table *tbl;
	struct rhash_head *he;
	unsigned int hash;
	
	tbl = rht_dereference_rcu((&rxtbl->rht)->tbl, &rxtbl->rht);
restart:
	hash = rht_key_hashfn(&rxtbl->rht, tbl, xid, rht_params);
	rht_for_each_rcu(he, tbl, hash) {
		if (params.obj_cmpfn ?
		    params.obj_cmpfn(&arg, rht_obj(&rxtbl->rht, he)) :
		    rhashtable_compare(&arg, rht_obj(&rxtbl->rht, he)))
			continue;
		
		return rfxid_fxid(rht_obj(&rxtbl->rht, he));
	}

	/* Ensure we see any new tables. */
	smp_rmb();

	tbl = rht_dereference_rcu(tbl->future_tbl, &rxtbl->rht);
	if (unlikely(tbl))
		goto restart;
	
	return NULL;
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

static struct fib_xid *rht_fxid_find_lock(void *parg,
	struct fib_xid_table *xtbl, const u8 *xid) __acquires(xip_bucket_lock)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rhashtable_compare_arg arg = {
		.ht = &rxtbl->rht,
		.key = xid,
	};
	const struct bucket_table *tbl;
	struct rhash_head *he;
	unsigned int hash;
	spinlock_t *lock;
	
	tbl = (&rxtbl->rht)->tbl;
restart:
	hash = rht_key_hashfn(&rxtbl->rht, tbl, xid, rht_params);
	lock = rht_bucket_lock(tbl, hash);
	spin_lock_bh(lock);
	rht_for_each(he, tbl, hash) {
		if (params.obj_cmpfn ?
		    params.obj_cmpfn(&arg, rht_obj(&rxtbl->rht, he)) :
		    rhashtable_compare(&arg, rht_obj(&rxtbl->rht, he)))
			continue;
		
		return rfxid_fxid(rht_obj(&rxtbl->rht, he));
	}

	/* Ensure we see any new tables. */
	smp_rmb();

	tbl = tbl->future_tbl;
	if (unlikely(tbl))
		goto restart;
	
	return NULL;	
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

static int rhashtable_walk_start_rcu(struct rhashtable_iter *iter)
{
	struct rhashtable *ht = iter->ht;

	spin_lock(&ht->lock);
	if (iter->walker->tbl)
		list_del(&iter->walker->list);
	spin_unlock(&ht->lock);

	if (!iter->walker->tbl) {
		iter->walker->tbl = rht_dereference_rcu(ht->tbl, ht);
		return -EAGAIN;
	}

	return 0;
}

void rhashtable_walk_stop_rcu(struct rhashtable_iter *iter)
{
	struct rhashtable *ht;
	struct bucket_table *tbl = iter->walker->tbl;

	if (!tbl)
		goto out;

	ht = iter->ht;

	spin_lock(&ht->lock);
	if (tbl->rehash < tbl->size)
		list_add(&iter->walker->list, &tbl->walkers);
	else
		iter->walker->tbl = NULL;
	spin_unlock(&ht->lock);

	iter->p = NULL;

out:
	return;
}

static int rht_iterate_xids_rcu(struct fib_xid_table *xtbl,
				 int (*rcu_callback)(struct fib_xid_table *xtbl,
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

	err = rhashtable_walk_start_rcu(&hti);
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
	rhashtable_walk_stop_rcu(&hti);
	rhashtable_walk_exit(&hti);
	return rc;
}

static int __rhashtable_insert_fast_locked(
	void *parg, struct rhashtable *ht, const void *key, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	struct rhashtable_compare_arg arg = {
		.ht = ht,
		.key = key,
	};
	struct bucket_table *tbl, *new_tbl;
	struct rhash_head *head;
	unsigned int elasticity;
	unsigned int hash = parg_hash(parg);
	int err;

restart:
	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);

	/* All insertions must grab the oldest table containing
	 * the hashed bucket that is yet to be rehashed.
	 */
	for (;;) {
		
		if (tbl->rehash <= hash)
			break;

		tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	}

	new_tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	if (unlikely(new_tbl)) {
		tbl = rhashtable_insert_slow(ht, key, obj, new_tbl);
		if (!IS_ERR_OR_NULL(tbl))
			goto slow_path;

		err = PTR_ERR(tbl);
		goto out;
	}

	err = -E2BIG;
	if (unlikely(rht_grow_above_max(ht, tbl)))
		goto out;

	if (unlikely(rht_grow_above_100(ht, tbl))) {
slow_path:
		
		err = rhashtable_insert_rehash(ht, tbl);
		rcu_read_unlock();
		if (err)
			return err;

		goto restart;
	}

	err = -EEXIST;
	elasticity = ht->elasticity;
	rht_for_each(head, tbl, hash) {
		if (key &&
		    unlikely(!(params.obj_cmpfn ?
			       params.obj_cmpfn(&arg, rht_obj(ht, head)) :
			       rhashtable_compare(&arg, rht_obj(ht, head)))))
			goto out;
		if (!--elasticity)
			goto slow_path;
	}

	err = 0;

	head = rht_dereference_bucket(tbl->buckets[hash], tbl, hash);

	RCU_INIT_POINTER(obj->next, head);

	rcu_assign_pointer(tbl->buckets[hash], obj);

	atomic_inc(&ht->nelems);
	if (rht_grow_above_75(ht, tbl))
		schedule_work(&ht->run_work);

out:
	rcu_read_unlock();

	return err;
}

static inline int rhashtable_lookup_insert_key_locked(
	void *parg, struct rhashtable *ht, const void *key,
	struct rhash_head *obj, const struct rhashtable_params params)
{
	BUG_ON(!ht->p.obj_hashfn || !key);

	return __rhashtable_insert_fast_locked(parg, ht, key, obj, params);
}

/* In this function, the condition xtbl->dead has not been checked before expansion */
static int rht_fxid_add_locked(void *parg, struct fib_xid_table *xtbl,
				struct fib_xid *fxid)
{
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	int rc;
	
	rc =  rhashtable_lookup_insert_key_locked(parg, &rxtbl->rht, fxid->fx_xid, &rfxid->node, &rht_params);
	if(IS_ERR(rc))
		goto out;
	
	atomic_inc(&xtbl->fxt_count);
 out: 	
	return rc;
}

/* In this function, the condition xtbl->dead has not been checked before expansion */ 
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

static inline int __rhashtable_remove_fast_locked(
	struct rhashtable *ht, struct bucket_table *tbl,
	struct rhash_head *obj, const struct rhashtable_params params)
{
	struct rhash_head __rcu **pprev;
	struct rhash_head *he;
	unsigned int hash;
	int err = -ENOENT;

	hash = rht_head_hashfn(ht, tbl, obj, params);
	pprev = &tbl->buckets[hash];
	rht_for_each(he, tbl, hash) {
		if (he != obj) {
			pprev = &he->next;
			continue;
		}

		rcu_assign_pointer(*pprev, obj->next);
		err = 0;
		break;
	}

	return err;
}

static inline int rhashtable_remove_fast_locked(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	struct bucket_table *tbl;
	int err;

	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);

	/* Because we have already taken (and released) the bucket
	 * lock in old_tbl, if we find that future_tbl is not yet
	 * visible then that guarantees the entry to still be in
	 * the old tbl if it exists.
	 */
	while ((err = __rhashtable_remove_fast_locked(ht, tbl, obj, params)) &&
	       (tbl = rht_dereference_rcu(tbl->future_tbl, ht)))
		;

	if (err)
		goto out;

	atomic_dec(&ht->nelems);
	if (unlikely(ht->p.automatic_shrinking &&
		     rht_shrink_below_30(ht, tbl)))
		schedule_work(&ht->run_work);

out:
	rcu_read_unlock();

	return err;
}

static void rht_fxid_rm_locked(void *parg, struct fib_xid_table *xtbl,
				struct fib_xid *fxid)
{
	struct rht_fib_xid_table *rxtlb = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	rhashtable_remove_fast_locked(&rxtbl->rht, &rfxid->node, rht_params);
	atomic_dec(&xtbl->fxt_count);
}

static void rht_fxid_rm(struct fib_xid_table *xtbl, struct fib_xid *fxid)
{
	struct rht_fib_xid_table *rxtlb = xtbl_rxtbl(xtbl);
	struct rht_fib_xid *rfxid = fxid_rfxid(fxid);
	rhashtable_remove_fast(&rxtbl->rht, &rfxid->node, rht_params);
	atomic_dec(&xtbl->fxt_count);
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

static inline int __rhashtable_replace_fast_locked(
	struct rhashtable *ht, struct bucket_table *tbl,
	struct rhash_head *obj_old, struct rhash_head *obj_new,
	const struct rhashtable_params params)
{
	struct rhash_head __rcu **pprev;
	struct rhash_head *he;
	unsigned int hash;
	int err = -ENOENT;

	/* Minimally, the old and new objects must have same hash
	 * (which should mean identifiers are the same).
	 */
	hash = rht_head_hashfn(ht, tbl, obj_old, params);
	if (hash != rht_head_hashfn(ht, tbl, obj_new, params))
		return -EINVAL;

	pprev = &tbl->buckets[hash];
	rht_for_each(he, tbl, hash) {
		if (he != obj_old) {
			pprev = &he->next;
			continue;
		}

		rcu_assign_pointer(obj_new->next, obj_old->next);
		rcu_assign_pointer(*pprev, obj_new);
		err = 0;
		break;
	}

	return err;
}

static inline int rhashtable_replace_fast_locked(
	struct rhashtable *ht, struct rhash_head *obj_old,
	struct rhash_head *obj_new,
	const struct rhashtable_params params)
{
	struct bucket_table *tbl;
	int err;

	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);

	/* Because we have already taken (and released) the bucket
	 * lock in old_tbl, if we find that future_tbl is not yet
	 * visible then that guarantees the entry to still be in
	 * the old tbl if it exists.
	 */
	while ((err = __rhashtable_replace_fast_locked(ht, tbl, obj_old,
						obj_new, params)) &&
	       (tbl = rht_dereference_rcu(tbl->future_tbl, ht)))
		;

	rcu_read_unlock();

	return err;
}

static void rht_fxid_replace_locked(struct fib_xid_table *xtbl,
				     struct fib_xid *old_fxid,
				     struct fib_xid *new_fxid)
{
	struct rht_fib_xid *old_rfxid = fxid_rfxid(old_fxid);
	struct rht_fib_xid *new_rfxid = fxid_rfxid(new_fxid);
	struct rht_fib_xid_table *rxtbl = xtbl_rxtbl(xtbl);
	rhashtable_replace_fast_locked(&rxtbl->rht, &old_rfxid->node, &new_rfxid->node, rht_params);
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
	.xtbl_init = rht_xtbl_init,
	.xtbl_death_work = rht_xtbl_death_work,

	.fxid_ppal_alloc = rht_fxid_ppal_alloc,
	.fxid_init = rht_fxid_init,

	.fxid_find_rcu = rht_fxid_find_rcu,
	.fxid_find_lock = rht_fxid_find_lock,
	.iterate_xids = rht_iterate_xids,
	.iterate_xids_rcu = rht_iterate_xids_rcu,

	.fxid_add = rht_fxid_add,
	.fxid_add_locked = rht_fxid_add_locked,

	.fxid_rm = rht_fxid_rm,
	.fxid_rm_locked = rht_fxid_rm_locked,
	.xid_rm = rht_xid_rm,

	.fxid_replace_locked = rht_fxid_replace_locked,

	.fib_unlock = rht_fib_unlock,

	.fib_newroute = rht_fib_newroute,
	.fib_delroute = rht_fib_delroute,

	.xtbl_dump_rcu = rht_xtbl_dump_rcu,
};
EXPORT_SYMBOL_GPL(xia_ppal_rht_rt_iops);
