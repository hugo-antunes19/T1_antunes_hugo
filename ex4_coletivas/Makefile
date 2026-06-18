CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
TARGETS = hello_gather minmax

all: $(TARGETS)

hello_gather: hello_gather.c
	$(CC) $(CFLAGS) -o $@ $<

minmax: minmax.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== hello_gather (4 procs) ==="
	mpiexec -n 4 ./hello_gather
	@echo ""
	@echo "=== minmax (4 procs, N=1000000) ==="
	mpiexec -n 4 ./minmax 1000000

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
