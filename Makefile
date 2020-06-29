export
CROSS_PREFIX = aarch64-linux-gnu
AS = ${CROSS_PREFIX}-as
CC = ${CROSS_PREFIX}-gcc
LD = ${CROSS_PREFIX}-ld
NM = ${CROSS_PREFIX}-nm
OBJCOPY = ${CROSS_PREFIX}-objcopy
BASE_DIR = $(shell pwd)
INCLUDE = -I ${BASE_DIR}/include -I ${BASE_DIR}/arch/arm64/include

CF = -Wall -nostdlib -fno-builtin -g -nostartfiles -ffreestanding -mgeneral-regs-only -fPIC
CFLAGS = ${CFLAG} # CFLAG = "-DPREEMPT"
CFLAGS += ${CF} ${INCLUDE}

QEMU_ARGS = -M raspi3 -nographic -serial null -serial mon:stdio

all: wow

wow: 
	make -C arch/arm64
	make -C init

	${LD} -T arch/arm64/kernel/vmlinux.lds -nostdlib -nostartfiles init/*.o arch/arm64/kernel/*.o -o vmlinux
	${OBJCOPY} -O binary vmlinux arch/arm64/boot/Image
	${NM} vmlinux > System.map

clean:
	rm vmlinux System.map > /dev/null 2>&1 || true
	make -C arch/arm64 clean
	make -C init clean

run:
	qemu-system-aarch64 ${QEMU_ARGS} -kernel arch/arm64/boot/Image

debug:
	qemu-system-aarch64 ${QEMU_ARGS} -kernel arch/arm64/boot/Image -S -gdb tcp::6666

