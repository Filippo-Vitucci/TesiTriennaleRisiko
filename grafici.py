import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import math
from scipy.stats import norm    # per inverso ripartizione gaussiana

nome_file = input("inserisci nome del file di dati (senza .dat): ")

with open(nome_file+'.dat' , 'r') as f:
    # salvo valori #define nella prima riga
    prima_riga = f.readline().strip().split()

    if len(prima_riga)==3:    # file montecarlo
        N_battaglie = int(prima_riga[0])
        Amax = int(prima_riga[1])
        Bmax = int(prima_riga[2])

    elif len(prima_riga)==2:    # file probEsatta
        Amax = int(prima_riga[0])
        Bmax = int(prima_riga[1])

    # resto dei dati
    dati = np.loadtxt(f)
    #   A    B    pA_mc    sig_pA_mc    n_scontri_media    sig_n_scontri_media         se file montecarlo
    #   A    B    pA_esatta     n_scontri_media_esatta                                 se file probEsatta



if len(prima_riga)==3:  #file montecarlo

    print("1 - grafico p(A,B) MC completo")
    print("2 - grafico p(A,B) MC spaghettificato")
    print("3 - grafico N_scontri_medio(A,B) MC completo")
    print("4 - grafico N_scontri_medio(A,B) MC spaghettificato")
    print("5 - curve di probabilita MC': B vs A")
    print("6 - curve di probabilita MC': B vs A/B")
    scelta = int(input("scegli il grafico da visualizzare: "))

    if scelta==1 or scelta==2:
        # estrazione delle colonne da dati
        if scelta==1:
            x = dati[:, 1]
            y = dati[:, 0]
            z = dati[:, 2]
        else:
            x = dati[0:Bmax , 1]
            y = dati[0:Bmax , 0]
            z = dati[0:Bmax , 2]
            spag = int(Bmax/20)
            for a in range(spag, Amax+1, spag):
                x = np.concatenate((x, dati[ (a*Bmax):((a+1)*Bmax) , 1 ]))
                y = np.concatenate((y, dati[ (a*Bmax):((a+1)*Bmax) , 0 ]))
                z = np.concatenate((z, dati[ (a*Bmax):((a+1)*Bmax) , 2 ]))
        # creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        surf = ax.scatter(x, y, z, c=z, cmap='viridis', marker='o')   #scattering dei punti
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)  # barra dei colori laterale
        # Etichette degli assi
        ax.set_xlabel('A')
        ax.set_ylabel('B')
        ax.set_zlabel('p(A,B)')
        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # apri immagine
        plt.show()

    elif scelta==3 or scelta==4:
        # estrazione delle colonne da dati
        if scelta==3:
            x = dati[:, 0]
            y = dati[:, 1]
            z = dati[:, 4]
        else:
            x = dati[0:Bmax , 0]
            y = dati[0:Bmax , 1]
            z = dati[0:Bmax , 4]
            spag = int(Bmax/20)
            for a in range(spag, Amax+1, spag):
                x = np.concatenate((x, dati[ (a*Bmax):((a+1)*Bmax) , 0 ]))
                y = np.concatenate((y, dati[ (a*Bmax):((a+1)*Bmax) , 1 ]))
                z = np.concatenate((z, dati[ (a*Bmax):((a+1)*Bmax) , 4 ]))
        # creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        surf = ax.scatter(x, y, z, c=z, cmap='viridis', marker='o')   #scattering dei punti
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)  # barra dei colori laterale
        # Etichette degli assi
        ax.set_xlabel('A')
        ax.set_ylabel('B')
        ax.set_zlabel('N_scontri_medio(A,B)')
        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # apri immagine
        plt.show()

    elif scelta==5 or scelta==6:
        p_curve = [0.10, 0.25, 0.50, 0.75, 0.90]
        colori = ["red", "orange", "yellow", "lightblue", "green"]
        A = dati[:,0]
        B = dati[:,1]
        pA = dati[:,2]
        #creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        plt.xlabel('B')
        if scelta==5:
            plt.ylabel('A')
        else:
            plt.ylabel('A/B')
        plt.title("Curve di probabilita'")

        for j in range(0,len(p_curve)):
            pc = p_curve[j]
            for Acurva in range(2,Amax+1):
                diff_min = 10
                for i in range( (Acurva-2)*Bmax , (Acurva-1)*Bmax):
                    if abs(pA[i]-pc)<diff_min:
                        diff_min = abs(pA[i]-pc)
                        Bcurva = B[i]
                        
                if scelta==5:
                    plt.scatter(Bcurva, Acurva, color=colori[j], marker='o', s=10)
                else:
                    plt.scatter(Bcurva, Acurva/Bcurva, color=colori[j], marker='o', s=10)
        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # apri immagine
        plt.show()
            
