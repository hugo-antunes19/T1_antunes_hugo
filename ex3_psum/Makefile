CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
TARGET  = mpi_psum

all: $(TARGET)

$(TARGET): mpi_psum.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== Soma paralela (p=4, N=10000000) ==="
	mpiexec -n 4 ./$(TARGET) 10000000

clean:
	rm -f $(TARGET)

.PHONY: all run clean
