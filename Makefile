obj-m += usb_to_i2c_driver.o

usb_to_i2c_driver-y := src/main.o src/file_operations_util.o src/hardware_handler.o

ccflags-y += -I$(PWD)/include -Wextra

DEVICE_TREE := detect_hw

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	dtc -@ -I dts -O dtb -o $(PWD)/$(DEVICE_TREE).dtbo $(PWD)/dts/$(DEVICE_TREE).dts

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(PWD)/$(DEVICE_TREE).dtbo
