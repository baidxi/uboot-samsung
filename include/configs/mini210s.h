#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC110		1	/* which is in a S5PC110 */

#include <linux/sizes.h>
#include <asm/arch/cpu.h>		/* get chip and board defs */

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x20000000

#define CONFIG_SPL_STACK	(CONFIG_SPL_TEXT_BASE + 0x17fff)
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x3000000)

#ifdef CONFIG_PWM_NX
/* fallback to pwm */
#define BACKLIGHT_CH		0
#define BACKLIGHT_DIV		0
#define BACKLIGHT_INV		0
#define BACKLIGHT_DUTY		50
#define BACKLIGHT_HZ		1000
#endif

#ifndef CONFIG_DRIVER_DM9000
#define CONFIG_DRIVER_DM9000 1
#define CONFIG_DM9000_BASE    0x88000000
#define DM9000_IO             (CONFIG_DM9000_BASE)
#define DM9000_DATA           (CONFIG_DM9000_BASE + 0x4)
#endif

#define CONFIG_SYS_MAX_NAND_DEVICE 1

#define CONFIG_SYS_NAND_BASE    0xB0E00000

#define PHYS_SDRAM_1        CONFIG_SYS_SDRAM_BASE   /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE   (512 << 20) /* 0x25A00000, 512 MB Bank #1 (modied by shl) */


#endif
