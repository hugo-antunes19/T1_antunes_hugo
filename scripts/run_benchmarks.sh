#!/bin/bash
# run_benchmarks.sh — Executa todos os benchmarks e salva resultados.
#
# Uso: bash run_benchmarks.sh
# Pré-requisitos: todos os exercícios devem estar compilados (make em cada diretório).

set -e

BASEDIR="$(cd "$(dirname "$0")/.." && pwd)"
RESULTS="$BASEDIR/results"
mkdir -p "$RESULTS"

PI="3.14159265358979323846"

echo "=================================================="
echo " Trabalho 1 — MPI Benchmarks"
echo " Resultados em: $RESULTS/"
echo "=================================================="

# -------------------------------------------------------------------
# Ex. 1a — Introspecção de SO (20 execuções)
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 1a: hello_os (20 execuções com p=8) ---"
cd "$BASEDIR/ex1_hello_os"
make -s
for run in {1..20}; do
    echo ">> Execução $run:"
    mpiexec -n 8 --oversubscribe ./hello_os
    echo ""
done 2>&1 | tee "$RESULTS/ex1a_output.txt"

# -------------------------------------------------------------------
# Ex. 1b — Hello em anel
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 1b: hello_ring (p=4) ---"
mpiexec -n 4 --oversubscribe ./hello_ring 2>&1 | tee "$RESULTS/ex1b_output.txt"

# -------------------------------------------------------------------
# Ex. 2a — Verificação p=3, n=10
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 2a: Trapézio generalizado (p=3, n=10) ---"
cd "$BASEDIR/ex2_trap"
make -s
mpiexec -n 3 --oversubscribe ./mpi_trap_generalized 0 $PI 10 2>&1 | tee "$RESULTS/ex2a_output.txt"

# -------------------------------------------------------------------
# Ex. 2b — Escalabilidade forte
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 2b: Escalabilidade forte (n=10000000) ---"
echo "p,n,integral,Tp" > "$RESULTS/ex2b_scalability.csv"
for p in 1 2 4 8; do
    output=$(mpiexec -n $p --oversubscribe ./mpi_trap_generalized 0 $PI 10000000)
    echo "$output"
    # Parse: RESULT: p=X n=Y integral=Z Tp=W
    tp=$(echo "$output" | head -1 | sed 's/.*Tp=//')
    integral=$(echo "$output" | head -1 | sed 's/.*integral=//;s/ Tp=.*//')
    echo "$p,10000000,$integral,$tp" >> "$RESULTS/ex2b_scalability.csv"
done 2>&1 | tee -a "$RESULTS/ex2b_output.txt"

# -------------------------------------------------------------------
# Ex. 2c — Convergência
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 2c: Convergência O(h²) (p=4) ---"
echo "n,h,error" > "$RESULTS/ex2c_convergence.csv"
for n in 64 256 1024 4096 16384; do
    output=$(mpiexec -n 4 --oversubscribe ./mpi_trap_generalized 0 $PI $n)
    echo "$output"
    # Parse error and h
    h=$(echo "$output" | grep "h =" | sed 's/.*h = //')
    error=$(echo "$output" | grep "Error =" | sed 's/.*Error = //;s/,.*//')
    echo "$n,$h,$error" >> "$RESULTS/ex2c_convergence.csv"
done 2>&1 | tee -a "$RESULTS/ex2c_output.txt"

# -------------------------------------------------------------------
# Ex. 3 — Soma paralela
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 3: Soma paralela (p=4, N=10000000) ---"
cd "$BASEDIR/ex3_psum"
make -s
mpiexec -n 4 --oversubscribe ./mpi_psum 10000000 2>&1 | tee "$RESULTS/ex3_output.txt"

# -------------------------------------------------------------------
# Ex. 4a — Hello com Gather
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 4a: hello_gather (p=4) ---"
cd "$BASEDIR/ex4_coletivas"
make -s
mpiexec -n 4 --oversubscribe ./hello_gather 2>&1 | tee "$RESULTS/ex4a_output.txt"

# -------------------------------------------------------------------
# Ex. 4b — MinMax
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 4b: minmax (p=4, N=1000000) ---"
mpiexec -n 4 --oversubscribe ./minmax 1000000 2>&1 | tee "$RESULTS/ex4b_output.txt"

# -------------------------------------------------------------------
# Ex. 5 — Vecadd Gather + Allgather
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 5: vecadd_gather (p=4, N=16) ---"
cd "$BASEDIR/ex5_vecadd"
make -s
mpiexec -n 4 --oversubscribe ./mpi_vecadd_gather 16 2>&1 | tee "$RESULTS/ex5_gather_output.txt"

echo ""
echo "--- Ex. 5: vecadd_allgather (p=4, N=16) ---"
mpiexec -n 4 --oversubscribe ./mpi_vecadd_allgather 16 2>&1 | tee "$RESULTS/ex5_allgather_output.txt"

# -------------------------------------------------------------------
# Ex. 6 — Student struct vs three Bcasts
# -------------------------------------------------------------------
echo ""
echo "--- Ex. 6: student_struct (p=4) ---"
cd "$BASEDIR/ex6_derived"
make -s
mpiexec -n 4 --oversubscribe ./student_struct 2>&1 | tee "$RESULTS/ex6_struct_output.txt"

echo ""
echo "--- Ex. 6: student_three_bcasts (p=4) ---"
mpiexec -n 4 --oversubscribe ./student_three_bcasts 2>&1 | tee "$RESULTS/ex6_three_bcasts_output.txt"

echo ""
echo "=================================================="
echo " Benchmarks completos! Resultados em $RESULTS/"
echo "=================================================="
