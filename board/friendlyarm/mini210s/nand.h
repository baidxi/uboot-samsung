/* add by shl*/

#ifndef __NAND_REG_H_
#define __NAND_REG_H_
#include <common.h>
#ifndef __ASSEMBLY__

#define NF8_ReadPage_Adv(a,b,c) (((int(*)(u32, u32, u8*))(*((u32 *)0xD0037F90)))(a,b,c))

#define MP0_1CON  (*(volatile u32 *)0xE02002E0)
#define	MP0_3CON  (*(volatile u32 *)0xE0200320)
#define	MP0_6CON  (*(volatile u32 *)0xE0200380)	
#define S5PV210_NAND_BASE   0xB0E00000

struct s5pv210_nand {
	u32	nfconf;
	u32	nfcont;
	u32	nfcmmd;
	u32	nfaddr;
	u32	nfdata;
	u32	nfmeccd0;
	u32	nfmeccd1;
	u32	nfseccd;
	u32 nfsblk;
	u32 nfeblk;
	u32	nfstat;
	u32 nfeccerr0;
	u32 nfeccerr1;
	u32 nfmecc0;
	u32 nfmecc1;
	u32 nfsecc;
	u32 nfmlcbitpt;
	u8 res0[0x1ffbc];
	u32 nfeccconf;
	u8 res1[0x1c];
	u32 nfecccont;
	u8 res2[0xc];
	u32 nfeccstat;
	u8 res3[0xc];
	u32 nfeccsecstat;
	u8 res4[0x4c];
	u32 nfeccprgecc0;
	u32 nfeccprgecc1;
	u32 nfeccprgecc2;
	u32 nfeccprgecc3;
	u32 nfeccprgecc4;
	u32 nfeccprgecc5;
	u32 nfeccprgecc6;
	u8 res5[0x14];
	u32 nfeccerl0;
	u32 nfeccerl1;
	u32 nfeccerl2;
	u32 nfeccerl3;
	u32 nfeccerl4;
	u32 nfeccerl5;
	u32 nfeccerl6;
	u32 nfeccerl7;
	u8 res6[0x10];
	u32 nfeccerp0;
	u32 nfeccerp1;
	u32 nfeccerp2;
	u32 nfeccerp3;
	u8 res7[0x10];
	u32 nfeccconecc0;
	u32 nfeccconecc1;
	u32 nfeccconecc2;
	u32 nfeccconecc3;
	u32 nfeccconecc4;
	u32 nfeccconecc5;
	u32 nfeccconecc6;
};

#endif

#endif
