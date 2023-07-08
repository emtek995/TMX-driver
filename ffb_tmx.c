#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>
#include "tmx.h"

spinlock_t lock;

static int tmx_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
    int ret = 0;
    struct tmx_data *drv_data;

    spin_lock_init(&lock);

    drv_data = kzalloc(sizeof(struct tmx_data), GFP_KERNEL);
    if (!drv_data)
    {
        ret = -ENOMEM;
        goto err;
    }

    drv_data->quirks = id->driver_data;
    hid_set_drvdata(hdev, (void *)drv_data);

    ret = hid_parse(hdev);
    if (ret)
    {
        hid_err(hdev, "parse failed");
        goto err;
    }

    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT & ~HID_CONNECT_FF);
    if (ret)
    {
        hid_err(hdev, "hw start failed");
        goto err;
    }

    return 0;

err:
    return ret;
}

static void tmx_remove(struct hid_device *hdev)
{
    struct tmx_data *drv_data;
    drv_data = hid_get_drvdata(hdev);
    kfree(drv_data);
}

static __u8 *tmx_report_fixup(struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize)
{
    rdesc = tmx_rdesc_fixed;
    *rsize = sizeof(tmx_rdesc_fixed);
    return rdesc;
}

static const struct hid_device_id tmx_devices[] = {
    {HID_USB_DEVICE(THRUSTMASTER_VID, TMX_FFB),
     .driver_data = tmx_ff_effects},
    {}};
MODULE_DEVICE_TABLE(hid, tmx_devices);

static struct hid_driver tmx_driver = {
    .name = "tmx",
    .id_table = tmx_devices,
    .probe = tmx_probe,
    .remove = tmx_remove,
    .report_fixup = tmx_report_fixup,
};