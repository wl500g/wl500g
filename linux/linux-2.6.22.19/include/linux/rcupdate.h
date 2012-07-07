/*
 * Read-Copy Update mechanism for mutual exclusion 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright IBM Corporation, 2001
 *
 * Author: Dipankar Sarma <dipankar@in.ibm.com>
 * 
 * Based on the original work by Paul McKenney <paulmck@us.ibm.com>
 * and inputs from Rusty Russell, Andrea Arcangeli and Andi Kleen.
 * Papers:
 * http://www.rdrop.com/users/paulmck/paper/rclockpdcsproof.pdf
 * http://lse.sourceforge.net/locking/rclock_OLS.2001.05.01c.sc.pdf (OLS2001)
 *
 * For detailed explanation of Read-Copy Update mechanism see -
 * 		http://lse.sourceforge.net/locking/rcupdate.html
 *
 */

#ifndef __LINUX_RCUPDATE_H
#define __LINUX_RCUPDATE_H

#ifdef __KERNEL__

#include <linux/cache.h>
#include <linux/spinlock.h>
#include <linux/threads.h>
#include <linux/percpu.h>
#include <linux/cpumask.h>
#include <linux/seqlock.h>
#include <linux/lockdep.h>
#include <linux/compiler.h>
#include <linux/completion.h>

/**
 * struct rcu_head - callback structure for use with RCU
 * @next: next update requests in a list
 * @func: actual update function to call after the grace period.
 */
struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *head);
};

/* Internal to kernel, but needed by rcupreempt.h. */
extern int rcu_scheduler_active;

#include <linux/rcuclassic.h>

#define RCU_HEAD_INIT 	{ .next = NULL, .func = NULL }
#define RCU_HEAD(head) struct rcu_head head = RCU_HEAD_INIT
#define INIT_RCU_HEAD(ptr) do { \
       (ptr)->next = NULL; (ptr)->func = NULL; \
} while (0)


/*
 * rcu_read_lock_bh_held() is defined out of line to avoid #include-file
 * hell.
 */
extern int rcu_read_lock_bh_held(void);

extern int rcu_needs_cpu(int cpu);

#ifdef CONFIG_DEBUG_LOCK_ALLOC

#ifdef CONFIG_PROVE_RCU
extern int rcu_is_cpu_idle(void);
#else /* !CONFIG_PROVE_RCU */
static inline int rcu_is_cpu_idle(void)
{
	return 0;
}
#endif /* else !CONFIG_PROVE_RCU */

extern int debug_lockdep_rcu_enabled(void);
extern struct lockdep_map rcu_lock_map;
# define rcu_read_acquire()	lock_acquire(&rcu_lock_map, 0, 0, 2, 1, _THIS_IP_)
# define rcu_read_release()	lock_release(&rcu_lock_map, 1, _THIS_IP_)
#else
# define rcu_read_acquire()	do { } while (0)
# define rcu_read_release()	do { } while (0)
#endif /* else CONFIG_DEBUG_LOCK_ALLOC */

/**
 * rcu_read_lock - mark the beginning of an RCU read-side critical section.
 *
 * When synchronize_rcu() is invoked on one CPU while other CPUs
 * are within RCU read-side critical sections, then the
 * synchronize_rcu() is guaranteed to block until after all the other
 * CPUs exit their critical sections.  Similarly, if call_rcu() is invoked
 * on one CPU while other CPUs are within RCU read-side critical
 * sections, invocation of the corresponding RCU callback is deferred
 * until after the all the other CPUs exit their critical sections.
 *
 * Note, however, that RCU callbacks are permitted to run concurrently
 * with RCU read-side critical sections.  One way that this can happen
 * is via the following sequence of events: (1) CPU 0 enters an RCU
 * read-side critical section, (2) CPU 1 invokes call_rcu() to register
 * an RCU callback, (3) CPU 0 exits the RCU read-side critical section,
 * (4) CPU 2 enters a RCU read-side critical section, (5) the RCU
 * callback is invoked.  This is legal, because the RCU read-side critical
 * section that was running concurrently with the call_rcu() (and which
 * therefore might be referencing something that the corresponding RCU
 * callback would free up) has completed before the corresponding
 * RCU callback is invoked.
 *
 * RCU read-side critical sections may be nested.  Any deferred actions
 * will be deferred until the outermost RCU read-side critical section
 * completes.
 *
 * It is illegal to block while in an RCU read-side critical section.
 */
