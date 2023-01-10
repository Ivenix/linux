/*
 * OF helpers for TI WL12xx radio
 *
 * Copyright (c) 2014 Remi Depommier <rde@ivenix.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/of.h>
//#include <linux/of_irq.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/wl12xx.h>

static struct wl12xx_platform_data wlan_data __initdata = {
	.irq = 0,
//	.use_eeprom = ,
	.board_ref_clock = WL12XX_REFCLOCK_38_XTAL, /* 38.4 MHz */
	.platform_quirks = 0,
//	.pwr_in_suspend = ,
};

static int __init of_wl12xx_init(void)
{
	struct device_node *np;
	int ret = -ENODEV;

	np = of_find_compatible_node(NULL, NULL, "ti,wl12xx");
	if (np) {
		int gpio, wl_en, bt_en;

		gpio = of_get_named_gpio_flags(np, "interrupt", 0, NULL);
		wl_en = of_get_named_gpio_flags(np, "wl_en", 0, NULL);
		bt_en = of_get_named_gpio_flags(np, "bt_en", 0, NULL);

		if (gpio >= 0) {
			gpio_request(gpio, "wlan_irq");
			gpio_direction_input(gpio);
			wlan_data.irq = gpio_to_irq(gpio);
		}

		if (wl_en >= 0) {
			gpio_request(wl_en, "wl_en");
			gpio_direction_output(wl_en, 0);
			wlan_data.wl_en = wl_en;
		}

		if (bt_en >= 0) {
			gpio_request(bt_en, "bt_en");
			gpio_direction_output(bt_en, 0);
			wlan_data.bt_en = bt_en;
		}

		ret = wl12xx_set_platform_data(&wlan_data);
		if (ret)
			printk(KERN_ERR "%s %d\n", __FUNCTION__, ret);
	}
	of_node_put(np);
	return ret;
}

module_init(of_wl12xx_init);

MODULE_DESCRIPTION("TI WL12xx OF driver");
MODULE_AUTHOR("Remi Depommier <rde@ivenix.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:of-wl12xx");
