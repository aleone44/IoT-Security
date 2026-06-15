def hex_to_bin_file(input_file, output_file):
    try:
        with open(input_file, 'r') as f:
            hex_data = f.read().strip().replace(" ", "").replace("\n", "")

        # Converte ogni carattere hex in 4 bit
        binary_data = bin(int(hex_data, 16))[2:].zfill(len(hex_data) * 4)

        with open(output_file, 'w') as f:
            f.write(binary_data)

        print(f"Successo! File binario creato: {output_file}")
        print(f"Dimensione originale: {len(hex_data)} caratteri hex")
        print(f"Dimensione finale: {len(binary_data)} bit")

    except Exception as e:
        print(f"Errore: {e}")


# Esecuzione
hex_to_bin_file('puf_dump_2.txt', 'puf_dump_bin_2.txt')