#define rcu_read_lock() __rcu_read_lock()

/**
 * rcu_read_unlock - marks the end of an RCU read-side critical section.
 *
 * See rcu_read_lock() for more information.
 */

/*
 * So where is rcu_write_lock()?  It does not exist, as there is no
 * way for writers to lock out RCU readers.  This is a feature, not
 * a bug -- this property is what provides RCU's performance benefits.
 * Of course, writers must coordinate with each other.  The normal
 * spinlock primitives work well for this, but any other technique may be
 * used as well.  RCU does not care how the writers keep out of each
 * others' way, as long as they do so.
 */
#define rcu_read_unlock() __rcu_read_unlock()

/**
 * rcu_read_lock_bh - mark the beginning of a softirq-only RCU critical section
 *
 * This is equivalent of rcu_read_lock(), but to be used when updates
 * are being done using call_rcu_bh(). Since call_rcu_bh() callbacks
 * consider completion of a softirq handler to be a quiescent state,
 * a process in RCU read-side critical section must be protected by
 * disabling softirqs. Read-side critical sections in interrupt context
 * can use just rcu_read_lock().
 *
 */
#define rcu_read_lock_bh() __rcu_read_lock_bh()

/*
 * rcu_read_unlock_bh - marks the end of a softirq-only RCU critical section
 *
 * See rcu_read_lock_bh() for more information.
 */
#define rcu_read_unlock_bh() __rcu_read_unlock_bh()

/**
 * rcu_read_lock_sched - mark the beginning of a RCU-classic critical section
 *
 * Should be used with either
 * - synchronize_sched()
 * or
 * - call_rcu_sched() and rcu_barrier_sched()
 * on the write-side to insure proper synchronization.
 */
#define rcu_read_lock_sched() preempt_disable()
#define rcu_read_lock_sched_notrace() preempt_disable_notrace()

/*
 * rcu_read_unlock_sched - marks the end of a RCU-classic critical section
 *
 * See rcu_read_lock_sched for more information.
 */
#define rcu_read_unlock_sched() preempt_enable()
#define rcu_read_unlock_sched_notrace() preempt_enable_notrace()


#ifdef CONFIG_PROVE_LOCKING

extern int debug_lockdep_rcu_enabled(void);

/**
 * rcu_read_lock_held() - might we be in RCU read-side critical section?
 *
 * If CONFIG_DEBUG_LOCK_ALLOC is selected, returns nonzero iff in an RCU
 * read-side critical section.  In absence of CONFIG_DEBUG_LOCK_ALLOC,
 * this assumes we are in an RCU read-side critical section unless it can
 * prove otherwise.  This is useful for debug checks in functions that
 * require that they be called within an RCU read-side critical section.
 *
 * Checks debug_lockdep_rcu_enabled() to prevent false positives during boot
 * and while lockdep is disabled.
 *
 * Note that rcu_read_lock() and the matching rcu_read_unlock() must
 * occur in the same context, for example, it is illegal to invoke
 * rcu_read_unlock() in process context if the matching rcu_read_lock()
 * was invoked from within an irq handler.
 */
static inline int rcu_read_lock_held(void)
{
	if (!debug_lockdep_rcu_enabled())
		return 1;
	return lock_is_held(&rcu_lock_map);
}

