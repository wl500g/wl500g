/* Simple code to turn various tables in an ELF file into alias definitions.
 * This deals with kernel datastructures where they should be
 * dealt with: in the kernel source.
 *
 * Copyright 2002-2003  Rusty Russell, IBM Corporation
 *           2003       Kai Germaschewski
 *
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#include "modpost.h"

/* We use the ELF typedefs for kernel_ulong_t but bite the bullet and
 * use either stdint.h or inttypes.h for the rest. */
#if KERNEL_ELFCLASS == ELFCLASS32
typedef Elf32_Addr	kernel_ulong_t;
#define BITS_PER_LONG 32
#else
typedef Elf64_Addr	kernel_ulong_t;
#define BITS_PER_LONG 64
#endif
#ifdef __sun__
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#include <ctype.h>
#include <stdbool.h>

typedef uint32_t	__u32;
typedef uint16_t	__u16;
typedef unsigned char	__u8;

/* Big exception to the "don't include kernel headers into userspace, which
 * even potentially has different endianness and word sizes, since
 * we handle those differences explicitly below */
#include "../../include/linux/mod_devicetable.h"

/* This array collects all instances that use the generic do_table */
struct devtable {
	const char *device_id; /* name of table, __mod_<name>_device_table. */
	unsigned long id_size;
	void *function;
};

/* We construct a table of pointers in an ELF section (pointers generally
 * go unpadded by gcc).  ld creates boundary syms for us. */
extern struct devtable *__start___devtable[], *__stop___devtable[];
#define ___cat(a,b) a ## b
#define __cat(a,b) ___cat(a,b)

#if __GNUC__ == 3 && __GNUC_MINOR__ < 3
# define __used			__attribute__((__unused__))
#else
# define __used			__attribute__((__used__))
#endif

/* Add a table entry.  We test function type matches while we're here. */
#define ADD_TO_DEVTABLE(device_id, type, function) \
	static struct devtable __cat(devtable,__LINE__) = {	\
		device_id + 0*sizeof((function)((const char *)NULL,	\
						(type *)NULL,		\
						(char *)NULL)),		\
		sizeof(type), (function) };				\
	static struct devtable *__attribute__((section("__devtable"))) \
		__used __cat(devtable_ptr,__LINE__) = &__cat(devtable,__LINE__)

#define ADD(str, sep, cond, field)                              \
do {                                                            \
        strcat(str, sep);                                       \
        if (cond)                                               \
                sprintf(str + strlen(str),                      \
                        sizeof(field) == 1 ? "%02X" :           \
                        sizeof(field) == 2 ? "%04X" :           \
                        sizeof(field) == 4 ? "%08X" : "",       \
                        field);                                 \
        else                                                    \
                sprintf(str + strlen(str), "*");                \
} while(0)

unsigned int cross_build = 0;
/**
 * Check that sizeof(device_id type) are consistent with size of section
 * in .o file. If in-consistent then userspace and kernel does not agree
 * on actual size which is a bug.
 * Also verify that the final entry in the table is all zeros.
 * Ignore both checks if build host differ from target host and size differs.
 **/
static void device_id_check(const char *modname, const char *device_id,
			    unsigned long size, unsigned long id_size,
			    void *symval)
{
	int i;

	if (size % id_size || size < id_size) {
		if (cross_build != 0)
			return;
		fatal("%s: sizeof(struct %s_device_id)=%lu is not a modulo "
		      "of the size of section __mod_%s_device_table=%lu.\n"
		      "Fix definition of struct %s_device_id "
		      "in mod_devicetable.h\n",
		      modname, device_id, id_size, device_id, size, device_id);
	}
	/* Verify last one is a terminator */
	for (i = 0; i < id_size; i++ ) {
		if (*(uint8_t*)(symval+size-id_size+i)) {
			fprintf(stderr,"%s: struct %s_device_id is %lu bytes.  "
				"The last of %lu is:\n",
				modname, device_id, id_size, size / id_size);
			for (i = 0; i < id_size; i++ )
				fprintf(stderr,"0x%02x ",
					*(uint8_t*)(symval+size-id_size+i) );
			fprintf(stderr,"\n");
			fatal("%s: struct %s_device_id is not terminated "
				"with a NULL entry!\n", modname, device_id);
		}
	}
}

