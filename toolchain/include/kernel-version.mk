# Use the default kernel version if the Makefile doesn't override it

ifeq ($(KERNEL),2.4)
  LINUX_VERSION?=2.4.37.11
else
ifeq ($(CONFIG_arm),y)
  LINUX_VERSION?=2.6.36.4
else
  LINUX_VERSION?=2.6.22.19
endif
endif
LINUX_RELEASE?=1

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(LINUX_VERSION))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(LINUX_VERSION))))

