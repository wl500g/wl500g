/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * A small micro-assembler. It is intentionally kept simple, does only
 * support a subset of instructions, and does not try to hide pipeline
 * effects like branch delay slots.
 *
 * Copyright (C) 2004, 2005, 2006, 2008  Thiemo Seufer
 * Copyright (C) 2005, 2007  Maciej W. Rozycki
 * Copyright (C) 2006  Ralf Baechle (ralf@linux-mips.org)
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/smp.h>
#include <linux/init.h>


#include <asm/inst.h>
#include <asm/bugs.h>

#include <asm/uasm.h>

enum fields {
	RS = 0x001,
	RT = 0x002,
	RD = 0x004,
	RE = 0x008,
	SIMM = 0x010,
	UIMM = 0x020,
	BIMM = 0x040,
	JIMM = 0x080,
	FUNC = 0x100,
	SET = 0x200,
	SCIMM = 0x400
};

#define OP_MASK		0x3f
#define OP_SH		26
#define RS_MASK		0x1f
#define RS_SH		21
#define RT_MASK		0x1f
#define RT_SH		16
#define RD_MASK		0x1f
#define RD_SH		11
#define RE_MASK		0x1f
#define RE_SH		6
#define IMM_MASK	0xffff
#define IMM_SH		0
#define JIMM_MASK	0x3ffffff
#define JIMM_SH		0
#define FUNC_MASK	0x3f
#define FUNC_SH		0
#define SET_MASK	0x7
#define SET_SH		0
#define SCIMM_MASK	0xfffff
#define SCIMM_SH	6

struct insn {
	enum opcode opcode;
	u32 match;
	enum fields fields;
};

/* This macro sets the non-variable bits of an instruction. */
#define M(a, b, c, d, e, f)					\
	((a) << OP_SH						\
	 | (b) << RS_SH						\
	 | (c) << RT_SH						\
	 | (d) << RD_SH						\
	 | (e) << RE_SH						\
	 | (f) << FUNC_SH)

