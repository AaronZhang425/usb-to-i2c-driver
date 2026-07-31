#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/i2c-dev.h>

#include "driver_file_operations_util.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Read data USB devices hosted by Pi Pico I2C slave");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");

static int major_device_num;

static int __init module_init_func(void) {
    printk("Initializing the custom Kerenel module\n");

    const struct file_operations* driver_fops = getFileOperations();

    major_device_num = register_chrdev(0, "usb_to_i2c", driver_fops);

    // Error getting major characer device failed
    if (major_device_num < 0) {
        printk("Could not assign major device number");
        return major_device_num;

    }
    
    printk("usb_to_i2c:\nMajor Device Number: %d\n", major_device_num);

    return 0;
    
}

static void __exit module_end_func(void) {
    printk("Exiting the custom Kernel module\n");
    
    unregister_chrdev(major_device_num, "usb_to_i2c"); 

}

module_init(module_init_func);
module_exit(module_end_func);