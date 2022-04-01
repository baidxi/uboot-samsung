#include <common.h>
#include <init.h>
#include <serial.h>
#include <asm/arch/gpio.h>
#include <asm/arch/cpu.h>
#include <asm/arch/uart.h>
#include <asm/arch/clock.h>

DECLARE_GLOBAL_DATA_PTR;

unsigned int spl_boot_device(void)
{
	return 0;
}

int timer_init(void)
{
	return 0;
}

#if !defined(CONFIG_SPL_FRAMEWORK_BOARD_INIT_F)

// void board_init_f(ulong dummy)
// {
	
// }

#endif

void spl_board_init(void)
{
	while(1) {

	}
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

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
	return 0;
}

struct serial_device *default_serial_console(void)
{
    return NULL;
}

void system_clock_init(void)
{
	s5p_set_cpu_id();
    struct s5pc110_clock *clock = (struct s5pc110_clock *)samsung_get_base_clock();

	writel(0, &clock->src0);
	writel(0x0000ffff, &clock->apll_lock);
	writel(0x0000ffff, &clock->mpll_lock);
	writel(0x14131440, &clock->div0);
	writel(set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV), &clock->apll_con);
	writel(set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV), &clock->mpll_con);
	writel(0x10001111, &clock->src0);
}