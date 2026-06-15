#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "mbedtls/aes.h"

// =======================================================
// 1. CONFIGURAZIONE WI-FI, SERVER E CRITTOGRAFIA
// =======================================================
const char* ssid = "Pixel";
const char* password = "sample1234";
// Inserisci l'IP del tuo computer seguito da :1880/auth
const char* serverName = "http://10.104.151.186:1880/auth"; 
// La nostra chiave segreta pre-condivisa (PSK). DEVE essere di 16 caratteri per AES-128
const char* aes_key = "SuperSegreta1234"; 

// =======================================================
// 2. CONFIGURAZIONE MEMORIA PUF
// =======================================================
#define PUF_SIZE 1024
// Area di memoria RTC isolata dal Bootloader (per evitare che venga sovrascritta)
RTC_NOINIT_ATTR uint8_t puf_data[PUF_SIZE]; 

// =======================================================
// 3. FUNZIONE DI CRITTOGRAFIA (AES-128 ECB + PKCS#7)
// =======================================================
String encryptAES(String plainText) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char*)aes_key, 128);

    // Calcolo del Padding PKCS#7 (AES lavora solo su blocchi multipli di 16 byte)
    int textLen = plainText.length();
    int padLen = 16 - (textLen % 16);
    int paddedLen = textLen + padLen;
    
    uint8_t* paddedText = (uint8_t*)malloc(paddedLen);
    memcpy(paddedText, plainText.c_str(), textLen);
    for (int i = textLen; i < paddedLen; i++) {
        paddedText[i] = padLen; // Riempie lo spazio vuoto con il valore del padding
    }

    // Array per il risultato cifrato
    uint8_t* encryptedText = (uint8_t*)malloc(paddedLen);

    // Cifratura a blocchi di 16 byte (Modalità ECB)
    for (int i = 0; i < paddedLen; i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, paddedText + i, encryptedText + i);
    }
    mbedtls_aes_free(&aes);

    // Converte i byte cifrati in una stringa Esadecimale leggibile per inviarla
    String hexOutput = "";
    for (int i = 0; i < paddedLen; i++) {
        char hex[3];
        sprintf(hex, "%02x", encryptedText[i]);
        hexOutput += hex;
    }

    free(paddedText);
    free(encryptedText);
    return hexOutput;
}

// =======================================================
// 4. ELABORAZIONE PUF (BINARIO E VON NEUMANN)
// =======================================================
String get_puf_binaria() {
    String raw_bin = "";
    // Passaggio A: Converte i byte esadecimali in una stringa di bit
    for (int i = 0; i < PUF_SIZE; i++) {
        for (int b = 7; b >= 0; b--) {
            if ((puf_data[i] >> b) & 1) raw_bin += "1";
            else raw_bin += "0";
        }
    }

    // Passaggio B: Applica l'algoritmo di Von Neumann
    String unbiased_bin = "";
    for (unsigned int i = 0; i < raw_bin.length() - 1; i += 2) {
        char bit1 = raw_bin[i];
        char bit2 = raw_bin[i+1];
        
        if (bit1 == '0' && bit2 == '1') unbiased_bin += "0";
        else if (bit1 == '1' && bit2 == '0') unbiased_bin += "1";
        // 00 e 11 vengono scartati
    }
    return unbiased_bin;
}

// =======================================================
// 5. INVIO DATI CIFRATI VIA WI-FI
// =======================================================
void setup_wifi_and_send(String puf_binaria) {
    WiFi.begin(ssid, password);
    Serial.print("\nConnessione al Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); Serial.print(".");
    }
    Serial.println("\nConnesso! IP: " + WiFi.localIP().toString());

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        // 1. Creiamo il JSON in chiaro con la PUF purificata
        String jsonChiaro = "{\"puf\":\"" + puf_binaria + "\", \"messaggio\":\"Allarme temperatura!\"}";
        
        // 2. LO CIFRIAMO CON AES-128
        String payloadCifrato = encryptAES(jsonChiaro);
        
        // 3. Creiamo un nuovo JSON che contiene SOLO la stringa cifrata
        String jsonDaInviare = "{\"dati_sicuri\":\"" + payloadCifrato + "\"}";
        
        Serial.println("\n[SECURE] Payload generato e cifrato con AES-128.");
        Serial.println("[SECURE] Intercettare il pacchetto Wi-Fi ora è inutile. Invio in corso...");
        
        int httpResponseCode = http.POST(jsonDaInviare);
        
        if (httpResponseCode > 0) {
            Serial.print("[SERVER] Risposta dal server Node-RED (Codice HTTP): ");
            Serial.println(httpResponseCode); // 200 = OK, 401 = Negato
        } else {
            Serial.print("[ERRORE] Errore di rete durante l'invio. Codice: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
}

// =======================================================
// 6. BLOCCO PRINCIPALE
// =======================================================
void setup() {
    Serial.begin(115200);
    delay(2000); 

    // Stampa i dati grezzi estratti dalla RAM (utile per il debug)
    Serial.println("\n--- STARTUP PUF RAW DATA (HEX) ---");
    for (int i = 0; i < PUF_SIZE; i++) {
        if (puf_data[i] < 0x10) Serial.print("0");
        Serial.print(puf_data[i], HEX);
    }
    Serial.println("\n--- END OF RAW DATA ---");

    Serial.println("\nElaborazione Von Neumann in corso sul microcontrollore...");
    String chiave_puf = get_puf_binaria();
    
    Serial.print("Chiave PUF finale generata (Binaria): ");
    Serial.println(chiave_puf);

    // Esegue il flusso di rete protetto
    setup_wifi_and_send(chiave_puf);
}

void loop() {
    // Essendo un'estrazione basata sull'accensione (Cold Boot), 
    // l'autenticazione si fa una sola volta all'avvio. 
    // Nessun codice necessario qui.
}