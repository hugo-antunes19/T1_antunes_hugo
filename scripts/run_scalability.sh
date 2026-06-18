#!/bin/bash
# run_scalability.sh — Executa o experimento de escalabilidade forte
# com múltiplas repetições e reporta a mediana.

set -e

PI="3.14159265358979323846"
N=10000000
REPS=11  # número ímpar para mediana exata
PROG="./mpi_trap_generalized"
SRC_DIR="/mnt/d/Arquivos/Desktop/cad/T1_antunes_hugo/ex2_trap"
OUT_DIR="/mnt/d/Arquivos/Desktop/cad/T1_antunes_hugo/results"

cd "$SRC_DIR"

echo "=== Compilando ==="
mpicc -O2 -Wall -o mpi_trap_generalized mpi_trap_generalized.c -lm
echo "OK"
echo ""

# Arquivo de saída com todos os dados brutos
RAW_FILE="$OUT_DIR/ex2b_scalability_raw.csv"
echo "p,rep,n,Tp" > "$RAW_FILE"

MEDIAN_FILE="$OUT_DIR/ex2b_scalability_median.csv"
echo "p,n,Tp_median" > "$MEDIAN_FILE"

for p in 1 2 4 8; do
    echo "=== p=$p ($REPS repetições) ==="
    TIMES=()
    for r in $(seq 1 $REPS); do
        # Extrair Tp da saída do programa
        OUTPUT=$(mpiexec --oversubscribe -n $p $PROG 0 $PI $N 2>/dev/null)
        TP=$(echo "$OUTPUT" | head -1 | grep -oP 'Tp=\K[0-9.]+')
        TIMES+=("$TP")
        echo "$p,$r,$N,$TP" >> "$RAW_FILE"
        printf "  rep %2d: Tp = %s s\n" "$r" "$TP"
    done

    # Ordenar e pegar mediana
    SORTED=($(printf '%s\n' "${TIMES[@]}" | sort -g))
    MID=$(( (REPS - 1) / 2 ))
    MEDIAN="${SORTED[$MID]}"
    echo "  >> Mediana: $MEDIAN s"
    echo "$p,$N,$MEDIAN" >> "$MEDIAN_FILE"
    echo ""
done

echo "=== Resultados salvos ==="
echo "  Raw:    $RAW_FILE"
echo "  Median: $MEDIAN_FILE"
echo ""
cat "$MEDIAN_FILE"