/* USB is special because the bcdDevice can be matched against a numeric range */
/* Looks like "usb:vNpNdNdcNdscNdpNicNiscNipNinN" */
static void do_usb_entry(struct usb_device_id *id,
			 unsigned int bcdDevice_initial, int bcdDevice_initial_digits,
			 unsigned char range_lo, unsigned char range_hi,
			 unsigned char max, struct module *mod)
{
	char alias[500];
	strcpy(alias, "usb:");
	ADD(alias, "v", id->match_flags&USB_DEVICE_ID_MATCH_VENDOR,
	    id->idVendor);
	ADD(alias, "p", id->match_flags&USB_DEVICE_ID_MATCH_PRODUCT,
	    id->idProduct);

	strcat(alias, "d");
	if (bcdDevice_initial_digits)
		sprintf(alias + strlen(alias), "%0*X",
			bcdDevice_initial_digits, bcdDevice_initial);
	if (range_lo == range_hi)
		sprintf(alias + strlen(alias), "%X", range_lo);
	else if (range_lo > 0 || range_hi < max) {
		if (range_lo > 0x9 || range_hi < 0xA)
			sprintf(alias + strlen(alias),
				"[%X-%X]",
				range_lo,
				range_hi);
		else {
			sprintf(alias + strlen(alias),
				range_lo < 0x9 ? "[%X-9" : "[%X",
				range_lo);
			sprintf(alias + strlen(alias),
				range_hi > 0xA ? "A-%X]" : "%X]",
				range_hi);
		}
	}
	if (bcdDevice_initial_digits < (sizeof(id->bcdDevice_lo) * 2 - 1))
		strcat(alias, "*");

	ADD(alias, "dc", id->match_flags&USB_DEVICE_ID_MATCH_DEV_CLASS,
	    id->bDeviceClass);
	ADD(alias, "dsc",
	    id->match_flags&USB_DEVICE_ID_MATCH_DEV_SUBCLASS,
	    id->bDeviceSubClass);
	ADD(alias, "dp",
	    id->match_flags&USB_DEVICE_ID_MATCH_DEV_PROTOCOL,
	    id->bDeviceProtocol);
	ADD(alias, "ic",
	    id->match_flags&USB_DEVICE_ID_MATCH_INT_CLASS,
	    id->bInterfaceClass);
	ADD(alias, "isc",
	    id->match_flags&USB_DEVICE_ID_MATCH_INT_SUBCLASS,
	    id->bInterfaceSubClass);
	ADD(alias, "ip",
	    id->match_flags&USB_DEVICE_ID_MATCH_INT_PROTOCOL,
	    id->bInterfaceProtocol);
	ADD(alias, "in",
	    id->match_flags&USB_DEVICE_ID_MATCH_INT_NUMBER,
	    id->bInterfaceNumber);

	/* Always end in a wildcard, for future extension */
	if (alias[strlen(alias)-1] != '*')
		strcat(alias, "*");
	buf_printf(&mod->dev_table_buf,
		   "MODULE_ALIAS(\"%s\");\n", alias);
}

/* Handles increment/decrement of BCD formatted integers */
/* Returns the previous value, so it works like i++ or i-- */
static unsigned int incbcd(unsigned int *bcd,
			   int inc,
			   unsigned char max,
			   size_t chars)
{
	unsigned int init = *bcd, i, j;
	unsigned long long c, dec = 0;

	/* If bcd is not in BCD format, just increment */
	if (max > 0x9) {
		*bcd += inc;
		return init;
	}

	/* Convert BCD to Decimal */
	for (i=0 ; i < chars ; i++) {
		c = (*bcd >> (i << 2)) & 0xf;
		c = c > 9 ? 9 : c; /* force to bcd just in case */
		for (j=0 ; j < i ; j++)
			c = c * 10;
		dec += c;
	}

	/* Do our increment/decrement */
	dec += inc;
	*bcd  = 0;

	/* Convert back to BCD */
	for (i=0 ; i < chars ; i++) {
		for (c=1,j=0 ; j < i ; j++)
			c = c * 10;
		c = (dec / c) % 10;
		*bcd += c << (i << 2);
	}
	return init;
}

