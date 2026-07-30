obj-m += usb_to_i2c_driver.o

usb_to_i2c_driver-y := src/main.o src/test_header.o

ccflags-y := -I include -Wextra

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
