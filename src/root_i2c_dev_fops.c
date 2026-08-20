#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/fs.h>

#include "root_i2c_dev_fops.h"
#include "driver_config.h"

// Indexes i2c clients using the minor device number as index
struct i2c_client *indexed_i2c_clients[MAX_DEVICES];

// struct private_data_container {
//     unsigned short i2c_addr;
// };

static int open_file(struct inode *inode_ptr, struct file *file_ptr) {
    pr_info("usb_to_i2c: open called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode_ptr),
        iminor(inode_ptr)
    );

    pr_info(
        "usb_to_i2c: i2c addr is %d\n",
        indexed_i2c_clients[iminor(inode_ptr)]->addr
    );

    // file_ptr->private_data = kzalloc(
    //     sizeof(struct private_data_container),
    //     GFP_KERNEL
    // );

    // if (!file_ptr->private_data) {
    //     pr_err("usb_to_i2c: Out of memory\n");
    //     return -ENOMEM;

    // }

    // struct *i2c_client i2c_client_ptr = indexed_i2c_clients[iminor(inode_ptr)];

    // if (!i2c_client_ptr) {
    //     pr_err("usb_to_i2c: Cannot access i2c_client for given minor device number");
    //     return -ENODEV;

    // }

    // ((struct private_data_container*) file_ptr->private_data)->i2c_addr = (
    //     i2c_client_ptr->addr
    // );

    // file_ptr->private_data = kmalloc(MEMSIZE, GFP_KERNEL);

    return 0;
    
}

static int release_file(struct inode *inode_ptr, struct file *file_ptr) {
    pr_info("usb_to_i2c: release called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode_ptr),
        iminor(inode_ptr)
    );

    pr_info(
        "usb_to_i2c: releasing i2c file with addr %d",
        indexed_i2c_clients[iminor(inode_ptr)]->addr
    );

    indexed_i2c_clients[iminor(inode_ptr)] = NULL;

    // kfree(file_ptr->private_data);
    // file_ptr->private_data = NULL;

    return 0;

}

static ssize_t read_file(
    struct file *file_ptr,
    char __user *user_buffer,
    size_t length,
    loff_t *offset
) {
    pr_info("usb_to_i2c: read called\n");
    
    // char *text = file_ptr -> private_data;

    // int not_copied, delta, to_copy = (
    //     (length + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    // );

    // not_copied = copy_to_user(user_buffer, &text[*offset], to_copy);
    // delta = to_copy - not_copied;

    // *offset += delta;

    // return delta;

    return -ENOTSUPP;

}

static ssize_t write_file(
    struct file *file_ptr,
    const char __user *user_buffer,
    size_t length,
    loff_t *offset
) {
    pr_info("usb_to_i2c: write called\n");
    
    // char *text = file_ptr -> private_data;

    // int not_copied, delta, to_copy = (
    //     (length + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    // );

    // not_copied = copy_from_user(&text[*offset], user_buffer, to_copy);
    // delta = to_copy - not_copied;

    // *offset += delta;

    // return delta;

    return -ENOTSUPP;

}

static long int file_ioctl(
    struct file *file_ptr,
    unsigned int cmd,
    unsigned long args
) {
    pr_info("usb_to_i2c: ioctl called with cmd: %d and args %lu\n", cmd, args);

    switch (cmd) {

        default:
            return -EOPNOTSUPP;

    }
    
    return 0;

}

static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .open = open_file,
    .release = release_file,
    .read = read_file,
    .write = write_file,
    .llseek = default_llseek,
    .unlocked_ioctl = file_ioctl
};

const struct file_operations *get_file_operations(void) {
    return &driver_fops; 

}