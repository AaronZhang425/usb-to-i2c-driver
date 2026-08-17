#include "driver_config.h"

#ifndef FILE_OPERATIONS_UTIL_H
#define FILE_OPERATIONS_UTIL_H

extern struct i2c_client *indexed_i2c_clients[];

const struct file_operations* get_file_operations(void);

#endif