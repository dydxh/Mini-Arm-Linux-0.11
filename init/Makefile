
all: initial

clean:
	rm *.o

C_FILES = $(wildcard ./*.c)
OBJ_FILES = $(C_FILES:./%.c=./%_c.o)

initial: $(OBJ_FILES)

%_c.o: %.c
	${CC} -c ${CFLAGS} -MMD $< -o $@

clean:
	rm *.o > /dev/null 2>&1 || true
	rm *.d > /dev/null 2>&1 || true
