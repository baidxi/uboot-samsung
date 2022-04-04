#include <common.h>
#include <config.h>
#include <init.h>
#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>

#include <asm/arch/cpu.h>

#if !defined(CONFIG_SPL_BUILD)
ENTRY(lowlevel_init)
	/*
	 * Setup a temporary stack. Global data is not available yet.
	 */
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_STACK)
	ldr	sp, =CONFIG_SPL_STACK
#else
	ldr	sp, =CONFIG_SYS_INIT_SP_ADDR
#endif
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
#ifdef CONFIG_SPL_DM
	mov	r9, #0
#else
	/*
	 * Set up global data for boards that still need it. This will be
	 * removed soon.
	 */
#ifdef CONFIG_SPL_BUILD
	ldr	r9, =gdata
#else
	sub	sp, sp, #GD_SIZE
	bic	sp, sp, #7
	mov	r9, sp
#endif
#endif
    push {ip, lr}
    bl arch_cpu_init
    pop {ip, pc}

ENDPROC(lowlevel_init)
#endif