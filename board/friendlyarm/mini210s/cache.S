#include <common.h>
#include <config.h>
#include <linux/linkage.h>

ENTRY(disable_l2cache)
    mrc p15, 0, r0, c1, c0, 1
    bic r0, r0, #(1<<1)
    mcr p15, 0, r0, c1, c0, 1
    mov pc,lr
ENDPROC(disable_l2cache)

ENTRY(enable_l2cache)
    mrc p15, 0, r0, c1, c0, 1
    orr r0, r0, #(1 << 1)
    mcr p15, 0, r0, c1, c0, 1
    mov pc, lr
ENDPROC(enable_l2cache)
