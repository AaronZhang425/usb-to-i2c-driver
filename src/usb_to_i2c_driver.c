#include <linux/module.h>
#include <linux/init.h>

#include "test_header.h"

static int __init init() {
    test_func();

    return 0;
}
    
static void __exit exit() {
    
}
    
// Temporary. Change to MIT later
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A very simple description for testing");
MODULE_AUTHOR("Aaron Zhang");
MODULE_VERSION("1.0");

module_init(init);
module_exit(exit);