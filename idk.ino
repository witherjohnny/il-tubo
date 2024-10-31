#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/* 
SDA -> analogico4
SCL -> analogico5
 */


const int hallPins[] = {7,8, 9, 10, 11, 12, 13};  // Pin a cui sono collegati i 6 sensori di Hall
const int nSensori =7;
float times[nSensori] = {-1,-1,-1,-1,-1,-1,-1};                         // Vettore per memorizzare i tempi di passaggio
bool sensorActivated[] = {false, false, false, false, false, false,false};  // Stato attivazione dei sensori

const int btnReset = 4;   // Pin del pulsante "Reset"
const int btnNext = 5;   // Pin del pulsante "Avanti"
const int btnPrev =6;   // Pin del pulsante "Indietro"
const int ledVerde = 1;  
const int ledRosso = 2;  

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
  
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRosso, OUTPUT);

  Serial.begin(9600);  // Avvia la comunicazione seriale per il monitor seriale
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pronto per  ");
  lcd.setCursor(0, 1);
  lcd.print("iniziare ");
}

void loop() {
  if(!finito){

    digitalWrite(ledVerde,HIGH);
    digitalWrite(ledRosso,LOW);
    
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
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("misure in ");
          lcd.setCursor(0, 1);
          lcd.print("corso ");
        }

        // Salva il tempo trascorso per il sensore attivato
        times[i] = (millis() - startTime) ; // Tempo in secondi
        sensorCount++;

        Serial.print("Sensore ");
        Serial.print(i + 1);
        Serial.print(" attraversato. Tempo: ");
        Serial.print(times[i]);
        Serial.println(" secondi");
        
        if (sensorCount == nSensori) {
          Serial.println("finito");
          finito= true;
          digitalWrite(ledVerde,LOW);
          digitalWrite(ledRosso,HIGH);
        }
        
      }
    }

  }else{
    for( int i =0; i< nSensori; i++){
      if(!sensorActivated[i]){ //controllo dati mancanti
        finito=false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("dati mancanti ");
        lcd.setCursor(0, 1);
        lcd.print("riprovare ");
        delay(3000);
        reset();
        break;

      }
    }
    digitalWrite(ledVerde,LOW);
    digitalWrite(ledRosso,HIGH);
    if(finito){ //secondo controllo perche esiste la possibilita di dati mancanti gestito con il controllo precedente
    // Gestione dello scorrimento dei dati con i pulsanti
      
    
      mostraDati();
      if (digitalRead(btnNext) == HIGH) {
        currentIndex++;
        if (currentIndex >= nSensori) currentIndex = 0;
        mostraDati();
        delay(200);  // Debounce per evitare doppie letture
      }
      else if (digitalRead(btnPrev) == HIGH) {
        currentIndex--;
        if (currentIndex < 0) currentIndex = nSensori - 1;
        mostraDati();
        delay(200);  // Debounce per evitare doppie letture
      }
      else if(digitalRead(btnReset)== HIGH){
        reset();
      }
      delay(33);
    }
  }
  
}
void reset(){
  finito = false;
  for ( int i = 0; i< nSensori; i++){
    times[i] = -1;
    sensorActivated[i]=false;
  }
  isFirst = true; 
  sensorCount = 0; 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("resettato ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 0);
  lcd.print("pronto per  ");
  lcd.setCursor(0, 1);
  lcd.print("iniziare ");
      
}
void mostraDati() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo ");
  lcd.print(currentIndex + 1);
  lcd.print(":");
  lcd.setCursor(0, 1);
  if(times[currentIndex] ==-1){
      lcd.print("dato mancante");
      lcd.print(" s");
  }else{
    lcd.print(times[currentIndex]);
  }

  
}
