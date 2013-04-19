
/* initialisation functions */

extern int devices_init(void);
extern int buses_init(void);
extern int classes_init(void);
extern int firmware_init(void);
#ifdef CONFIG_SYS_HYPERVISOR
extern int hypervisor_init(void);
#else
static inline int hypervisor_init(void) { return 0; }
#endif
extern int platform_bus_init(void);
extern int system_bus_init(void);
extern int cpu_dev_init(void);
extern int attribute_container_init(void);

extern int bus_add_device(struct device * dev);
extern void bus_attach_device(struct device * dev);
extern void bus_remove_device(struct device * dev);
extern struct bus_type *get_bus(struct bus_type * bus);
extern void put_bus(struct bus_type * bus);

extern int bus_add_driver(struct device_driver *);
extern void bus_remove_driver(struct device_driver *);

extern void driver_detach(struct device_driver * drv);
extern int driver_probe_device(struct device_driver *, struct device *);
static inline int driver_match_device(struct device_driver *drv,
				      struct device *dev)
{
	return drv->bus->match ? drv->bus->match(dev, drv) : 1;
}

extern void sysdev_shutdown(void);
extern int sysdev_suspend(pm_message_t state);
extern int sysdev_resume(void);

extern char *make_class_name(const char *name, struct kobject *kobj);

extern void devres_release_all(struct device *dev);

extern struct kset devices_subsys;
