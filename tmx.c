#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

#define THRUSTMASTER_VID 0x044F
// TMX wheel identifies as a Thrustmaster TMX GIP Racing Wheel on connection
// After first init packet sent it changes to a Thrustmaster FFB Racing Wheel
// After the second init packet is sent in becomes the Thrustmaster TMX Racing Wheel
#define TMX_GIP 0xB67E
//#define TMX_FFB 0xB65D

static void tmx_control_handler(struct urb *urb)
{
    if (urb->status == 0)
    {
        printk("Success!\n");
    }
    else
    {
        printk("Error maybe?\n");
    }
}

static int tmx_probe(struct usb_interface *iface, const struct usb_device_id *id)
{
    int value = 1;

    struct usb_ctrlrequest packet = {
        .bRequestType = 0x41,
        .bRequest = 83,
        .wValue = cpu_to_le16(value),
        .wIndex = 0,
        .wLength = 0};

    struct usb_device *device = interface_to_usbdev(iface);
    // Pass 0 for packet size because only sending
    // to control endpoint
    struct urb *tmxurb = usb_alloc_urb(0, GFP_KERNEL);

    // Wheel goes through 2 stages to init
    printk("probing for wheel\n");
    if (id->idProduct != TMX_GIP)
    {
        printk("Not a known wheel\n");
        return -ENODEV;
    }

    usb_fill_control_urb(tmxurb, device, usb_sndctrlpipe(device, 0), (unsigned char *)&packet, 0, 0, tmx_control_handler, 0);

    usb_submit_urb(tmxurb, GFP_KERNEL);

    usb_free_urb(tmxurb);

    return 0;
}

static void tmx_disconnect(struct usb_interface *iface)
{
    printk("disconnect the wheel\n");
}

static const struct usb_device_id tmx_devices[] = {
    {USB_DEVICE(THRUSTMASTER_VID, TMX_GIP)},
    //    {USB_DEVICE(THRUSTMASTER_VID, TMX_FFB)},
    {}};

MODULE_DEVICE_TABLE(usb, tmx_devices);

static struct usb_driver tmx_driver = {
    .name = "tmx",
    .id_table = tmx_devices,
    .probe = tmx_probe,
    .disconnect = tmx_disconnect};

extern struct hid_driver hid_tmx_driver;

static int __init tmx_init(void)
{
    if (0 > usb_register(&tmx_driver))
    {
        return -1;
    }
    if (0 > hid_register_driver(&hid_tmx_driver))
    {
        return -1;
    }
    return 0;
}

static void __exit tmx_exit(void)
{
    hid_unregister_driver(&hid_tmx_driver);
    usb_deregister(&tmx_driver);
}

module_init(tmx_init);
module_exit(tmx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edward Way <emtek995+tmx@gmail.com>");
MODULE_DESCRIPTION("TMX Driver");
