#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include "test_header.h"

// static __init init();
// static __exit exit();

static int __init module_init_func(void) {
    test_func();

    return 0;
}
    
static void __exit module_end_func(void) {
    
}
    
// Temporary. Change to Doul MIT/GPL later
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A very simple description for testing");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("0.1");

module_init(module_init_func);
module_exit(module_end_func);