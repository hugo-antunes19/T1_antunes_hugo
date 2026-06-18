CC      = mpicc
CFLAGS  = -O2 -Wall
TARGETS = hello_os hello_ring

all: $(TARGETS)

hello_os: hello_os.c
	$(CC) $(CFLAGS) -o $@ $<

hello_ring: hello_ring.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "=== hello_os (8 procs) ==="
	mpiexec -n 8 ./hello_os
	@echo ""
	@echo "=== hello_ring (4 procs) ==="
	mpiexec -n 4 ./hello_ring

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