static void do_usb_entry_multi(struct usb_device_id *id, struct module *mod)
{
	unsigned int devlo, devhi;
	unsigned char chi, clo, max;
	int ndigits;

	id->match_flags = TO_NATIVE(id->match_flags);
	id->idVendor = TO_NATIVE(id->idVendor);
	id->idProduct = TO_NATIVE(id->idProduct);

	devlo = id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO ?
		TO_NATIVE(id->bcdDevice_lo) : 0x0U;
	devhi = id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI ?
		TO_NATIVE(id->bcdDevice_hi) : ~0x0U;

	/* Figure out if this entry is in bcd or hex format */
	max = 0x9; /* Default to decimal format */
	for (ndigits = 0 ; ndigits < sizeof(id->bcdDevice_lo) * 2 ; ndigits++) {
		clo = (devlo >> (ndigits << 2)) & 0xf;
		chi = ((devhi > 0x9999 ? 0x9999 : devhi) >> (ndigits << 2)) & 0xf;
		if (clo > max || chi > max) {
			max = 0xf;
			break;
		}
	}

	/*
	 * Some modules (visor) have empty slots as placeholder for
	 * run-time specification that results in catch-all alias
	 */
	if (!(id->idVendor | id->idProduct | id->bDeviceClass | id->bInterfaceClass))
		return;

	/* Convert numeric bcdDevice range into fnmatch-able pattern(s) */
	for (ndigits = sizeof(id->bcdDevice_lo) * 2 - 1; devlo <= devhi; ndigits--) {
		clo = devlo & 0xf;
		chi = devhi & 0xf;
		if (chi > max)	/* If we are in bcd mode, truncate if necessary */
			chi = max;
		devlo >>= 4;
		devhi >>= 4;

		if (devlo == devhi || !ndigits) {
			do_usb_entry(id, devlo, ndigits, clo, chi, max, mod);
			break;
		}

		if (clo > 0x0)
			do_usb_entry(id,
				     incbcd(&devlo, 1, max,
					    sizeof(id->bcdDevice_lo) * 2),
				     ndigits, clo, max, max, mod);

		if (chi < max)
			do_usb_entry(id,
				     incbcd(&devhi, -1, max,
					    sizeof(id->bcdDevice_lo) * 2),
				     ndigits, 0x0, chi, max, mod);
	}
}

static void do_usb_table(void *symval, unsigned long size,
			 struct module *mod)
{
	unsigned int i;
	const unsigned long id_size = sizeof(struct usb_device_id);

	device_id_check(mod->name, "usb", size, id_size, symval);

	/* Leave last one: it's the terminator. */
	size -= id_size;

	for (i = 0; i < size; i += id_size)
		do_usb_entry_multi(symval + i, mod);
}

/* Looks like: hid:bNvNpN */
static int do_hid_entry(const char *filename,
			     struct hid_device_id *id, char *alias)
{
	id->bus = TO_NATIVE(id->bus);
	id->vendor = TO_NATIVE(id->vendor);
	id->product = TO_NATIVE(id->product);

	sprintf(alias, "hid:b%04X", id->bus);
	ADD(alias, "v", id->vendor != HID_ANY_ID, id->vendor);
	ADD(alias, "p", id->product != HID_ANY_ID, id->product);

	return 1;
}
ADD_TO_DEVTABLE("hid", struct hid_device_id, do_hid_entry);

/* Looks like: ieee1394:venNmoNspNverN */
static int do_ieee1394_entry(const char *filename,
			     struct ieee1394_device_id *id, char *alias)
{
	id->match_flags = TO_NATIVE(id->match_flags);
	id->vendor_id = TO_NATIVE(id->vendor_id);
	id->model_id = TO_NATIVE(id->model_id);
	id->specifier_id = TO_NATIVE(id->specifier_id);
	id->version = TO_NATIVE(id->version);

	strcpy(alias, "ieee1394:");
	ADD(alias, "ven", id->match_flags & IEEE1394_MATCH_VENDOR_ID,
	    id->vendor_id);
	ADD(alias, "mo", id->match_flags & IEEE1394_MATCH_MODEL_ID,
	    id->model_id);
	ADD(alias, "sp", id->match_flags & IEEE1394_MATCH_SPECIFIER_ID,
	    id->specifier_id);
	ADD(alias, "ver", id->match_flags & IEEE1394_MATCH_VERSION,
	    id->version);

