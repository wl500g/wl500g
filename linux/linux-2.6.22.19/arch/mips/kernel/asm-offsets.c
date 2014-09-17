/*
 * offset.c: Calculate pt_regs and task_struct offsets.
 *
 * Copyright (C) 1996 David S. Miller
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 *
 * Kevin Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.
 */
#define __GENERATING_OFFSETS_S
#include <linux/compat.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <asm/gdb-stub.h>
#include <asm/ptrace.h>
#include <asm/processor.h>

#define text(t) __asm__("\n->#" t)
#define _offset(type, member) (&(((type *)NULL)->member))
#define offset(string, ptr, member) \
	__asm__("\n->" string " %0" : : "i" (_offset(ptr, member)))
#define constant(string, member) \
	__asm__("\n->" string " %0" : : "ri" (member))
#define size(string, size) \
	__asm__("\n->" string " %0" : : "i" (sizeof(size)))
#define linefeed text("")

void output_ptreg_defines(void)
{
	text("MIPS pt_regs offsets.");
	offset("PT_R0", struct pt_regs, regs[0]);
	offset("PT_R1", struct pt_regs, regs[1]);
	offset("PT_R2", struct pt_regs, regs[2]);
	offset("PT_R3", struct pt_regs, regs[3]);
	offset("PT_R4", struct pt_regs, regs[4]);
	offset("PT_R5", struct pt_regs, regs[5]);
	offset("PT_R6", struct pt_regs, regs[6]);
	offset("PT_R7", struct pt_regs, regs[7]);
	offset("PT_R8", struct pt_regs, regs[8]);
	offset("PT_R9", struct pt_regs, regs[9]);
	offset("PT_R10", struct pt_regs, regs[10]);
	offset("PT_R11", struct pt_regs, regs[11]);
	offset("PT_R12", struct pt_regs, regs[12]);
	offset("PT_R13", struct pt_regs, regs[13]);
	offset("PT_R14", struct pt_regs, regs[14]);
	offset("PT_R15", struct pt_regs, regs[15]);
	offset("PT_R16", struct pt_regs, regs[16]);
	offset("PT_R17", struct pt_regs, regs[17]);
	offset("PT_R18", struct pt_regs, regs[18]);
	offset("PT_R19", struct pt_regs, regs[19]);
	offset("PT_R20", struct pt_regs, regs[20]);
	offset("PT_R21", struct pt_regs, regs[21]);
	offset("PT_R22", struct pt_regs, regs[22]);
	offset("PT_R23", struct pt_regs, regs[23]);
	offset("PT_R24", struct pt_regs, regs[24]);
	offset("PT_R25", struct pt_regs, regs[25]);
	offset("PT_R26", struct pt_regs, regs[26]);
	offset("PT_R27", struct pt_regs, regs[27]);
	offset("PT_R28", struct pt_regs, regs[28]);
	offset("PT_R29", struct pt_regs, regs[29]);
	offset("PT_R30", struct pt_regs, regs[30]);
	offset("PT_R31", struct pt_regs, regs[31]);
	offset("PT_LO", struct pt_regs, lo);
	offset("PT_HI", struct pt_regs, hi);
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	offset("PT_ACX", struct pt_regs, acx);
#endif
	offset("PT_EPC", struct pt_regs, cp0_epc);
	offset("PT_BVADDR", struct pt_regs, cp0_badvaddr);
	offset("PT_STATUS", struct pt_regs, cp0_status);
	offset("PT_CAUSE", struct pt_regs, cp0_cause);
#ifdef CONFIG_MIPS_MT_SMTC
	offset("PT_TCSTATUS", struct pt_regs, cp0_tcstatus);
#endif /* CONFIG_MIPS_MT_SMTC */
	size("PT_SIZE", struct pt_regs);
	linefeed;
}

