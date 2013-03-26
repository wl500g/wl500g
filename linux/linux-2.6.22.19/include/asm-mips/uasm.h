/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, 2005, 2006, 2008  Thiemo Seufer
 * Copyright (C) 2005  Maciej W. Rozycki
 * Copyright (C) 2006  Ralf Baechle (ralf@linux-mips.org)
 */

#include <linux/types.h>
#include <asm/elf.h>

enum opcode {
	insn_invalid,
	insn_addu, insn_addiu, insn_and, insn_andi, insn_beq,
	insn_beql, insn_bgez, insn_bgezl, insn_bltz, insn_bltzl,
	insn_bne, insn_daddu, insn_daddiu, insn_dmfc0, insn_dmtc0,
	insn_dsll, insn_dsll32, insn_dsra, insn_dsrl, insn_dsrl32,
	insn_dsubu, insn_eret, insn_ext, insn_ins,
	insn_j, insn_jal, insn_jr, insn_ld,
	insn_ll, insn_lld, insn_lui, insn_lw, insn_mfc0, insn_mtc0,
	insn_or, insn_ori, insn_rfe, insn_sc, insn_scd, insn_sd, insn_sll,
	insn_sra, insn_srl, insn_subu, insn_sw, insn_tlbp, insn_tlbwi,
	insn_tlbwr, insn_xor, insn_xori,
	insn_syscall
};

void __cpuinit build_insn(u32 **buf, enum opcode opc, ...);

#define Ip_u1u2u3(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u2u1u3(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u3u1u2(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u1u2s3(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, signed int c)

#define Ip_u2s3u1(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, signed int b, unsigned int c)

#define Ip_u2u1s3(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, signed int c)

#define Ip_u2u1msbu3(op)						\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c,	\
	   unsigned int d)

#define Ip_u2u1mmsbu3(op)						\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c,	\
	   unsigned int d)

#define Ip_u1u2(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, unsigned int b)

#define Ip_u1s2(op)							\
static inline void __cpuinit						\
uasm_i##op(u32 **buf, unsigned int a, signed int b)

#define Ip_u1(op)							\
static inline void __cpuinit uasm_i##op(u32 **buf, unsigned int a)

#define Ip_0(op)							\
static inline void __cpuinit uasm_i##op(u32 **buf)

