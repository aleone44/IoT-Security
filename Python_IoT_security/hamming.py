import os


def calcola_distanza_hamming(bin1, bin2):
    """
    Calcola la Distanza di Hamming e la percentuale di errore tra due stringhe binarie.
    Confronta solo fino alla lunghezza della stringa più corta.
    """
    # Trova la lunghezza minima per allineare le stringhe
    min_len = min(len(bin1), len(bin2))

    if min_len == 0:
        return 0, 0, 0

    # Taglia le stringhe alla stessa lunghezza
    s1 = bin1[:min_len]
    s2 = bin2[:min_len]

    # Conta quanti bit sono diversi nella stessa posizione
    distanza = sum(1 for bit1, bit2 in zip(s1, s2) if bit1 != bit2)

    # Calcola la percentuale di differenza
    percentuale = (distanza / min_len) * 100

    return distanza, percentuale, min_len


# --- CONFIGURAZIONE FILE ---
# Inserisci qui i nomi dei tuoi due file da confrontare
file_1 = "puf_dump_vn_1.txt"
file_2 = "puf_dump_vn_2.txt"

# --- ESECUZIONE ---
try:
    with open(file_1, "r") as f1, open(file_2, "r") as f2:
        stringa_1 = f1.read().strip()
        stringa_2 = f2.read().strip()

    print(f"Confronto in corso tra {file_1} e {file_2}...\n" + "-" * 40)

    distanza, percentuale, bit_confrontati = calcola_distanza_hamming(stringa_1, stringa_2)

    print(f"Bit confrontati: {bit_confrontati}")
    print(f"Bit DIVERSI (Distanza di Hamming): {distanza}")
    print(f"Percentuale di differenza: {percentuale:.2f}%")

    # Interpretazione dei risultati per l'esame
    print("\n--- ANALISI PER L'ESAME ---")
    if percentuale < 40:
        print("✅ RISULTATO: Ottima AFFIDABILITÀ (Intra-Distance).")
        print("Questi due dump provengono quasi certamente dallo STESSO chip.")
    elif 40 <= percentuale <= 60:
        print("✅ RISULTATO: Ottima UNICITÀ (Inter-Distance).")
        print("Questi due dump provengono quasi certamente da DUE CHIP DIVERSI.")
    else:
        print("⚠️ RISULTATO IBRIDO: La differenza è anomala.")

except FileNotFoundError as e:
    print(f"Errore: File non trovato. Assicurati che i file esistano. Dettagli: {e}")