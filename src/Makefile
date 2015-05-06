MAKE := make
obj-m := myproc.o
KERNELDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

All:  proc sampleapp

proc : myproc.c
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

sampleapp : sampleapp.c batteryStatus.c powerManager.c
	gcc -o sampleapp sampleapp.c
	gcc -o batteryStatus batteryStatus.c
	gcc -o powerManager powerManager.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