static struct insn insn_table[] __cpuinitdata = {
	{ insn_addiu, M(addiu_op, 0, 0, 0, 0, 0), RS | RT | SIMM },
	{ insn_addu, M(spec_op, 0, 0, 0, 0, addu_op), RS | RT | RD },
	{ insn_and, M(spec_op, 0, 0, 0, 0, and_op), RS | RT | RD },
	{ insn_andi, M(andi_op, 0, 0, 0, 0, 0), RS | RT | UIMM },
	{ insn_beq, M(beq_op, 0, 0, 0, 0, 0), RS | RT | BIMM },
	{ insn_beql, M(beql_op, 0, 0, 0, 0, 0), RS | RT | BIMM },
	{ insn_bgez, M(bcond_op, 0, bgez_op, 0, 0, 0), RS | BIMM },
	{ insn_bgezl, M(bcond_op, 0, bgezl_op, 0, 0, 0), RS | BIMM },
	{ insn_bltz, M(bcond_op, 0, bltz_op, 0, 0, 0), RS | BIMM },
	{ insn_bltzl, M(bcond_op, 0, bltzl_op, 0, 0, 0), RS | BIMM },
	{ insn_bne, M(bne_op, 0, 0, 0, 0, 0), RS | RT | BIMM },
	{ insn_daddiu, M(daddiu_op, 0, 0, 0, 0, 0), RS | RT | SIMM },
	{ insn_daddu, M(spec_op, 0, 0, 0, 0, daddu_op), RS | RT | RD },
	{ insn_dmfc0, M(cop0_op, dmfc_op, 0, 0, 0, 0), RT | RD | SET},
	{ insn_dmtc0, M(cop0_op, dmtc_op, 0, 0, 0, 0), RT | RD | SET},
	{ insn_dsll, M(spec_op, 0, 0, 0, 0, dsll_op), RT | RD | RE },
	{ insn_dsll32, M(spec_op, 0, 0, 0, 0, dsll32_op), RT | RD | RE },
	{ insn_dsra, M(spec_op, 0, 0, 0, 0, dsra_op), RT | RD | RE },
	{ insn_dsrl, M(spec_op, 0, 0, 0, 0, dsrl_op), RT | RD | RE },
	{ insn_dsrl32, M(spec_op, 0, 0, 0, 0, dsrl32_op), RT | RD | RE },
	{ insn_dsubu, M(spec_op, 0, 0, 0, 0, dsubu_op), RS | RT | RD },
	{ insn_eret,  M(cop0_op, cop_op, 0, 0, 0, eret_op),  0 },
	{ insn_ins, M(spec3_op, 0, 0, 0, 0, ins_op), RS | RT | RD | RE },
	{ insn_ext, M(spec3_op, 0, 0, 0, 0, ext_op), RS | RT | RD | RE },
	{ insn_j,  M(j_op, 0, 0, 0, 0, 0),  JIMM },
	{ insn_jal,  M(jal_op, 0, 0, 0, 0, 0),  JIMM },
	{ insn_jr,  M(spec_op, 0, 0, 0, 0, jr_op),  RS },
	{ insn_ld,  M(ld_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_ll,  M(ll_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_lld,  M(lld_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_lui,  M(lui_op, 0, 0, 0, 0, 0),  RT | SIMM },
	{ insn_lw,  M(lw_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_mfc0,  M(cop0_op, mfc_op, 0, 0, 0, 0),  RT | RD | SET},
	{ insn_mtc0,  M(cop0_op, mtc_op, 0, 0, 0, 0),  RT | RD | SET},
	{ insn_or,  M(spec_op, 0, 0, 0, 0, or_op),  RS | RT | RD },
	{ insn_ori,  M(ori_op, 0, 0, 0, 0, 0),  RS | RT | UIMM },
	{ insn_rfe,  M(cop0_op, cop_op, 0, 0, 0, rfe_op),  0 },
	{ insn_sc,  M(sc_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_scd,  M(scd_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_sd,  M(sd_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_sll,  M(spec_op, 0, 0, 0, 0, sll_op),  RT | RD | RE },
	{ insn_sra,  M(spec_op, 0, 0, 0, 0, sra_op),  RT | RD | RE },
	{ insn_srl,  M(spec_op, 0, 0, 0, 0, srl_op),  RT | RD | RE },
	{ insn_subu,  M(spec_op, 0, 0, 0, 0, subu_op),  RS | RT | RD },
	{ insn_sw,  M(sw_op, 0, 0, 0, 0, 0),  RS | RT | SIMM },
	{ insn_tlbp,  M(cop0_op, cop_op, 0, 0, 0, tlbp_op),  0 },
	{ insn_tlbwi,  M(cop0_op, cop_op, 0, 0, 0, tlbwi_op),  0 },
	{ insn_tlbwr,  M(cop0_op, cop_op, 0, 0, 0, tlbwr_op),  0 },
	{ insn_xor,  M(spec_op, 0, 0, 0, 0, xor_op),  RS | RT | RD },
	{ insn_xori,  M(xori_op, 0, 0, 0, 0, 0),  RS | RT | UIMM },
	{ insn_syscall, M(spec_op, 0, 0, 0, 0, syscall_op), SCIMM},
	{ insn_invalid, 0, 0 }
};

#undef M

static inline __cpuinit u32 build_rs(u32 arg)
{
	WARN(arg & ~RS_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return (arg & RS_MASK) << RS_SH;
}

static inline __cpuinit u32 build_rt(u32 arg)
{
	WARN(arg & ~RT_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return (arg & RT_MASK) << RT_SH;
}

static inline __cpuinit u32 build_rd(u32 arg)
{
	WARN(arg & ~RD_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return (arg & RD_MASK) << RD_SH;
}

static inline __cpuinit u32 build_re(u32 arg)
{
	WARN(arg & ~RE_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return (arg & RE_MASK) << RE_SH;
}

static inline __cpuinit u32 build_simm(s32 arg)
{
	WARN(arg > 0x7fff || arg < -0x8000,
	     KERN_WARNING "Micro-assembler field overflow\n");

	return arg & 0xffff;
}

static inline __cpuinit u32 build_uimm(u32 arg)
{
	WARN(arg & ~IMM_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return arg & IMM_MASK;
}

static inline __cpuinit u32 build_bimm(s32 arg)
{
	WARN(arg > 0x1ffff || arg < -0x20000,
	     KERN_WARNING "Micro-assembler field overflow\n");

	if (arg & 0x3)
		printk(KERN_WARNING "Invalid micro-assembler branch target\n");

	return ((arg < 0) ? (1 << 15) : 0) | ((arg >> 2) & 0x7fff);
}

static inline __cpuinit u32 build_jimm(u32 arg)
{
	WARN(arg & ~((JIMM_MASK) << 2),
	     KERN_WARNING "Micro-assembler field overflow\n");

	return (arg >> 2) & JIMM_MASK;
}

static inline __cpuinit u32 build_scimm(u32 arg)
{
	WARN(arg & ~SCIMM_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return (arg & SCIMM_MASK) << SCIMM_SH;
}

static inline __cpuinit u32 build_func(u32 arg)
{
	WARN(arg & ~FUNC_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return arg & FUNC_MASK;
}

static inline __cpuinit u32 build_set(u32 arg)
{
	WARN(arg & ~SET_MASK, KERN_WARNING "Micro-assembler field overflow\n");

	return arg & SET_MASK;
}

/*
 * The order of opcode arguments is implicitly left to right,
 * starting with RS and ending with FUNC or IMM.
 */
void __cpuinit build_insn(u32 **buf, enum opcode opc, ...)
{
	struct insn *ip = NULL;
	unsigned int i;
	va_list ap;
	u32 op;

	for (i = 0; insn_table[i].opcode != insn_invalid; i++)
		if (insn_table[i].opcode == opc) {
			ip = &insn_table[i];
			break;
		}

	if (!ip /*|| (opc == insn_daddiu && r4k_daddiu_bug())*/)
		panic("Unsupported Micro-assembler instruction %d", opc);

	op = ip->match;
	va_start(ap, opc);
	if (ip->fields & RS)
		op |= build_rs(va_arg(ap, u32));
	if (ip->fields & RT)
		op |= build_rt(va_arg(ap, u32));
	if (ip->fields & RD)
		op |= build_rd(va_arg(ap, u32));
	if (ip->fields & RE)
		op |= build_re(va_arg(ap, u32));
	if (ip->fields & SIMM)
		op |= build_simm(va_arg(ap, s32));
	if (ip->fields & UIMM)
		op |= build_uimm(va_arg(ap, u32));
	if (ip->fields & BIMM)
		op |= build_bimm(va_arg(ap, s32));
	if (ip->fields & JIMM)
		op |= build_jimm(va_arg(ap, u32));
	if (ip->fields & FUNC)
		op |= build_func(va_arg(ap, u32));
	if (ip->fields & SET)
		op |= build_set(va_arg(ap, u32));
	if (ip->fields & SCIMM)
		op |= build_scimm(va_arg(ap, u32));
	va_end(ap);

	**buf = op;
	(*buf)++;
}


/* Handle labels. */
void __cpuinit UASM_i_LA_mostly(u32 **buf, unsigned int rs, long addr)
{
	if (!uasm_in_compat_space_p(addr)) {
		uasm_i_lui(buf, rs, uasm_rel_highest(addr));
		if (uasm_rel_higher(addr))
			uasm_i_daddiu(buf, rs, rs, uasm_rel_higher(addr));
		if (uasm_rel_hi(addr)) {
			uasm_i_dsll(buf, rs, rs, 16);
			uasm_i_daddiu(buf, rs, rs, uasm_rel_hi(addr));
			uasm_i_dsll(buf, rs, rs, 16);
		} else
			uasm_i_dsll32(buf, rs, rs, 0);
	} else
		uasm_i_lui(buf, rs, uasm_rel_hi(addr));
}

void __cpuinit UASM_i_LA(u32 **buf, unsigned int rs, long addr)
{
	UASM_i_LA_mostly(buf, rs, addr);
	if (uasm_rel_lo(addr)) {
		if (!uasm_in_compat_space_p(addr))
			uasm_i_daddiu(buf, rs, rs, uasm_rel_lo(addr));
		else
			uasm_i_addiu(buf, rs, rs, uasm_rel_lo(addr));
	}
}

/* Handle relocations. */
static inline void __cpuinit
__resolve_relocs(struct uasm_reloc *rel, struct uasm_label *lab)
{
	long laddr = (long)lab->addr;
	long raddr = (long)rel->addr;

	switch (rel->type) {
	case R_MIPS_PC16:
		*rel->addr |= build_bimm(laddr - (raddr + 4));
		break;

	default:
		panic("Unsupported Micro-assembler relocation %d",
		      rel->type);
	}
}

void __cpuinit
uasm_resolve_relocs(struct uasm_reloc *rel, struct uasm_label *lab)
{
	struct uasm_label *l;

	for (; rel->lab != UASM_LABEL_INVALID; rel++)
		for (l = lab; l->lab != UASM_LABEL_INVALID; l++)
			if (rel->lab == l->lab)
				__resolve_relocs(rel, l);
}

void __cpuinit
uasm_move_relocs(struct uasm_reloc *rel, u32 *first, u32 *end, long off)
{
	for (; rel->lab != UASM_LABEL_INVALID; rel++)
		if (rel->addr >= first && rel->addr < end)
			rel->addr += off;
}

void __cpuinit
uasm_move_labels(struct uasm_label *lab, u32 *first, u32 *end, long off)
{
	for (; lab->lab != UASM_LABEL_INVALID; lab++)
		if (lab->addr >= first && lab->addr < end)
			lab->addr += off;
}

void __cpuinit
uasm_copy_handler(struct uasm_reloc *rel, struct uasm_label *lab, u32 *first,
		  u32 *end, u32 *target)
{
	long off = (long)(target - first);

	memcpy(target, first, (end - first) * sizeof(u32));

	uasm_move_relocs(rel, first, end, off);
	uasm_move_labels(lab, first, end, off);
}

int __cpuinit uasm_insn_has_bdelay(struct uasm_reloc *rel, u32 *addr)
{
	for (; rel->lab != UASM_LABEL_INVALID; rel++) {
		if (rel->addr == addr
		    && (rel->type == R_MIPS_PC16
			|| rel->type == R_MIPS_26))
			return 1;
	}

	return 0;
}

/* Convenience functions for labeled branches. */
void __cpuinit
uasm_il_bltz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_bltz(p, reg, 0);
}

void __cpuinit
uasm_il_b(u32 **p, struct uasm_reloc **r, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_b(p, 0);
}

void __cpuinit
uasm_il_beqz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_beqz(p, reg, 0);
}

void __cpuinit
uasm_il_beqzl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_beqzl(p, reg, 0);
}

void __cpuinit
uasm_il_bnez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_bnez(p, reg, 0);
}

void __cpuinit
uasm_il_bgezl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_bgezl(p, reg, 0);
}

void __cpuinit
uasm_il_bgez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid)
{
	uasm_r_mips_pc16(r, *p, lid);
	uasm_i_bgez(p, reg, 0);
}
