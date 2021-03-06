/* 
 * lge/lge_board/debug/lge_handle_panic.c
 *
 * Copyright (C) 2010 LGE, Inc
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/setup.h>
#include "../include/board_lge.h"

#define PANIC_HANDLER_NAME "panic-handler"
#define PANIC_DUMP_CONSOLE 0
#define PANIC_MAGIC_KEY	0x12345678
#define CRASH_ARM9		0x87654321
#define CRASH_REBOOT	0x618E1000

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
#define CRASH_HANDLER_ENABLE 	0x63680001
#endif

struct crash_log_dump {
	unsigned int magic_key;
#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
	unsigned int enable;
#endif
	unsigned int size;
	unsigned char buffer[0];
};

static struct crash_log_dump *crash_dump_log;
static unsigned int crash_buf_size = 0;
static int crash_store_flag = 0;

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
static int crash_handler_enable = 0;
#endif

static DEFINE_SPINLOCK(lge_panic_lock);

#if defined(CONFIG_LGE_HIDDEN_RESET_PATCH)
int hidden_reset_enable = 0;
module_param_named(hidden_reset_enable, hidden_reset_enable,
		   int, S_IRUGO | S_IWUSR | S_IWGRP);
int on_hidden_reset = 0;

static int __init check_hidden_reset(char *reset_mode)
{
	if (!strncmp(reset_mode, "on", 2)) {
		on_hidden_reset = 1;
		printk("reboot mode: hidden rest %s\n", "on");
	}
	return 1;
}

__setup("lge.hreset=", check_hidden_reset);

static int copy_frame_buffer(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	return NOTIFY_DONE;
}

static ssize_t is_hidden_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	return sprintf(buf, "%d\n", on_hidden_reset);
}

static DEVICE_ATTR(is_hreset, S_IRUGO | S_IWUGO, is_hidden_show, NULL);

static struct notifier_block panic_handler_block2 = {
	.notifier_call = copy_frame_buffer,
};
#endif

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
static int __init check_crash_handler(char *reset_mode)
{
	if (!strncmp(reset_mode, "0x63680001", 10)) {
		crash_handler_enable = 1;
		printk(KERN_INFO "crash_handler_enable in user mode\n");
	}

	return 1;
}

__setup("lge.crash_enable=", check_crash_handler);
#endif

#if defined(CONFIG_LGE_SILENT_RESET_PATCH)
#define SRESET_MAGIC  0x68720010
struct silent_reset_flag {
	unsigned int magic_key;
};

struct silent_reset_flag *sreset_flag = NULL;
int sreset_enable = 0;

static int sreset_enable_set(const char *val, struct kernel_param *kp)
{
	int ret;
	ret = param_set_int(val, kp);
	if (ret)
		return ret;

	if (sreset_enable) {
		if (sreset_flag)
			sreset_flag->magic_key = SRESET_MAGIC;
		pr_info("silent reset activated\n");
	} else {
		if (sreset_flag)
			sreset_flag->magic_key = 0;
		pr_info("silent reset deactivated\n");
	}

	return 0;
}

module_param_call(sreset_enable, sreset_enable_set, param_get_int,
		  &sreset_enable, S_IRUGO | S_IWUSR | S_IWGRP);

int on_silent_reset = 0;
module_param_named(on_silent_reset, on_silent_reset,
		   int, S_IRUGO | S_IWUSR | S_IWGRP);

static int __init check_silent_reset(char *reset_mode)
{
	if (!strncmp(reset_mode, "on", 2)) {
		on_silent_reset = 1;
		printk(KERN_INFO "reboot mode : silent reset %s\n", "on");
	}
	return 1;
}

__setup("is.sreset=", check_silent_reset);

static ssize_t is_silent_show(struct device *dev, struct device_attribute *addr,
			      char *buf)
{
	return sprintf(buf, "%d\n", on_silent_reset);
}

static DEVICE_ATTR(is_sreset, S_IRUGO | S_IWUSR | S_IWGRP, is_silent_show,
		   NULL);
#endif

static int dummy_arg;
static int gen_bug(const char *val, struct kernel_param *kp)
{
	BUG();

	return 0;
}

module_param_call(gen_bug, gen_bug, param_get_bool, &dummy_arg,
		  S_IWUSR | S_IRUGO);

static int gen_panic(const char *val, struct kernel_param *kp)
{
	panic("generate test-panic");

	return 0;
}

module_param_call(gen_panic, gen_panic, param_get_bool, &dummy_arg,
		  S_IWUSR | S_IRUGO);

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)

#define WDT0_RST 		0x38
#define WDT0_EN 		0x40
#define WDT0_BARK_TIME 	0x4C
#define WDT0_BITE_TIME 	0x5C
extern void __iomem *msm_timer_get_timer0_base(void);

static int gen_wdt_bark(const char *val, struct kernel_param *kp)
{
	static void __iomem *msm_tmr0_base;
	msm_tmr0_base = msm_timer_get_timer0_base();
	__raw_writel(0, msm_tmr0_base + WDT0_EN);
	__raw_writel(1, msm_tmr0_base + WDT0_RST);
	__raw_writel(0x31F3, msm_tmr0_base + WDT0_BARK_TIME);
	__raw_writel(5 * 0x31F3, msm_tmr0_base + WDT0_BITE_TIME);
	__raw_writel(1, msm_tmr0_base + WDT0_EN);
	return 0;
}

module_param_call(gen_wdt_bark, gen_wdt_bark, param_get_bool, &dummy_arg,
		  S_IWUSR | S_IRUGO);

static int gen_hw_reset(const char *val, struct kernel_param *kp)
{
	static void __iomem *msm_tmr0_base;
	msm_tmr0_base = msm_timer_get_timer0_base();
	__raw_writel(0, msm_tmr0_base + WDT0_EN);
	__raw_writel(1, msm_tmr0_base + WDT0_RST);
	__raw_writel(5 * 0x31F3, msm_tmr0_base + WDT0_BARK_TIME);
	__raw_writel(0x31F3, msm_tmr0_base + WDT0_BITE_TIME);
	__raw_writel(1, msm_tmr0_base + WDT0_EN);
	return 0;
}

module_param_call(gen_hw_reset, gen_hw_reset, param_get_bool, &dummy_arg,
		  S_IWUSR | S_IRUGO);

#endif

#if !(defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU))
static int crash_handle_enable = 1;
module_param_named(crash_handle_enable, crash_handle_enable,
		   int, S_IRUGO | S_IWUSR | S_IWGRP);
#endif

void set_crash_store_enable(void)
{
	crash_store_flag = 1;

	return;
}

void set_crash_store_disable(void)
{
	crash_store_flag = 0;

	return;
}

void store_crash_log(char *p)
{
	if (!crash_store_flag)
		return;

	if (crash_dump_log->size == crash_buf_size)
		return;

	for (; *p; p++) {
		if (*p == '[') {
			for (; *p != ']'; p++) ;
			p++;
			if (*p == ' ')
				p++;
		}

		if (*p == '<') {
			for (; *p != '>'; p++) ;
			p++;
		}

		crash_dump_log->buffer[crash_dump_log->size] = *p;
		crash_dump_log->size++;
	}
	crash_dump_log->buffer[crash_dump_log->size] = 0;

	return;
}

static int restore_crash_log(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	unsigned long flags;

	crash_store_flag = 0;

	spin_lock_irqsave(&lge_panic_lock, flags);

	crash_dump_log->magic_key = PANIC_MAGIC_KEY;

	spin_unlock_irqrestore(&lge_panic_lock, flags);

	return NOTIFY_DONE;
}

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
static int crash_handler_enable_set(const char *val, struct kernel_param *kp)
{
	int ret;
	ret = param_set_int(val, kp);

	if (ret)
		return ret;

	if (crash_handler_enable) {
		if (crash_dump_log)
			crash_dump_log->enable = CRASH_HANDLER_ENABLE;
		pr_info("demigod crash handler activated\n");
	} else {
		if (crash_dump_log)
			crash_dump_log->enable = 0;
		pr_info("demigod crash handler deactivated\n");
	}

	return 0;
}

module_param_call(crash_handler_enable, crash_handler_enable_set, param_get_int,
		  &crash_handler_enable, S_IRUGO | S_IWUSR | S_IWGRP);
#endif

static struct notifier_block panic_handler_block = {
	.notifier_call = restore_crash_log,
};

static int __init lge_panic_handler_probe(struct platform_device *pdev)
{
	struct resource *res = pdev->resource;
	size_t start;
	size_t buffer_size;
	void *buffer;
	int ret = 0;
#if defined(CONFIG_LGE_SILENT_RESET_PATCH)
	void *sreset_flag_buf;
	size_t sreset_start;
#endif

	if (res == NULL || pdev->num_resources != 1 ||
	    !(res->flags & IORESOURCE_MEM)) {
		printk(KERN_ERR
		       "lge_panic_handler: invalid resource, %p %d flags "
		       "%lx\n", res, pdev->num_resources, res ? res->flags : 0);
		return -ENXIO;
	}

	buffer_size = res->end - res->start + 1;
	start = res->start;
	printk(KERN_INFO "lge_panic_handler: got buffer at %zx, size %zx\n",
	       start, buffer_size);
	buffer = ioremap(res->start, buffer_size);
	if (buffer == NULL) {
		printk(KERN_ERR "lge_panic_handler: failed to map memory\n");
		return -ENOMEM;
	}

	crash_dump_log = (struct crash_log_dump *)buffer;
	memset(crash_dump_log, 0, buffer_size);
	crash_dump_log->magic_key = 0;
	crash_dump_log->size = 0;
	crash_buf_size = buffer_size - offsetof(struct crash_log_dump, buffer);

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT) || defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
	if (crash_handler_enable) {
		crash_dump_log->enable = CRASH_HANDLER_ENABLE;
	} else {
		crash_dump_log->enable = 0;
	}
#endif
#if defined(CONFIG_LGE_SILENT_RESET_PATCH)
	/* crash buffer + ctx size + hrset */
	sreset_start = res->end + 1 + 1024 + 1024;
	sreset_flag_buf = ioremap(sreset_start, 1024);
	printk(KERN_INFO "lge_silent_reset: got buffer at %zx, size 1024\n",
	       sreset_start);
	if (!sreset_flag_buf) {
		pr_err("sreset flag buffer: failed to map memory\n");
		return -ENOMEM;
	}

	sreset_flag = (struct silent_reset_flag *)sreset_flag_buf;

	ret = device_create_file(&pdev->dev, &dev_attr_is_sreset);
	if (ret < 0) {
		printk(KERN_ERR "device_create_file error!\n");
		return ret;
	}

	if (sreset_enable) {
		sreset_flag->magic_key = SRESET_MAGIC;
	} else {
		sreset_flag->magic_key = 0;
	}
