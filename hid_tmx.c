#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>
#include "tmx.h"

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

static int hid_tmx_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
    int value = 7;

    struct usb_ctrlrequest packet = {
        .bRequestType = 0x41,
        .bRequest = 83,
        .wValue = cpu_to_le16(value),
        .wIndex = 0,
        .wLength = 0};

    struct usb_device *device = interface_to_usbdev(to_usb_interface(hdev->dev.parent));
    // Pass 0 for packet size because only sending
    // to control endpoint
    struct urb *tmxurb = usb_alloc_urb(0, GFP_KERNEL);

    // Wheel goes through 2 stages to init
    printk("probing for wheel\n");
    if (id->product != TMX_HID)
    {
        printk("Not a known wheel\n");
        return -ENODEV;
    }

    usb_fill_control_urb(tmxurb, device, usb_sndctrlpipe(device, 0), (unsigned char *)&packet, 0, 0, tmx_control_handler, 0);

    usb_submit_urb(tmxurb, GFP_KERNEL);

    usb_free_urb(tmxurb);

    return 0;
}

static void hid_tmx_remove(struct hid_device *hdev)
{
    printk("disconnect the wheel\n");
}

static const struct hid_device_id hid_tmx_devices[] = {
    {HID_USB_DEVICE(THRUSTMASTER_VID, TMX_HID)},
    {HID_USB_DEVICE(THRUSTMASTER_VID, TMX_FFB)},
    {}};

MODULE_DEVICE_TABLE(hid, hid_tmx_devices);

struct hid_driver hid_tmx_driver = {
    .name = "tmx",
    .id_table = hid_tmx_devices,
    .probe = hid_tmx_probe,
    .remove = hid_tmx_remove};
