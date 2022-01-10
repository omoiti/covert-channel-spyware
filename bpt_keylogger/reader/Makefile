CC=gcc
CFLAGS=-c -Wall
SPY_TARGETS=spy.c branch.s

all: spy

spy: $(SPY_TARGETS)
	$(CC) $^ -o $@


.PHONY: clean
clean:
	@echo "Cleaning..."
	-rm -f spy *.o
	@echo "Finished cleaning."
