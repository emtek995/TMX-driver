TARGET = tmx_driver

obj-m += $(TARGET).o

tmx_driver-objs += tmx.o hid_tmx.o ffb_tmx.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
