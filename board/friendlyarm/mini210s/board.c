#include <common.h>
#include <init.h>
#include <asm/io.h>
#include <dm9000.h>
#include <command.h>
#include <dm/uclass.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <asm/arch/sromc.h>
#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

static void dm9000_pre_init(void)
{
	u32 smc_bw_conf, smc_bc_conf;
	/* Ethernet needs bus width of 16 bits */
	smc_bw_conf = SMC_DATA16_WIDTH(CONFIG_ENV_SROM_BANK)
		| SMC_BYTE_ADDR_MODE(CONFIG_ENV_SROM_BANK);

	smc_bc_conf = SMC_BC_TACS(0) | SMC_BC_TCOS(1) | SMC_BC_TACC(2)
	| SMC_BC_TCOH(1) | SMC_BC_TAH(0) | SMC_BC_TACP(0) | SMC_BC_PMC(0);

	s5p_config_sromc(CONFIG_ENV_SROM_BANK, smc_bw_conf, smc_bc_conf);
}

int board_init(void)
{
	dm9000_pre_init();
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

	writeb(1, 0xE02000A0);		// disable beeper
	writeb(0, 0xE02000A4);
	return 0;
}
#endif

void board_mtdparts_default(const char **mtdids, const char **mtdparts)
{
	
}

void coloured_LED_init(void)
{
	gpio_cfg_pin(S5PC110_GPIO_J20, S5P_GPIO_FUNC(1));
	gpio_cfg_pin(S5PC110_GPIO_J21, S5P_GPIO_FUNC(1));
	gpio_cfg_pin(S5PC110_GPIO_J22, S5P_GPIO_FUNC(1));
	gpio_cfg_pin(S5PC110_GPIO_J23, S5P_GPIO_FUNC(1));
}

int board_eth_init(struct bd_info *bis)
{
	return dm9000_initialize(bis);
}