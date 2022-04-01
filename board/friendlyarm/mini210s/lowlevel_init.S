#include <common.h>
#include <config.h>
#include <linux/linkage.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>

_TEXT_BASE:
    .word CONFIG_SYS_TEXT_BASE

ENTRY(lowlevel_init)
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_STACK)
    ldr sp, = CONFIG_SPL_STACK
#else
    ldr sp, = CONFIG_SYS_INIT_SP_ADDR
#endif
    bic sp, sp, #7 /* 8-byte alignment for ABI compliance */
#ifdef CONFIG_SPL_DM
    mov r9, #0
#else

#ifdef CONFIG_SPL_BUILD
    ldr r9, = gdata
#else
    sub sp, sp, #GD_SIZE
    bic sp, sp, #7
    mov r9, sp
#endif
#endif
    push {lr}
    ldr r0, = S5PC110_RST_STAT
    ldr r1, [r0]
    bic r1, r1, #0xfff6ffff
    cmp r1, #0x10000
    beq wakeup_reset_pre
    cmp r1, #0x80000
    beq wakeup_reset_from_didle

    /* IO Retention release */

    ldr r0, =S5PC110_OTHERS
    ldr r1, [r0]
    ldr r2, =IO_RET_REL
    orr r1, r1, r2
    str r1, [r0]

    /* Disable watchdog */
    ldr r0, =S5PC110_WATCHDOG_BASE
    mov r1, #0
    str r1, [r0]

    /* SRAM (2MB) init for SMDKC110 */    
    /* GPJ1 SROM_ADDR_16to21 */
    ldr r0, =S5PC110_GPIO_BASE
    ldr r1, [r0, #0x260]    /* GPJ1CON_OFFSTE */
    bic r1, r1, #0xFFFFFF
    ldr r2, =0x444444
    orr r1, r1, r2
    str r1, [r0, #0x260]

    ldr r1, [r0, #0x268]
    ldr r2, =0x3ff
    bic r1, r1, r2
    str r1, [r0, #0x268]

    /* GPJ4 SROM_ADDR_16to21 */
    ldr r1, [r0, #0x2c0]
    bic r1, r1, #(0xf << 16)
    ldr r2, =(0x4 << 16)
    orr r1, r1, r2
    str r1, [r0, #0x2c0]

    ldr r1, [r0, #0x2c8]
    ldr r2, =(0x3 << 8)
    bic r1, r1, r2
    str r1, [r0, #0x2c8]

    /* CS0 - 16bit sram, enable nBE, Byte base address */
    ldr r0, =S5PC110_SROMC_BASE
    mov r1, #0x1
    str r1, [r0]

    /* PS_HOLD pin(GPH0_0 set to high */
    ldr r0, =S5PC110_CLOCK_BASE + PS_HOLD_CONTROL_OFFSET
    ldr r1, [r0]
    orr r1, r1, #0x300
    orr r1, r1, #0x1
    str r1, [r0]

    /*
     * when we already run in ram, we don't need to relocate U-Boot.
     * and actually, memory controller must be configured befor U-Boot.
     * is running in ram. 
     */

    ldr r0, =0x00ffffff
    bic r1, pc, r0      /* r0 <- current base addr of code */
    ldr r2, _TEXT_BASE  /* r1 <- original base addr in ram */
    bic r2, r2, r0      /* r0 <- currnet bas addr of code */
    cmp r1, r2
    beq 1f
    /* init system clock */
    bl system_clock_init
    /* memory init */
    bl dram_init
1:
    ldr r0,=S5PC110_RST_STAT
    ldr r1, [r0]
    bic r1, r1, #0xfffeffff
    cmp r1, #0x10000
    beq wakeup_reset_pre

    ldr r0, =0xe010c300
    orr r1, r1,#(0x1 << 23)
    str r1, [r0]

    pop {pc}

wakeup_reset_from_didle:
    ldr r0, =  S5PC110_CLOCK_BASE
lockloop:
    ldr r1, [r0, #0x100]        /* APLL CON0 */
    and r1, r1, #(1 << 29)
    cmp r1, #(1 << 29)
    bne lockloop
    beq exit_wakeup

wakeup_reset_pre:
    mrc p15, 0, r1, c1, c0, 1 @Read CP15 Auxiliary control register 
    and r1, r1, #0x80000000	@Check L2RD is disable or not
    cmp r1, #0x80000000
    bne wakeup_reset     

    bl disable_l2cache
    bl v7_flush_dcache_all
    bl enable_l2cache

wakeup_reset:
    bl system_clock_init    
    
exit_wakeup:
    ldr r0, = S5PC110_INFORM0 + 0x0
    ldr r1, [r0]
    mov pc, r1

ENDPROC(lowlevel_init)