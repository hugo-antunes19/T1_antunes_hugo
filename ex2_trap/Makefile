CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
PI      = 3.14159265358979323846
TARGET  = mpi_trap_generalized

all: $(TARGET)

$(TARGET): mpi_trap_generalized.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== Verificação: p=3, n=10, sin(x) em [0, pi] (exato = 2) ==="
	mpiexec -n 3 ./$(TARGET) 0 $(PI) 10
	@echo ""
	@echo "=== Escalabilidade forte: n=10000000 ==="
	@for p in 1 2 4 8; do \
		mpiexec -n $$p ./$(TARGET) 0 $(PI) 10000000; \
	done
	@echo ""
	@echo "=== Convergência (bônus): p=4 ==="
	@for n in 64 256 1024 4096 16384; do \
		mpiexec -n 4 ./$(TARGET) 0 $(PI) $$n; \
	done

clean:
	rm -f $(TARGET)

.PHONY: all run clean
