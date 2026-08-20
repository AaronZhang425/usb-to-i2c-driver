#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/idr.h>

#include "driver_config.h"
#include "root_i2c_dev_fops.h"
#include "i2c_dev_commands.h"

struct i2c_client_management_info {
    dev_t dev_num;
};

static DEFINE_IDA(id_allocator);

// Represents the base device number. Only the major number component is used
static dev_t base_device_num;
static struct cdev cdev_info;
static struct class *device_class;

static const struct of_device_id device_ids[] = {
    {.compatible = "aaron,usb_to_i2c_converter"},
    {} /*Empty element signifies end of list*/
};

MODULE_DEVICE_TABLE(of, device_ids);

static struct i2c_device_id i2c_ids[] = {
    {"usb_to_i2c_converter", 0},
    {}
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static int i2c_device_probe(struct i2c_client *client) {
    pr_info("usb_to_i2c: Probe function\n");

    if (client->addr != 0x17) {
        pr_info("usb_to_i2c: i2c device address does not match target");
        return -1;

    }
    
    struct i2c_client_management_info *i2c_management_data = devm_kzalloc(
        &client->dev,
        sizeof(struct i2c_client_management_info),
        GFP_KERNEL
    );

    if (i2c_management_data) {
        pr_err("usb_to_i2c: Not enouth memory to allocate private data for i2c client");
        return -ENOMEM;

    }

#ifdef MAX_DEVICES
    int minor_num = ida_alloc_range(&id_allocator, 0, MAX_DEVICES - 1, GFP_KERNEL);
#else
    int minor_num = ida_alloc_range(&id_allocator, 0, MINORMASK, GFP_KERNEL);
#endif

    if (minor_num == -ENOMEM) {
        pr_err("usb_to_i2c: Not enouth memory to allocate another minor num");
        return -ENOMEM;

    }

    if (minor_num == -ENOSPC) {
        pr_err("usb_to_i2c: No more ids for minor num");
        return -ENOSPC;

    }

    i2c_management_data->dev_num = MKDEV(
        MAJOR(base_device_num), minor_num
    );

    i2c_set_clientdata(client, i2c_management_data);

    indexed_i2c_clients[minor_num] = client;

    if (
        !device_create(
            device_class,
            NULL,
            i2c_management_data->dev_num,
            NULL,
            "usb_to_i2c%d",
            0
        )
    ) {
        pr_err("usb_to_i2c: Could not create \"usb_to_i2c_class[#]\" device\n");       
        return -ENOMEM;
        // goto delete_class;
        
    }
    
    
    pr_info("usb_to_i2c: Created device under /sys/class/usb_to_i2c_class0\n");    

    // uint8_t read_buffer[I2C_INIT_BUFFER_SIZE] = {0};

    // First element is size of each init block
    // Second element is number of init blocks
    __u8 start_signal = INIT_SIG;
    __u8 init_buffer_config[2] = {0};

    struct i2c_msg msgs[] = {
        {
            .addr = client->addr,
            .flags = 0,
            .len = 1,
            .buf = &INIT_SIG
        },
        {
            .addr = client->addr,
            .flags = I2C_M_RD,
            .len = 2, // bytes to read; not msgs length or num of msg
            .buf = init_buffer_config
        }
    };

    int msgs_sent = -1;
    
    for (int i = 0; i < 3 && msgs_sent != 2; i++) {
        msgs_sent = i2c_transfer(client->adapter, msgs, 2);

    }

    if (msgs_sent != 2) {
        pr_err("usb_to_i2c: Cannot get size of init buffer. Tried 3 times.\n");
        pr_info("usb_to_i2c: Number of msgs sent in last attempt: %u\n", msgs_sent);
        pr_info("usb_to_i2c: Expected to send 2 msgs\n");

        return -EIO; 

    }

    __u8 init_buffer[init_buffer_config[0] * init_buffer_config[1]] = {0};

    return 0;

};

static void i2c_device_remove(struct i2c_client *client) {
    pr_info("usb_to_i2c: Probe function\n");

    struct i2c_client_management_info *i2c_management_data = i2c_get_clientdata(
        client
    );

    dev_t dev_num = i2c_management_data->dev_num;

    indexed_i2c_clients[MINOR(dev_num)] = NULL;

    ida_free(&id_allocator, MINOR(dev_num));

    device_destroy(device_class, i2c_management_data->dev_num);

}

static struct i2c_driver driver_info = {
    .probe = i2c_device_probe,
    .remove = i2c_device_remove,
    .id_table = i2c_ids,
    .driver = {
        .name = "usb_to_i2c_driver",
        .of_match_table = device_ids
    }
};


static int __init module_init_func(void) {
    pr_notice("Initializing the custom usb_to_i2c module\n");
    
    // Stores the status of operations
    int status;
    

#ifdef STATIC_BASE_DEV_NUM // If a static device number is defined, use it.
    base_device_num = STATIC_DEV_NUM;
    status = register_chrdev_region(base_device_num, MINORMASK + 1, "usb_to_i2c");
#else
    // Dynamically allocate a region of minor device numbers
    status = alloc_chrdev_region(&base_device_num, 0, MINORMASK + 1, "usb_to_i2c");
#endif
    
    if (status) {
        pr_err("usb_to_i2c: Could not reserve a region of device numbers\n");
        return status;
        
    }
    
    cdev_info.owner = THIS_MODULE;
    
    // Get the driver file operations available
    const struct file_operations *driver_fops = get_file_operations();
    
    // Create a chracter device
    cdev_init(&cdev_info, driver_fops);
    status = cdev_add(&cdev_info, base_device_num, MINORMASK + 1);
    
    if (status) {
        pr_err("usb_to_i2c: error adding cdev\n");
        goto free_device_num_alloc;
        
    }
    
    pr_info(
        "usb_to_i2c:\nBase major: %d\nBase minor: %d\n",
        MAJOR(base_device_num),
        MINOR(base_device_num)
    );
    
    device_class = class_create("usb_to_i2c_class");
    
    if (!device_class) {
        pr_err("usb_to_i2c: Could not create \"usb_to_i2c_class\" device class\n");
        status = ENOMEM;
        goto delete_cdev;
        
    }
    
    return i2c_add_driver(&driver_info);

delete_cdev:
    cdev_del(&cdev_info);
    
free_device_num_alloc:
    unregister_chrdev_region(base_device_num, MINORMASK + 1);
    return status;
    
}
    
static void __exit module_end_func(void) {
    pr_notice("Exiting the custom Kernel module\n");
    
    ida_destroy(&id_allocator);

    // device_destroy(device_class, device_number);
    i2c_del_driver(&driver_info);
    
    class_unregister(device_class);
    class_destroy(device_class);
    cdev_del(&cdev_info);
    unregister_chrdev_region(base_device_num, MINORMASK + 1);
    
    // platform_driver_unregister(&driver_info);

}
    
module_init(module_init_func);
module_exit(module_end_func);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Read data USB devices hosted by Pi Pico I2C slave");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");