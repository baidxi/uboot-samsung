#include <common.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <asm/types.h>
#include <asm/io.h>

#include <asm/arch/gpio.h>

#define OTG_DMA_MODE    1
#define DEBUG_SETUP 0
#define DEBUG_EP0   0
#define DEBUG_ISR   0
#define DEBUG_OUT_EP    0
#define DEBUG_IN_EP     0

#include <usb/s3c_udc.h>

#define EP0_CON     0
#define EP_MASK     0xf

const static char *state_name[] = {
    "WAIT_FOR_SETUP",
    "DATA_STATE_XMIT",
    "DATA_STATE_NEED_ZLP",
    "WAIT_FOR_OUT_STATUS",
    "DATA_STATE_RECV",
    "WAIT_FOR_COMPLETE",
    "WAIT_FOR_OUT_COMPLETE",
    "WAIT_FOR_IN_COMPLETE",
    "WATI_FOR_NULL_COMPLETE"
};

#define DRIVER_DESC "S3C HS USB OTG Device Driver, (c) Samsung Electronics"
#define DRIVER_VERSION "15 March 2009"

struct s3c_udc *the_controller;

static const char driver_name[] = "s3c-udc";
static const char driver_desc[] = DRIVER_DESC;
static const char ep0name[] = "ep0-control";

static unsigned int ep0_fifo_size   = EP0_FIFO_SIZE;
static unsigned int ep_fifo_size = EP_FIFO_SIZE;
static unsigned int ep_fifo_size2 = EP_FIEO_SIZE2;
static int reset_available   = 1;

static struct usb_ctrlrequest *usb_ctrl;
static dma_addr_t usb_ctrl_dma_addr;

void set_udc_gadget_private_data(void *p)
{
    debug_cond(DEBUG_SETUP != 0, "%s: the_controller: 0x%p, p: 0x%p\n", __func__, the_controller, p);
    the_controller->gadget.dev.device_data = p;
}

void *get_udc_gadget_private_data(struct usb_gadget *gadget)
{
    return gadget->dev.device_data;
}


static int s3c_ep_enable(struct usb_ep *ep, const struct usb_endpoint_descriptor *desc)
{

}

static int s3c_ep_disable(struct usb_ep *ep)
{

}

static struct usb_request *s3c_alloc_request(struct usb_ep *ep, gfp_t gfp_flags)
{

}

static void s3c_free_request(struct usb_ep *ep, struct usb_request *req)
{

}

static int s3c_queue(struct usb_ep *ep, struct usb_request *req, gfp_t gfp_flags)
{

}

static int s3c_dequeue(struct usb_ep *ep, struct usb_request *req)
{

}

static int s3c_udc_set_halt(struct usb_ep *ep, int val)
{

}

static void s3c_fifo_status(struct usb_ep *ep)
{

}

static void s3c_fifo_flush(struct usb_ep *ep)
{
    
}

static struct usb_ep_ops s3c_ep_ops = {
    .enable = s3c_ep_enable,
    .disable = s3c_ep_disable,

    .alloc_request = s3c_alloc_request,
    .free_request = s3c_free_request,
    
    .queue = s3c_queue,
    .dequeue = s3c_dequeue,

    .set_halt = s3c_udc_set_halt,
    .fifo_status = s3c_fifo_status,
    .fifo_flush = s3c_fifo_flush,
};

