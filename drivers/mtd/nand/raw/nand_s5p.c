#include <common.h>
#include <asm/arch/cpu.h>
#include <nand.h>
#include <linux/mtd/rawnand.h>
#include <linux/bug.h>
#include <dm/read.h>
#include <dm/device_compat.h>
#include <asm/arch/nand.h>
#include <asm/arch/gpio.h>

static void s5p_nand_select_chip(struct mtd_info *mtd, int ctl)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    switch(ctl) {
        case -1:
            chip->cmd_ctrl(mtd, NAND_CMD_NONE, 0 | NAND_CTRL_CHANGE);
            return;
        case 0:
        case 1:
            chip->cmd_ctrl(mtd, NAND_CMD_NONE, (ctl + 1)<< 1 | NAND_NCE | NAND_CTRL_CHANGE);
        case 2:
        case 3:
            chip->cmd_ctrl(mtd, NAND_CMD_NONE, (20  + ctl) << 1 | NAND_NCE | NAND_CTRL_CHANGE);
            break;
        default:
            BUG();
    }
}

static uint8_t s5p_nand_read_byte(struct mtd_info *mtd)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;

    return readb(&reg->nfdata);
}

static void s5p_nand_write_byte(struct mtd_info *mtd, uint8_t byte)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;

    writeb(byte, &reg->nfdata);
}

static void s5p_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;

    for (int i = 0; i < len; i++) {
        writeb(buf[i], &reg->nfdata);
    }
}

static void s5p_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;

    for (int i = 0; i < len; i++){
        buf[i] = readb(&reg->nfdata);
    }
}

static void s5p_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *controller = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = controller->reg;

	if (cmd == NAND_CMD_NONE) {
		if (ctrl & NAND_CTRL_CHANGE) {
            if (ctrl & NAND_NCE) {
                u8 n = (ctrl >> 1) & 0x1f;
                if (n) {
                    writel(readl(&reg->nfcont) & ~(1 << n), &reg->nfcont);
                }
                readl(&reg->nfstat);
            }
		}
	} else {
        if (ctrl & NAND_CTRL_CHANGE) {
            if (ctrl & NAND_ALE) {
                writel(cmd, &reg->nfaddr);
            } else if (ctrl & NAND_CLE) {
                writel(cmd, &reg->nfcmd);
            }
        } else {
            if (ctrl & NAND_ALE) {
                writel(cmd, &reg->nfaddr);
            } else if (ctrl & NAND_CLE) {
                writel(cmd, &reg->nfcmd);
            }
        }
    }
}

static int s5p_dev_ready(struct mtd_info *mtd)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;

    return readl(&reg->nfstat) & 0x01;
}

static void s5p_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_reg *reg = ctrl->reg;
    struct s5p_nand_ecc *ecc = ctrl->ecc;

    writel(readl(&reg->nfconf) | (0x3 << 23), &reg->nfconf);

    if (mode == NAND_ECC_READ)
    {
        writel(readl(&ecc->nfeccont) & ~(0x1 << 16), &ecc->nfeccont);
        writel(readl(&ecc->nfeccstat) | (0x1 << 24), &ecc->nfeccstat);        
    } else if (mode == NAND_ECC_WRITE) {
        /* set 8/12/16bit Ecc direction to Encoding */
        writel(readl(&ecc->nfeccont) | (0x1 << 16), &ecc->nfeccont);
        /* clear 8/12/16bit ecc encode done */
        writel(readl(&ecc->nfeccstat) | (0x1 << 25), &ecc->nfeccstat);
    }
    /* Initialize main area ECC decoder/encoder */
    writel(readl(&reg->nfcont) | (0x1 << 5), &reg->nfcont);
	/* The ECC message size(For 512-byte message, you should set 511)
     * 8-bit ECC/512B 
     */

    writel(((chip->ecc.size -1) << 16) | 0x3, &ecc->nfecconf);
    writel(readl(&reg->nfstat) | (0x1 << 4) | (0x1 << 5), &reg->nfstat);
	/* Initialize main area ECC decoder/ encoder */
    writel(readl(&ecc->nfeccont) | (0x1 << 2), &ecc->nfeccont);
    /* Unlock Main area ECC   */
    writel(readl(&reg->nfcont) & ~(0x1 << 7), &reg->nfcont);
}

