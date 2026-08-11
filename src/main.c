#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/property.h>

#include "file_operations_util.h"
#include "hardware_handler.h"

// static const struct of_device_id device_ids[] = {
//     {.compatible = "pipico,usb_to_i2c_converter"},
//     {} /*Empty element signifies end of list*/

// };

// MODULE_DEVICE_TABLE(of, device_ids);

// static struct i2c_device_id i2c_ids[] {
//     {"usb_to_i2c_converter", 0},
//     {}
// }

// MODULE_DEVICE_TABLE(i2c, i2c_ids);

// static int i2c_deivce_probe(
//     struct i2c_client *client,
//     const struct i2c_device_id *id
// ) {
//     pr_info("usb_to_i2c: Probe function\n");

//     // struct device = client->dev;

//     // 0x17 is the temporary hard coded address of the i2c device.
//     // Later should be replaced with a read of device tree
//     if (!client->addr = 0x17) {
//         pr_info("usb_to_i2c: i2c device address does not match target");
//         return -1;

//     }

//     return 0;
// };

// // Placeholder probe
// // static int device_probe(struct platform_device *platform_device_ptr) {
// //     dev_info(
// //         &platform_device_ptr->dev,
// //         "usb_to_i2c: Probe function is falled\n"
// //     );

// //     // if (!device_property_present(dev, "compatible")) {
// //     //     pr_err("Cannot get compatiable string");
// //     //     return -1;

// //     // }

// //     return 0;

// // }


// static int i2c_device_remove(struct i2c_lient *client) {
//     pr_info("usb_to_i2c: Probe function\n");

//     return 0;
// }

// // Placeholder remove
// // static void device_remove(struct platform_device *platform_device_ptr) {
// //     dev_info(
// //         &platform_device_ptr->dev,
// //         "usb_to_i2c: Remove function is falled\n"
// //     );

// // }

// static struct i2c_driver driver_info {
//     .probe = i2c_device_probe,
//     .remove = i2c_device_remove,
//     .id_table = i2c_ids,
//     .driver = {
//         .name = "usb_to_i2c_driver",
//         .of_match_table = device_ids
//     }
// };


// Represents the device number. Contains the major and minor numbers
static dev_t device_number;

static struct cdev cdev_info;

static struct class *device_class;

static int __init module_init_func(void) {
    pr_notice("Initializing the custom usb_to_i2c module\n");
    
    // return platform_driver_register(&driver_info);
    // Stores the status of operations
    int status;
    
    #ifdef STATIC_DEVICE_NUMBER
        device_number = STATIC_DEVICE_NUMBER;
        status = register_chrdev_region(device_number, MINORMASK + 1, "usb_to_i2c");
        
    #else
    // Dynamically allocate a region of minor device numbers
        status = alloc_chrdev_region(&device_number, 0, MINORMASK + 1, "usb_to_i2c");
    
    #endif
    
    if (status) {
        pr_err("usb_to_i2c: Could not reserve a region of device numbers\n");
        return status;
        
    }
    
    cdev_info.owner = THIS_MODULE;
    
    // Get the driver file operations available
    const struct file_operations *driver_fops = getFileOperations();
    
    // Create a chracter device
    cdev_init(&cdev_info, driver_fops);
    status = cdev_add(&cdev_info, device_number, MINORMASK + 1);
    
    if (status) {
        pr_err("usb_to_i2c: error adding cdev\n");
        goto free_device_number;
        
    }
    
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        MAJOR(device_number),
        MINOR(device_number)
    );
    
    device_class = class_create("usb_to_i2c_class");
    
    if (!device_class) {
        pr_err("usb_to_i2c: Could not create \"usb_to_i2c_class\" device class\n");
        status = ENOMEM;
        goto delete_cdev;
        
    }
    
    if (
        !device_create(
            device_class,
            NULL,
            device_number,
            NULL,
            "usb_to_i2c%d",
            0
        )
    ) {
        pr_err("usb_to_i2c: Could not create \"usb_to_i2c_class0\" device\n");       
        status = ENOMEM;
        goto delete_class;
        
    }
    
    pr_info("usb_to_i2c: Created device under /sys/class/usb_to_i2c_class0\n");
    
    return i2c_add_driver(get_i2c_driver_info());
    // return platform_driver_register(get_i2c_driver_info());
    // return platform_driver_register(&driver_info);
    
delete_class:
    class_unregister(device_class);
    class_destroy(device_class);
    
delete_cdev:
    cdev_del(&cdev_info);
    
free_device_number:
    unregister_chrdev_region(device_number, MINORMASK + 1);
    return status;
    
}
    
static void __exit module_end_func(void) {
    pr_notice("Exiting the custom Kernel module\n");
    
    
    device_destroy(device_class, device_number);
    class_unregister(device_class);
    class_destroy(device_class);
    cdev_del(&cdev_info);
    unregister_chrdev_region(device_number, MINORMASK + 1);
    
    i2c_del_driver(get_i2c_driver_info());
    // platform_driver_unregister(&driver_info);

}
    
module_init(module_init_func);
module_exit(module_end_func);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Read data USB devices hosted by Pi Pico I2C slave");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");