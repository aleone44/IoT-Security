# IoT-Security
-1 PUF_starter_1.ino serve per estrarre la puf dall'esp e cominicarla a nodered tramite wifi
-2 Python_IoT_security folder contiene i file python per convertire la chiave, step 1 file convertitore,2 von neumann, 3 distanza di hamming. i file txt contengono le chiavi a seguito dei vari step di trasformazione, sono due, uno per ogni dispositivo.
-3 il file json percmette di creare l'ambiente node-red

funzionamento:
- il dispositivo esp32 di oollega al pc, arduino legge ed estrae la puf in esadecimale.
-  pythpn converte prima in intero poi in binario ed effettua delle modifiche(cutting delle parti ridondanti), dopodichè verifica la distanza fra le due chiavi e determina se si tratta dello stesso dispositivo o meno.
- il file .ino nel frattempo ha inviato direttamente la chiave tramite wi fi a nodered, che effettua la verifica del dispositivo, se è ritenuto valido permette la trasmissione di un messaggio di prova.
