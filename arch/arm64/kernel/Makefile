
C_FILES = $(wildcard ./*.c)
ASM_FILES = $(wildcard ./*.S)
OBJ_FILES = $(C_FILES:./%.c=./%_c.o)
OBJ_FILES += $(ASM_FILES:./%.S=./%_s.o)

all: ${OBJ_FILES}

%_c.o: %.c
	${CC} -c ${CFLAGS} -MMD $< -o $@

%_s.o: %.S
	${CC} -c ${CFLAGS} -MMD $< -o $@

clean:
	rm *.o > /dev/null 2>&1 || true
	rm *.d > /dev/null 2>&1 || true
