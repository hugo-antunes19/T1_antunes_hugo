#!/usr/bin/env python3
"""plot_convergence.py — Gráfico log-log de convergência O(h²) (Ex. 2c).

Lê resultados de ex2c_convergence.csv e gera um gráfico log-log
de |erro| vs h, com regressão linear para medir a inclinação
(deve ser ≈ 2 para a regra do trapézio).

Uso: python3 plot_convergence.py [caminho_csv]
  Default: ../results/ex2c_convergence.csv
"""

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else \
               os.path.join(os.path.dirname(__file__), '..', 'results', 'ex2c_convergence.csv')

    # Ler CSV: n,h,error
    data = np.genfromtxt(csv_path, delimiter=',', skip_header=1, dtype=None, encoding='utf-8')
    n_vals = np.array([row[0] for row in data], dtype=int)
    h_vals = np.array([row[1] for row in data], dtype=float)
    err_vals = np.array([row[2] for row in data], dtype=float)

    # Filtrar pontos com erro zero (ou muito pequeno)
    mask = err_vals > 1e-20
    h_plot = h_vals[mask]
    err_plot = err_vals[mask]
    n_plot = n_vals[mask]

    # Regressão linear em log-log para medir inclinação
    log_h = np.log10(h_plot)
    log_err = np.log10(err_plot)
    coeffs = np.polyfit(log_h, log_err, 1)
    slope = coeffs[0]
    intercept = coeffs[1]

    out_dir = os.path.join(os.path.dirname(csv_path))
    os.makedirs(out_dir, exist_ok=True)

    # Estilo global
    plt.rcParams.update({
        'font.size': 12,
        'font.family': 'serif',
        'figure.figsize': (7, 5),
        'axes.grid': True,
        'grid.alpha': 0.3,
    })

    fig, ax = plt.subplots()

    # Dados medidos
    ax.loglog(h_plot, err_plot, 'o', color='#2196F3', markersize=10,
              label=r'$|I_{aprox} - 2|$ medido', zorder=3)

    # Linha de regressão
    h_line = np.logspace(np.log10(h_plot.min()) - 0.2, np.log10(h_plot.max()) + 0.2, 100)
    err_line = 10**intercept * h_line**slope
    ax.loglog(h_line, err_line, '-', color='#FF5722', linewidth=2,
              label=f'Regressão: inclinação = {slope:.2f}')

    # Referência O(h²)
    ref_err = err_plot[-1] * (h_line / h_plot[-1])**2
    ax.loglog(h_line, ref_err, '--', color='gray', linewidth=1.5,
              label=r'Referência $O(h^2)$', alpha=0.7)

    # Anotar cada ponto com n
    for i in range(len(h_plot)):
        ax.annotate(f'n={n_plot[i]}', (h_plot[i], err_plot[i]),
                    textcoords="offset points", xytext=(10, 5),
                    fontsize=9, color='#333')

    ax.set_xlabel(r'Tamanho do passo $h$')
    ax.set_ylabel(r'Erro absoluto $|I_{aprox} - 2|$')
    ax.set_title(r'Convergência da Regra do Trapézio — $\int_0^\pi \sin(x)\,dx$')
    ax.legend(loc='upper left')

    fig.tight_layout()
    fig.savefig(os.path.join(out_dir, 'convergence_loglog.png'), dpi=150)
    print(f"Salvo: {os.path.join(out_dir, 'convergence_loglog.png')}")
    plt.close()

    # --- Imprimir tabela ---
    print("\n=== Tabela de Convergência ===")
    print(f"{'n':>6}  {'h':>14}  {'|erro|':>14}")
    print("-" * 40)
    for i in range(len(n_vals)):
        print(f"{n_vals[i]:>6}  {h_vals[i]:>14.6e}  {err_vals[i]:>14.6e}")
    print(f"\nInclinação medida: {slope:.4f} (esperado ≈ 2.0)")


if __name__ == '__main__':
    main()
