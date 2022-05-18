#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

#define THRUSTMASTER_VID 0x044F
// TMX wheel identifies as a Thrustmaster TMX GIP Racing Wheel on connection
// After first init packet sent it changes to a Thrustmaster FFB Racing Wheel
// After the second init packet is sent in becomes the Thrustmaster TMX Racing Wheel
#define TMX_GIP 0xB67E
#define TMX_FFB 0xB65D

static void tmx_control_handler(struct urb *urb) {
    if (urb->status == 0) {
        printk("Success!\n");
    } else {
        printk("Error maybe?\n");
    }
}

static int hid_tmx_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
    int value = 7;
    struct usb_device *device = interface_to_usbdev(to_usb_interface(hdev->dev.parent));
    // Pass 0 for packet size because only sending
    // to control endpoint
    struct urb *tmxurb = usb_alloc_urb(0, GFP_KERNEL);


    // Wheel goes through 2 stages to init
    printk("probing for wheel\n");
    if (id->product != TMX_FFB) {
        printk("Not a known wheel\n");
        return -ENODEV;
    }

    struct usb_ctrlrequest packet = {
        .bRequestType = 0x41,
        .bRequest = 83,
        .wValue = cpu_to_le16(value),
        .wIndex = 0,
        .wLength = 0
    };

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
//    {HID_USB_DEVICE(THRUSTMASTER_VID, TMX_GIP)},
    {HID_USB_DEVICE(THRUSTMASTER_VID, TMX_FFB)},
    {}};

MODULE_DEVICE_TABLE(hid, hid_tmx_devices);

struct hid_driver hid_tmx_driver = {
    .name = "tmx",
    .id_table = hid_tmx_devices,
    .probe = hid_tmx_probe,
    .remove = hid_tmx_remove
};

