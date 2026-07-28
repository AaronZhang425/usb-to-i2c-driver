obj-m += usb_to_i2c_driver.o

KDIR ?= /lib/modules/$(shell uname -r)/build

usb_to_i2c_driver-y := src/usb_to_i2c_driver.o

ccflags-y := -I include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