static int s5p_nand_cal_ecc(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
    struct s5p_nand_ecc *ecc = ctrl->ecc;
    struct s5p_nand_reg *reg = ctrl->reg;

    u32 nfeccpregecc0 = 0, nfeccpregecc1 = 0, nfeccpregecc2 = 0, nfeccpregecc3 = 0;

    writel(readl(&reg->nfcont) | (1 << 7), &reg->nfcont);

    /* 读取13 Byte的Ecc Code */
    nfeccpregecc0 = readl(&ecc->nfeccprgecc0); 
    nfeccpregecc1 = readl(&ecc->nfeccprgecc1); 
    nfeccpregecc2 = readl(&ecc->nfeccprgecc2); 
    nfeccpregecc3 = readl(&ecc->nfeccprgecc3);    

	ecc_code[0] = nfeccpregecc0 & 0xff;
	ecc_code[1] = (nfeccpregecc0 >> 8) & 0xff;
	ecc_code[2] = (nfeccpregecc0 >> 16) & 0xff;
	ecc_code[3] = (nfeccpregecc0 >> 24) & 0xff;
	ecc_code[4] = nfeccpregecc1 & 0xff;
	ecc_code[5] = (nfeccpregecc1 >> 8) & 0xff;
	ecc_code[6] = (nfeccpregecc1 >> 16) & 0xff;
	ecc_code[7] = (nfeccpregecc1 >> 24) & 0xff;
	ecc_code[8] = nfeccpregecc2 & 0xff;
	ecc_code[9] = (nfeccpregecc2 >> 8) & 0xff;
	ecc_code[10] = (nfeccpregecc2 >> 16) & 0xff;
	ecc_code[11] = (nfeccpregecc2 >> 24) & 0xff;
	ecc_code[12] = nfeccpregecc3 & 0xff;

    return 0;
}

static int s5p_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
    if (read_ecc[0] == calc_ecc[0] && 
        read_ecc[1] == calc_ecc[1] &&
        read_ecc[2] == calc_ecc[2])
        return 0;

    printf("s5pv210 nand correct data: not implemented\n");
    return -1;
}

// static u16 s5p_nand_get_pagesize(struct mtd_info *mtd)
// {
//     bool is_mlc;
//     bool pagesize;
//     struct nand_chip *chip = mtd_to_nand(mtd);
//     struct s5p_nand_ctrl *ctrl = container_of(chip, struct s5p_nand_ctrl, chip);
//     struct s5p_nand_reg *reg = ctrl->reg;

//     is_mlc = (readl(&reg->nfconf)) & (1 << 3);
//     pagesize = (readl(&reg->nfconf) & (1 << 2));

//     return is_mlc ? pagesize ? 2048 : 4096 : pagesize ? 512 : 2048; 
// }

#define NF8_ReadPage_Adv(a,b,c) (((int(*)(u32, u32, u8*))(*((u32 *) 0xD0037F90)))(a,b,c))

// typedef int(*s5p_internal_func)(u32, u32, u8*);

int s5p_nand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int oob_required, int page)
{
    /* mini210s使用的NAND FLASH一个块64页 */ 
    printf("nand read page %d buf %p\n", page, buf);
    return NF8_ReadPage_Adv(page / 64, page % 64, buf);

    // u16 pagesize = s5p_nand_get_pagesize(mtd);

    // printf("pagesize = %d page = %d\n", pagesize, page);
    // // for (int i = 0; i < page; i++) {

    // // }
}

static struct nand_ecclayout nand_oob_64 = {
	.eccbytes = 52,		/* 2048 / 512 * 13 */
	.eccpos = {	12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
				22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
				32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
				42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
				52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
				62, 63},
	/* 0和1用于保存坏块标记，12~63保存ecc，剩余2~11为free */
	.oobfree = {
			{.offset = 2,
			.length = 10}
		}
};

