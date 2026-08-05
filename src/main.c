#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/i2c-dev.h>
#include <linux/slab.h>

#include "driver_file_operations_util.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Read data USB devices hosted by Pi Pico I2C slave");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");

// static int major_device_num;

// Represents the device number. Contains the major and minor numbers
static dev_t device_number;

static struct cdev cdev_info;

static struct class* device_class;

static int __init module_init_func(void) {
    pr_notice("Initializing the custom usb_to_i2c module\n");
 
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
    const struct file_operations* driver_fops = getFileOperations();

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
    
    // major_device_num = register_chrdev(0, "usb_to_i2c", driver_fops);

    // // Error getting major character device failed
    // if (major_device_num < 0) {
    //     printk(KERN_ERR "Could not assign major device number.\n");
    //     printk(KERN_ERR "Failed major number: %d\n", major_device_num);
    //     return major_device_num;

    // }

    // prink("usb_to_i2c:\nMajor device number: %d\n", major_device_num);

    device_class = class_create("usb_to_i2c_class");

    if (!device_class) {
        pr_err("usb_to_i2c: Could not create \"usb_to_i2c_class\" device class\n");
        status = ENOMEM;
        goto delete_cdev;

    }

    if(
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

    return 0;

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
    
    // unregister_chrdev(major_device_num);

    device_destroy(device_class, device_number);
    class_unregister(device_class);
    class_destroy(device_class);
    cdev_del(&cdev_info);
    unregister_chrdev_region(device_number, MINORMASK + 1);

}

module_init(module_init_func);
module_exit(module_end_func);