void output_task_defines(void)
{
	text("MIPS task_struct offsets.");
	offset("TASK_STATE", struct task_struct, state);
	offset("TASK_THREAD_INFO", struct task_struct, stack);
	offset("TASK_FLAGS", struct task_struct, flags);
	offset("TASK_MM", struct task_struct, mm);
	offset("TASK_PID", struct task_struct, pid);
	size(  "TASK_STRUCT_SIZE", struct task_struct);
	linefeed;
}

void output_thread_info_defines(void)
{
	text("MIPS thread_info offsets.");
	offset("TI_TASK", struct thread_info, task);
	offset("TI_EXEC_DOMAIN", struct thread_info, exec_domain);
	offset("TI_FLAGS", struct thread_info, flags);
	offset("TI_TP_VALUE", struct thread_info, tp_value);
	offset("TI_CPU", struct thread_info, cpu);
	offset("TI_PRE_COUNT", struct thread_info, preempt_count);
	offset("TI_ADDR_LIMIT", struct thread_info, addr_limit);
	offset("TI_RESTART_BLOCK", struct thread_info, restart_block);
	offset("TI_REGS", struct thread_info, regs);
	constant("_THREAD_SIZE", THREAD_SIZE);
	constant("_THREAD_MASK", THREAD_MASK);
	linefeed;
}

void output_thread_defines(void)
{
	text("MIPS specific thread_struct offsets.");
	offset("THREAD_REG16", struct task_struct, thread.reg16);
	offset("THREAD_REG17", struct task_struct, thread.reg17);
	offset("THREAD_REG18", struct task_struct, thread.reg18);
	offset("THREAD_REG19", struct task_struct, thread.reg19);
	offset("THREAD_REG20", struct task_struct, thread.reg20);
	offset("THREAD_REG21", struct task_struct, thread.reg21);
	offset("THREAD_REG22", struct task_struct, thread.reg22);
	offset("THREAD_REG23", struct task_struct, thread.reg23);
	offset("THREAD_REG29", struct task_struct, thread.reg29);
	offset("THREAD_REG30", struct task_struct, thread.reg30);
	offset("THREAD_REG31", struct task_struct, thread.reg31);
	offset("THREAD_STATUS", struct task_struct,
	       thread.cp0_status);
	offset("THREAD_FPU", struct task_struct, thread.fpu);

	offset("THREAD_BVADDR", struct task_struct, \
	       thread.cp0_badvaddr);
	offset("THREAD_BUADDR", struct task_struct, \
	       thread.cp0_baduaddr);
	offset("THREAD_ECODE", struct task_struct, \
	       thread.error_code);
	linefeed;
}

