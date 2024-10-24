#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const int hallPins[] = {8, 9, 10, 11, 12, 13};  // Pin a cui sono collegati i 6 sensori di Hall
const int nSensori =2;
unsigned long times[nSensori];                         // Vettore per memorizzare i tempi di passaggio
bool sensorActivated[] = {false, false, false, false, false, false};  // Stato attivazione dei sensori

const int btnNext = 6;   // Pin del pulsante "Avanti"
const int btnPrev = 7;   // Pin del pulsante "Indietro"

unsigned long startTime = 0;                    // Tempo di inizio
bool isFirst = true;                          // Flag per indicare se il timer è partito
int sensorCount = 0;                            // Contatore del numero di sensori attraversati
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool finito = false;
int currentIndex = 0;         // Indice corrente per visualizzare i dati
bool canScroll = false;       // Flag che permette lo scrolling

void setup() {
  // Imposta tutti i pin dei sensori come input
  lcd.init();
  lcd.backlight();  // Accende la retroilluminazione
  // Scrivi un messaggio sulla prima riga
  lcd.setCursor(0, 0);
  for (int i = 0; i < nSensori; i++) {
    pinMode(hallPins[i], INPUT);
  }
  
  Serial.begin(9600);  // Avvia la comunicazione seriale per il monitor seriale
}

void loop() {
  
  if(!finito){
    for (int i = 0; i < nSensori; i++) {
      int sensorState = digitalRead(hallPins[i]); // Legge lo stato del sensore

      /*     Serial.print(i);  // Stampa il valore di i
      Serial.print(":");  // Stampa i due punti
      Serial.println(sensorState);  // Stampa il valore di sensorState e va a capo
      */
      if (sensorState == LOW && !sensorActivated[i]) {
        // Il magnete ha appena attraversato il sensore i
        sensorActivated[i] = true;

        if (isFirst) {
          // Se è il primo sensore, avvia il timer
          startTime = millis();
          isFirst = false;
          Serial.println("Timer partito!");
        }

        // Salva il tempo trascorso per il sensore attivato
        times[i] = (millis() - startTime) / 1000; // Tempo in secondi
        sensorCount++;

        Serial.print("Sensore ");
        Serial.print(i + 1);
        Serial.print(" attraversato. Tempo: ");
        Serial.print(times[i]);
        Serial.println(" secondi");
        
        if (sensorCount == nSensori) {
          finito= true;
        }
        
      }
      delay(33); // Delay di 33 secondi
    }

  }else{
// Gestione dello scorrimento dei dati con i pulsanti
    if (digitalRead(btnNext) == LOW) {
      currentIndex++;
      if (currentIndex >= nSensori) currentIndex = 0;
      mostraDati();
      delay(200);  // Debounce per evitare doppie letture
    }

    if (digitalRead(btnPrev) == LOW) {
      currentIndex--;
      if (currentIndex < 0) currentIndex = nSensori - 1;
      mostraDati();
      delay(200);  // Debounce per evitare doppie letture
    }



  }
}
void mostraDati() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo ");
  lcd.print(currentIndex + 1);
  lcd.print(":");
  lcd.setCursor(0, 1);
  lcd.print(times[currentIndex]);
  lcd.print(" s");

  // Alla fine mostra la legge della caduta di un grave (tempo al quadrato)
  /* if (currentIndex == nSensori - 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Caduta grave:");
    lcd.setCursor(0, 1);
    lcd.print("t^2 = ");
    lcd.print(times[currentIndex] * times[currentIndex]);
    lcd.print(" s^2");
  } */
}
