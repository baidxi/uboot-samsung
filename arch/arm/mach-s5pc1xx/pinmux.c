// SPDX-License-Identifier: GPL-2.0+
/*
 * Dummy functions to keep s5p_goni building (although it won't work)
 *
 * Copyright 2018 Google LLC
 * Written by Simon Glass <sjg@chromium.org>
 */

#include <common.h>
#include <fdtdec.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/sromc.h>
#include <asm/gpio.h>

static void s5pc1xx_uart_pinconfig(int id)
{

}
static void sp5c1xx_mmc_config(int peripheral, int flags)
{
	int start = 0, end = 0;
	int start_ext = 0, end_ext = 0;
	unsigned int func = 0, func_ext = 0;
	switch(peripheral) {
		case PERIPH_ID_SDMMC0:
			start = S5PC110_GPIO_G00;
			end = S5PC110_GPIO_G06;
			func = S5P_GPIO_FUNC(2);
			if (flags & PINMUX_FLAG_8BIT_MODE) {
				start_ext = S5PC110_GPIO_G13;
				end_ext = S5PC110_GPIO_G16;
				func_ext = S5P_GPIO_FUNC(3);
			}
		break;
	}
	for (int i = start; i < end + 1; i++) {
		gpio_cfg_pin(i, func);
		gpio_set_pull(i, S5P_GPIO_PULL_NONE);
		gpio_set_drv(i, S5P_GPIO_DRV_4X);
	}

	if (flags & PINMUX_FLAG_8BIT_MODE) {
		for (int i = start_ext; i < end_ext+1; i++) {
			gpio_set_pull(i, S5P_GPIO_PULL_NONE);
			gpio_cfg_pin(i, func_ext);
			gpio_set_drv(i, S5P_GPIO_DRV_4X);
		}
	}
}

int exynos_pinmux_config(int peripheral, int flags)
{
	switch(peripheral) {
		case PERIPH_ID_UART0:
		case PERIPH_ID_UART1:
		case PERIPH_ID_UART2:
		case PERIPH_ID_UART3:
			s5pc1xx_uart_pinconfig(peripheral);
		break;
		case PERIPH_ID_SDMMC0:
		case PERIPH_ID_SDMMC1:
		case PERIPH_ID_SDMMC2:
		case PERIPH_ID_SDMMC3:
			sp5c1xx_mmc_config(peripheral, flags);
		break;
	}
	return 0;
}

int pinmux_decode_periph_id(const void *blob, int node)
{
	int err;
	u32 cell[3];

	err = fdtdec_get_int_array(blob, node, "interrupts", cell, ARRAY_SIZE(cell));

	if (err) {
		return PERIPH_ID_NONE;
	}

	return cell[1];
}
