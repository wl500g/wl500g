obj-$(CONFIG_USB_IP_COMMON) += usbip-core.o
usbip-core-objs := usbip_common.o usbip_event.o

obj-$(CONFIG_USB_IP_VHCI_HCD) += vhci-hcd.o
vhci-hcd-objs := vhci_sysfs.o vhci_tx.o vhci_rx.o vhci_hcd.o

obj-$(CONFIG_USB_IP_HOST) += usbip-host.o
usbip-host-objs := stub_dev.o stub_main.o stub_rx.o stub_tx.o

ifeq ($(CONFIG_USB_IP_DEBUG_ENABLE),y)
	EXTRA_CFLAGS += -DDEBUG
endif
