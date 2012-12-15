/*
 * Linux Broadcom BCM47xx GPIO char driver
 *
 * Copyright (C) 2008, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: linux_gpio.c,v 1.10 2008/03/28 19:25:35 Exp $
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#include <typedefs.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmdevs.h>

#include <linux_gpio.h>

/* handle to the sb */
static si_t *gpio_sih;

/* major number assigned to the device and device handles */
static int gpio_major;
static struct class *gpiodev_class = NULL;

static int
gpio_open(struct inode *inode, struct file * file)
{
	MOD_INC_USE_COUNT;
	return 0;
}

static int
gpio_release(struct inode *inode, struct file * file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

static int
gpio_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct gpio_ioctl gpioioc;

	if (copy_from_user(&gpioioc, (struct gpio_ioctl *)arg, sizeof(struct gpio_ioctl)))
		return -EFAULT;

	switch (cmd) {
		case GPIO_IOC_RESERVE:
			gpioioc.val = si_gpioreserve(gpio_sih, gpioioc.mask, GPIO_APP_PRIORITY);
			break;
		case GPIO_IOC_RELEASE:
			/*
			 * releasing the gpio doesn't change the current
			 * value on the GPIO last write value
			 * persists till some one overwrites it
			 */
			gpioioc.val = si_gpiorelease(gpio_sih, gpioioc.mask, GPIO_APP_PRIORITY);
			break;
		case GPIO_IOC_OUT:
			gpioioc.val = si_gpioout(gpio_sih, gpioioc.mask, gpioioc.val,
			                         GPIO_APP_PRIORITY);
			break;
		case GPIO_IOC_OUTEN:
			gpioioc.val = si_gpioouten(gpio_sih, gpioioc.mask, gpioioc.val,
			                           GPIO_APP_PRIORITY);
			break;
		case GPIO_IOC_IN:
			gpioioc.val = si_gpioin(gpio_sih);
			break;
		default:
			break;
	}
	if (copy_to_user((struct gpio_ioctl *)arg, &gpioioc, sizeof(struct gpio_ioctl)))
		return -EFAULT;

	return 0;

}
static const struct file_operations gpio_fops = {
	owner:		THIS_MODULE,
	open:		gpio_open,
	release:	gpio_release,
	ioctl:		gpio_ioctl
};

static int __init
gpio_init(void)
{
	if (!(gpio_sih = si_kattach(SI_OSH)))
		return -ENODEV;

	if ((gpio_major = register_chrdev(0, "gpio", &gpio_fops)) < 0)
		return gpio_major;
	gpiodev_class = class_create(THIS_MODULE, "gpio");
	if (IS_ERR(gpiodev_class)) {
		printk("Error creating gpio class\n");
		return -1;
	}

	/* Add the device gpio0 */
	class_device_create(gpiodev_class, NULL, MKDEV(gpio_major, 0), NULL, "gpio");

	return 0;
}

static void __exit
gpio_exit(void)
{
	if (gpiodev_class != NULL) {
		class_device_destroy(gpiodev_class, MKDEV(gpio_major, 0));
		class_destroy(gpiodev_class);
	}

	gpiodev_class = NULL;
	if (gpio_major >= 0)
		unregister_chrdev(gpio_major, "gpio");
	si_detach(gpio_sih);
}

module_init(gpio_init);
module_exit(gpio_exit);
