#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "driver_file_operations_util.h"

#define MEMSIZE 64

static int open_file(struct inode* inode, struct file* file) {
    pr_info("usb_to_i2c: open called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode),
        iminor(inode)
    );

    file -> private_data = kmalloc(MEMSIZE, GFP_KERNEL);

    if (!file -> private_data) {
        pr_err("usb_to_i2c: Out of memory\n");
        return ENOMEM;

    }

    return 0;
    
}

static int release_file(struct inode* inode, struct file* file) {
    pr_info("usb_to_i2c: release called");
    pr_info(
        "usb_to_i2c:\nMajor: %d\nMinor: %d\n",
        imajor(inode),
        iminor(inode)
    );

    kfree(file -> private_data);

    return 0;

}

static ssize_t read_file(
    struct file* file,
    char* __user user_buffer,
    size_t length,
    loff_t* offset
) {
    pr_info("usb_to_i2c: read called\n");
    
    char* text = file -> private_data;

    int not_copied, delta, to_copy = (
        (length + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    );

    not_copied = copy_to_user(user_buffer, &text[*offset], to_copy);
    delta = to_copy - not_copied;

    *offset += delta;

    return delta;

}

static ssize_t write_file(
    struct file* file,
    const char* __user user_buffer,
    size_t length,
    loff_t* offset
) {
    pr_info("usb_to_i2c: write called\n");
    
    char *text = file -> private_data;

    int not_copied, delta, not_copied = (
        (len + *offset) < MEMSIZE ? length : (MEMSIZE - *offset)
    );

    not_copied = copy_from_user(&text[*offset], user_buffer, to_copy)
    delta = to_copy - not_copied;

    *offset += delta;

    return delta;

}

static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .open = open_file,
    .release = release_file,
    .read = read_file,
    .write = write_file
};

const struct file_operations* getFileOperations(void) {
    return &driver_fops; 

}