/**
 * rcu_dereference_check - rcu_dereference with debug checking
 *
 * Do an rcu_dereference(), but check that the context is correct.
 * For example, rcu_dereference_check(gp, rcu_read_lock_held()) to
 * ensure that the rcu_dereference_check() executes within an RCU
 * read-side critical section.  It is also possible to check for
 * locks being held, for example, by using lockdep_is_held().
 */
#define rcu_dereference_check(p, c) \
	({ \
		if (debug_lockdep_rcu_enabled() && !(c)) \
			lockdep_rcu_suspicious(__FILE__, __LINE__, \
				"suspicious rcu_dereference_check() usage"); \
		rcu_dereference_raw(p); \
	})

/**
 * rcu_dereference_protected - fetch RCU pointer when updates prevented
 *
 * Return the value of the specified RCU-protected pointer, but omit
 * both the smp_read_barrier_depends() and the ACCESS_ONCE().  This
 * is useful in cases where update-side locks prevent the value of the
 * pointer from changing.  Please note that this primitive does -not-
 * prevent the compiler from repeating this reference or combining it
 * with other references, so it should not be used without protection
 * of appropriate locks.
 */
#define rcu_dereference_protected(p, c) \
	({ \
		if (debug_lockdep_rcu_enabled() && !(c)) \
			lockdep_rcu_suspicious(__FILE__, __LINE__, \
				"suspicious rcu_dereference_protected() usage"); \
		(p); \
	})

#else

#define rcu_dereference_check(p, c)	rcu_dereference_raw(p)
#define rcu_dereference_protected(p, c) (p)

#endif /* CONFIG_PROVE_LOCKING */

/**
 * rcu_access_pointer - fetch RCU pointer with no dereferencing
 *
 * Return the value of the specified RCU-protected pointer, but omit the
 * smp_read_barrier_depends() and keep the ACCESS_ONCE().  This is useful
 * when the value of this pointer is accessed, but the pointer is not
 * dereferenced, for example, when testing an RCU-protected pointer against
 * NULL.  This may also be used in cases where update-side locks prevent
 * the value of the pointer from changing, but rcu_dereference_protected()
 * is a lighter-weight primitive for this use case.
 */
#define rcu_access_pointer(p)	ACCESS_ONCE(p)

/**
 * rcu_dereference_raw - fetch an RCU-protected pointer
 *
 * The caller must be within some flavor of RCU read-side critical
 * section, or must be otherwise preventing the pointer from changing,
 * for example, by holding an appropriate lock.  This pointer may later
 * be safely dereferenced.  It is the caller's responsibility to have
 * done the right thing, as this primitive does no checking of any kind.
 *
 * Inserts memory barriers on architectures that require them
 * (currently only the Alpha), and, more importantly, documents
 * exactly which pointers are protected by RCU.
 */
#define rcu_dereference_raw(p)	({ \
				typeof(p) _________p1 = ACCESS_ONCE(p); \
				smp_read_barrier_depends(); \
				(_________p1); \
				})

/**
 * rcu_dereference - fetch an RCU-protected pointer, checking for RCU
 *
 * Makes rcu_dereference_check() do the dirty work.
 */
#define rcu_dereference(p) \
	rcu_dereference_check(p, rcu_read_lock_held())

/**
 * rcu_dereference_bh - fetch an RCU-protected pointer, checking for RCU-bh
 *
 * Makes rcu_dereference_check() do the dirty work.
 */
#define rcu_dereference_bh(p) \
		rcu_dereference_check(p, rcu_read_lock_bh_held())

/**
 * rcu_dereference_sched - fetch RCU-protected pointer, checking for RCU-sched
 *
 * Makes rcu_dereference_check() do the dirty work.
 */
#define rcu_dereference_sched(p) \
		rcu_dereference_check(p, rcu_read_lock_sched_held())

