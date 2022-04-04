#include <common.h>
#include <init.h>
#include <serial.h>
#include <asm/arch/gpio.h>
#include <asm/arch/cpu.h>
#include <asm/arch/uart.h>
#include <asm/arch/clock.h>
#include <asm/spl.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;
typedef void __noreturn (*image_entry_noargs_t)(void);
unsigned int spl_boot_device(void)
{
	u32 OM = *(volatile u32 *)(0xe0000004);	// OM Register
	OM &= 0x1f;

	if (OM == 0x2) {
		return BOOT_DEVICE_NAND;
	} else if(OM == 0xc) {
		u32 sdmmc_base = *(volatile u32 *)(0xd0037488);

		if (sdmmc_base == 0xeb000000)
			return BOOT_DEVICE_MMC1;
		else if (sdmmc_base == 0xeb200000)
			return BOOT_DEVICE_MMC2;
		else
			printf("unknow boot device 0x%x\n", sdmmc_base);
	}

	while(1) {
		
	}
}

#ifdef CONFIG_NAND_BOOT

#include "nand.h"


void boot_from_nand(void)
{
	u32 cfg = 0;
	int ret;
	int i = 0;
	int offset = 0x4000 / 2048;
	struct spl_image_info entry;
	u8 *header = (u8 *)0x23000000;
	image_entry_noargs_t image_entry;

	u32 bl2size = 512 * 1024; // 512k
	int pages = 0;

	struct s5pv210_nand *nand_reg = (struct s5pv210_nand *)S5PV210_NAND_BASE;
	/* initialize hardware */
	/* HCLK_PSYS=133MHz(7.5ns) */
	cfg =	(0x1 << 23) |	/* Disable 1-bit and 4-bit ECC */
			/* 下面3个时间参数稍微比计算出的值大些（我这里依次加1），否则读写不稳定 */
			(0x3 << 12) |	/* 7.5ns * 2 > 12ns tALS tCLS */
			(0x2 << 8) | 	/* (1+1) * 7.5ns > 12ns (tWP) */
			(0x1 << 4) | 	/* (0+1) * 7.5 > 5ns (tCLH/tALH) */
			(0x0 << 3) | 	/* SLC NAND Flash */
			(0x0 << 2) |	/* 2KBytes/Page */
			(0x1 << 1);		/* 5 address cycle */

	writel(cfg, &nand_reg->nfconf);
	
	writel((0x1 << 1) | (0x1 << 0), &nand_reg->nfcont);
	/* Disable chip select and Enable NAND Flash Controller */

	/* Config GPIO */
	MP0_1CON &= ~(0xFFFF << 8);
	MP0_1CON |= (0x3333 << 8);
	MP0_3CON = 0x22222222;
	MP0_6CON = 0x22222222;

	memset(&entry, '\0', sizeof(entry));

	pages = bl2size / 2048;

	for (; i < pages; i++, header += 2048, offset += 1)
		NF8_ReadPage_Adv(offset / 64, offset % 64, header);

	entry.flags |= SPL_COPY_PAYLOAD_ONLY;

	ret = spl_parse_image_header(&entry, NULL, (const struct image_header *)header);

	if (ret) {
		printf("parse spl load failed\n");
		while(1) {

		}
	}
	memcpy((void *)entry.entry_point, header + sizeof(entry), entry.size);

	image_entry = (image_entry_noargs_t)entry.entry_point;

	image_entry();
}
#endif

#ifdef CONFIG_SD_BOOT

#define CopySDMMCtoMem(ch, sb, bs, dst, i) \
	(((u8(*)(int, u32, unsigned short, u32*, u8))\
	(*((u32 *)0xD0037F98)))(ch, sb, bs, dst, i))

void boot_from_sd(u8 ch)
{
	int ret;
	struct spl_image_info entry;
	u8 *header = (u8 *)0x23000000;
	image_entry_noargs_t image_entry;
	u32 bl2size = 512 * 1024; // 512k

	printf("boot from sdcard %d ch\n", ch);
 	memset(&entry, '\0', sizeof(entry));

	CopySDMMCtoMem(ch, 33, bl2size / 512, (u32 *)header, 0);

	entry.flags |= SPL_COPY_PAYLOAD_ONLY;

	ret = spl_parse_image_header(&entry, NULL, (const struct image_header *)header);

	if (ret) {
		printf("parse spl load failed\n");
		while(1) {

		}
	}
	memcpy((u8 *)entry.entry_point, header + sizeof(entry) + 0x1c, entry.size); 

	image_entry = (image_entry_noargs_t)entry.entry_point;

	image_entry();
}
#endif

void spl_board_init(void)
{
	unsigned int device = spl_boot_device();
	
	switch(device) {
		#ifdef CONFIG_NAND_BOOT
			case BOOT_DEVICE_NAND:
				return boot_from_nand();
		#endif
		#ifdef CONFIG_SD_BOOT
			case BOOT_DEVICE_MMC1:
				return boot_from_sd(0);
			case BOOT_DEVICE_MMC2:
				return boot_from_sd(2);
		#endif
			default:
				printk("boot device not support %d\n", device);

	}
	while(1) {

	}
}

int timer_init(void)
{
	return 0;
}

#if !defined(CONFIG_SPL_FRAMEWORK_BOARD_INIT_F)

#endif

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
	writel((0<<0)|(4<<4)|(4<<8)|(1<<12)|(3<<16)|(1<<20)|(4<<24)|(1<<28), &clock->div0);
	writel(set_pll(125, 3, 1), &clock->apll_con);
	writel(set_pll(667, 12,1), &clock->mpll_con);
	writel(set_pll(48, 3, 2), &clock->epll_con);
	writel(set_pll(108, 6, 3), &clock->vpll_con);
	writel((1<<0)|(1<<4)|(1<<8)|(1<<12), &clock->src0);
}