void output_thread_fpu_defines(void)
{
	offset("THREAD_FPR0",
	       struct task_struct, thread.fpu.fpr[0]);
	offset("THREAD_FPR1",
	       struct task_struct, thread.fpu.fpr[1]);
	offset("THREAD_FPR2",
	       struct task_struct, thread.fpu.fpr[2]);
	offset("THREAD_FPR3",
	       struct task_struct, thread.fpu.fpr[3]);
	offset("THREAD_FPR4",
	       struct task_struct, thread.fpu.fpr[4]);
	offset("THREAD_FPR5",
	       struct task_struct, thread.fpu.fpr[5]);
	offset("THREAD_FPR6",
	       struct task_struct, thread.fpu.fpr[6]);
	offset("THREAD_FPR7",
	       struct task_struct, thread.fpu.fpr[7]);
	offset("THREAD_FPR8",
	       struct task_struct, thread.fpu.fpr[8]);
	offset("THREAD_FPR9",
	       struct task_struct, thread.fpu.fpr[9]);
	offset("THREAD_FPR10",
	       struct task_struct, thread.fpu.fpr[10]);
	offset("THREAD_FPR11",
	       struct task_struct, thread.fpu.fpr[11]);
	offset("THREAD_FPR12",
	       struct task_struct, thread.fpu.fpr[12]);
	offset("THREAD_FPR13",
	       struct task_struct, thread.fpu.fpr[13]);
	offset("THREAD_FPR14",
	       struct task_struct, thread.fpu.fpr[14]);
	offset("THREAD_FPR15",
	       struct task_struct, thread.fpu.fpr[15]);
	offset("THREAD_FPR16",
	       struct task_struct, thread.fpu.fpr[16]);
	offset("THREAD_FPR17",
	       struct task_struct, thread.fpu.fpr[17]);
	offset("THREAD_FPR18",
	       struct task_struct, thread.fpu.fpr[18]);
	offset("THREAD_FPR19",
	       struct task_struct, thread.fpu.fpr[19]);
	offset("THREAD_FPR20",
	       struct task_struct, thread.fpu.fpr[20]);
	offset("THREAD_FPR21",
	       struct task_struct, thread.fpu.fpr[21]);
	offset("THREAD_FPR22",
	       struct task_struct, thread.fpu.fpr[22]);
	offset("THREAD_FPR23",
	       struct task_struct, thread.fpu.fpr[23]);
	offset("THREAD_FPR24",
	       struct task_struct, thread.fpu.fpr[24]);
	offset("THREAD_FPR25",
	       struct task_struct, thread.fpu.fpr[25]);
	offset("THREAD_FPR26",
	       struct task_struct, thread.fpu.fpr[26]);
	offset("THREAD_FPR27",
	       struct task_struct, thread.fpu.fpr[27]);
	offset("THREAD_FPR28",
	       struct task_struct, thread.fpu.fpr[28]);
	offset("THREAD_FPR29",
	       struct task_struct, thread.fpu.fpr[29]);
	offset("THREAD_FPR30",
	       struct task_struct, thread.fpu.fpr[30]);
	offset("THREAD_FPR31",
	       struct task_struct, thread.fpu.fpr[31]);

	offset("THREAD_FCR31",
	       struct task_struct, thread.fpu.fcr31);
	linefeed;
}

void output_mm_defines(void)
{
	text("Size of struct page");
	size("STRUCT_PAGE_SIZE", struct page);
	linefeed;
	text("Linux mm_struct offsets.");
	offset("MM_USERS", struct mm_struct, mm_users);
	offset("MM_PGD", struct mm_struct, pgd);
	offset("MM_CONTEXT", struct mm_struct, context);
	linefeed;
	constant("_PAGE_SIZE", PAGE_SIZE);
	constant("_PAGE_SHIFT", PAGE_SHIFT);
	linefeed;
	constant("_PGD_T_SIZE", sizeof(pgd_t));
	constant("_PMD_T_SIZE", sizeof(pmd_t));
	constant("_PTE_T_SIZE", sizeof(pte_t));
	linefeed;
	constant("_PGD_T_LOG2", PGD_T_LOG2);
	constant("_PMD_T_LOG2", PMD_T_LOG2);
	constant("_PTE_T_LOG2", PTE_T_LOG2);
	linefeed;
	constant("_PGD_ORDER", PGD_ORDER);
	constant("_PMD_ORDER", PMD_ORDER);
	constant("_PTE_ORDER", PTE_ORDER);
	linefeed;
	constant("_PMD_SHIFT", PMD_SHIFT);
	constant("_PGDIR_SHIFT", PGDIR_SHIFT);
	linefeed;
	constant("_PTRS_PER_PGD", PTRS_PER_PGD);
	constant("_PTRS_PER_PMD", PTRS_PER_PMD);
	constant("_PTRS_PER_PTE", PTRS_PER_PTE);
	linefeed;
}

