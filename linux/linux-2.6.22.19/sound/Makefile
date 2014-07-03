# Makefile for the Linux sound card driver
#

obj-$(CONFIG_SOUND) += soundcore.o
obj-$(CONFIG_SOUND_PRIME) += sound_firmware.o
obj-$(CONFIG_SOUND_PRIME) += oss/
obj-$(CONFIG_DMASOUND) += oss/
obj-$(CONFIG_SND) += core/ i2c/ drivers/ synth/ usb/ soc/
ifneq ($(CONFIG_SND),)
  obj-$(CONFIG_ISA)	 += isa/
  obj-$(CONFIG_PCI)	 += pci/
  obj-$(CONFIG_PCCARD)	 += pcmcia/
  obj-$(CONFIG_ARM)	 += arm/
  obj-$(CONFIG_MIPS)	 += mips/
  obj-$(CONFIG_PARISC)	 += parisc/
  obj-$(CONFIG_PPC)	 += ppc/
  obj-$(CONFIG_SPARC)	 += sparc/
endif
obj-$(CONFIG_SND_AOA) += aoa/

# This one must be compilable even if sound is configured out
obj-$(CONFIG_AC97_BUS) += ac97_bus.o

ifeq ($(CONFIG_SND),y)
  obj-y += last.o
endif

soundcore-objs  := sound_core.o
