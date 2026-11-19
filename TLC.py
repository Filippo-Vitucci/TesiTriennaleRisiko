import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import math
import pandas as pd

df_esatta = pd.read_csv("probEsatta.dat", sep=r'\s+', header=None, skiprows=1)
df_esatta.columns = ['A', 'B', 'p', 'ns']
# elimino elementi con A>100 o B>100:
df_esatta = df_esatta[(df_esatta['A'] <= 100) & (df_esatta['B'] <= 100)]

p_esatta = df_esatta['p'].to_numpy()
ns_esatta = df_esatta['ns'].to_numpy()

diz_file = {
    "cento_100vs100.dat": ["blue", "$10^2$ SIMULAZIONI", "10_2"],
    "mille_100vs100.dat": ["red", "$10^3$ SIMULAZIONI", "10_3"],
    "diecimila_100vs100.dat": ["green", "$10^4$ SIMULAZIONI", "10_4"],
    "centomila_100vs100.dat": ["purple", "$10^5$ SIMULAZIONI", "10_5"],
    "unmilione_100vs100.dat": ["orange", "$10^6$ SIMULAZIONI", "10_6"],
    "diecimilioni_100vs100.dat": ["cyan", "$10^7$ SIMULAZIONI", "10_7"]
}

for nome_file in diz_file.keys():

    # salvo prima riga a parte ( Nsimulazioni A B )
    with open(nome_file, 'r', encoding='utf-8') as f:
        prima_riga = f.readline().strip()
    # .strip() rimuove spazi o a capo superflui a inizio e fine stringa
    # dividiamo la riga usando gli spazi interni come separatore (.split())
    elementi = prima_riga.split()
    N = int(elementi[0])
    A = int(elementi[1])
    B = int(elementi[2])

    # carico resto del file con Pandas, saltando la prima riga (skiprows=1)
    df = pd.read_csv(nome_file, sep=r'\s+', header=None, skiprows=1)

    # assegno nomi alle colonne dei dati rimanenti
    df.columns = ['A', 'B', 'p', 'sig_p', 'ns', 'sig_ns']

    # calcolo chi2 ridotto per probabilita'
    p_mc = df['p'].to_numpy()
    sig_p_mc = df['sig_p'].to_numpy()
    # Crea una maschera booleana per selezionare solo i punti con sigma > 0
    mask = sig_p_mc > 0
    # Calcola il chi quadro ridotto solo sui punti validi (evita divisioni per 0)
    chi2_p = np.sum(((p_esatta[mask] - p_mc[mask]) / sig_p_mc[mask]) ** 2) / np.sum(mask)

    print(f"\nNsim = {N}\nchi2_p = {chi2_p}")

    # calcolo chi2 ridotto per numero scontri
    ns_mc = df['ns'].to_numpy()
    sig_ns_mc = df['sig_ns'].to_numpy()
    # Crea una maschera booleana per selezionare solo i punti con sigma > 0
    mask = sig_ns_mc > 0
    # Calcola il chi quadro ridotto solo sui punti validi (evita divisioni per 0)
    chi2_ns = np.sum(((ns_esatta[mask] - ns_mc[mask]) / sig_ns_mc[mask]) ** 2) / np.sum(mask)

    print(f"chi2_ns = {chi2_ns}")

    # considero solo curve di probabilita' per fare plot 2D
    p_curva = 0.5
    idx = (df['p'] - p_curva).abs().groupby(df['A']).idxmin()
    # .groupby(df['A']).idxmin(): Raggruppa i dati per ciascun valore di A e
    # restituisce l'indice (idxmin) della riga con p piu' vicina a p_curva al variare di B
    df = df.loc[idx]
    # estrae dal DataFrame originale solo le righe corrispondenti a quegli indici

    ############################################################################################

    # inizio grafico probabilita'
    x = df['A']          # Valori di A sull'asse X
    y = df['p']          # Valori di p sull'asse Y
    y_err = df['sig_p']     # Barre di errore

    # plot della prob esatta
    # plotto le prob per le stesse coppie (A,B) di sopra
    df_esatta_filtrato = pd.merge(df_esatta, df[['A', 'B']], on=['A', 'B'], how='inner')
    x_esatta = df_esatta_filtrato["A"]
    y_esatta = df_esatta_filtrato["p"]

    plt.figure(figsize=(9, 6))

    # grafico NON riscalato
    """
    plt.errorbar(
        x, y, 
        yerr=y_err, 
        fmt='.',
        capsize=4,        # Piccole stanghette alle estremita' delle barre d'errore
        color=diz_file[nome_file][0],     # colore punti e colore errorbar
        ecolor=diz_file[nome_file][0]    
    )

    plt.plot(
        x_esatta, y_esatta, 
        linestyle='None',          # Linea continua ('--' per tratteggiata, ':' per puntinata)
        #linewidth=2,            # Spessore della linea
        marker='o',             # (Opzionale) Aggiunge un cerchietto in corrispondenza di ogni punto
        markersize=5,           # Dimensione dei cerchietti
        color='black',           # Colore della linea
        #label='Etichetta'       # Etichetta per la legenda
    )
    """

    # per grafico riscalato

    # converto in numpy array
    x_esatta = x_esatta.to_numpy()
    y_esatta = y_esatta.to_numpy()
    y = y.to_numpy()
    # riscalo
    y_riscalato = y_esatta-y

    plt.errorbar(
        x_esatta, y_riscalato, 
        yerr=y_err, 
        fmt='.',
        capsize=4,        # Piccole stanghette alle estremita' delle barre d'errore
        color=diz_file[nome_file][0],     # colore punti e colore errorbar
        ecolor=diz_file[nome_file][0]    
    )

    plt.hlines(y=0, xmin=x_esatta[0], xmax=x_esatta[-1], colors='black', linestyles='--')

    # Personalizzazione del grafico
    plt.xlabel('$A$')
    plt.ylabel('$f\, (A,B)-P(A,B)$')
    plt.ylim(-0.2 , 0.2)
    plt.title(diz_file[nome_file][1])
    plt.grid(True, linestyle='--', alpha=0.6)
    #plt.savefig("immaginiTesi/p_riscalato_" + diz_file[nome_file][2] + ".png")
    plt.show()

    ############################################################################################

    # inizio grafico numero_scontri
    x = df['A']          # Valori di A sull'asse X
    y = df['ns']          # Valori di p sull'asse Y
    y_err = df['sig_ns']     # Barre di errore

    # plotto per le stesse coppie (A,B) di sopra
    df_esatta_filtrato = pd.merge(df_esatta, df[['A', 'B']], on=['A', 'B'], how='inner')
    x_esatta = df_esatta_filtrato["A"]
    y_esatta = df_esatta_filtrato["ns"]

    plt.figure(figsize=(9, 6))

    # per grafico NON riscalato
    """
    plt.errorbar(
        x, y, 
        yerr=y_err, 
        fmt='.',
        capsize=4,        # Piccole stanghette alle estremita' delle barre d'errore
        color=diz_file[nome_file][0],     # colore punti e colore errorbar
        ecolor=diz_file[nome_file][0]    
    )

    plt.plot(
        x_esatta, y_esatta,
        linestyle='None',          # Linea continua ('--' per tratteggiata, ':' per puntinata)
        #linewidth=2,              # Spessore della linea
        marker='o',                # (Opzionale) Aggiunge un cerchietto in corrispondenza di ogni punto
        markersize=5,              # Dimensione dei cerchietti
        color='black',             # Colore della linea
        #label='Etichetta'         # Etichetta per la legenda
    )

    # Personalizzazione del grafico
    plt.xlabel('$A$')
    plt.ylabel('$n_s(A,B)$')
    plt.title(diz_file[nome_file][1])
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.show()
    """

    # per grafico riscalato

    # converto in numpy array
    x_esatta = x_esatta.to_numpy()
    y_esatta = y_esatta.to_numpy()
    y = y.to_numpy()
    # riscalo
    y_riscalato = y_esatta-y

    plt.errorbar(
        x_esatta, y_riscalato, 
        yerr=y_err, 
        fmt='.',
        capsize=4,        # Piccole stanghette alle estremita' delle barre d'errore
        color=diz_file[nome_file][0],     # colore punti e colore errorbar
        ecolor=diz_file[nome_file][0]    
    )

    plt.hlines(y=0, xmin=x_esatta[0], xmax=x_esatta[-1], colors='black', linestyles='--')

    # Personalizzazione del grafico
    plt.xlabel('$A$')
    plt.ylabel('$n_s^{Montecarlo}-n_s^{esatto}$')
    plt.ylim(-1.0 , 1.0)
    plt.title(diz_file[nome_file][1])
    plt.grid(True, linestyle='--', alpha=0.6)
    #plt.savefig("immaginiTesi/ns_riscalato_" + diz_file[nome_file][2] + ".png")
    plt.show()