	return 1;
}
ADD_TO_DEVTABLE("ieee1394", struct ieee1394_device_id, do_ieee1394_entry);

/* Looks like: pci:vNdNsvNsdNbcNscNiN. */
static int do_pci_entry(const char *filename,
			struct pci_device_id *id, char *alias)
{
	/* Class field can be divided into these three. */
	unsigned char baseclass, subclass, interface,
		baseclass_mask, subclass_mask, interface_mask;

	id->vendor = TO_NATIVE(id->vendor);
	id->device = TO_NATIVE(id->device);
	id->subvendor = TO_NATIVE(id->subvendor);
	id->subdevice = TO_NATIVE(id->subdevice);
	id->class = TO_NATIVE(id->class);
	id->class_mask = TO_NATIVE(id->class_mask);

	strcpy(alias, "pci:");
	ADD(alias, "v", id->vendor != PCI_ANY_ID, id->vendor);
	ADD(alias, "d", id->device != PCI_ANY_ID, id->device);
	ADD(alias, "sv", id->subvendor != PCI_ANY_ID, id->subvendor);
	ADD(alias, "sd", id->subdevice != PCI_ANY_ID, id->subdevice);

	baseclass = (id->class) >> 16;
	baseclass_mask = (id->class_mask) >> 16;
	subclass = (id->class) >> 8;
	subclass_mask = (id->class_mask) >> 8;
	interface = id->class;
	interface_mask = id->class_mask;

	if ((baseclass_mask != 0 && baseclass_mask != 0xFF)
	    || (subclass_mask != 0 && subclass_mask != 0xFF)
	    || (interface_mask != 0 && interface_mask != 0xFF)) {
		warn("Can't handle masks in %s:%04X\n",
		     filename, id->class_mask);
		return 0;
	}

	ADD(alias, "bc", baseclass_mask == 0xFF, baseclass);
	ADD(alias, "sc", subclass_mask == 0xFF, subclass);
	ADD(alias, "i", interface_mask == 0xFF, interface);
	return 1;
}
ADD_TO_DEVTABLE("pci", struct pci_device_id, do_pci_entry);

/* looks like: "ccw:tNmNdtNdmN" */
static int do_ccw_entry(const char *filename,
			struct ccw_device_id *id, char *alias)
{
	id->match_flags = TO_NATIVE(id->match_flags);
	id->cu_type = TO_NATIVE(id->cu_type);
	id->cu_model = TO_NATIVE(id->cu_model);
	id->dev_type = TO_NATIVE(id->dev_type);
	id->dev_model = TO_NATIVE(id->dev_model);

	strcpy(alias, "ccw:");
	ADD(alias, "t", id->match_flags&CCW_DEVICE_ID_MATCH_CU_TYPE,
	    id->cu_type);
	ADD(alias, "m", id->match_flags&CCW_DEVICE_ID_MATCH_CU_MODEL,
	    id->cu_model);
	ADD(alias, "dt", id->match_flags&CCW_DEVICE_ID_MATCH_DEVICE_TYPE,
	    id->dev_type);
	ADD(alias, "dm", id->match_flags&CCW_DEVICE_ID_MATCH_DEVICE_MODEL,
	    id->dev_model);
	return 1;
}
ADD_TO_DEVTABLE("ccw", struct ccw_device_id, do_ccw_entry);

/* looks like: "ap:tN" */
static int do_ap_entry(const char *filename,
		       struct ap_device_id *id, char *alias)
{
	sprintf(alias, "ap:t%02X", id->dev_type);
	return 1;
}
ADD_TO_DEVTABLE("ap", struct ap_device_id, do_ap_entry);

/* Looks like: "serio:tyNprNidNexN" */
static int do_serio_entry(const char *filename,
			  struct serio_device_id *id, char *alias)
{
	id->type = TO_NATIVE(id->type);
	id->proto = TO_NATIVE(id->proto);
	id->id = TO_NATIVE(id->id);
	id->extra = TO_NATIVE(id->extra);