static void s5p_nand_hw_init(struct s5p_nand_reg *reg)
{
    u32 cfg =	(0x1 << 23) |	/* Disable 1-bit and 4-bit ECC */
        /* 下面3个时间参数稍微比计算出的值大些（我这里依次加1），否则读写不稳定 */
        (0x3 << 12) |	/* 7.5ns * 2 > 12ns tALS tCLS */
        (0x2 << 8) | 	/* (1+1) * 7.5ns > 12ns (tWP) */
        (0x1 << 4) | 	/* (0+1) * 7.5 > 5ns (tCLH/tALH) */
        (0x0 << 3) | 	/* SLC NAND Flash */
        (0x0 << 2) |	/* 2KBytes/Page */
        (0x1 << 1);		/* 5 address cycle */
    writel(cfg, &reg->nfconf);

    for (int i = S5PC110_GPIO_MP012; i < S5PC110_GPIO_MP015 + 1; i++) {
        gpio_cfg_pin(i, S5P_GPIO_FUNC(3));
    }

    for (int i = S5PC110_GPIO_MP030; i < S5PC110_GPIO_MP037 + 1; i++) {
        gpio_cfg_pin(i, S5P_GPIO_FUNC(2));
    }

    for (int i = S5PC110_GPIO_MP060; i < S5PC110_GPIO_MP067 + 1; i++) {
       gpio_cfg_pin(i, S5P_GPIO_FUNC(2)); 
    }

    writel(readl(&reg->nfcont) | 0x1, &reg->nfcont);
}

static int s5p_nfc_probe(struct udevice *dev)
{    

    struct s5p_nand_ctrl *nfc = dev_get_priv(dev);
    struct nand_chip *chip = &nfc->chip;
    struct mtd_info *mtd;
    ofnode child;
    int err;
    struct s5p_nand_reg *reg;

    nfc->reg = (void *)dev_read_addr_name(dev, "nand");
    reg = (struct s5p_nand_reg *)nfc->reg;
    nfc->ecc = (void *)dev_read_addr_name(dev, "ecc");

    mtd = nand_to_mtd(chip);

    ofnode_for_each_subnode(child, dev_ofnode(dev))
        nand_set_flash_node(chip, child);

    nand_set_controller_data(chip, nfc);

    chip->select_chip = s5p_nand_select_chip;
    chip->cmd_ctrl = s5p_hwcontrol;
    chip->dev_ready = s5p_dev_ready;
    chip->read_byte = s5p_nand_read_byte;
    chip->read_buf = s5p_nand_read_buf;
    chip->write_byte = s5p_nand_write_byte;
    chip->write_buf = s5p_nand_write_buf;

    chip->options |= NAND_BUSWIDTH_AUTO;

    chip->ecc.hwctl = s5p_nand_enable_hwecc;
    chip->ecc.calculate = s5p_nand_cal_ecc;
    chip->ecc.correct = s5p_nand_correct_data;
    chip->ecc.strength = 1;
    chip->ecc.layout = &nand_oob_64;
    chip->ecc.read_page = s5p_nand_read_page_hwecc;

    /* init hardware */
    s5p_nand_hw_init(reg);

    err = nand_scan(mtd, 1);

    if (err) {
        dev_err(dev, "Failed to scan nand: %d\n", err);
        return err;
    }


    err = nand_register(0, mtd);

    if (err) {
        dev_err(dev, "Failed to register MTD: %d\n", err);
        return err;
    }

    return 0;
}

static const struct udevice_id s5p_nfc_of_ids[] = {
    {.compatible = "s5p,nfc"},
    {}
};

U_BOOT_DRIVER(s5p_nfc) = {
    .name = "s5p_nfc",
    .id = UCLASS_MTD,
    .of_match = s5p_nfc_of_ids,
    .probe = s5p_nfc_probe,
    .priv_auto = sizeof(struct s5p_nand_ctrl),
};

UCLASS_DRIVER(s5p_nfc) = {
    .id = UCLASS_MTD,
    .name = "s5p_nfc",
    .flags = DM_UC_FLAG_NO_AUTO_SEQ,
};

void board_nand_init(void)
{
    struct udevice *dev;
    int ret;

    ret = uclass_get_device_by_driver(UCLASS_MTD, DM_DRIVER_GET(s5p_nfc), &dev);

    if (ret && ret != -ENODEV) 
        pr_err("Failed to initialize %s (error %d)\n", dev->name, ret);

}