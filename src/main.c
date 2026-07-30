#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c-dev.h>

#include "test_header.h"

// Temporary. Change to Doul MIT/GPL later
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("A very simple description for testing");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");

static int __init module_init_func(void) {
    // Testing if Makefile can find
    int sum = add(5, 6);
    printk("Test sum: %d\n", sum);

    return 0;
}
    
static void __exit module_end_func(void) {
    
}


module_init(module_init_func);
module_exit(module_end_func);