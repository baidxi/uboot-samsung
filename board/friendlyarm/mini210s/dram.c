#include <common.h>
#include <asm/arch/cpu.h>
#include <config.h>
#include <asm/io.h>
#include <asm/arch/dmc.h>

void dram_init(void)
{
    struct s5pv210_dmc0 *const dmc0 = (struct s5pv210_dmc0 *)samsung_get_base_dmc0();
	struct s5pv210_dmc1 *const dmc1 = (struct s5pv210_dmc1 *)samsung_get_base_dmc1();
	
	/* DMC0 */
	writel(0x00101000, &dmc0->phycontrol0);
	writel(0x00101002, &dmc0->phycontrol0);			/* DLL on */
	writel(0x00000086, &dmc0->phycontrol1);
	writel(0x00101003, &dmc0->phycontrol0);			/* DLL start */
	
	while ((readl(&dmc0->phystatus) & 0x7) != 0x7); /* wait DLL locked */

	writel(0x0FFF2350, &dmc0->concontrol);			/* Auto Refresh Counter should be off */
	writel(0x00202430, &dmc0->memcontrol);			/* Dynamic power down should be off */
	writel(0x20E01323, &dmc0->memconfig0);
	
	writel(0xFF000000, &dmc0->prechconfig);
	writel(0xFFFF00FF, &dmc0->pwrdnconfig);
	
	writel(0x00000618, &dmc0->timingaref);			/* 7.8us * 200MHz = 1560 = 0x618  */
	writel(0x19233309, &dmc0->timingrow);
	writel(0x23240204, &dmc0->timingdata);
	writel(0x09C80232, &dmc0->timingpower);
	
	writel(0x07000000, &dmc0->directcmd);			/* NOP */
	writel(0x01000000, &dmc0->directcmd);			/* PALL */
	writel(0x00020000, &dmc0->directcmd);			/* EMRS2 */
	writel(0x00030000, &dmc0->directcmd);			/* EMRS3 */
	writel(0x00010400, &dmc0->directcmd);			/* EMRS enable DLL */
	writel(0x00000542, &dmc0->directcmd);			/* DLL reset */
	writel(0x01000000, &dmc0->directcmd); 			/* PALL */
	writel(0x05000000, &dmc0->directcmd);			/* auto refresh */
	writel(0x05000000, &dmc0->directcmd);			/* auto refresh */
	writel(0x00000442, &dmc0->directcmd);			/* DLL unreset */
	writel(0x00010780, &dmc0->directcmd);			/* OCD default */
	writel(0x00010400, &dmc0->directcmd);			/* OCD exit */
	
	writel(0x0FF02030, &dmc0->concontrol);			/* auto refresh on */
	writel(0xFFFF00FF, &dmc0->pwrdnconfig);
	writel(0x00202400, &dmc0->memcontrol);

	/* DMC1 */
	writel(0x00101000, &dmc1->phycontrol0);
	writel(0x00101002, &dmc1->phycontrol0);			/* DLL on */
	writel(0x00000086, &dmc1->phycontrol1);
	writel(0x00101003, &dmc1->phycontrol0);			/* DLL start */

	while ((readl(&dmc1->phystatus) & 0x7) != 0x7); /* wait DLL locked */

	writel(0x0FFF2350, &dmc1->concontrol);			/* Auto Refresh Counter should be off */
	writel(0x00202430, &dmc1->memcontrol);			/* Dynamic power down should be off */
	writel(0x40E01323, &dmc1->memconfig0);
	
	writel(0xFF000000, &dmc1->prechconfig);
	writel(0xFFFF00FF, &dmc1->pwrdnconfig);
	
	writel(0x00000618, &dmc1->timingaref);			/* 7.8us * 200MHz = 1560 = 0x618  */
	writel(0x19233309, &dmc1->timingrow);
	writel(0x23240204, &dmc1->timingdata);
	writel(0x09C80232, &dmc1->timingpower);
	
	writel(0x07000000, &dmc1->directcmd);			/* NOP */
	writel(0x01000000, &dmc1->directcmd);			/* PALL */
	writel(0x00020000, &dmc1->directcmd);			/* EMRS2 */
	writel(0x00030000, &dmc1->directcmd);			/* EMRS3 */
	writel(0x00010400, &dmc1->directcmd);			/* EMRS enable DLL */
	writel(0x00000542, &dmc1->directcmd);			/* DLL reset */
	writel(0x01000000, &dmc1->directcmd); 			/* PALL */
	writel(0x05000000, &dmc1->directcmd);			/* auto refresh */
	writel(0x05000000, &dmc1->directcmd);			/* auto refresh */
	writel(0x00000442, &dmc1->directcmd);			/* DLL unreset */
	writel(0x00010780, &dmc1->directcmd);			/* OCD default */
	writel(0x00010400, &dmc1->directcmd);			/* OCD exit */
	
	writel(0x0FF02030, &dmc1->concontrol);			/* auto refresh on */
	writel(0xFFFF00FF, &dmc1->pwrdnconfig);
	writel(0x00202400, &dmc1->memcontrol);
}