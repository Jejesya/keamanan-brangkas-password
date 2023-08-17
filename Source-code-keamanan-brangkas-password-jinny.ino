#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Password.h>
#include <Keypad.h>

const byte ROWS = 4; // Four rows
const byte COLS = 4; //  columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = { 4, 5, 6, 7 }; // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[COLS] = { A0, A1, A2, A3 }; // Connect keypad COL0, COL1, COL2, and COL3 to these Arduino pins.

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27, 16, 2);
Password password = Password("261001"); //Masukan Password yang diinginkan.

const int buzzer = 2;
const int relay = 8;

int alarm = 0;
uint8_t alarmStat = 0;
uint8_t maxError = 5;
uint8_t pwMode = 0;
uint8_t pwPos = 0;
bool access = false;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  keypad.addEventListener(keypadEvent); //add an event listener for this keypad

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  lcd.setCursor (0, 0);
  lcd.print(F("Brangkas Jinny"));
  lcd.setCursor (3, 1);
  lcd.print(F("Please"));
  for (int i = 0; i < 3; i++) {
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  noTone (buzzer);
  delay(50);
}

void loop() {
  keypad.getKey();

  if (alarm >= maxError) {
    alarmStat = 1;
  }

  if (alarmStat == 0 && pwMode == 0) {
    lcd.setCursor (0, 0);
    lcd.print(F(" -Buka Brangkas- "));
    lcd.setCursor (0, 1);
    lcd.print(F("Masukan Password"));
  }

  if (alarmStat == 1) {
    lcd.setCursor (0, 0);
    lcd.print(F("-TERKUNCI-"));
    lcd.setCursor (0, 1);
    lcd.print(F(" Tunggu Selama "));
    for (int i = 60; i > 0; i--) {
      tone (buzzer, 1800);
      lcd.setCursor (13, 1); lcd.print(i);
      lcd.print(F("  ")); delay (1000);
    }
    noTone (buzzer);
    alarmStat = 0;
    alarm = 0;
  }
}

void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:
      Serial.print("Pressed: ");
      Serial.println(eKey);

      pwMode = 1;
      pwPos = pwPos + 1;
      if (pwPos == 1) {
        lcd.clear();
      }
      lcd.setCursor (0, 0);
      lcd.print(F("Masukan Password"));
      if (pwPos < 7) {
        lcd.setCursor (4 + pwPos, 1);
        lcd.print(F("*"));
      }

      switch (eKey) {
        case '*': checkPassword(); break;
        case '#': password.reset();
          pwPos = 0;
          lcd.clear();
          delay(200);
          lcd.setCursor (0, 0);
          lcd.print(F("Ulangi"));
          break;

        default: password.append(eKey);
      }
  }
}

void checkPassword() {
  if (password.evaluate()) {
    Serial.println("Password Benar");
    lcd.setCursor (0, 1);
    lcd.print(F("   -Terbuka-   "));
    alarm = 0;
    diterima();
    password.reset();
    pwPos = 0;
  }

  else {
    Serial.println("Password Salah");
    lcd.setCursor (0, 1);
    lcd.print(F("   -ditolak-   "));
    alarm = alarm + 1;
    ditolak();
    password.reset();
    pwPos = 0;
  }
}

void diterima() {
  digitalWrite(relay, LOW);
  lcd.setCursor (0, 0);
  lcd.print(F("Brangkas Terbuka"));
  lcd.setCursor (0, 1);
  lcd.print(F("Selama 3 Detik"));
  for (int i = 3; i > 0; i--) {
    lcd.setCursor (15, 1); lcd.print(i);
    delay (1000);
  }

  digitalWrite(relay, HIGH);
  pwMode = 0;
  lcd.clear();
  delay(50);
}

void ditolak() {
  tone (buzzer, 900);
  delay(200);
  noTone(buzzer);
  delay(200);
  tone (buzzer, 900);
  delay(200);
  noTone (buzzer);
  delay(500);
  pwMode = 0;
  lcd.clear();
  delay(50);
}