#ifdef CONFIG_32BIT
void output_sc_defines(void)
{
	text("Linux sigcontext offsets.");
	offset("SC_REGS", struct sigcontext, sc_regs);
	offset("SC_FPREGS", struct sigcontext, sc_fpregs);
	offset("SC_ACX", struct sigcontext, sc_acx);
	offset("SC_MDHI", struct sigcontext, sc_mdhi);
	offset("SC_MDLO", struct sigcontext, sc_mdlo);
	offset("SC_PC", struct sigcontext, sc_pc);
	offset("SC_FPC_CSR", struct sigcontext, sc_fpc_csr);
	offset("SC_FPC_EIR", struct sigcontext, sc_fpc_eir);
	offset("SC_HI1", struct sigcontext, sc_hi1);
	offset("SC_LO1", struct sigcontext, sc_lo1);
	offset("SC_HI2", struct sigcontext, sc_hi2);
	offset("SC_LO2", struct sigcontext, sc_lo2);
	offset("SC_HI3", struct sigcontext, sc_hi3);
	offset("SC_LO3", struct sigcontext, sc_lo3);
	linefeed;
}
#endif

#ifdef CONFIG_64BIT
void output_sc_defines(void)
{
	text("Linux sigcontext offsets.");
	offset("SC_REGS", struct sigcontext, sc_regs);
	offset("SC_FPREGS", struct sigcontext, sc_fpregs);
	offset("SC_MDHI", struct sigcontext, sc_mdhi);
	offset("SC_MDLO", struct sigcontext, sc_mdlo);
	offset("SC_PC", struct sigcontext, sc_pc);
	offset("SC_FPC_CSR", struct sigcontext, sc_fpc_csr);
	linefeed;
}
#endif

#ifdef CONFIG_MIPS32_COMPAT
void output_sc32_defines(void)
{
	text("Linux 32-bit sigcontext offsets.");
	offset("SC32_FPREGS", struct sigcontext32, sc_fpregs);
	offset("SC32_FPC_CSR", struct sigcontext32, sc_fpc_csr);
	offset("SC32_FPC_EIR", struct sigcontext32, sc_fpc_eir);
	linefeed;
}
#endif

void output_signal_defined(void)
{
	text("Linux signal numbers.");
	constant("_SIGHUP", SIGHUP);
	constant("_SIGINT", SIGINT);
	constant("_SIGQUIT", SIGQUIT);
	constant("_SIGILL", SIGILL);
	constant("_SIGTRAP", SIGTRAP);
	constant("_SIGIOT", SIGIOT);
	constant("_SIGABRT", SIGABRT);
	constant("_SIGEMT", SIGEMT);
	constant("_SIGFPE", SIGFPE);
	constant("_SIGKILL", SIGKILL);
	constant("_SIGBUS", SIGBUS);
	constant("_SIGSEGV", SIGSEGV);
	constant("_SIGSYS", SIGSYS);
	constant("_SIGPIPE", SIGPIPE);
	constant("_SIGALRM", SIGALRM);
	constant("_SIGTERM", SIGTERM);
	constant("_SIGUSR1", SIGUSR1);
	constant("_SIGUSR2", SIGUSR2);
	constant("_SIGCHLD", SIGCHLD);
	constant("_SIGPWR", SIGPWR);
	constant("_SIGWINCH", SIGWINCH);
	constant("_SIGURG", SIGURG);
	constant("_SIGIO", SIGIO);
	constant("_SIGSTOP", SIGSTOP);
	constant("_SIGTSTP", SIGTSTP);
	constant("_SIGCONT", SIGCONT);
	constant("_SIGTTIN", SIGTTIN);
	constant("_SIGTTOU", SIGTTOU);
	constant("_SIGVTALRM", SIGVTALRM);
	constant("_SIGPROF", SIGPROF);
	constant("_SIGXCPU", SIGXCPU);
	constant("_SIGXFSZ", SIGXFSZ);
	linefeed;
}

void output_irq_cpustat_t_defines(void)
{
	text("Linux irq_cpustat_t offsets.");
	offset("IC_SOFTIRQ_PENDING", irq_cpustat_t, __softirq_pending);
	size("IC_IRQ_CPUSTAT_T", irq_cpustat_t);
	linefeed;
}

