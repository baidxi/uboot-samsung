#include <common.h>
#include <init.h>
#include <dm/uclass.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	printf("%s\n", __func__);
    return 0;
}

#ifdef CONFIG_DEBUG_UART_BOARD_INIT

void board_debug_uart_init(void)
{
	gpio_cfg_pin(S5PC110_GPIO_A00, S5P_GPIO_FUNC(2));
	gpio_cfg_pin(S5PC110_GPIO_A01, S5P_GPIO_FUNC(2));
	gpio_cfg_pin(S5PC110_GPIO_A10, S5P_GPIO_FUNC(2));
	gpio_cfg_pin(S5PC110_GPIO_A11, S5P_GPIO_FUNC(2));
}

#endif

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);

    return 0;
}

#ifdef CONFIG_BOARD_EARLY_INIT_R

int board_early_init_r(void)
{
	struct udevice *dev;
	int ret;

	gd->baudrate = CONFIG_BAUDRATE;

	ret = uclass_get_device_by_seq(UCLASS_SERIAL, 0, &dev);

	if (ret) {
		printf("Error getting serial dev for early console! (%d)\n",
		       ret);
		return ret;
	}

	gd->cur_serial_dev = dev;
	gd->flags |= GD_FLG_SERIAL_READY;
	gd->have_console = 1;

	return 0;
}
#endif