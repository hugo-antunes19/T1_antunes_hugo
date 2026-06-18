#!/usr/bin/env python3
"""
Gera gráficos de escalabilidade forte (speedup e eficiência)
para o Exercício 2(b) com n = 10^7 — dados de mediana (11 reps).
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

# Medianas coletadas (11 repetições cada)
p  = np.array([1, 2, 4, 8])
Tp = np.array([0.065372, 0.034137, 0.017820, 0.011797])

T1 = Tp[0]
Sp = T1 / Tp
Ep = Sp / p

output_dir = r"d:\Arquivos\Desktop\cad\T1_antunes_hugo\results"

plt.rcParams.update({
    'font.size': 12,
    'axes.labelsize': 13,
    'axes.titlesize': 14,
    'legend.fontsize': 11,
    'figure.figsize': (7, 5),
    'figure.dpi': 150,
})

# ── Gráfico 1: Speedup ──
fig, ax = plt.subplots()
ax.plot(p, Sp, 'o-', color='#2563eb', linewidth=2, markersize=8, label=r'$S_p$ medido (mediana)')
ax.plot(p, p,  '--', color='#9ca3af', linewidth=1.5, label='Speedup ideal')
ax.set_xlabel('Número de processos ($p$)')
ax.set_ylabel(r'Speedup $S_p = T_1 / T_p$')
ax.set_title(r'Escalabilidade forte — $n = 10^7$, $f(x) = \sin(x)$')
ax.set_xticks(p)
ax.set_xlim(0.5, 8.5)
ax.set_ylim(0, 9)
ax.legend(loc='upper left')
ax.grid(True, alpha=0.3)
for i in range(len(p)):
    ax.annotate(f'{Sp[i]:.2f}', (p[i], Sp[i]),
                textcoords='offset points', xytext=(10, -5), fontsize=10)
fig.tight_layout()
fig.savefig(f'{output_dir}/scalability_speedup.png', bbox_inches='tight')
print(f"Salvo: scalability_speedup.png")
plt.close()

# ── Gráfico 2: Eficiência ──
fig, ax = plt.subplots()
ax.plot(p, Ep, 's-', color='#dc2626', linewidth=2, markersize=8, label=r'$E_p$ medido (mediana)')
ax.axhline(y=0.8, color='#f59e0b', linestyle='--', linewidth=1.5, label=r'$E_p = 0.8$')
ax.axhline(y=1.0, color='#9ca3af', linestyle=':', linewidth=1, alpha=0.5)
ax.set_xlabel('Número de processos ($p$)')
ax.set_ylabel(r'Eficiência $E_p = S_p / p$')
ax.set_title(r'Eficiência paralela — $n = 10^7$ (mediana de 11 execuções)')
ax.set_xticks(p)
ax.set_xlim(0.5, 8.5)
ax.set_ylim(0, 1.15)
ax.legend(loc='lower left')
ax.grid(True, alpha=0.3)
for i in range(len(p)):
    ax.annotate(f'{Ep[i]:.4f}', (p[i], Ep[i]),
                textcoords='offset points', xytext=(10, 8), fontsize=10)
ax.fill_between([0.5, 8.5], 0, 0.8, alpha=0.05, color='red')
fig.tight_layout()
fig.savefig(f'{output_dir}/scalability_efficiency.png', bbox_inches='tight')
print(f"Salvo: scalability_efficiency.png")
plt.close()

# ── Gráfico 3: Tempo ──
fig, ax = plt.subplots()
ax.plot(p, Tp * 1000, 'D-', color='#059669', linewidth=2, markersize=8, label=r'$T_p$ (mediana)')
ax.set_xlabel('Número de processos ($p$)')
ax.set_ylabel(r'Tempo $T_p$ (ms)')
ax.set_title(r'Tempo de execução — $n = 10^7$ (mediana de 11 execuções)')
ax.set_xticks(p)
ax.set_xlim(0.5, 8.5)
ax.legend()
ax.grid(True, alpha=0.3)
for i in range(len(p)):
    ax.annotate(f'{Tp[i]*1000:.1f} ms', (p[i], Tp[i]*1000),
                textcoords='offset points', xytext=(10, 5), fontsize=10)
fig.tight_layout()
fig.savefig(f'{output_dir}/scalability_time.png', bbox_inches='tight')
print(f"Salvo: scalability_time.png")
plt.close()

print("\nDados (mediana de 11 execuções):")
print(f"{'p':>3}  {'Tp (s)':>10}  {'Sp':>8}  {'Ep':>8}")
for i in range(len(p)):
    print(f"{p[i]:>3}  {Tp[i]:>10.6f}  {Sp[i]:>8.4f}  {Ep[i]:>8.4f}")
