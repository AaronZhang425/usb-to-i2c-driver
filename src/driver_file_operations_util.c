#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "driver_file_operations_util.h"

static int open_file(struct inode* inode, struct file* file) {
    
    return 0;
}

static int release_file(struct inode* inode, struct file* file) {
    
    return 0;
}

static ssize_t read_file(
    struct file* file,
    char __user* buf,
    size_t len,
    loff_t* off
) {
    
    return 0;
}

static ssize_t write_file(
    struct file* file,
    const char __user* buf,
    size_t len,
    loff_t *off
) {
    
    return 0;
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