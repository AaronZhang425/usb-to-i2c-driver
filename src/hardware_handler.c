#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>

#include "hardware_handler.h"

static const struct of_device_id device_ids[] = {
    {.compatible = "pipico,usb_to_i2c_converter"},
    {} /*Empty element signifies end of list*/

};

MODULE_DEVICE_TABLE(of, device_ids);

static struct i2c_device_id i2c_ids[] = {
    {"usb_to_i2c_converter", 0},
    {}
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static int i2c_device_probe(
    struct i2c_client *client
) {
    pr_info("usb_to_i2c: Probe function\n");

    // struct device = client->dev;

    // 0x17 is the temporary hard coded address of the i2c device.
    // Later should be replaced with a read of device tree
    if (client->addr != 0x17) {
        pr_info("usb_to_i2c: i2c device address does not match target");
        return -1;

    }

    return 0;
};

// Placeholder probe
// static int device_probe(struct platform_device *platform_device_ptr) {
//     dev_info(
//         &platform_device_ptr->dev,
//         "usb_to_i2c: Probe function is falled\n"
//     );

//     // if (!device_property_present(dev, "compatible")) {
//     //     pr_err("Cannot get compatiable string");
//     //     return -1;

//     // }

//     return 0;

// }


static void i2c_device_remove(struct i2c_client *client) {
    pr_info("usb_to_i2c: Probe function\n");

}

// Placeholder remove
// static void device_remove(struct platform_device *platform_device_ptr) {
//     dev_info(
//         &platform_device_ptr->dev,
//         "usb_to_i2c: Remove function is falled\n"
//     );

// }

static struct i2c_driver driver_info = {
    .probe = i2c_device_probe,
    .remove = i2c_device_remove,
    .id_table = i2c_ids,
    .driver = {
        .name = "usb_to_i2c_driver",
        .of_match_table = device_ids
    }
};

struct i2c_driver *get_i2c_driver_info(void) {
    return &driver_info;

}