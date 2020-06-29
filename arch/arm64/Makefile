all: core

core:
	make -C kernel

clean:
	rm boot/Image > /dev/null 2>&1 || true
	make -C kernel clean
