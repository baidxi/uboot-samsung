#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>
#include <linux/stringify.h>

/* High Level Configuration Options */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC110		1	/* which is in a S5PC110 */

#include <linux/sizes.h>
#include <asm/arch/cpu.h>		/* get chip and board defs */

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x20000000

#define CONFIG_SPL_STACK	(CONFIG_SPL_TEXT_BASE + 0x16fff)
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x3000000)

#ifdef CONFIG_PWM_NX
/* fallback to pwm */
#define BACKLIGHT_CH		0
#define BACKLIGHT_DIV		0
#define BACKLIGHT_INV		0
#define BACKLIGHT_DUTY		50
#define BACKLIGHT_HZ		1000
#endif

#define CONFIG_ENV_SROM_BANK   1       /* Select SROM Bank-1 for Ethernet(modied by shl) */
#define CONFIG_DM9000_NO_SROM
#ifndef CONFIG_DRIVER_DM9000
#define CONFIG_DRIVER_DM9000 1
#define CONFIG_DM9000_BASE    0x88000000
#define DM9000_IO             (CONFIG_DM9000_BASE)
#define DM9000_DATA           (CONFIG_DM9000_BASE + 0x4)

#endif

#define CONFIG_SYS_MAX_NAND_DEVICE 1

#define PHYS_SDRAM_1        CONFIG_SYS_SDRAM_BASE   /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE   (512 << 20) /* 0x25A00000, 512 MB Bank #1 (modied by shl) */

#define CONFIG_IPADDR			192.168.10.10
#define CONFIG_SERVERIP			192.168.10.1

#define CONFIG_EXTRA_ENV_SETTINGS \
    "menucm=bootmenu\0" \
    "bootmenu_0=Boot From tftp=samsung_boot tftp\0" \
    "bootmenu_1=Boot From SD card=samsung_boot sdcard\0" \
    "bootmenu_2=Boot From Nand=samsung_boot nand\0" \
    "bootmenu_3=Configure menu=edit_env\0" \
    "bootmenu_delay=30\0" \
    "kernel_loadaddr=0x23000000\0" \
    "bootfile=kernel.itb\0" \
    ""

#endif