void output_gdbreg_defines(void)
{
	text("MIPS struct gdb_regs offsets.");
	offset("GDB_FR_REG0",  struct gdb_regs, reg0);
	offset("GDB_FR_REG1",  struct gdb_regs, reg1);
	offset("GDB_FR_REG2",  struct gdb_regs, reg2);
	offset("GDB_FR_REG3",  struct gdb_regs, reg3);
	offset("GDB_FR_REG4",  struct gdb_regs, reg4);
	offset("GDB_FR_REG5",  struct gdb_regs, reg5);
	offset("GDB_FR_REG6",  struct gdb_regs, reg6);
	offset("GDB_FR_REG7",  struct gdb_regs, reg7);
	offset("GDB_FR_REG8",  struct gdb_regs, reg8);
	offset("GDB_FR_REG9",  struct gdb_regs, reg9);
	offset("GDB_FR_REG10", struct gdb_regs, reg10);
	offset("GDB_FR_REG11", struct gdb_regs, reg11);
	offset("GDB_FR_REG12", struct gdb_regs, reg12);
	offset("GDB_FR_REG13", struct gdb_regs, reg13);
	offset("GDB_FR_REG14", struct gdb_regs, reg14);
	offset("GDB_FR_REG15", struct gdb_regs, reg15);
	offset("GDB_FR_REG16", struct gdb_regs, reg16);
	offset("GDB_FR_REG17", struct gdb_regs, reg17);
	offset("GDB_FR_REG18", struct gdb_regs, reg18);
	offset("GDB_FR_REG19", struct gdb_regs, reg19);
	offset("GDB_FR_REG20", struct gdb_regs, reg20);
	offset("GDB_FR_REG21", struct gdb_regs, reg21);
	offset("GDB_FR_REG22", struct gdb_regs, reg22);
	offset("GDB_FR_REG23", struct gdb_regs, reg23);
	offset("GDB_FR_REG24", struct gdb_regs, reg24);
	offset("GDB_FR_REG25", struct gdb_regs, reg25);
	offset("GDB_FR_REG26", struct gdb_regs, reg26);
	offset("GDB_FR_REG27", struct gdb_regs, reg27);
	offset("GDB_FR_REG28", struct gdb_regs, reg28);
	offset("GDB_FR_REG29", struct gdb_regs, reg29);
	offset("GDB_FR_REG30", struct gdb_regs, reg30);
	offset("GDB_FR_REG31", struct gdb_regs, reg31);
	linefeed;

	offset("GDB_FR_STATUS", struct gdb_regs, cp0_status);
	offset("GDB_FR_HI",     struct gdb_regs, hi);
	offset("GDB_FR_LO",     struct gdb_regs, lo);
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	offset("GDB_FR_ACX",    struct gdb_regs, acx);
#endif
	offset("GDB_FR_BADVADDR", struct gdb_regs, cp0_badvaddr);
	offset("GDB_FR_CAUSE",    struct gdb_regs, cp0_cause);
	offset("GDB_FR_EPC",      struct gdb_regs, cp0_epc);
	linefeed;

	offset("GDB_FR_FPR0",  struct gdb_regs, fpr0);
	offset("GDB_FR_FPR1",  struct gdb_regs, fpr1);
	offset("GDB_FR_FPR2",  struct gdb_regs, fpr2);
	offset("GDB_FR_FPR3",  struct gdb_regs, fpr3);
	offset("GDB_FR_FPR4",  struct gdb_regs, fpr4);
	offset("GDB_FR_FPR5",  struct gdb_regs, fpr5);
	offset("GDB_FR_FPR6",  struct gdb_regs, fpr6);
	offset("GDB_FR_FPR7",  struct gdb_regs, fpr7);
	offset("GDB_FR_FPR8",  struct gdb_regs, fpr8);
	offset("GDB_FR_FPR9",  struct gdb_regs, fpr9);
	offset("GDB_FR_FPR10", struct gdb_regs, fpr10);
	offset("GDB_FR_FPR11", struct gdb_regs, fpr11);
	offset("GDB_FR_FPR12", struct gdb_regs, fpr12);
	offset("GDB_FR_FPR13", struct gdb_regs, fpr13);
	offset("GDB_FR_FPR14", struct gdb_regs, fpr14);
	offset("GDB_FR_FPR15", struct gdb_regs, fpr15);
	offset("GDB_FR_FPR16", struct gdb_regs, fpr16);
	offset("GDB_FR_FPR17", struct gdb_regs, fpr17);
	offset("GDB_FR_FPR18", struct gdb_regs, fpr18);
	offset("GDB_FR_FPR19", struct gdb_regs, fpr19);
	offset("GDB_FR_FPR20", struct gdb_regs, fpr20);
	offset("GDB_FR_FPR21", struct gdb_regs, fpr21);
	offset("GDB_FR_FPR22", struct gdb_regs, fpr22);
	offset("GDB_FR_FPR23", struct gdb_regs, fpr23);
	offset("GDB_FR_FPR24", struct gdb_regs, fpr24);
	offset("GDB_FR_FPR25", struct gdb_regs, fpr25);
	offset("GDB_FR_FPR26", struct gdb_regs, fpr26);
	offset("GDB_FR_FPR27", struct gdb_regs, fpr27);
	offset("GDB_FR_FPR28", struct gdb_regs, fpr28);
	offset("GDB_FR_FPR29", struct gdb_regs, fpr29);
	offset("GDB_FR_FPR30", struct gdb_regs, fpr30);
	offset("GDB_FR_FPR31", struct gdb_regs, fpr31);
	linefeed;

	offset("GDB_FR_FSR",   struct gdb_regs, cp1_fsr);
	offset("GDB_FR_FIR",   struct gdb_regs, cp1_fir);
	offset("GDB_FR_FRP",   struct gdb_regs, frame_ptr);
	offset("GDB_FR_DUMMY", struct gdb_regs, dummy);

	offset("GDB_FR_CP0_INDEX   ", struct gdb_regs, cp0_index);
	offset("GDB_FR_CP0_RANDOM  ", struct gdb_regs, cp0_random);
	offset("GDB_FR_CP0_ENTRYLO0", struct gdb_regs, cp0_entrylo0);
	offset("GDB_FR_CP0_ENTRYLO1", struct gdb_regs, cp0_entrylo1);
	offset("GDB_FR_CP0_CONTEXT ", struct gdb_regs, cp0_context);
	offset("GDB_FR_CP0_PAGEMASK", struct gdb_regs, cp0_pagemask);
	offset("GDB_FR_CP0_WIRED   ", struct gdb_regs, cp0_wired);
	offset("GDB_FR_CP0_REG7    ", struct gdb_regs, cp0_reg7);
	offset("GDB_FR_CP0_REG8    ", struct gdb_regs, cp0_reg8);
	offset("GDB_FR_CP0_REG9    ", struct gdb_regs, cp0_reg9);
	offset("GDB_FR_CP0_ENTRYHI ", struct gdb_regs, cp0_entryhi);
	offset("GDB_FR_CP0_REG11   ", struct gdb_regs, cp0_reg11);
	offset("GDB_FR_CP0_REG12   ", struct gdb_regs, cp0_reg12);
	offset("GDB_FR_CP0_REG13   ", struct gdb_regs, cp0_reg13);
	offset("GDB_FR_CP0_REG14   ", struct gdb_regs, cp0_reg14);
	offset("GDB_FR_CP0_PRID    ", struct gdb_regs, cp0_prid);
	size("GDB_FR_SIZE", struct gdb_regs);
}
