#ifndef __S3C_USB_GADGET
#define __S3C_USB_GADGET

#include <asm/errno.h>
#include <linux/sizes.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/list.h>

#define PHY0_SLEEP  (1 << 5)

#define DMA_BUF_SIE (16 * SZ_1K)

#define EP0_FIFO_SIZE   64
#define EP_FIFO_SIZE    512
#define EP_FIEO_SIZE2   1024

#define S3C_MAX_EPS     4
#define S3C_MAX_HW_EPS  16

enum {
    WAIT_FOR_SETUP = 0,
    DATA_STATE_XMIT,
    DATA_STATE_NEED_ZLP,
    WAIT_FOR_OUT_STATUS,
    DATA_STATE_RECV,
    WAIT_FOR_COMPLETE,
    WAIT_FOR_OUT_COMPLETE,
    WAIT_FOR_IN_COMPLETE,
    WAIT_FOR_NULL_COMPLETE,
};

enum {
    TEST_J_SEL = 1,
    TEST_K_SEL,
    TEST_SE0_NAK_SEL,
    TEST_PACKET_SEL,
    TEST_FORCE_NEABLE_SEL,
};

enum ep_type {
    EP_TYPE_CTRL,
    EP_TYPE_BULK_IN,
    EP_TYPE_BULK_OUT,
    EP_TYPE_INT,
};

struct s3c_plat_otg_data {
    int (*phy_control)(int on);
    unsigned int regs_phy;
    unsigned int regs_otg;
    unsigned int usb_phy_ctrl;
    unsigned int usb_falgs;
};

struct s3c_udc;

struct s3c_ep {
    struct usb_ep ep;
    struct s3c_udc *dev;
    
    const struct usb_endpoint_descriptor *desc;
    struct list_head queue;
    unsigned long pio_irq;
    int len;
    void *dma_buf;

    u8 stopped;
    u8 bEndpointAddress;
    u8 bmAttributes;

    enum ep_type ep_type;
    int fifo_num;
};

struct s3c_request {
    struct usb_request req;
    struct list_head queue;
};

struct s3c_udc {
    struct usb_gadget gadget;
    struct usb_gadget_driver *driver;

    struct s3c_plat_otg_data *pdata;
    
    int ep0state;
    struct s3c_ep ep[S3C_MAX_EPS];
    unsigned char usb_address;
    unsigned req_pending:1;
    unsigned req_std:1;
};

#define ep_is_in(__EP) (((__EP)->bEndpointAddress & USB_DIR_IN) == USB_DIR_IN)
#define ep_idx(__EP)   ((__EP)->bEndpointAddr & 0xf)
#define ep_maxpacket(__EP) ((__EP)->ep.maxpacket)

extern void otg_phy_init(struct s3c_udc *dev);
extern void otg_phy_off(struct s3c_udc *dev);

extern void s3c_udc_ep_set_stall(struct s3c_udc *dev);
extern int s3c_udc_probe(struct s3c_plat_otg_data *pdata);
#endif
