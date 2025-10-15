CC=gcc
CFLAGS=-I.
TARGETS=sequential_min_max parallel_min_max launcher

all: $(TARGETS)

sequential_min_max: utils.o find_min_max.o sequential_min_max.c
	$(CC) -o $@ sequential_min_max.c utils.o find_min_max.o $(CFLAGS)

parallel_min_max: utils.o find_min_max.o parallel_min_max.c
	$(CC) -o $@ parallel_min_max.c utils.o find_min_max.o $(CFLAGS)

# Новый target для launcher
launcher: launcher.c
	$(CC) -o $@ launcher.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -o $@ -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -o $@ -c find_min_max.c $(CFLAGS)

clean:
	rm -f $(TARGETS) *.o min_max_*.txt

help:
	@echo "Available targets:"
	@echo "  all       - Build all programs (default)"
	@echo "  sequential_min_max - Build sequential version"
	@echo "  parallel_min_max   - Build parallel version" 
	@echo "  launcher  - Build program launcher"
	@echo "  clean     - Remove all compiled files"
	@echo "  help      - Show this help message"

.PHONY: all clean help