obj-m += usb_to_i2c_driver.o

usb_to_i2c_driver-y := src/main.o src/driver_file_operations_util.o

ccflags-y += -I$(PWD)/include -Wextra

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