elif len(prima_riga)==2:   # file probEsatta

    print("1 - grafico p(A,B) esatta completo")
    print("2 - grafico p(A,B) esatta spaghettificato")
    print("3 - grafico N_scontri_medio(A,B) esatta completo")
    print("4 - grafico N_scontri_medio(A,B) esatta spaghettificato")
    print("5 - curve di probabilita esatta': B vs A")
    print("6 - curve di probabilita esatta': B vs A/B")
    scelta = int(input("scegli il grafico da visualizzare: "))

    if scelta==1 or scelta==2:
        # estrazione delle colonne da dati
        if scelta==1:
            x = dati[:, 1]
            y = dati[:, 0]
            z = dati[:, 2]
        else:
            x = dati[0:Bmax , 1]
            y = dati[0:Bmax , 0]
            z = dati[0:Bmax , 2]
            spag = int(Bmax/20)
            for a in range(spag, Amax+1, spag):
                x = np.concatenate((x, dati[ (a*Bmax):((a+1)*Bmax) , 1 ]))
                y = np.concatenate((y, dati[ (a*Bmax):((a+1)*Bmax) , 0 ]))
                z = np.concatenate((z, dati[ (a*Bmax):((a+1)*Bmax) , 2 ]))
        # creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        surf = ax.scatter(x, y, z, c=z, cmap='viridis', marker='o')   #scattering dei punti
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)  # barra dei colori laterale
        # Etichette degli assi
        ax.set_xlabel('$B$')
        ax.set_ylabel('$A$')
        ax.set_zlabel('$P(A,B)$')
        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # apri immagine
        plt.show()

    elif scelta==3 or scelta==4:
        # estrazione delle colonne da dati
        if scelta==3:
            x = dati[:, 0]
            y = dati[:, 1]
            z = dati[:, 3]
        else:
            x = dati[0:Bmax , 0]
            y = dati[0:Bmax , 1]
            z = dati[0:Bmax , 3]
            spag = int(Bmax/20)
            for a in range(spag, Amax+1, spag):
                x = np.concatenate((x, dati[ (a*Bmax):((a+1)*Bmax) , 0 ]))
                y = np.concatenate((y, dati[ (a*Bmax):((a+1)*Bmax) , 1 ]))
                z = np.concatenate((z, dati[ (a*Bmax):((a+1)*Bmax) , 3 ]))
        # creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        surf = ax.scatter(x, y, z, c=z, cmap='viridis', marker='o')   #scattering dei punti
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)  # barra dei colori laterale
        # Etichette degli assi
        ax.set_xlabel('$B$')
        ax.set_ylabel('$A$')
        ax.set_zlabel('$n_s(A,B)$')
        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # apri immagine
        plt.show()

    elif scelta==5 or scelta==6:
        p_curve = [0.10, 0.25, 0.50, 0.75, 0.90]
        colori = ["red", "orange", "yellow", "lightblue", "green"]
        A = dati[:,0]
        B = dati[:,1]
        pA = dati[:,2]
        #creazione del grafico
        fig = plt.figure(figsize=(10, 7))
        plt.xlabel('B')
        if scelta==5:
            plt.ylabel('A')
        else:
            plt.ylabel('A/B')
        plt.title("A/B per curve di probabilita")

        for j in range(0,len(p_curve)):
            pc = p_curve[j]
            for Acurva in range(2,Amax+1):
                diff_min = 10
                for i in range( (Acurva-2)*Bmax , (Acurva-1)*Bmax):
                    if abs(pA[i]-pc)<diff_min:
                        diff_min = abs(pA[i]-pc)
                        Bcurva = B[i]

                if scelta==5:
                    if Acurva==2:
                        plt.scatter(Bcurva, Acurva, color=colori[j], marker='o', label=f"P(A,B) = {p_curve[j]*100}%", s=10)
                    else:
                        plt.scatter(Bcurva, Acurva, color=colori[j], marker='o', s=10)
                else:
                    if Acurva==2:
                        plt.scatter(Bcurva, Acurva/Bcurva, color=colori[j], marker='o', label=f"P(A,B) = {p_curve[j]*100}%", s=10)
                    else:
                        plt.scatter(Bcurva, Acurva/Bcurva, color=colori[j], marker='o', s=10)

            if scelta==6:           
                # stima teorica della pendenza
                m_50 = 1.7104
                sig_eff = 2.7467
                # 650 indicativo per il plot, regolabile
                x = np.linspace(2, 650, 1000)
                y = m_50 + norm.ppf(p_curve[j])*sig_eff/np.sqrt(x)
                plt.plot(
                    x, y, 
                    linestyle='--',          # Linea continua ('--' per tratteggiata, ':' per puntinata)
                    #linewidth=2,            # Spessore della linea
                    marker='None',             # (Opzionale) Aggiunge un cerchietto in corrispondenza di ogni punto
                    markersize=5,           # Dimensione dei cerchietti
                    color='black',           # Colore della linea
                    #label='Etichetta'       # Etichetta per la legenda
                )

        # Salva direttamente come immagine PNG
        plt.savefig(nome_file+'.png', dpi=300, bbox_inches='tight')
        # mostra legenda
        plt.legend(loc="best")
        # apri immagine
        plt.show()

