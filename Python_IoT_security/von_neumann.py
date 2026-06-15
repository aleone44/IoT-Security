import os


def apply_von_neumann(binary_str):
    """
    Applica l'algoritmo di Von Neumann per bilanciare i bit.
    01 -> 0
    10 -> 1
    00, 11 -> Scartati
    """
    unbiased_bits = ""
    # Ciclo a salti di 2
    for i in range(0, len(binary_str) - 1, 2):
        pair = binary_str[i:i + 2]
        if pair == "01":
            unbiased_bits += "0"
        elif pair == "10":
            unbiased_bits += "1"
        # 00 e 11 vengono ignorati
    return unbiased_bits


# --- CONFIGURAZIONE FILE ---
input_filename = "puf_dump_bin_2.txt"
output_filename = "puf_dump_vn_2.txt"

# --- ESECUZIONE ---
try:
    # 1. Legge i dati grezzi in binario
    with open(input_filename, "r") as f:
        raw_bits = f.read().strip()

    # 2. Applica l'algoritmo
    clean_bits = apply_von_neumann(raw_bits)

    # 3. Salva i dati puliti nel nuovo file
    with open(output_filename, "w") as f:
        f.write(clean_bits)

    # 4. Stampa un report a schermo
    print("Elaborazione completata con successo!\n" + "-" * 40)
    print(f"File di origine: {input_filename}")
    print(f"Bit totali (Grezzi): {len(raw_bits)}")
    print(f"Bit estratti (Puliti): {len(clean_bits)}")

    # Calcola e mostra la resa percentuale
    if len(raw_bits) > 0:
        resa = (len(clean_bits) / len(raw_bits)) * 100
        print(f"Resa di estrazione:  {resa:.2f}%")
        print(f"\nIl risultato è stato salvato in: {output_filename}")

except FileNotFoundError:
    print(f"Errore: Il file '{input_filename}' non è stato trovato.")
    print("Assicurati che sia nella stessa cartella di questo script Python.")
except Exception as e:
    print(f"Si è verificato un errore imprevisto: {e}")