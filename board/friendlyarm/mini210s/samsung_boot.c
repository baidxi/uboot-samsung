#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <fs.h>
#include <part.h>

static int do_tftpboot(struct cmd_tbl *cmdtp, int flag, const char *load_addr)
{
    int ret;
    const char *serverip = env_get("serverip");
    char * const args[] = {
        "tftpboot",
        load_addr
    };
    if (!serverip) {
        printf("serverip not set\n");
        return CMD_RET_FAILURE;
    }

    net_ping_ip = string_to_ip(serverip);

    if (net_ping_ip.s_addr == 0) {
        printf("server invalid\n");
        return CMD_RET_FAILURE;
    }

    if (net_loop(PING) < 0) {
		printf("FAIL:host %s is not alive\n", serverip);
		return CMD_RET_FAILURE;
	}

    ret = do_tftpb(cmdtp, flag, 2, args);

    if (ret) {
        return CMD_RET_FAILURE;
    }

    return  do_bootm(cmdtp, flag, 2, args);

}

static int do_sdboot(struct cmd_tbl *cmdtp, int flag, const char *load_addr)
{
    int part_num, dev_num;
    char dev_part[4];
    int found = -1;
    int ret;
    const char *fname = env_get("bootfile");
    
    if (!fname) {
        printf("WARNING:bootfile env not set, use default name\n");
        fname = "kernel.itb";
    }

    for (dev_num = 0; dev_num < 3; dev_num++) {
        for (part_num = 0; part_num < 3; part_num++) {
            snprintf(dev_part, 10, "%d:%d", dev_num, part_num);
            if (fs_set_blk_dev("mmc", dev_part, FS_TYPE_ANY) == 0) {
                if ((found = fs_exists(fname))) {
                    printf("%s Found on mmc %s\n", fname, dev_part);
                    break;
                }
            }
        }
        if (found)
            break;
    }

    if (found < 0) {
        printf("%s not found in mmc card\n", fname);
        return CMD_RET_FAILURE;
    }

    char *const load_args[] = {
        "sdboot",
        "mmc",
        dev_part,
        load_addr,
        fname
    };

    ret = do_load(cmdtp, flag, 5, load_args, FS_TYPE_ANY);

    if (ret != 0)
        return CMD_RET_FAILURE;

    char *const boot_args[] = {
        "bootm",
        load_addr
    };

    return do_bootm(cmdtp, flag, 2, boot_args);

}

static int do_nandboot(const char *loaddr)
{
    return CMD_RET_SUCCESS;
}

static int do_samsung_boot(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    const char *boot_dev = NULL;
    int ret = CMD_RET_FAILURE;
    const char *loadaddr = NULL;

    if (argc != 2)
        return CMD_RET_USAGE;

    boot_dev = argv[1];
    loadaddr = env_get("kernel_loadaddr");

    if (strcmp(boot_dev, "tftp") == 0) {
        ret = do_tftpboot(cmdtp, flag, loadaddr);
    } else if (strcmp(boot_dev, "nand") == 0) {
        ret = do_nandboot(loadaddr);
    } else if (strcmp(boot_dev, "sdcard") == 0) {
        ret = do_sdboot(cmdtp, flag, loadaddr);
    } else {
        printf("%s boot device unsupported\n", boot_dev);
        return CMD_RET_FAILURE;
    }

    return ret;
}

U_BOOT_CMD(
    samsung_boot, 2, 1, do_samsung_boot,
    "samsung bootcmd",
    "[media] \n"
    " - boot from media"
);