/*
 * net/sched/sch_fifo.c	The simplest FIFO queue.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <net/pkt_sched.h>

/* 1 band FIFO pseudo-"scheduler" */

struct fifo_sched_data
{
	u32 limit;
};

static int bfifo_enqueue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct fifo_sched_data *q = qdisc_priv(sch);

	if (likely(sch->qstats.backlog + skb->len <= q->limit))
		return qdisc_enqueue_tail(skb, sch);

	return qdisc_reshape_fail(skb, sch);
}

static int pfifo_enqueue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct fifo_sched_data *q = qdisc_priv(sch);

	if (likely(skb_queue_len(&sch->q) < q->limit))
		return qdisc_enqueue_tail(skb, sch);

	return qdisc_reshape_fail(skb, sch);
}

static int fifo_init(struct Qdisc *sch, struct rtattr *opt)
{
	struct fifo_sched_data *q = qdisc_priv(sch);

	if (opt == NULL) {
		u32 limit = sch->dev->tx_queue_len ? : 1;

		if (sch->ops == &bfifo_qdisc_ops)
			limit *= sch->dev->mtu;

		q->limit = limit;
	} else {
		struct tc_fifo_qopt *ctl = RTA_DATA(opt);

		if (RTA_PAYLOAD(opt) < sizeof(*ctl))
			return -EINVAL;

		q->limit = ctl->limit;
	}

	return 0;
}

static int fifo_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct fifo_sched_data *q = qdisc_priv(sch);
	struct tc_fifo_qopt opt = { .limit = q->limit };

	RTA_PUT(skb, TCA_OPTIONS, sizeof(opt), &opt);
	return skb->len;

rtattr_failure:
	return -1;
}

struct Qdisc_ops pfifo_qdisc_ops = {
	.id		=	"pfifo",
	.priv_size	=	sizeof(struct fifo_sched_data),
	.enqueue	=	pfifo_enqueue,
	.dequeue	=	qdisc_dequeue_head,
	.requeue	=	qdisc_requeue,
	.drop		=	qdisc_queue_drop,
	.init		=	fifo_init,
	.reset		=	qdisc_reset_queue,
	.change		=	fifo_init,
	.dump		=	fifo_dump,
	.owner		=	THIS_MODULE,
};

struct Qdisc_ops bfifo_qdisc_ops = {
	.id		=	"bfifo",
	.priv_size	=	sizeof(struct fifo_sched_data),
	.enqueue	=	bfifo_enqueue,
	.dequeue	=	qdisc_dequeue_head,
	.requeue	=	qdisc_requeue,
	.drop		=	qdisc_queue_drop,
	.init		=	fifo_init,
	.reset		=	qdisc_reset_queue,
	.change		=	fifo_init,
	.dump		=	fifo_dump,
	.owner		=	THIS_MODULE,
};

EXPORT_SYMBOL(bfifo_qdisc_ops);
EXPORT_SYMBOL(pfifo_qdisc_ops);

/* Pass size change message down to embedded FIFO */
int fifo_set_limit(struct Qdisc *q, unsigned int limit)
{
	struct rtattr *rta;
	int ret = -ENOMEM;

	/* Hack to avoid sending change message to non-FIFO */
	if (strncmp(q->ops->id + 1, "fifo", 4) != 0)
		return 0;

	rta = kmalloc(RTA_LENGTH(sizeof(struct tc_fifo_qopt)), GFP_KERNEL);
	if (rta) {
		rta->rta_type = RTM_NEWQDISC;
		rta->rta_len = RTA_LENGTH(sizeof(struct tc_fifo_qopt));
		((struct tc_fifo_qopt *)RTA_DATA(rta))->limit = limit;

		ret = q->ops->change(q, rta);
		kfree(rta);
	}
	return ret;
}
EXPORT_SYMBOL(fifo_set_limit);

struct Qdisc *fifo_create_dflt(struct Qdisc *sch, struct Qdisc_ops *ops,
			       unsigned int limit)
{
	struct Qdisc *q;
	int err = -ENOMEM;

	q = qdisc_create_dflt(sch->dev, ops, TC_H_MAKE(sch->handle, 1));
	if (q) {
		err = fifo_set_limit(q, limit);
		if (err < 0) {
			qdisc_destroy(q);
			q = NULL;
		}
	}

	return q ? : ERR_PTR(err);
}
EXPORT_SYMBOL(fifo_create_dflt);
