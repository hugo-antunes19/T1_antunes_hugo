CC      = mpicc
CFLAGS  = -O2 -Wall
TARGETS = student_struct student_three_bcasts

all: $(TARGETS)

student_struct: student_struct.c
	$(CC) $(CFLAGS) -o $@ $<

student_three_bcasts: student_three_bcasts.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "=== Versão com tipo derivado (1 Bcast) ==="
	mpiexec -n 4 ./student_struct
	@echo ""
	@echo "=== Versão com 3 Bcasts separados ==="
	mpiexec -n 4 ./student_three_bcasts

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
