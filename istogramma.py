import matplotlib.pyplot as plt
import numpy as np

# Chiede il nome del file senza estensione
nome_base = input(
    "Inserisci il nome del file di cui vuoi stampare l'istogramma (senza .dat): "
).strip()

if not nome_base.endswith(".dat"):
    nome_file = nome_base + ".dat"
else:
    nome_file = nome_base

# Legge la prima riga e salva i 7 valori nelle rispettive variabili
with open(nome_file, "r") as f:
    prima_riga = f.readline().split()

Nsim = int(prima_riga[0])
A = int(prima_riga[1])
B = int(prima_riga[2])
pA_mc = float(prima_riga[3])
sig_pA_mc = float(prima_riga[4])
n_scontri_media = float(prima_riga[5])
sig_n_scontri_media = float(prima_riga[6])

# Carica i dati dalla seconda riga in poi
dati = np.loadtxt(nome_file, skiprows=1)

# Creazione del grafico
plt.figure(figsize=(10, 6))

# Genera i bordi dei bin dal minimo al massimo dei dati con passo fissato
larghezza_bin = 10
bins = np.arange(np.min(dati), np.max(dati) + larghezza_bin, larghezza_bin)

plt.hist(
    dati,
    bins = bins,
    # bins= "auto",     
    density=True,
    color="steelblue",
    edgecolor="steelblue",
    alpha=0.8,
)

# Titolo 
plt.title(f"Distribuzione normalizzata:  $N_{{sim}}=10^{int(np.log10(Nsim))}$;  $A={A}$;  $B={B}$")
plt.xlabel("$S_{n_s}$")
plt.ylabel("conteggi normalizzati")
plt.grid(axis="y", linestyle="--", alpha=0.6)

# #############################    Blocco per la gaussiana teorica    ####################################
mu = 0.0
sigma = 2.7467 * np.sqrt(B)

# Crea una griglia fitta di punti lungo l'asse x dell'istogramma
x = np.linspace(np.min(dati), np.max(dati), 1000)

# Densità di probabilità gaussiana (PDF)
gaussiana = (1.0 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x - mu) / sigma) ** 2)

# Traccia la curva tratteggiata e aggiunge la legenda
plt.plot(
    x,
    gaussiana,
    color="crimson",
    linestyle="--",
    linewidth=2,
    #label=rf"Gaussiana ($\mu=0$, $\sigma_{{eff}}={sigma:.2f}$)",
)
#plt.legend(fontsize=11)
# ########################################################################################################

plt.show()