#define I_u1u2u3(op)					\
Ip_u1u2u3(op)						\
{							\
	build_insn(buf, insn##op, a, b, c);		\
}

#define I_u2u1u3(op)					\
Ip_u2u1u3(op)						\
{							\
	build_insn(buf, insn##op, b, a, c);		\
}

#define I_u3u1u2(op)					\
Ip_u3u1u2(op)						\
{							\
	build_insn(buf, insn##op, b, c, a);		\
}

#define I_u1u2s3(op)					\
Ip_u1u2s3(op)						\
{							\
	build_insn(buf, insn##op, a, b, c);		\
}

#define I_u2s3u1(op)					\
Ip_u2s3u1(op)						\
{							\
	build_insn(buf, insn##op, c, a, b);		\
}

#define I_u2u1s3(op)					\
Ip_u2u1s3(op)						\
{							\
	build_insn(buf, insn##op, b, a, c);		\
}

#define I_u2u1msbu3(op)					\
Ip_u2u1msbu3(op)					\
{							\
	build_insn(buf, insn##op, b, a, c+d-1, c);	\
}							\

#define I_u2u1mmsbu3(op)				\
Ip_u2u1mmsbu3(op)					\
{							\
	build_insn(buf, insn##op, b, a, d-1, c);	\
}							\

#define I_u1u2(op)					\
Ip_u1u2(op)						\
{							\
	build_insn(buf, insn##op, a, b);		\
}

#define I_u1s2(op)					\
Ip_u1s2(op)						\
{							\
	build_insn(buf, insn##op, a, b);		\
}

#define I_u1(op)					\
Ip_u1(op)						\
{							\
	build_insn(buf, insn##op, a);			\
}

#define I_0(op)						\
Ip_0(op)						\
{							\
	build_insn(buf, insn##op);			\
}

I_u2u1s3(_addiu)
I_u3u1u2(_addu)
I_u2u1u3(_andi)
I_u3u1u2(_and)
I_u1u2s3(_beq)
I_u1u2s3(_beql)
I_u1s2(_bgez)
I_u1s2(_bgezl)
I_u1s2(_bltz)
I_u1s2(_bltzl)
I_u1u2s3(_bne)
I_u1u2u3(_dmfc0)
I_u1u2u3(_dmtc0)
I_u2u1s3(_daddiu)
I_u3u1u2(_daddu)
I_u2u1u3(_dsll)
I_u2u1u3(_dsll32)
I_u2u1u3(_dsra)
I_u2u1u3(_dsrl)
I_u2u1u3(_dsrl32)
I_u3u1u2(_dsubu)
I_0(_eret)
I_u2u1mmsbu3(_ext)
I_u2u1msbu3(_ins)
I_u1(_j)
I_u1(_jal)
I_u1(_jr)
I_u2s3u1(_ld)
I_u2s3u1(_ll)
I_u2s3u1(_lld)
I_u1s2(_lui)
I_u2s3u1(_lw)
I_u1u2u3(_mfc0)
I_u1u2u3(_mtc0)
I_u2u1u3(_ori)
I_u3u1u2(_or)
I_0(_rfe)
I_u2s3u1(_sc)
I_u2s3u1(_scd)
I_u2s3u1(_sd)
I_u2u1u3(_sll)
I_u2u1u3(_sra)
I_u2u1u3(_srl)
I_u3u1u2(_subu)
I_u2s3u1(_sw)
I_0(_tlbp)
I_0(_tlbwi)
I_0(_tlbwr)
I_u3u1u2(_xor)
I_u2u1u3(_xori)
I_u1(_syscall)

/* Handle labels. */
struct uasm_label {
	u32 *addr;
	int lab;
};

static inline void __cpuinit uasm_build_label(struct uasm_label **lab, u32 *addr, int lid)
{
	(*lab)->addr = addr;
	(*lab)->lab = lid;
	(*lab)++;
}

static inline int __cpuinit uasm_in_compat_space_p(long addr)
{
	/* Is this address in 32bit compat space? */
#ifdef CONFIG_64BIT
	return (((addr) & 0xffffffff00000000L) == 0xffffffff00000000L);
#else
	return 1;
#endif
}

static inline int __cpuinit uasm_rel_highest(long val)
{
#ifdef CONFIG_64BIT
	return ((((val + 0x800080008000L) >> 48) & 0xffff) ^ 0x8000) - 0x8000;
#else
	return 0;
#endif
}

static inline int __cpuinit uasm_rel_higher(long val)
{
#ifdef CONFIG_64BIT
	return ((((val + 0x80008000L) >> 32) & 0xffff) ^ 0x8000) - 0x8000;
#else
	return 0;
#endif
}

static inline int __cpuinit uasm_rel_hi(long val)
{
	return ((((val + 0x8000L) >> 16) & 0xffff) ^ 0x8000) - 0x8000;
}

static inline int __cpuinit uasm_rel_lo(long val)
{
	return ((val & 0xffff) ^ 0x8000) - 0x8000;
}
void UASM_i_LA_mostly(u32 **buf, unsigned int rs, long addr);
void UASM_i_LA(u32 **buf, unsigned int rs, long addr);

#define UASM_L_LA(lb)							\
static inline void __cpuinit uasm_l##lb(struct uasm_label **lab, u32 *addr) \
{									\
	uasm_build_label(lab, addr, label##lb);				\
}

/* convenience macros for instructions */
#ifdef CONFIG_64BIT
# define UASM_i_LW(buf, rs, rt, off) uasm_i_ld(buf, rs, rt, off)
# define UASM_i_SW(buf, rs, rt, off) uasm_i_sd(buf, rs, rt, off)
# define UASM_i_SLL(buf, rs, rt, sh) uasm_i_dsll(buf, rs, rt, sh)
# define UASM_i_SRA(buf, rs, rt, sh) uasm_i_dsra(buf, rs, rt, sh)
# define UASM_i_SRL(buf, rs, rt, sh) uasm_i_dsrl(buf, rs, rt, sh)
# define UASM_i_MFC0(buf, rt, rd...) uasm_i_dmfc0(buf, rt, rd)
# define UASM_i_MTC0(buf, rt, rd...) uasm_i_dmtc0(buf, rt, rd)
# define UASM_i_ADDIU(buf, rs, rt, val) uasm_i_daddiu(buf, rs, rt, val)
# define UASM_i_ADDU(buf, rs, rt, rd) uasm_i_daddu(buf, rs, rt, rd)
# define UASM_i_SUBU(buf, rs, rt, rd) uasm_i_dsubu(buf, rs, rt, rd)
# define UASM_i_LL(buf, rs, rt, off) uasm_i_lld(buf, rs, rt, off)
# define UASM_i_SC(buf, rs, rt, off) uasm_i_scd(buf, rs, rt, off)
#else
# define UASM_i_LW(buf, rs, rt, off) uasm_i_lw(buf, rs, rt, off)
# define UASM_i_SW(buf, rs, rt, off) uasm_i_sw(buf, rs, rt, off)
# define UASM_i_SLL(buf, rs, rt, sh) uasm_i_sll(buf, rs, rt, sh)
# define UASM_i_SRA(buf, rs, rt, sh) uasm_i_sra(buf, rs, rt, sh)
# define UASM_i_SRL(buf, rs, rt, sh) uasm_i_srl(buf, rs, rt, sh)
# define UASM_i_MFC0(buf, rt, rd...) uasm_i_mfc0(buf, rt, rd)
# define UASM_i_MTC0(buf, rt, rd...) uasm_i_mtc0(buf, rt, rd)
# define UASM_i_ADDIU(buf, rs, rt, val) uasm_i_addiu(buf, rs, rt, val)
# define UASM_i_ADDU(buf, rs, rt, rd) uasm_i_addu(buf, rs, rt, rd)
# define UASM_i_SUBU(buf, rs, rt, rd) uasm_i_subu(buf, rs, rt, rd)
# define UASM_i_LL(buf, rs, rt, off) uasm_i_ll(buf, rs, rt, off)
# define UASM_i_SC(buf, rs, rt, off) uasm_i_sc(buf, rs, rt, off)
#endif

#define uasm_i_b(buf, off) uasm_i_beq(buf, 0, 0, off)
#define uasm_i_beqz(buf, rs, off) uasm_i_beq(buf, rs, 0, off)
#define uasm_i_beqzl(buf, rs, off) uasm_i_beql(buf, rs, 0, off)
#define uasm_i_bnez(buf, rs, off) uasm_i_bne(buf, rs, 0, off)
#define uasm_i_bnezl(buf, rs, off) uasm_i_bnel(buf, rs, 0, off)
#define uasm_i_move(buf, a, b) UASM_i_ADDU(buf, a, 0, b)
#define uasm_i_nop(buf) uasm_i_sll(buf, 0, 0, 0)
#define uasm_i_ssnop(buf) uasm_i_sll(buf, 0, 0, 1)
#define uasm_i_ehb(buf) uasm_i_sll(buf, 0, 0, 3)

/* Handle relocations. */
struct uasm_reloc {
	u32 *addr;
	unsigned int type;
	int lab;
};

/* This is zero so we can use zeroed label arrays. */
#define UASM_LABEL_INVALID 0

static inline void __cpuinit
uasm_r_mips_pc16(struct uasm_reloc **rel, u32 *addr, int lid)
{
	(*rel)->addr = addr;
	(*rel)->type = R_MIPS_PC16;
	(*rel)->lab = lid;
	(*rel)++;
}

void uasm_resolve_relocs(struct uasm_reloc *rel, struct uasm_label *lab);
void uasm_move_relocs(struct uasm_reloc *rel, u32 *first, u32 *end, long off);
void uasm_move_labels(struct uasm_label *lab, u32 *first, u32 *end, long off);
void uasm_copy_handler(struct uasm_reloc *rel, struct uasm_label *lab,
	u32 *first, u32 *end, u32 *target);
int uasm_insn_has_bdelay(struct uasm_reloc *rel, u32 *addr);

/* Convenience functions for labeled branches. */
void uasm_il_bltz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_b(u32 **p, struct uasm_reloc **r, int lid);
void uasm_il_beqz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_beqzl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bnez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bgezl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bgez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
