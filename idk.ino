
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* 
SDA -> analogico4
SCL -> analogico5
*/

const int hallPins[] = {7, 8, 9, 10, 11, 12, 13};  // Pin per i 6 sensori di Hall
const int nSensori = 7;
float times[nSensori] = {-1, -1, -1, -1, -1, -1, -1}; // Memorizza i tempi di passaggio
bool sensorActivated[] = {false, false, false, false, false, false, false}; // Stato attivazione sensori

const int btnReset = 4;
const int btnNext = 5;
const int btnPrev = 6;
const int ledVerde = 1;
const int ledRosso = 2;

unsigned long startTime = 0;
bool isFirst = true; // Flag per avvio del timer
int sensorCount = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool finito = false;
int currentIndex = 0;
bool canScroll = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  for (int i = 0; i < nSensori; i++) {
    pinMode(hallPins[i], INPUT);
  }

  pinMode(btnReset, INPUT);
  pinMode(btnNext, INPUT);
  pinMode(btnPrev, INPUT);
  
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRosso, OUTPUT);

  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pronto per");
  lcd.setCursor(0, 1);
  lcd.print("iniziare");
}

void loop() {
  if (!finito) {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledRosso, LOW);
    
    for (int i = 0; i < nSensori; i++) {
      int sensorState = digitalRead(hallPins[i]); // Legge lo stato del sensore

      if (sensorState == LOW && !sensorActivated[i]) {
        // Verifica che solo il primo sensore attivi il timer
        if (i == 0 && isFirst) {
          startTime = millis();
          isFirst = false;
          sensorActivated[i] = true;
          sensorCount++;
          Serial.println("Timer partito!");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("misure in corso");
        } else if (!isFirst && sensorActivated[0]) {
          // Solo se il primo sensore è stato attivato, leggi i successivi
          sensorActivated[i] = true;
          times[i] = (millis() - startTime); 
          sensorCount++;
          
          Serial.print("Sensore ");
          Serial.print(i + 1);
          Serial.print(" attraversato. Tempo: ");
          Serial.print(times[i]);
          Serial.println(" ms");
          
          if (sensorCount == nSensori) {
            Serial.println("Misurazione completata.");
            finito = true;
            digitalWrite(ledVerde, LOW);
            digitalWrite(ledRosso, HIGH);
          }
        }
      }
    }
  } else {
    for (int i = 0; i < nSensori; i++) {
      if (!sensorActivated[i]) {
        finito = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dati mancanti");
        lcd.setCursor(0, 1);
        lcd.print("Riprovare");
        delay(3000);
        reset();
        break;
      }
    }
    if (finito) {
      mostraDati();
      if (digitalRead(btnNext) == HIGH) {
        currentIndex++;
        if (currentIndex >= nSensori) currentIndex = 0;
        mostraDati();
        delay(200);
      } else if (digitalRead(btnPrev) == HIGH) {
        currentIndex--;
        if (currentIndex < 0) currentIndex = nSensori - 1;
        mostraDati();
        delay(200);
      }
      delay(33);
    }
  }
  
  if (digitalRead(btnReset) == HIGH) {
    reset();
    delay(200);
  }
}

void reset() {
  finito = false;
  for (int i = 0; i < nSensori; i++) {
    times[i] = -1;
    sensorActivated[i] = false;
  }
  isFirst = true;
  sensorCount = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resettato");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pronto per");
  lcd.setCursor(0, 1);
  lcd.print("iniziare");
}

void mostraDati() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo ");
  lcd.print(currentIndex + 1);
  lcd.print(":");
  lcd.setCursor(0, 1);
  if (times[currentIndex] == -1) {
    lcd.print("Dati mancanti");
  } else {
    lcd.print(times[currentIndex]);
    lcd.print(" ms");
  }
}