#endif

	/* Setup panic notifier */
	atomic_notifier_chain_register(&panic_notifier_list,
				       &panic_handler_block);

#if defined(CONFIG_LGE_HIDDEN_RESET_PATCH)
	ret = device_create_file(&pdev->dev, &dev_attr_is_hreset);
	if (ret < 0) {
		printk("device_create_file error!\n");
		return ret;
	}
	atomic_notifier_chain_register(&panic_notifier_list,
				       &panic_handler_block2);
#endif

	return ret;
}

static int __devexit lge_panic_handler_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver panic_handler_driver __refdata = {
	.probe = lge_panic_handler_probe,
	.remove = __devexit_p(lge_panic_handler_remove),
	.driver = {
		   .name = PANIC_HANDLER_NAME,
		   .owner = THIS_MODULE,
		   },
};

static int __init lge_panic_handler_init(void)
{
	return platform_driver_register(&panic_handler_driver);
}

static void __exit lge_panic_handler_exit(void)
{
	platform_driver_unregister(&panic_handler_driver);
}

module_init(lge_panic_handler_init);
module_exit(lge_panic_handler_exit);

MODULE_DESCRIPTION("LGE panic handler driver");
MODULE_AUTHOR("SungEun Kim <cleaneye.kim@lge.com>");
MODULE_LICENSE("GPL");
