CC      = mpicc
CFLAGS  = -O2 -Wall
TARGETS = mpi_vecadd_gather mpi_vecadd_allgather

all: $(TARGETS)

mpi_vecadd_gather: mpi_vecadd_gather.c
	$(CC) $(CFLAGS) -o $@ $<

mpi_vecadd_allgather: mpi_vecadd_allgather.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "=== vecadd com Gather (4 procs, N=16) ==="
	mpiexec -n 4 ./mpi_vecadd_gather 16
	@echo ""
	@echo "=== vecadd com Allgather (4 procs, N=16) ==="
	mpiexec -n 4 ./mpi_vecadd_allgather 16

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