/**
 * rcu_assign_pointer - assign (publicize) a pointer to a newly
 * initialized structure that will be dereferenced by RCU read-side
 * critical sections.  Returns the value assigned.
 *
 * Inserts memory barriers on architectures that require them
 * (pretty much all of them other than x86), and also prevents
 * the compiler from reordering the code that initializes the
 * structure after the pointer assignment.  More importantly, this
 * call documents which pointers will be dereferenced by RCU read-side
 * code.
 */

#define rcu_assign_pointer(p, v) \
	({ \
		if (!__builtin_constant_p(v) || \
		    ((v) != NULL)) \
			smp_wmb(); \
		(p) = (v); \
	})

/**
 * RCU_INIT_POINTER() - initialize an RCU protected pointer
 *
 * Initialize an RCU-protected pointer in such a way to avoid RCU-lockdep
 * splats.
 */
#define RCU_INIT_POINTER(p, v) \
		p = (typeof(*v) __force __rcu *)(v)

/* Infrastructure to implement the synchronize_() primitives. */

struct rcu_synchronize {
	struct rcu_head head;
	struct completion completion;
};

extern void wakeme_after_rcu(struct rcu_head  *head);

/**
 * synchronize_sched - block until all CPUs have exited any non-preemptive
 * kernel code sequences.
 *
 * This means that all preempt_disable code sequences, including NMI and
 * hardware-interrupt handlers, in progress on entry will have completed
 * before this primitive returns.  However, this does not guarantee that
 * softirq handlers will have completed, since in some kernels, these
 * handlers can run in process context, and can block.
 *
 * This primitive provides the guarantees made by the (now removed)
 * synchronize_kernel() API.  In contrast, synchronize_rcu() only
 * guarantees that rcu_read_lock() sections will have completed.
 * In "classic RCU", these two guarantees happen to be one and
 * the same, but can differ in realtime RCU implementations.
 */
#define synchronize_sched() __synchronize_sched()

/**
 * call_rcu - Queue an RCU callback for invocation after a grace period.
 * @head: structure to be used for queueing the RCU updates.
 * @func: actual update function to be invoked after the grace period
 *
 * The update function will be invoked some time after a full grace
 * period elapses, in other words after all currently executing RCU
 * read-side critical sections have completed.  RCU read-side critical
 * sections are delimited by rcu_read_lock() and rcu_read_unlock(),
 * and may be nested.
 */
extern void call_rcu(struct rcu_head *head,
			      void (*func)(struct rcu_head *head));

/**
 * call_rcu_bh - Queue an RCU for invocation after a quicker grace period.
 * @head: structure to be used for queueing the RCU updates.
 * @func: actual update function to be invoked after the grace period
 *
 * The update function will be invoked some time after a full grace
 * period elapses, in other words after all currently executing RCU
 * read-side critical sections have completed. call_rcu_bh() assumes
 * that the read-side critical sections end on completion of a softirq
 * handler. This means that read-side critical sections in process
 * context must not be interrupted by softirqs. This interface is to be
 * used when most of the read-side critical sections are in softirq context.
 * RCU read-side critical sections are delimited by :
 *  - rcu_read_lock() and  rcu_read_unlock(), if in interrupt context.
 *  OR
 *  - rcu_read_lock_bh() and rcu_read_unlock_bh(), if in process context.
 *  These may be nested.
 */
extern void call_rcu_bh(struct rcu_head *head,
			void (*func)(struct rcu_head *head));

/* Exported common interfaces */
extern void call_rcu_sched(struct rcu_head *head,
                           void (*func)(struct rcu_head *rcu));
extern void synchronize_rcu(void);
extern void rcu_barrier(void);
extern void rcu_barrier_bh(void);
extern void rcu_barrier_sched(void);

/* Internal to kernel */
extern void rcu_init(void);
extern void rcu_scheduler_starting(void);
extern void rcu_check_callbacks(int cpu, int user);

extern long rcu_batches_completed(void);
extern long rcu_batches_completed_bh(void);

#endif /* __KERNEL__ */
#endif /* __LINUX_RCUPDATE_H */