	strcpy(alias, "serio:");
	ADD(alias, "ty", id->type != SERIO_ANY, id->type);
	ADD(alias, "pr", id->proto != SERIO_ANY, id->proto);
	ADD(alias, "id", id->id != SERIO_ANY, id->id);
	ADD(alias, "ex", id->extra != SERIO_ANY, id->extra);

	return 1;
}

/* looks like: "pnp:dD" */
static int do_pnp_entry(const char *filename,
			struct pnp_device_id *id, char *alias)
{
	sprintf(alias, "pnp:d%s", id->id);
	return 1;
}
ADD_TO_DEVTABLE("serio", struct serio_device_id, do_serio_entry);

/* looks like: "pnp:cCdD..." */
static int do_pnp_card_entry(const char *filename,
			struct pnp_card_device_id *id, char *alias)
{
	int i;

	sprintf(alias, "pnp:c%s", id->id);
	for (i = 0; i < PNP_MAX_DEVICES; i++) {
		if (! *id->devs[i].id)
			break;
		sprintf(alias + strlen(alias), "d%s", id->devs[i].id);
	}
	return 1;
}

/* Looks like: pcmcia:mNcNfNfnNpfnNvaNvbNvcNvdN. */
static int do_pcmcia_entry(const char *filename,
			   struct pcmcia_device_id *id, char *alias)
{
	unsigned int i;

	id->match_flags = TO_NATIVE(id->match_flags);
	id->manf_id = TO_NATIVE(id->manf_id);
	id->card_id = TO_NATIVE(id->card_id);
	id->func_id = TO_NATIVE(id->func_id);
	id->function = TO_NATIVE(id->function);
	id->device_no = TO_NATIVE(id->device_no);

	for (i=0; i<4; i++) {
		id->prod_id_hash[i] = TO_NATIVE(id->prod_id_hash[i]);
       }

       strcpy(alias, "pcmcia:");
       ADD(alias, "m", id->match_flags & PCMCIA_DEV_ID_MATCH_MANF_ID,
	   id->manf_id);
       ADD(alias, "c", id->match_flags & PCMCIA_DEV_ID_MATCH_CARD_ID,
	   id->card_id);
       ADD(alias, "f", id->match_flags & PCMCIA_DEV_ID_MATCH_FUNC_ID,
	   id->func_id);
       ADD(alias, "fn", id->match_flags & PCMCIA_DEV_ID_MATCH_FUNCTION,
	   id->function);
       ADD(alias, "pfn", id->match_flags & PCMCIA_DEV_ID_MATCH_DEVICE_NO,
	   id->device_no);
       ADD(alias, "pa", id->match_flags & PCMCIA_DEV_ID_MATCH_PROD_ID1, id->prod_id_hash[0]);
       ADD(alias, "pb", id->match_flags & PCMCIA_DEV_ID_MATCH_PROD_ID2, id->prod_id_hash[1]);
       ADD(alias, "pc", id->match_flags & PCMCIA_DEV_ID_MATCH_PROD_ID3, id->prod_id_hash[2]);
       ADD(alias, "pd", id->match_flags & PCMCIA_DEV_ID_MATCH_PROD_ID4, id->prod_id_hash[3]);

       return 1;
}
ADD_TO_DEVTABLE("pcmcia", struct pcmcia_device_id, do_pcmcia_entry);

static int do_of_entry (const char *filename, struct of_device_id *of, char *alias)
{
    int len;
    char *tmp;
    len = sprintf (alias, "of:N%sT%s",
                    of->name[0] ? of->name : "*",
                    of->type[0] ? of->type : "*");

    if (of->compatible[0])
        sprintf (&alias[len], "%sC%s",
                     of->type[0] ? "*" : "",
                     of->compatible);

    /* Replace all whitespace with underscores */
    for (tmp = alias; tmp && *tmp; tmp++)
        if (isspace (*tmp))
            *tmp = '_';

    return 1;
}
ADD_TO_DEVTABLE("of", struct of_device_id, do_of_entry);

