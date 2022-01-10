CC=gcc
CFLAGS=-c
LDFLAGS=-pthread
KEYLOG_TARGETS=main.o keylogger.o find_event_file.o br_taken.s br_nottaken.s

all: keylog

keylog: $(KEYLOG_TARGETS)
	$(CC) $^ -o $@ $(LDFLAGS)



.PHONY: clean
clean:
	@echo "Cleaning..."
	-rm -f keylog *.o
	@echo "Finished cleaning."
