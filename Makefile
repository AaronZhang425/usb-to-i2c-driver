obj-m += usb_to_i2c_driver.o

KDIR ?= /lib/modules/$(shell uname -r)/build

usb_to_i2c_driver-y := $(src)/src/main.o

ccflags-y := -I$(src)/include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(src) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
