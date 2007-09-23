/*
 * Early initialization code for BCM94710 boards
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: prom.c,v 1.5 2009/06/08 17:32:33 Exp $
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <hndsoc.h>
#include <siutils.h>
#include <hndcpu.h>
#include <mipsinc.h>
#include <mips74k_core.h>

#define MB      << 20

#ifdef  CONFIG_HIGHMEM

#define EXTVBASE        0xc0000000
#define ENTRYLO(x)      ((pte_val(pfn_pte((x) >> PAGE_SHIFT, PAGE_KERNEL_UNCACHED)) >> 6) | 1)
#define UNIQUE_ENTRYHI(idx) (CKSEG0 + ((idx) << (PAGE_SHIFT + 1)))

static unsigned long tmp_tlb_ent __initdata;

/* Initialize the wired register and all tlb entries to 
 * known good state.
 */
void __init
early_tlb_init(void)
{
	unsigned long  index;
	struct cpuinfo_mips *c = &current_cpu_data;

	tmp_tlb_ent = c->tlbsize;

	/* printk(KERN_ALERT "%s: tlb size %ld\n", __FUNCTION__, c->tlbsize); */

	/*
	* initialize entire TLB to uniqe virtual addresses
	* but with the PAGE_VALID bit not set
	*/
	write_c0_wired(0);
	write_c0_pagemask(PM_DEFAULT_MASK);

	write_c0_entrylo0(0);   /* not _PAGE_VALID */
	write_c0_entrylo1(0);

	for (index = 0; index < c->tlbsize; index++) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(index+32));
		write_c0_index(index);
		mtc0_tlbw_hazard();
		tlb_write_indexed();
	}

	tlbw_use_hazard();

}

void __init
add_tmptlb_entry(unsigned long entrylo0, unsigned long entrylo1,
		 unsigned long entryhi, unsigned long pagemask)
{
/* write one tlb entry */
	--tmp_tlb_ent;
	write_c0_index(tmp_tlb_ent);
	write_c0_pagemask(pagemask);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();
}
#endif  /* CONFIG_HIGHMEM */

void __init
prom_init(void)
{
	unsigned long mem, before, offset;
#ifdef  CONFIG_HIGHMEM
	unsigned long extmem = 0;
#endif

	mips_machgroup = MACH_GROUP_BRCM;
	mips_machtype = MACH_BCM947XX;

	/* Figure out memory size by finding aliases.
	 *
	 * We assume that there will be no more than 128 MB of low memory,
	 * and that the memory size will be a multiple of 1 MB.
	 *
	 * We set 'before' to be the amount of memory (in MB) before this
	 * function, i.e. one MB less than the number  of MB of memory that we
	 * *know* we have.  And we set 'offset' to be the address of 'prominit'
	 * minus 'before', so that KSEG0 or KSEG1 base + offset < 4 MB.
	 * This prevents us from overrunning 128 MB and causing a bus error.
 	 */

	before = ((unsigned long) &prom_init) & (127 MB);
	offset = ((unsigned long) &prom_init) - before;
	for (mem = before + (1 MB); mem < (128 MB); mem += (1 MB)) {
		if (*(unsigned long *)(offset + mem) ==
		    *(unsigned long *)(prom_init)) {
			/*
			 * We may already be well past the end of memory at
			 * this point, so we'll have to compensate for it.
			 */
			mem -= before;
			break;
		}
	}

#ifdef  CONFIG_HIGHMEM
	if (mem == 128 MB) {

		early_tlb_init();
		/* Add one temporary TLB entries to map SDRAM Region 2.
		*      Physical        Virtual
		*      0x80000000      0xc0000000      (1st: 256MB)
		*      0x90000000      0xd0000000      (2nd: 256MB)
		*/
		add_tmptlb_entry(ENTRYLO(SI_SDRAM_R2),
				 ENTRYLO(SI_SDRAM_R2 + (256 MB)),
				 EXTVBASE, PM_256M);

		offset = EXTVBASE + __pa(offset);
		for (extmem = (128 MB); extmem < (512 MB); extmem <<= 1) {
			if (*(unsigned long *)(offset + extmem) ==
				*(unsigned long *)(prom_init)) {
				break;
			}
		}

		extmem -= mem;
		/* Keep tlb entries back in consistent state */
		early_tlb_init();
	}
#endif  /* CONFIG_HIGHMEM */

	add_memory_region(SI_SDRAM_BASE, mem, BOOT_MEM_RAM);

#ifdef  CONFIG_HIGHMEM
	if (extmem) {
		/* We should deduct 0x1000 from the second memory
		 * region, because of the fact that processor does prefetch.
		 * Now that we are deducting a page from second memory 
		 * region, we could add the earlier deducted 4KB (from first bank)
		 * to the second region (the fact that 0x80000000 -> 0x88000000
		 * shadows 0x0 -> 0x8000000)
		 */
		if (MIPS74K(current_cpu_data.processor_id) && (mem == (128 MB)))
			extmem -= 0x1000;
		add_memory_region(SI_SDRAM_R2 + (128 MB) - 0x1000, extmem, BOOT_MEM_RAM);
	}
#endif  /* CONFIG_HIGHMEM */
}

void __init
prom_free_prom_memory(void)
{
}