static int do_vio_entry(const char *filename, struct vio_device_id *vio,
		char *alias)
{
	char *tmp;

	sprintf(alias, "vio:T%sS%s", vio->type[0] ? vio->type : "*",
			vio->compat[0] ? vio->compat : "*");

	/* Replace all whitespace with underscores */
	for (tmp = alias; tmp && *tmp; tmp++)
		if (isspace (*tmp))
			*tmp = '_';

	return 1;
}
ADD_TO_DEVTABLE("vio", struct vio_device_id, do_vio_entry);

static int do_i2c_entry(const char *filename, struct i2c_device_id *i2c, char *alias)
{
	strcpy(alias, "i2c:");
	ADD(alias, "id", 1, i2c->id);
	return 1;
}
ADD_TO_DEVTABLE("i2c", struct i2c_device_id, do_i2c_entry);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static void do_input(char *alias,
		     kernel_ulong_t *arr, unsigned int min, unsigned int max)
{
	unsigned int i;

	for (i = min; i < max; i++)
		if (arr[i / BITS_PER_LONG] & (1L << (i%BITS_PER_LONG)))
			sprintf(alias + strlen(alias), "%X,*", i);
}

/* input:b0v0p0e0-eXkXrXaXmXlXsXfXwX where X is comma-separated %02X. */
static int do_input_entry(const char *filename, struct input_device_id *id,
			  char *alias)
{
	sprintf(alias, "input:");

	ADD(alias, "b", id->flags & INPUT_DEVICE_ID_MATCH_BUS, id->bustype);
	ADD(alias, "v", id->flags & INPUT_DEVICE_ID_MATCH_VENDOR, id->vendor);
	ADD(alias, "p", id->flags & INPUT_DEVICE_ID_MATCH_PRODUCT, id->product);
	ADD(alias, "e", id->flags & INPUT_DEVICE_ID_MATCH_VERSION, id->version);

	sprintf(alias + strlen(alias), "-e*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_EVBIT)
		do_input(alias, id->evbit, 0, INPUT_DEVICE_ID_EV_MAX);
	sprintf(alias + strlen(alias), "k*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_KEYBIT)
		do_input(alias, id->keybit,
			 INPUT_DEVICE_ID_KEY_MIN_INTERESTING,
			 INPUT_DEVICE_ID_KEY_MAX);
	sprintf(alias + strlen(alias), "r*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_RELBIT)
		do_input(alias, id->relbit, 0, INPUT_DEVICE_ID_REL_MAX);
	sprintf(alias + strlen(alias), "a*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_ABSBIT)
		do_input(alias, id->absbit, 0, INPUT_DEVICE_ID_ABS_MAX);
	sprintf(alias + strlen(alias), "m*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_MSCIT)
		do_input(alias, id->mscbit, 0, INPUT_DEVICE_ID_MSC_MAX);
	sprintf(alias + strlen(alias), "l*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_LEDBIT)
		do_input(alias, id->ledbit, 0, INPUT_DEVICE_ID_LED_MAX);
	sprintf(alias + strlen(alias), "s*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_SNDBIT)
		do_input(alias, id->sndbit, 0, INPUT_DEVICE_ID_SND_MAX);
	sprintf(alias + strlen(alias), "f*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_FFBIT)
		do_input(alias, id->ffbit, 0, INPUT_DEVICE_ID_FF_MAX);
	sprintf(alias + strlen(alias), "w*");
	if (id->flags & INPUT_DEVICE_ID_MATCH_SWBIT)
		do_input(alias, id->swbit, 0, INPUT_DEVICE_ID_SW_MAX);
	return 1;
}
ADD_TO_DEVTABLE("input", struct input_device_id, do_input_entry);

static int do_eisa_entry(const char *filename, struct eisa_device_id *eisa,
		char *alias)
{
	if (eisa->sig[0])
		sprintf(alias, EISA_DEVICE_MODALIAS_FMT "*", eisa->sig);
	return 1;
}
ADD_TO_DEVTABLE("eisa", struct eisa_device_id, do_eisa_entry);

/* Looks like: parisc:tNhvNrevNsvN */
static int do_parisc_entry(const char *filename, struct parisc_device_id *id,
		char *alias)
{
	id->hw_type = TO_NATIVE(id->hw_type);
	id->hversion = TO_NATIVE(id->hversion);
	id->hversion_rev = TO_NATIVE(id->hversion_rev);
	id->sversion = TO_NATIVE(id->sversion);

	strcpy(alias, "parisc:");
	ADD(alias, "t", id->hw_type != PA_HWTYPE_ANY_ID, id->hw_type);
	ADD(alias, "hv", id->hversion != PA_HVERSION_ANY_ID, id->hversion);
	ADD(alias, "rev", id->hversion_rev != PA_HVERSION_REV_ANY_ID, id->hversion_rev);
	ADD(alias, "sv", id->sversion != PA_SVERSION_ANY_ID, id->sversion);

	return 1;
}
ADD_TO_DEVTABLE("parisc", struct parisc_device_id, do_parisc_entry);

/* Does namelen bytes of name exactly match the symbol? */
static bool sym_is(const char *name, unsigned namelen, const char *symbol)
{
	if (namelen != strlen(symbol))
		return false;

	return memcmp(name, symbol, namelen) == 0;
}

static void do_table(void *symval, unsigned long size,
		     unsigned long id_size,
		     const char *device_id,
		     void *function,
		     struct module *mod)
{
	unsigned int i;
	char alias[500];
	int (*do_entry)(const char *, void *entry, char *alias) = function;

	device_id_check(mod->name, device_id, size, id_size, symval);
	/* Leave last one: it's the terminator. */
	size -= id_size;

	for (i = 0; i < size; i += id_size) {
		if (do_entry(mod->name, symval+i, alias)) {
			/* Always end in a wildcard, for future extension */
			if (alias[strlen(alias)-1] != '*')
				strcat(alias, "*");
			buf_printf(&mod->dev_table_buf,
				   "MODULE_ALIAS(\"%s\");\n", alias);
		}
	}
}

/* Create MODULE_ALIAS() statements.
 * At this time, we cannot write the actual output C source yet,
 * so we write into the mod->dev_table_buf buffer. */
void handle_moddevtable(struct module *mod, struct elf_info *info,
			Elf_Sym *sym, const char *symname)
{
	void *symval;
	char *zeros = NULL;
	const char *name;
	unsigned int namelen;

	/* We're looking for a section relative symbol */
	if (!sym->st_shndx || sym->st_shndx >= info->hdr->e_shnum)
		return;

	/* We're looking for an object */
	if (ELF_ST_TYPE(sym->st_info) != STT_OBJECT)
		return;

	/* All our symbols are of form <prefix>__mod_XXX_device_table. */
	name = strstr(symname, "__mod_");
	if (!name)
		return;
	name += strlen("__mod_");
	namelen = strlen(name);
	if (namelen < strlen("_device_table"))
		return;
	if (strcmp(name + namelen - strlen("_device_table"), "_device_table"))
		return;
	namelen -= strlen("_device_table");

	/* Handle all-NULL symbols allocated into .bss */
	if (info->sechdrs[sym->st_shndx].sh_type & SHT_NOBITS) {
		zeros = calloc(1, sym->st_size);
		symval = zeros;
	} else {
		symval = (void *)info->hdr
			+ info->sechdrs[sym->st_shndx].sh_offset
			+ sym->st_value;
	}

	/* First handle the "special" cases */
	if (sym_is(name, namelen, "usb"))
		do_usb_table(symval, sym->st_size, mod);
	else if (sym_is(name, namelen, "pnp"))
		do_table(symval, sym->st_size,
			 sizeof(struct pnp_device_id), "pnp",
			 do_pnp_entry, mod);
	else if (sym_is(name, namelen, "pnp_card"))
		do_table(symval, sym->st_size,
			 sizeof(struct pnp_card_device_id), "pnp_card",
			 do_pnp_card_entry, mod);
	else {
		struct devtable **p;

		for (p = __start___devtable; p < __stop___devtable; p++) {
			if (sym_is(name, namelen, (*p)->device_id)) {
				do_table(symval, sym->st_size, (*p)->id_size,
					 (*p)->device_id, (*p)->function, mod);
				break;
			}
		}
	}
	free(zeros);
}

/* Now add out buffered information to the generated C source */
void add_moddevtable(struct buffer *buf, struct module *mod)
{
	buf_printf(buf, "\n");
	buf_write(buf, mod->dev_table_buf.p, mod->dev_table_buf.pos);
	free(mod->dev_table_buf.p);
}
