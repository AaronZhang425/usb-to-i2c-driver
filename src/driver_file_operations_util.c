#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "driver_file_operations_util.h"

#define MEMSIZE 64

static int open_file(struct inode *inode_ptr, struct file *file_ptr) {
    pr_info("usb_to_i2c: open called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode_ptr),
        iminor(inode_ptr)
    );

    file_ptr->private_data = kmalloc(MEMSIZE, GFP_KERNEL);

    if (!file_ptr->private_data) {
        pr_err("usb_to_i2c: Out of memory\n");
        return ENOMEM;

    }

    return 0;
    
}

static int release_file(struct inode *inode_ptr, struct file *file_ptr) {
    pr_info("usb_to_i2c: release called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode_ptr),
        iminor(inode_ptr)
    );

    kfree(file_ptr->private_data);

    return 0;

}

static ssize_t read_file(
    struct file *file_ptr,
    char __user *user_buffer,
    size_t length,
    loff_t *offset
) {
    pr_info("usb_to_i2c: read called\n");
    
    char *text = file_ptr -> private_data;

    int not_copied, delta, to_copy = (
        (length + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    );

    not_copied = copy_to_user(user_buffer, &text[*offset], to_copy);
    delta = to_copy - not_copied;

    *offset += delta;

    return delta;

}

static ssize_t write_file(
    struct file *file_ptr,
    const char __user *user_buffer,
    size_t length,
    loff_t *offset
) {
    pr_info("usb_to_i2c: write called\n");
    
    char *text = file_ptr -> private_data;

    int not_copied, delta, to_copy = (
        (length + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    );

    not_copied = copy_from_user(&text[*offset], user_buffer, to_copy);
    delta = to_copy - not_copied;

    *offset += delta;

    return delta;

}

static long int file_ioctl(
    struct file *file_ptr,
    unsigned int cmd,
    unsigned long args
) {
    pr_info("usb_to_i2c: ioctl called with cmd: %d and args %lu\n", cmd, args);

    switch (cmd) {

        default:
            return EOPNOTSUPP;

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

const struct file_operations *getFileOperations(void) {
    return &driver_fops; 

}