/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2005, Psyent Corporation <www.psyent.com>
 * Scott McNutt <smcnutt@psyent.com>
 * (C) Copyright 2010, Thomas Chou <thomas@wytron.com.tw>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * BOARD/CPU
 */

/*
 * SERIAL
 */
#define CONFIG_SYS_NS16550_MEM32

/*
 * Flash
 */
#define CONFIG_SYS_MAX_FLASH_SECT	1024

/*
 * NET options
 */
#define CONFIG_SYS_RX_ETH_BUFFER	0
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE

/*
 * MEMORY ORGANIZATION
 * -Monitor at top of sdram.
 * -The heap is placed below the monitor
 * -The stack is placed below the heap (&grows down).
 */
#define CONFIG_SYS_SDRAM_BASE		0xc8000000
#define CONFIG_SYS_SDRAM_SIZE		0x08000000
#define CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_SYS_MONITOR_LEN		0x80000	/* Reserve 512k */
#define CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_SDRAM_BASE + \
					 CONFIG_SYS_SDRAM_SIZE - \
					 CONFIG_SYS_MONITOR_LEN)

#endif /* __CONFIG_H */
